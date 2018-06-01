//
// Created by rsbat on 5/31/18.
//

#include "huffman.h"
#include "bitstream.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <bitset>
#include <unordered_map>
#include <cassert>
#include <stack>

namespace huffman {

    typedef std::shared_ptr<tree> tree_ptr_t;

    tree_ptr_t build_tree(const std::vector<size_t> &counts, const std::vector<unsigned char> &chars) {
        std::vector<std::pair<size_t, tree_ptr_t>> arr;
        std::transform(counts.begin(), counts.end(), chars.begin(), std::back_inserter(arr),
                       [](size_t cnt, unsigned char ch) -> std::pair<size_t, tree_ptr_t> { return {cnt,
                                                                                           std::make_shared<tree>(
                                                                                                   ch)};
                       });
        std::sort(arr.begin(), arr.end(), std::greater<>());

        while (arr.size() < 2) {
            arr.emplace_back(0, std::make_shared<tree>());
        }

        // TODO optimize
        while (arr.size() > 1) {
            auto lhs = arr.back();
            arr.pop_back();
            auto rhs = arr.back();
            arr.pop_back();

            arr.emplace_back(lhs.first + rhs.first, std::make_shared<tree>(lhs.second, rhs.second, 0));
            std::sort(arr.begin(), arr.end(), std::greater<>());
        }

        return arr[0].second;
    }

    void output_dfs(const tree_ptr_t &v, std::vector<bool>& structure, std::vector<unsigned char>& chars,
                    std::unordered_map<unsigned char, std::vector<bool>>& mp, std::vector<bool>& cur) {
        if (v->left != nullptr) {
            structure.push_back(true);
            cur.push_back(false);
            output_dfs(v->left, structure, chars, mp, cur);

            structure.push_back(true);
            cur.back() = true;
            output_dfs(v->right, structure, chars, mp, cur);

            cur.pop_back();
        } else {
            mp[v->ch] = cur;
            chars.push_back(v->ch);
        }
        structure.push_back(false);
    }

    void write_vector(const std::vector<bool>& vec, std::ostream& output) {
        obitstream stream(output);

        for (bool bit : vec) {
            stream << bit;
        }
        stream.flush();
    }

    void write_encoded(const tree_ptr_t &tr, size_t count, std::istream &input, std::ostream &output) {
        std::bitset<8> buffer;
        size_t in_buffer = 0;
        std::unordered_map<unsigned char, std::vector<bool>> mp;
        std::vector<bool> cur;
        std::vector<bool> tree_structure;
        std::vector<unsigned char> alphabet;

        output_dfs(tr, tree_structure, alphabet, mp, cur);
        uint32_t sz = static_cast<uint32_t>(alphabet.size());

        assert(tree_structure.size() == 4 * alphabet.size() - 3);

        output.write(reinterpret_cast<const char*>(&sz), 4); // write alphabet size
        output.write(reinterpret_cast<const char*>(alphabet.data()), sz); // write alphabet in dfs order
        write_vector(tree_structure, output); // write tree structure

        output.write(reinterpret_cast<const char*>(&count), 4);

        for (size_t i = 0; i < count; i++) {
            unsigned char ch;
            input >> std::noskipws >> ch;
            size_t ptr = 0;
            while (ptr != mp[ch].size()) {
                buffer[8 - in_buffer - 1] = mp[ch][ptr];
                in_buffer++;

                if (in_buffer == 8) {
                    auto tmp = buffer.to_ulong();
                    output.write(reinterpret_cast<const char *>(&tmp), 1);
                    in_buffer = 0;
                    buffer.reset();
                }
                ptr++;
            }
        }

        if (in_buffer > 0) {
            auto tmp = buffer.to_ulong();
            output.write(reinterpret_cast<const char *>(&tmp), 1);
        }
    }

    tree read_tree(std::istream& input) {
        uint32_t sz;

        input.read(reinterpret_cast<char*>(&sz), 4);
        std::vector<unsigned char> alphabet(sz);

        input.read(reinterpret_cast<char*>(alphabet.data()), sz);

        std::stack<tree_ptr_t> stack;
        stack.push(std::make_shared<tree>());
        size_t ptr = 0;
        ibitstream stream(input);
        for (size_t i = 0; i < (4 * sz - 3 + 7) / 8 * 8; i++) {
            bool b;
            stream >> b;

            if (b) {
                stack.push(std::make_shared<tree>());
            } else {
                if (stack.top()->left == nullptr) {
                    stack.top()->ch = alphabet[ptr];
                    ptr++;
                }
                tree_ptr_t tmp = stack.top();
                stack.pop();

                if (stack.empty()) {
                    stack.push(tmp);
                } else {
                    if (stack.top()->left == nullptr) {
                        stack.top()->left = tmp;
                    } else {
                        stack.top()->right = tmp;
                    }
                }
            }
        }

        return *stack.top();
    }

    void read_encoded(std::istream &input, std::ostream &output) {
        tree_ptr_t tr = std::make_shared<tree>(read_tree(input));
        tree_ptr_t node = tr;

        size_t count = 0;
        input.read(reinterpret_cast<char*>(&count), 4);

        unsigned char ch;
        while (count > 0 && input >> std::noskipws >> ch) {
            for (size_t i = 0; i < 8 && count > 0; i++) {
                if ((ch >> (8 - i - 1)) & 1) {
                    node = node->right;
                } else {
                    node = node->left;
                }

                if (node->left == nullptr) {
                    output << node->ch;
                    node = tr;
                    count--;
                }
            }
        }
    }
}