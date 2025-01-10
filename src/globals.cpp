// Copyright 1996-2012, 2014, 2016-2019, 2021-2024 by Jon Dart.  All Rights Reserved.

#include "globals.h"
#include "hash.h"
#include "scoring.h"
#include "search.h"
//#ifdef SYZYGY_TBS
//#include "syzygy.h"
//#endif
#include "bitbase.cpp"

#ifdef _MAC
extern "C" {
#include <libproc.h>
};
#elif !defined(_MSC_VER)
// assume POSIX system
extern "C" {
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/resource.h>
}
#endif
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
#include <filesystem>
#endif

//#ifdef SYZYGY_TBS
//static bool tb_init = false;
//
//bool globals::tb_init_done() { return tb_init; }
//int globals::EGTBMenCount = 0;
//#endif

#ifdef _WIN32
static constexpr char PATH_CHAR = '\\';
#else
static constexpr char PATH_CHAR = '/';
#endif

MoveArray *globals::gameMoves;
BookReader globals::openingBook;
std::ofstream globals::game_file;
Options globals::options;

std::mutex globals::input_lock;

//#ifdef SYZYGY_TBS
//std::mutex globals::syzygy_lock;
//#endif
bool globals::polling_terminated;
std::string globals::debugPrefix;
nnue::Network globals::network;
bool globals::nnueInitDone = false;

#ifdef _WIN32
static const char *LEARN_FILE_NAME = "arasan.lrn";
#else
static const char *LEARN_FILE_NAME = ".arasan.lrn";
#endif

static const char *DEFAULT_BOOK_NAME = "book.bin";

static const char *RC_FILE_NAME = "arasan.rc";

const size_t globals::LINUX_STACK_SIZE = 4 * 1024 * 1024;

static bool absolutePath(const std::string &fileName) {
#ifdef _WIN32
    auto pos = fileName.find(':');
    if (pos == std::string::npos) {
        pos = 0;
    } else if (pos + 1 < fileName.size()) {
        ++pos;
    }
#else
    size_t pos = 0;
#endif
    return pos >= fileName.size() || fileName[pos] == PATH_CHAR;
}

// Note argv[0] is the name of the command used to execute the
// program. If the program was not executed from the directory
// in which it is located, but was found through the PATH, then
// this may not indicate the actual file location of the
// executable.
// This function returns the full path of the executable, the finding
// of which is OS-dependent
//
static void getExecutablePath(std::string &path) {
    path = "";
#ifdef _WIN32
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, MAX_PATH)) {
        path = std::string(szPath);
    }
#elif defined(_MAC)
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    pathbuf[0] = '\0';
    int ret = proc_pidpath(getpid(), pathbuf, sizeof(pathbuf));
    if (ret > 0) {
        path = std::string(pathbuf);
    }
#else
    char result[PATH_MAX];
    result[0] = '\0';
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    path = std::string(result, (count > 0) ? count : 0);
#if defined(__APPLE__)
    if (path.empty() && getenv("XCTestBundlePath") == nullptr) {
        path = std::getenv("HOME");
    }
#endif
#endif
}

std::string globals::derivePath(const std::string &fileName) {
    std::string path;
    getExecutablePath(path);
    return derivePath(path, fileName);
}

std::string globals::derivePath(const std::string &base, const std::string &fileName) {
    std::string result(base);
    size_t pos;
    pos = result.rfind(PATH_CHAR, std::string::npos);
    if (pos == std::string::npos) {
        return fileName;
    } else {
#if defined(__APPLE__)
    if (getenv("XCTestBundlePath") == nullptr) {
        return result + "/" + fileName;
    }
#endif
        return result.substr(0, pos + 1) + fileName;
    }
}

bool globals::initGlobals() {
    //Does not work on iOS/VisionOS/macOS
#if (!defined(_WIN32) && !defined(__APPLE__))
    struct rlimit rl;
    const rlim_t STACK_MAX = static_cast<rlim_t>(LINUX_STACK_SIZE);
    auto result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < STACK_MAX)
        {
            rl.rlim_cur = STACK_MAX;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result)
            {
                std::cerr << "failed to increase stack size" << std::endl;
                return false;
            }
        }
    }
#endif
    globals::gameMoves = new MoveArray();
    globals::polling_terminated = false;
    return true;
}

bool globals::loadNetwork(const std::string &fname, bool verbose) {
    std::ifstream in(fname, std::ios_base::in | std::ios_base::binary);
    if (in.fail()) {
        if (verbose) {
            std::cout << "warning: failed to open network file " << fname << ": " <<
                strerror(errno) << std::endl;
        }
        return false;
    }
    in >> network;
    if (in.fail()) {
        if (verbose) std::cout << "warning: failure reading network file " << fname << std::endl;
        return false;
    } else {
        if (verbose) std::cout << "loaded network from file " << fname << std::endl;
        return true;
    }
}

void CDECL globals::cleanupGlobals(void) {
    if (openingBook.is_open()) openingBook.close();
    delete gameMoves;
    Scoring::cleanup();
    Bitboard::cleanup();
    Board::cleanup();
    if (game_file.is_open()) game_file.close();
}

bool globals::initOptions(bool autoLoadRC, const char *rcPath,
                          bool memorySet, bool cpusSet) {
    if (autoLoadRC) {
        const std::string stdRcPath(derivePath(RC_FILE_NAME));
        // try to read arasan.rc file
        // failure to read is not an error: .rc file is optional
        options.init(stdRcPath, memorySet, cpusSet);
    } else if (rcPath != nullptr) {
        if (!options.init(rcPath)) { // explicit path was given so fail on error
            std::cerr << "failed to load options from " << rcPath << std::endl;
            return false;
        }
    }
#ifdef _WIN32
    const char *homeDirEnv = "USERPROFILE";
    const char *appDirEnv = "APPDATA";
#else
    const char *homeDirEnv = "HOME";
    const char *appDirEnv = "HOME";
#endif
    std::string userDir, appDir;
    if (std::getenv(homeDirEnv)) {
        userDir = std::getenv(homeDirEnv);
    } else {
        std::cerr << "warning: could not obtain home directory location" << std::endl;
    }
    if (std::getenv(appDirEnv)) {
#ifdef _WIN32
        appDir = std::getenv(appDirEnv);
        appDir += "\\Arasan";
        if (!std::filesystem::exists(appDir)) {
            std::error_code ec;
            std::filesystem::create_directory(appDir, ec);
            if (ec.value() != 0) {
                std::cerr << "failed to create application data directory: " << ec.message()
                          << std::endl;
            }
        }
#else
        appDir = getenv(appDirEnv);
#endif
    } else {
        std::cerr << "warning: could not obtain application data directory location" << std::endl;
    }
    // Also attempt to read .rc from the user's home directory.
    // If present, this overrides the file at the install location.
    if (autoLoadRC && userDir.size()) {
#if defined(_WIN32) || defined(__APPLE__)
        std::string userRcFile(RC_FILE_NAME);
#else
        std::string userRcFile(".");
        userRcFile += RC_FILE_NAME;
#endif
        // failure to read is not an error: .rc file is optional
        options.init(derivePath(userDir + PATH_CHAR, userRcFile), memorySet, cpusSet);
    }
    if (appDir.size()) {
        options.learning.learn_file_name = derivePath(appDir + PATH_CHAR, LEARN_FILE_NAME);
    }
    // if book path not set in the .rc file, set a default here
    if (options.book.book_path == "") {
        options.book.book_path = derivePath(DEFAULT_BOOK_NAME);
    }
    return true;
}

void globals::initGameFile() {
    if (globals::options.games.store_games && !game_file.is_open()) {
        std::string pathname;
        if (globals::options.games.game_pathname == "") {
            pathname = globals::derivePath("games.pgn");
        } else {
            pathname = globals::options.games.game_pathname;
        }
        game_file.open(pathname.c_str(), std::ios::out | std::ios::app);
        if (!game_file.good()) {
            std::cerr << "# warning: cannot open game file. Games will not be saved." << std::endl;
        }
    }
}

void globals::delayedInit(bool verbose) {
//#ifdef SYZYGY_TBS
//    if (options.search.use_tablebases && !globals::tb_init_done()) {
//        EGTBMenCount = 0;
//        std::string path;
//        if (options.search.syzygy_path == "") {
//            options.search.syzygy_path = derivePath("syzygy");
//        }
//        path = options.search.syzygy_path;
//        EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
//        tb_init = true;
//        if (verbose) {
//            if (EGTBMenCount) {
//                std::stringstream msg;
//                msg << debugPrefix << "found " << EGTBMenCount
//                    << "-man Syzygy tablebases in directory " << path << std::endl;
//                std::cout << msg.str();
//            } else {
//                std::cout << debugPrefix
//                          << "warning: no Syzygy tablebases found, path may be missing or invalid"
//                          << std::endl;
//            }
//        }
//    }
//#endif
    if (!nnueInitDone) {
        if (options.search.nnueFile.size()) {
            const std::string &nnueFile = options.search.nnueFile;
            const std::string &nnuePath = absolutePath(nnueFile) ? nnueFile.c_str() : derivePath(nnueFile);
            
            nnueInitDone = loadNetwork(nnuePath, verbose);
            
            if (!nnueInitDone) {
                std::string home = std::getenv("HOME");
                const std::string nnueFullPath = home + "/" + nnueFile;
                nnueInitDone = loadNetwork(nnueFullPath, verbose);
            }
        } else if (verbose) {
            std::cout << debugPrefix << "error: no NNUE file path was set, network not loaded"
                      << std::endl;
        }
        if (!nnueInitDone) {
            // This is now a fatal error
            std::cerr << "failed to load network, terminating." << std::endl;
            globals::cleanupGlobals();
            return;
        }
    }
    // also initialize the book here
    if (options.book.book_enabled && !openingBook.is_open()) {
        if (openingBook.open(options.book.book_path.c_str()) && verbose) {
            std::cout << debugPrefix << "warning: opening book not found or invalid" << std::endl;
        } else if (verbose) {
            std::cout << debugPrefix << "loaded opening book from file " << options.book.book_path
                      << std::endl;
        }
    }
    initGameFile();
}

void globals::unloadTb() {
//#ifdef SYZYGY_TBS
//    if (tb_init_done()) {
//        // Note: Syzygy code will free any existing memory if
//        // initialized more than once. So no need to do anything here.
//        tb_init = false;
//    }
//#endif
}
