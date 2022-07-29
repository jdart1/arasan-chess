// Copyright 1997-2018, 2021 by Jon Dart. All Rights Reserved.
//
#include "tester.h"
#include "chessio.h"
#include "globals.h"
#include "notation.h"

#include <iomanip>

using namespace std::placeholders;

void Tester::do_test(SearchController *searcher, const std::string &test_file, const TestOptions &opts)
{
    int depth_limit = opts.depth_limit;
    int time_limit = opts.time_limit;
    SearchType type;
    if (depth_limit >= 0) {
        type = FixedDepth;
        time_limit = Constants::INFINITE_TIME;
    }
    else {
        type = FixedTime;
        depth_limit = Constants::MaxPly;
    }
    
    Options tmp(globals::options);
    globals::options.book.book_enabled = 0;
    globals::options.learning.position_learning = 0;

    globals::delayedInit();

    Board board;
    std::ifstream pos_file( test_file.c_str(), std::ios::in);
    if (!pos_file) {
        std::cout << "Failed to open EPD file." << std::endl;
        return;
    }
    TestTotals testTotals;

    std::string buf;
    while (!pos_file.eof()) {
        std::getline(pos_file,buf);
        if (!pos_file) {
            std::cout << "Error reading EPD file." << std::endl;
            return;
        }
        // Try to parse this line as an EPD command.
        std::stringstream stream(buf);
        std::string id, comment;
        EPDRecord epd_rec;
        if (!ChessIO::readEPDRecord(stream,board,epd_rec)) break;
        if (epd_rec.hasError()) {
            std::cerr << "error in EPD record ";
            if (id.length()>0) std::cerr << id;
            std::cerr << ": ";
            std::cerr << epd_rec.getError();
            std::cerr << std::endl;
        }
        else {
            TestStatus testStats;
            int illegal=0;
            id = "";
            for (unsigned i = 0; i < epd_rec.getSize(); i++) {
                std::string key, val;
                epd_rec.getData(i,key,val);
                if (key == "bm" || key == "am") {
                    Move m;
                    std::stringstream s(val);
                    while (!s.eof()) {
                        std::string moveStr;
                        // skips spaces
                        s >> moveStr;
                        if (s.bad() || s.fail() || !moveStr.length()) {
                            std::cerr << "error reading solution move " << val << std::endl;
                            break;
                        }
                        m = Notation::value(board,board.sideToMove(),Notation::InputFormat::SAN,moveStr);
                        if (IsNull(m)) {
                            ++illegal;
                        } else {
                            testStats.solution_moves.push_back(m);
                        }
                        testStats.avoid = (key == "am");
                    }
                }
                else if (key == "id") {
                    id = val;
                }
                else if (key == "c0") {
                    comment = val;
                }
            }
            if (illegal) {
                std::cerr << "illegal or invalid solution move(s) for EPD record ";
                if (id.length()>0) std::cerr << id;
                std::cerr << std::endl;
                continue;
            }
            else if (testStats.solution_moves.size() == 0) {
                std::cerr << "no solution move(s) for EPD record ";
                if (id.length()>0) std::cerr << id;
                std::cerr << std::endl;
                continue;
            }
            std::cout << id << ' ';
            if (comment.length()) std::cout << comment << ' ';
            if (testStats.avoid) {
                std::cout << "am ";
            }
            else {
                std::cout << "bm";
            }
            for (Move m : testStats.solution_moves) {
                std::cout << ' ';
                Notation::image(board,m,Notation::OutputFormat::SAN,std::cout);
            }
            std::cout << std::endl;
            MoveSet excludes;
            testTotals.total_tests++;
            Statistics stats;
            auto old_post = searcher->registerPostFunction(
                std::bind(&Tester::post_test,this,_1,searcher,std::cref(opts),std::ref(testStats)));
            auto old_monitor = searcher->registerMonitorFunction(
                std::bind(&Tester::monitor,this,_1,_2,std::cref(opts),std::ref(testStats)));
            for (int index = 0; index < opts.moves_to_search; index++) {
                searcher->clearHashTables();
                stats.clear();

                MoveSet includes;
                Move result = searcher->findBestMove(board,
                                  type,
                                  time_limit, 0, depth_limit,
                                  0, 0, stats,
                                  opts.verbose ? TalkLevel::Test : TalkLevel::Silent,
                                  excludes, includes);
                // ensure stats and search history are updated with latest results
                post_test(stats, searcher, opts, testStats);
                if (excludes.size())
                    std::cout << "result(" << excludes.size()+1 << "):";
                else
                    std::cout << "result:";
                std::cout << '\t';
                Notation::image(board,result,Notation::OutputFormat::SAN,std::cout);
                std::cout << "\tscore: ";
                Scoring::printScore(stats.display_value,std::cout);
                std::cout <<  '\t';
                globals::gameMoves->removeAll();

                if (IsNull(result)) break;
                testTotals.total_time += searcher->getElapsedTime();
                testTotals.total_nodes += stats.num_nodes;
                excludes.insert(result);
                bool correct = testStats.solution_time >=0 &&
                    solution_match(testStats.solution_moves,
                                   result,testStats.avoid);
                if (index == 0) {
                    // only put solutions in summary at end if they are
                    // made on the first search attempt.
                    testTotals.solution_times.push_back((int)testStats.solution_time);
                    if (correct) {
                        testTotals.total_correct++;
                    }
                }
                std::ios_base::fmtflags original_flags = std::cout.flags();
                std::cout << std::setprecision(4);
                if (correct) {
                    std::cout << "\t++ solved in " << (float)testStats.solution_time/1000.0 <<
                        " sec. (";
                    print_nodes(testStats.solution_nodes,std::cout);
                }
                else {
                    std::cout << "\t** not solved in " <<
                        (float)searcher->getElapsedTime()/1000.0 << " secs. (";
                    print_nodes(stats.num_nodes,std::cout);
                }
                std::cout << " nodes)" << std::endl;
                std::cout.flags(original_flags);
                std::cout << stats.best_line_image << std::endl;
                const auto &sp = testStats.search_progress;
                if (index == 0 && correct) {
                    auto it = std::find_if(sp.rbegin(),sp.rend(),
                                           [this,&testStats](const TestStatus::SearchProgress &s) -> bool {
                                               return solution_match(testStats.solution_moves,
                                                                     s.move,
                                                                     testStats.avoid);});
                    if (it != sp.rend()) {
                        testTotals.time_to_find_total += it->time;
                        testTotals.depth_to_find_total += it->depth;
                        testTotals.nodes_to_find_total += it->num_nodes;
                    }
                }
            }
            searcher->registerPostFunction(old_post);
            searcher->registerMonitorFunction(old_monitor);
        }

        int c;
        while (!pos_file.eof()) {
            c = pos_file.get();
            if (!isspace(c) && c != '\n') {
                if (!pos_file.eof()) {
                    pos_file.putback(c);
                }
                break;
            }
        }
    }
    pos_file.close();
    std::cout << std::endl << "solution times:" << std::endl;
    std::cout << "         ";
    unsigned i = 0;
    for (i = 0; i < 10; i++)
        std::cout << i << "      ";
    std::cout << std::endl;
    double score = 0.0;
    for (i = 0; i < testTotals.solution_times.size(); i++) {
        char digits[15];
        if (i == 0) {
            sprintf(digits,"% 4d |       ",i);
            std::cout << std::endl << digits;
        }
        else if ((i+1) % 10 == 0) {
            sprintf(digits,"% 4d |",(i+1)/10);
            std::cout << std::endl << digits;
        }
        if (testTotals.solution_times[i] == -1) {
            std::cout << "  ***  ";
        }
        else {
            sprintf(digits,"%6.2f ",testTotals.solution_times[i]/1000.0);
            std::cout << digits;
            score += (float)time_limit/1000.0 - testTotals.solution_times[i]/1000.0;
        }
    }
    std::cout << std::endl << std::endl << "correct : " << testTotals.total_correct << '/' <<
        testTotals.total_tests << std::endl;
    if (testTotals.total_correct) {
        std::string avg = "";
        if (testTotals.total_correct > 1) avg = "avg. ";
        std::cout << avg << "nodes to solution : ";
        uint64_t avg_nodes = testTotals.nodes_to_find_total/testTotals.total_correct;
        if (avg_nodes > 1000000L) {
            std::cout << (float)(avg_nodes)/1000000.0 << "M" << std::endl;
        }
        else {
            std::cout << (float)(avg_nodes)/1000.0 << "K" << std::endl;
        }
        std::cout << avg << "depth to solution : " << (float)(testTotals.depth_to_find_total)/testTotals.total_correct << std::endl;
        std::cout << avg << "time to solution  : " << (float)(testTotals.time_to_find_total)/(1000.0*testTotals.total_correct) << " sec." << std::endl;
    }
    globals::options = tmp;
}

bool Tester::solution_match(const std::vector<Move> &solution_moves,
                            Move result, bool avoid) const noexcept {
    bool match = false;
    for (Move m : solution_moves) {
        if (MovesEqual(m,result)) {
            match = true;
            break;
        }
    }
    return avoid ? !match : match;
}

// This function is called with "post," i.e. intermediate search, results during a test
// suite run.
void Tester::post_test(const Statistics &stats,
                       SearchController *searcher,
                       const TestOptions &opts,
                       TestStatus &testStats)
{
    Move best = stats.best_line[0];
    auto &sp = testStats.search_progress;
    if (sp.empty() ||
        !MovesEqual(sp.back().move,best)) {
        sp.push_back(TestStatus::SearchProgress(best,
                                                stats.value,
                                                searcher->getElapsedTime(),
                                                stats.depth,
                                                stats.num_nodes));
    }
    if (solution_match(testStats.solution_moves,best,testStats.avoid)) {
        if ((int)stats.depth > testStats.last_iteration_depth) {
            // Wait 2 sec before counting iterations correct, unless
            // we found a mate
            if (searcher->getElapsedTime() >= 200 ||
                stats.value > Constants::MATE_RANGE) {
                ++testStats.iterations_correct;
            }
            testStats.last_iteration_depth = stats.depth;
        }
        if (testStats.iterations_correct >= opts.early_exit_plies) {
            testStats.early_exit = 1;
        }
        if (testStats.solution_time == -1) {
            testStats.solution_time = searcher->getElapsedTime();
            testStats.solution_nodes = stats.num_nodes;
        }
    }
    else {
        testStats.solution_time = -1;   // not solved yet, or has moved off
        // solution
        testStats.iterations_correct = 0;
    }
}

int Tester::monitor(SearchController * /*s*/,
                    const Statistics & /*stats*/,
                    const TestOptions & /*opts*/,
                    TestStatus &testStats) {
    return testStats.early_exit;
}

void Tester::print_nodes(uint64_t nodes, std::ostream &out) {
    if (nodes >= 1000000) {
        out << (float)(nodes)/1000000.0 << "M";
    }
    else if (nodes >= 1000) {
        out << (float)(nodes)/1000.0 << "K";
    }
    else
        out << nodes;
}




