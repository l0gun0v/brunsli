// Copyright (c) Google LLC 2019
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <cstdio>
#include <cstdlib>
#include <string>
#include <brunsli/decoder.h>

inline void error_usage(){
    fprintf(stderr, "Usage: dbrunsli FILE [OUTPUT_FILE, default=FILE.jpg] --bench\n");
    exit(EXIT_FAILURE);
}

inline void error_in_empty(){
    fprintf(stderr, "Empty input file name.\n");
    exit(EXIT_FAILURE);
}

inline void error_out_empty(){
    fprintf(stderr, "Empty output file name.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 5) {
        error_usage();
    }
    bool bench = false;
    int in_file_name_argv = 1;
    int out_file_name_argv = 2;
    if(std::string(argv[1]) == "--bench"){
        if(argc == 2){
           error_usage();
        }
        in_file_name_argv++;
        out_file_name_argv++;
        bench = true;
    } else if(argc >= 3 && std::string(argv[2]) == "--bench") {
        bench = true;
        out_file_name_argv++;
    } else if(argc >= 4 && std::string(argv[3]) == "--bench") {
        bench = true;
    }
    std::string file_name = std::string(argv[in_file_name_argv]);
    if (file_name.empty()) {
        error_in_empty();
    }
    std::string outfile_name = file_name + ".jpg";
    if(out_file_name_argv < argc){
        outfile_name = std::string(argv[out_file_name_argv]);
        if (outfile_name.empty()) {
            error_out_empty();
        }
    }
    bool ok = BrunsliDecoder(DefaultDecoder(), bench).decode(file_name, outfile_name);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
