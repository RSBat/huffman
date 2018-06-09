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
#include <map>

namespace huffman {
    struct tree_node {
        std::shared_ptr<tree_node> left;
        std::shared_ptr<tree_node> right;
        unsigned char ch;

        tree_node() :left(nullptr), right(nullptr), ch(0) {};

        explicit tree_node(unsigned char ch) : left(nullptr), right(nullptr), ch(ch) {};

        tree_node(std::shared_ptr<tree_node>& lhs, std::shared_ptr<tree_node>& rhs, unsigned char ch)
                : left(lhs), right(rhs), ch(0) {};
    };

    struct code_sequence {
        std::vector<unsigned char> seq;
        std::bitset<CHAR_BIT> last;
        size_t in_last;

        code_sequence() : seq(), last(0), in_last(0) {};

        explicit code_sequence(std::vector<bool>& vec) : seq(), last(0), in_last(0) {
            for (auto&& word : vec) {
                last[CHAR_BIT - 1 - in_last] = word;
                ++in_last;
                if (in_last == CHAR_BIT) {
                    seq.push_back(static_cast<unsigned char>(last.to_ulong()));
                    in_last = 0;
                    last.reset();
                }
            }
        }
    };

    struct corrupted_tree {};
    struct corrupted_data {};

    std::shared_ptr<tree_node> build_tree(const std::vector<std::pair<size_t, unsigned char>>& counts);

    std::unordered_map<unsigned char, code_sequence> tree_to_map(const std::shared_ptr<tree_node>& tree);

    void write_symbols(std::istream& input, obitstream& output, uint64_t count, const std::shared_ptr<tree_node>& tree);

    void write_tree(obitstream& output, const std::shared_ptr<tree_node>& tree);

    void write_encoded(const std::shared_ptr<tree_node>&, size_t count, std::istream& input, std::ostream& output);

    void write_encoded(const std::shared_ptr<tree_node>&, size_t count, std::istream& input, obitstream& output);

    tree_node read_tree(ibitstream& input);

    void read_symbols(ibitstream& input, std::ostream& output, uint64_t count, const std::shared_ptr<tree_node>& tree);

    void read_encoded(std::istream& input, std::ostream& output);

    void read_encoded(ibitstream& input, std::ostream& output);
}

#endif //HUFFMAN_HUFFMAN_H
