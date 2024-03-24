// Copyright 1996-2012, 2014, 2016-2019, 2021-2024 by Jon Dart.  All Rights Reserved.

#include "globals.h"
#include "hash.h"
#include "scoring.h"
#include "search.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "bitbase.cpp"

#ifdef _MAC
extern "C" {
#include <libproc.h>
};
#elif !defined(_MSC_VER)
// assume POSIX system
extern "C" {
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
}
#endif

#include <cstdlib>
#include <filesystem>

#ifdef SYZYGY_TBS
static bool tb_init = false;

bool globals::tb_init_done() { return tb_init; }
int globals::EGTBMenCount = 0;
#endif

#ifdef _WIN32
static constexpr char PATH_CHAR = '\\';
#else
static constexpr char PATH_CHAR = '/';
#endif

MoveArray *globals::gameMoves;
Options globals::options;
BookReader globals::openingBook;

std::mutex globals::input_lock;

#ifdef SYZYGY_TBS
std::mutex globals::syzygy_lock;
#endif
bool globals::polling_terminated;
std::string globals::debugPrefix;
#ifdef TUNE
Tune globals::tune_params;
#endif
#ifdef NNUE
nnue::Network globals::network;
bool globals::nnueInitDone = false;
#endif

#ifdef _WIN32
static const char *LEARN_FILE_NAME = "arasan.lrn";
#else
static const char *LEARN_FILE_NAME = ".arasan.lrn";
#endif

static const char *DEFAULT_BOOK_NAME = "book.bin";

static const char *RC_FILE_NAME = "arasan.rc";

const size_t globals::LINUX_STACK_SIZE = 4 * 1024 * 1024;

#ifdef NNUE
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
#endif

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
        return result.substr(0, pos + 1) + fileName;
    }
}

int globals::initGlobals() {
    globals::gameMoves = new MoveArray();
    globals::polling_terminated = false;
    return 1;
}

#ifdef NNUE
int globals::loadNetwork(const std::string &fname) {
    std::ifstream in(fname, std::ios_base::in | std::ios_base::binary);
    in >> network;
    if (!in.good()) {
        return 0;
    }
    return 1;
}
#endif

void CDECL globals::cleanupGlobals(void) {
    openingBook.close();
    delete gameMoves;
    Scoring::cleanup();
    Bitboard::cleanup();
    Board::cleanup();
}

void globals::initOptions() {
    std::string rcPath(derivePath(RC_FILE_NAME));
    // try to read arasan.rc file
    options.init(rcPath);
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
    if (userDir.size()) {
#ifdef _WIN32
        std::string userRcfile(RC_FILE_NAME);
#else
        std::string userRcFile(".");
        userRcFile += RC_FILE_NAME;
#endif
        options.init(derivePath(userDir + PATH_CHAR, userRcFile));
    }
    if (appDir.size()) {
        options.learning.learn_file_name = derivePath(appDir + PATH_CHAR, LEARN_FILE_NAME);
    }
    // if path not set in the .rc file, set a default here
    if (options.book.book_path == "") {
        options.book.book_path = derivePath(DEFAULT_BOOK_NAME);
    }
}

void globals::delayedInit(bool verbose) {
#ifdef SYZYGY_TBS
    if (options.search.use_tablebases && !globals::tb_init_done()) {
        EGTBMenCount = 0;
        std::string path;
        if (options.search.syzygy_path == "") {
            options.search.syzygy_path = derivePath("syzygy");
        }
        path = options.search.syzygy_path;
        EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
        tb_init = true;
        if (verbose) {
            if (EGTBMenCount) {
                std::stringstream msg;
                msg << debugPrefix << "found " << EGTBMenCount
                    << "-man Syzygy tablebases in directory " << path << std::endl;
                std::cout << msg.str();
            } else {
                std::cout << debugPrefix
                          << "warning: no Syzygy tablebases found, path may be missing or invalid"
                          << std::endl;
            }
        }
    }
#endif
#ifdef NNUE
    if (options.search.useNNUE && !nnueInitDone) {
        if (options.search.nnueFile.size()) {
            const std::string &nnuePath = options.search.nnueFile;
            nnueInitDone =
                loadNetwork(absolutePath(nnuePath) ? nnuePath.c_str() : derivePath(nnuePath));
            if (verbose) {
                if (nnueInitDone) {
                    std::cout << debugPrefix << "loaded network from file ";
                } else {
                    std::cout << debugPrefix << "warning: failed to load network file ";
                }
                std::cout << nnuePath << std::endl;
            }
        } else if (verbose) {
            std::cout << debugPrefix << "warning: no NNUE file path was set, network not loaded"
                      << std::endl;
        }
    }
#endif
    // also initialize the book here
    if (options.book.book_enabled && !openingBook.is_open()) {
        if (openingBook.open(options.book.book_path.c_str()) && verbose) {
            std::cout << debugPrefix << "warning: opening book not found or invalid" << std::endl;
        } else if (verbose) {
            std::cout << debugPrefix << "loaded opening book from file " << options.book.book_path
                      << std::endl;
        }
    }
}

void globals::unloadTb() {
#ifdef SYZYGY_TBS
    if (tb_init_done()) {
        // Note: Syzygy code will free any existing memory if
        // initialized more than once. So no need to do anything here.
        tb_init = false;
    }
#endif
}
