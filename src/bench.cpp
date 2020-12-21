// Copyright 2020 by Jon Dart. All Rights Reserved
#include "bench.h"
#include "globals.h"
#include "notation.h"
#include "search.h"
#include "chessio.h"

#include <array>
#include <sstream>

static const std::array<std::string,25> epds = {
    "r2q1rk1/pb1nbp1p/1pp1pp2/8/2BPN2P/5N2/PPP1QPP1/2KR3R w - - bm Nfg5; id \"arasan20.112\"; c0 \"Loop M1-P 4CPU-Deep Junior 10.1 4CPU, CEGT Quad 40/120 2007\";",
    "r1b1k2r/ppqn1ppp/2pbpn2/4N3/2BP4/2N5/PPP2PPP/R1BQR1K1 w kq - bm Nxf7; id \"arasan18.190\"; c0 \"Houdini 2.0c Pro x64 1CPU-Naum 4.2 1CPU, 2012 (Brent M)\";",
    "r4rk1/1b1n1pb1/3p2p1/1p1Pq1Bp/2p1P3/2P2RNP/1PBQ2P1/5R1K w - - bm Nf5; id \"arasan20.14\"; c0 \"Tal-Spassky, Tilburg 1980\";",
    "3rkb1r/1p3p2/p1n1p3/q5pp/2PpP3/1P4P1/P1Q1BPKP/R2N3R b k - bm d3; id \"ECM.986\";",
    "4rrk1/1bp2ppp/p1q2b1B/1pn2B2/4N1Q1/2P4P/PP3PP1/3RR1K1 w - - bm Nxc5; id \"ECM.1016\";",
    "5rk1/1pp3p1/3ppr1p/pP2p2n/4P2q/P2PQ2P/2P1NPP1/R4RK1 b - - bm Rf3; id \"arasan20.13\"; c0 \"Arasan-Crafty, test game 2012\";",
    "r1b1k2r/p1pq1npp/Ppnp1p2/8/2N1PP2/4B3/B3Q1PP/1R3RK1 w kq - bm e5; c0 \"L702X-CrazyHorse, Tuesday 09 Feb Rapid Engine Tour 15+5, Infinity Chess 2016\";",
    "2r2r2/p2qppkp/3p2p1/3P1P2/2n2R2/7R/P5PP/1B1Q2K1 w - - bm Rxh7+; id \"ECM.1600\";",
    "b3r1k1/2rN1ppp/2n1p3/p7/P3BP2/1R6/q1P2QPP/3R2K1 w - - bm Bxh7+; id \"ECM.1606\";",
    "r4rk1/p4ppp/qp2p3/b5B1/n1R5/5N2/PP2QPPP/1R4K1 w - - bm Bf6; id \"arasan20.38\"; c0 \"Alekhine-Sterk, Budapest 1921\";",
    "r2qrb1k/1p1b2p1/p2ppn1p/8/3NP3/1BN5/PPP3QP/1K3RR1 w - - bm e5; id \"arasan20.147\"; c0 \"Spassky-Petrosian, World Chmp (19) 1969\";",
    "1r2rbk1/1p1n1p2/p3b1p1/q2NpNPp/4P2Q/1P5R/6BP/5R1K w - h6 bm Ng3; id \"arasan20.181\"; c0 \"Carlsen-Anand, Corus Wijk aan Zee 2008\";",
    "2b3r1/2p3Np/k1p5/p1bn4/PpN2p2/1P6/1BP2PPP/4R1K1 b - - bm f3; c0 \"Pedone 1.7-Arasan 20.4.1, Gsei Web Tournament semi-finals (6) 2018\";",
    "2n3r1/p2r1kbp/1p2p1p1/1PPp1p1P/1P1P1PP1/3BBK2/7R/7R b - - bm Rc7; c0 \"Rubinstein-Landau, Rotterdam Four Masters 1931\";",
    "3R4/pp2r1pk/q1p3bp/2P2r2/PP6/2Q3P1/6BP/5RK1 w - - bm Rxf5; id \"arasan20.153\"; c0 \"Zhak-Pavlov, ch-RUS sf ICCF 2010\";",
    "7R/r1p1q1pp/3k4/1p1n1Q2/3N4/8/1PP2PPP/2B3K1 w - - bm Rd8+; c0 \"Keres-Pomar Salamanca, Madrid 1943\";",
    "6r1/2p2p1k/p5q1/1p2P1p1/6RP/5Q2/PP4P1/6K1 w - - bm Qh3; c0 \"Karpov-Beliavsky, October Revolution 60 years, Leningrad 1977\";",
    "8/4k3/p2p2p1/P1pPn2p/1pP1P2P/1P1NK1P1/8/8 w - - bm g4; id \"MGV12-E: Razuvaev - Ostojic Berlin 1988\";",
    "8/1p3p2/p1krpp1p/P6P/2P2PP1/8/1PK5/4R3 w - - bm Rd1; c0 \"AH Endgames test, #177\";",
    "r4k2/q4npp/P2P4/2p1R3/2N5/6PP/Q5K1/8 w - - bm Re7; id \"ECM.1028\";",
    "6k1/6pp/p3R3/2p5/Pr6/1P4P1/1P2KP2/8 w - - bm a5; c0 \"Svidler-Navara, EU-chT (Men), Heraklion 2007\";",
    "8/2k5/2PrR1p1/7p/5p1P/5P1K/6P1/8 w - - bm Rxd6; id \"arasan20.187\"; c0 \"Topalov-Short, Madrid Magistral 1997\";",
    "8/7p/p1k5/1p6/1P4P1/8/5K1P/8 b - - bm Kb6; c0 \"Shirov-Morozevich, Petrov mem. Jurmala 2012\";",
    "2k5/3b4/PP3K2/7p/4P3/1P6/8/8 w - - bm Ke7; c0 \"E_E_T 006 - B vs L\";",
    "8/8/4b3/4k3/7P/3R1K2/3pr3/8 b - - bm Bg4+; c0 \"Kartunen-Topalov, ECC Open 2015\";"
};

Bench::Results Bench::bench(int hashSize, int depth, int cores, bool verbose)
{
    auto tmp_hash = options.search.hash_table_size;
    int tmp_cores = options.search.ncpus;
    options.search.hash_table_size = hashSize;
    options.search.ncpus = std::min<int>(cores,Constants::MaxCPUs);

    SearchController *searcher = new SearchController();
    searcher->updateSearchOptions();
    searcher->setThreadCount(options.search.ncpus);

    Results results;
    for (string s : epds) {
        benchLine(searcher, s, results, depth, verbose);
    }
    delete searcher;
    options.search.hash_table_size = tmp_hash;
    options.search.ncpus = tmp_cores;
    return results;
}

void Bench::benchLine(SearchController *searcher, const std::string &epd, Bench::Results &results, int depthLimit, bool verbose)
{
    std::stringstream stream(epd);
    string id, comment;
    EPDRecord epd_rec;
    Board board;
    if (!ChessIO::readEPDRecord(stream,board,epd_rec)) return;
    epd_rec.getVal("id",id);
    epd_rec.getVal("c0",comment);
    if (epd_rec.hasError()) {
        cerr << "error in EPD record ";
        if (id.length()>0) cerr << id;
        cerr << ": ";
        cerr << epd_rec.getError();
        cerr << endl;
    }
    else {
        if (verbose) {
            if (id.length()>0) {
                cout << id << endl;;
            }
            else if (comment.length()>0) {
                cout << comment << endl;
            }
        }

        Statistics stats;
        MoveSet includes, excludes;
        Move result = searcher->findBestMove(board,
                                             FixedDepth,
                                             999999, 0, depthLimit,
                                             0, 0, stats,
                                             verbose ? TalkLevel::Test : TalkLevel::Silent,
                                             excludes, includes);
        if (verbose) {
            Notation::image(board,result,Notation::OutputFormat::SAN,cout);
            cout << " Nodes: ";
            cout << stats.num_nodes;
            cout << " Time: ";
            cout << searcher->getElapsedTime() << endl;
        }
        results.nodes += stats.num_nodes;
        results.time += searcher->getElapsedTime();
    }
}

std::ostream & operator << (std::ostream &o, const Bench::Results &results)
{
    o << "Time\t: " << results.time << endl;
    o << "Nodes\t: " << results.nodes << endl;
    o << "NPS\t: "  << 1000*results.nodes/results.time << endl;
    return o;
}
