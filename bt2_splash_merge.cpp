#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

void usage(int argc, char** argv) {
    std::cerr << "Usage: <bowtie2_file> <splash_file> <output_file>\n";
}

struct Params {
    std::string bowtie2_file;
    std::string splash_file;
    std::string output_file;
};

Params parse_params(int argc, char** argv) {
    if (argc < 4) {
        usage(argc, argv);
        exit(1);
    }

    Params params;
    params.bowtie2_file = argv[1];
    params.splash_file = argv[2];
    params.output_file = argv[3];

    return params;
}

std::vector<std::string> split_line(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> res;
    std::string tmp;
    while(iss >> tmp)
        res.push_back(tmp);
    return res;
}

void get_anchor_and_rest_of_line(const std::string& line, std::string& anchor, std::string& rest) {
    auto x = line.find_first_of('\t');
    if (x == std::string::npos) {
        std::cerr << "Error: cannot split line by tab: " << line << '\n';
        exit(1);
    }
    rest = line.substr(x + 1);
    anchor = line.substr(0, x);
}

int main(int argc, char** argv) {
    auto params = parse_params(argc, argv);
    std::ifstream in_bt2(params.bowtie2_file);
    if (!in_bt2) {
        std::cerr << "Error: cannot open file " << params.bowtie2_file << "\n";
        exit(1);
    }

    std::ifstream in_splash(params.splash_file);
    if (!in_splash) {
        std::cerr << "Error: cannot open file " << params.splash_file << "\n";
        exit(1);
    }

    std::ofstream out(params.output_file);
    if (!out) {
        std::cerr << "Error: cannot open file " << params.output_file << "\n";
        exit(1);
    }

    std::string line_bt2, line_splash;
    std::getline(in_bt2, line_bt2);
    std::getline(in_splash, line_splash);

    std::string anchor_bt2, rest_bt2;
    get_anchor_and_rest_of_line(line_bt2, anchor_bt2, rest_bt2);

    std::string anchor_splash, rest_splash;
    get_anchor_and_rest_of_line(line_splash, anchor_splash, rest_splash);


    out << anchor_bt2 << "\t" << rest_bt2 << "\t" << rest_splash << "\n";

    std::string prev_anch_bt2, prev_anch_splash;

    {
        if(!std::getline(in_bt2, line_bt2))
            return 0;
        if(!std::getline(in_splash, line_splash))
            return 0;

        get_anchor_and_rest_of_line(line_bt2, anchor_bt2, rest_bt2);
        get_anchor_and_rest_of_line(line_splash, anchor_splash, rest_splash);
        
        while(true) {
            if (prev_anch_bt2 != "" && prev_anch_bt2 >= anchor_bt2) {
                std::cerr << "Error: anchors in bt2 are not sorted\n";
                exit(1);
            }
            if (prev_anch_splash != "" && prev_anch_splash >= anchor_splash) {
                std::cerr << "Error: anchors in splash are not sorted\n";
                exit(1);
            }

            if (anchor_bt2 == anchor_splash) {
                out << anchor_bt2 << "\t" << rest_bt2 << "\t" << rest_splash << "\n";

                 if (!std::getline(in_bt2, line_bt2))
                    return 0;
                prev_anch_bt2 = anchor_bt2;
                get_anchor_and_rest_of_line(line_bt2, anchor_bt2, rest_bt2);

                 if(!std::getline(in_splash, line_splash))
                    return 0;
                prev_anch_splash = anchor_splash;
                get_anchor_and_rest_of_line(line_splash, anchor_splash, rest_splash);
                
            } else if (anchor_bt2 < anchor_splash) {
                 if (!std::getline(in_bt2, line_bt2))
                    return 0;
                prev_anch_bt2 = anchor_bt2;
                get_anchor_and_rest_of_line(line_bt2, anchor_bt2, rest_bt2);
            } else {
                if(!std::getline(in_splash, line_splash))
                    return 0;
                prev_anch_splash = anchor_splash;
                get_anchor_and_rest_of_line(line_splash, anchor_splash, rest_splash);
            }
        }
    }   
}
