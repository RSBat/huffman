//
// Created by rsbat on 6/6/18.
//

#include "library/huffman.h"

#include <gtest/gtest.h>

#include <sstream>
#include <vector>
#include <algorithm>

std::vector<std::pair<size_t, unsigned char>> calc_count(const std::string& s) {
    std::vector<std::pair<size_t, unsigned char>> ans(256);
    for (size_t i = 0; i < 256; i++) {
        ans[i] = {0, static_cast<unsigned char>(i)};
    }

    for (auto ch : s) {
        auto uch = static_cast<unsigned char>(ch);
        ans[uch].first++;
    }

    ans.erase(std::remove_if(ans.begin(), ans.end(), [](std::pair<size_t, unsigned char> p) { return p.first == 0; }),
              ans.end());
    return ans;
}

std::string run_encode_decode(const std::string& s) {
    std::istringstream istream(s);
    std::ostringstream oencoded;

    std::shared_ptr<huffman::tree> tree = huffman::build_tree(calc_count(s));

    huffman::write_encoded(tree, s.size(), istream, oencoded);

    std::istringstream iencoded(oencoded.str());
    std::ostringstream decoded;

    huffman::read_encoded(iencoded, decoded);
    return decoded.str();
}

TEST(encode_decode, empty) {
    std::string test = "";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, a) {
    std::string test = "a";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, multiple_a) {
    std::string test = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, ab) {
    std::string test = "ab";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, alphabet) {
    std::string test = "abcdefghijklmnopqrstuvwxyz";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, capital_alphabet) {
    std::string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    ASSERT_EQ(test, run_encode_decode(test));
}

TEST(encode_decode, random) {
    for (size_t i = 0; i < 100; i++) {
        std::string test;
        for (size_t j = 0; j < 1000; j++) {
            test.push_back(static_cast<char>(rand() % 255));
        }

        ASSERT_EQ(test, run_encode_decode(test));
    }
}