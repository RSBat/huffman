//
// Created by rsbat on 6/1/18.
//

#include "huffman.h"

#include <iostream>
#include <fstream>
#include <algorithm>

int main() {

    std::ifstream input("output.txt", std::ios::binary);
    std::ofstream output("decoded.txt", std::ios::binary);

    huffman::read_encoded(input, output);
    return 0;
}