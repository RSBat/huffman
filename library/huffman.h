//
// Created by rsbat on 5/31/18.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include <memory>
#include <utility>
#include <vector>
#include <ostream>
#include <istream>

namespace huffman {
    struct tree {
        std::shared_ptr<tree> left;
        std::shared_ptr<tree> right;
        unsigned char ch;

        tree() :left(nullptr), right(nullptr), ch(0) {};

        explicit tree(unsigned char ch) : left(nullptr), right(nullptr), ch(ch) {};

        tree(std::shared_ptr<tree> lhs, std::shared_ptr<tree> rhs, unsigned char ch)
                : left(std::move(lhs)), right(std::move(rhs)), ch(0) {};
    };

    std::shared_ptr<tree> build_tree(const std::vector<std::pair<size_t, unsigned char>>& counts);

    void write_encoded(const std::shared_ptr<tree>&, size_t count, std::istream& input, std::ostream& output);

    tree read_tree(std::istream& input);

    void read_encoded(std::istream& input, std::ostream& output);
}

#endif //HUFFMAN_HUFFMAN_H
