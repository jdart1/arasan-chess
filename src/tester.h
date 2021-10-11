// Support for the "test" command (for running test suites).
// Copyright 1997-2018, 2021 by Jon Dart. All Rights Reserved.
//
#ifndef _TESTER_H
#define _TESTER_H

#include "search.h"

#include <vector>

class Tester 
{

public:
    Tester() = default;

    virtual ~Tester() = default;

    // Options for the test suite suite run
    struct TestOptions {
        int depth_limit;
        int time_limit;
        int early_exit_plies;
        int moves_to_search;
        bool verbose;
        
        TestOptions() :
            depth_limit(-1),
            time_limit(Constants::INFINITE_TIME),
            early_exit_plies(Constants::MaxPly),
            moves_to_search(1),
            verbose(false) {
        }
    };

    // execute the "test" command
    void do_test(SearchController *searcher, const std::string &test_file, const TestOptions &opts);

private:
    // Status and configuration for one EPD test
    struct TestStatus
    {
        int early_exit;
        time_t solution_time;
        uint64_t solution_nodes;
        int last_iteration_depth;
        std::vector<Move> solution_moves;
        bool avoid;
        int iterations_correct;

        struct SearchProgress
        {
            SearchProgress(Move m,score_t val,
                           time_t t,
                           int ply,
                           uint64_t nodes) :
                move(m),value(val),time(t),
                depth(ply),num_nodes(nodes)
                {
                }
            Move move;
            score_t value;
            time_t time;
            int depth;
            uint64_t num_nodes;
        };

        std::vector<SearchProgress> search_progress;

        TestStatus():
            early_exit(0),
            solution_time(-1), // indicates "not solved"
            solution_nodes(0ULL),
            last_iteration_depth(-1),
            avoid(false),
            iterations_correct(0)
            {
            }
    };

    // Running totals across multiple EPDs
    struct TestTotals {
        uint64_t total_nodes;
        int total_correct;
        int total_tests;
        uint64_t total_time;
        std::vector<int> solution_times;
        uint64_t nodes_to_find_total;
        int depth_to_find_total;
        uint64_t time_to_find_total;

        TestTotals() :
            total_nodes(0ULL),
            total_correct(0ULL),
            total_tests(0),
            total_time(0ULL),
            nodes_to_find_total(0),
            depth_to_find_total(0),
            time_to_find_total(0ULL)
            {
            }
    };

    bool solution_match(const std::vector<Move> &solution_moves,
                        Move result, bool avoid) const noexcept;

    // "post" function, called from search
    void post_test(const Statistics &stats, SearchController *searcher,
                   const TestOptions &opts,
                   TestStatus &testStats);

    // monitor function, called from search
    int monitor(SearchController *s, const Statistics &stats, const TestOptions &opts,
                TestStatus &testStats);

    void print_nodes(uint64_t nodes, std::ostream &out);

};

#endif
