#include "huffman.h"

#include <iostream>
#include <fstream>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Usage: huffman [-d] input_file [output_file]" << std::endl;
        return 0;
    }

    bool decode = false;
    if (std::string(argv[1]) == "-d") {
        decode = true;
    }

    std::ifstream input(argv[1 + decode], std::ios::binary);
    std::ofstream output(argv[2 + decode], std::ios::binary);

    if (decode) {
        huffman::read_encoded(input, output);
    } else {
        std::vector<size_t> counts(256);
        unsigned char ch;
        size_t cnt = 0;
        while (input >> std::noskipws >> ch) {
            counts[ch]++;
            cnt++;
        }

        std::vector<unsigned char> chars;
        for (unsigned char c = 0; c != 255; c++) {
            if (counts[c] != 0) { chars.push_back(c); }
        }
        if (counts[255]) { chars.push_back(static_cast<unsigned char &&>(255)); }

        counts.erase(std::remove(counts.begin(), counts.end(), 0), counts.end());

        std::shared_ptr<huffman::tree> tree = huffman::build_tree(counts, chars);

        input.clear();
        input.seekg(0);

        huffman::write_encoded(tree, cnt, input, output);
    }
    return 0;
}