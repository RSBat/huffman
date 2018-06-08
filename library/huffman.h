//
// Created by rsbat on 5/31/18.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include "bitstream.h"

#include <memory>
#include <utility>
#include <vector>
#include <ostream>
#include <istream>
#include <unordered_map>

namespace huffman {
    struct tree {
        std::shared_ptr<tree> left;
        std::shared_ptr<tree> right;
        unsigned char ch;

        tree() :left(nullptr), right(nullptr), ch(0) {};

        explicit tree(unsigned char ch) : left(nullptr), right(nullptr), ch(ch) {};

        tree(std::shared_ptr<tree>& lhs, std::shared_ptr<tree>& rhs, unsigned char ch)
                : left(lhs), right(rhs), ch(0) {};
    };

    std::shared_ptr<tree> build_tree(const std::vector<std::pair<size_t, unsigned char>>& counts);

    std::unordered_map<unsigned char, std::vector<bool>> tree_to_map(const std::shared_ptr<tree>& tree);

    void write_symbols(std::istream& input, obitstream& output, uint64_t count, const std::shared_ptr<tree>& tree);

    void write_tree(obitstream& output, const std::shared_ptr<tree>& tree);

    void write_encoded(const std::shared_ptr<tree>&, size_t count, std::istream& input, std::ostream& output);

    void write_encoded(const std::shared_ptr<tree>&, size_t count, std::istream& input, obitstream& output);

    tree read_tree(ibitstream& input);

    void read_symbols(ibitstream& input, std::ostream& output, uint64_t count, const std::shared_ptr<tree>& tree);

    void read_encoded(std::istream& input, std::ostream& output);

    void read_encoded(ibitstream& input, std::ostream& output);
}

#endif //HUFFMAN_HUFFMAN_H
