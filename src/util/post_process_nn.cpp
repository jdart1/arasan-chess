// Copyright 2024, 2025 by Jon Dart. All Rights Reserved.
#include <fstream>
#include "util.h"

// Adds the network version to a file
// Usage: post_process_nn in_file out_file

#include "nnue.h"

int main(int argc, char **argv)
{
    std::string in_name, out_name;
    if (argc < 3) {
        std::cerr << "Expected filenames" << std::endl;
        return -1;
    }
    else {
        in_name = argv[1];
        out_name = argv[2];

        std::ifstream infile(in_name,std::ios::in | std::ios::binary);

        if (infile.fail()) {
            std::cerr << "failed to open input file";
            return -1;
        }

        std::ofstream outfile(out_name,std::ios::out | std::ios::binary);

        if (outfile.fail()) {
            std::cerr << "failed to open output file";
            return -1;
        }

        int32_t ver = to_little_endian32(nnue::NetworkParams::NN_VERSION);
        outfile.write(reinterpret_cast<char*>(&ver),4);

        char buf[32768];

        while (!infile.eof()) {
            infile.read(buf,32768);
            if (infile.bad()) {
                std::cerr << "read error" << std::endl;
                return -1;
            }
            outfile.write(buf,infile.gcount());
            if (outfile.fail()) {
                std::cerr << "write error" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}
