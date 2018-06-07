//
// Created by rsbat on 6/6/18.
//

#include "library/huffman.h"

#include <gtest/gtest.h>

#include <sstream>

std::vector

TEST(encode_decode, empty) {
    std::istringstream istream("");
    std::ostringstream oencoded;

    std::shared_ptr<huffman::tree> tree = huffman::build_tree(std::vector<size_t>(), std::vector<unsigned char>());

    huffman::write_encoded(tree, 0, istream, oencoded);

    std::istringstream iencoded(oencoded.str());
    std::ostringstream decoded;

    huffman::read_encoded(iencoded, oencoded);

    ASSERT_EQ("", decoded.str());
}