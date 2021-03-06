//
// Created by rsbat on 5/31/18.
//

#include "bitstream.h"
#include "huffman.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

namespace huffman {

    typedef std::unique_ptr<tree_node> tree_ptr_t;
    typedef std::unordered_map<unsigned char, code_sequence> alphabet_map;

    tree_ptr_t build_tree(const std::vector<std::pair<size_t, unsigned char>> &counts) {
        std::vector<std::pair<size_t, tree_ptr_t>> arr;
        std::transform(counts.begin(), counts.end(), std::back_inserter(arr),
                       [](std::pair<size_t, unsigned char> p) -> std::pair<size_t, tree_ptr_t> {
                           return {p.first, std::make_unique<tree_node>(p.second)};
                       });

        while (arr.size() < 2) {
            arr.emplace_back(0, std::make_unique<tree_node>());
        }

        std::sort(arr.begin(), arr.end());
        std::vector<std::pair<size_t, tree_ptr_t>> aux;

        size_t i = 0;
        size_t j = 0;

        while (arr.size() + aux.size() > 1 + i + j) {
            std::pair<size_t, tree_ptr_t> lhs;
            std::pair<size_t, tree_ptr_t> rhs;

            if (i == arr.size()) {
                lhs = std::move(aux[j]);
                j++;
            } else if (j == aux.size()) {
                lhs = std::move(arr[i]);
                i++;
            } else if (arr[i].first < aux[j].first) {
                lhs = std::move(arr[i]);
                i++;
            } else {
                lhs = std::move(aux[j]);
                j++;
            }

            if (i == arr.size()) {
                rhs = std::move(aux[j]);
                j++;
            } else if (j == aux.size()) {
                rhs = std::move(arr[i]);
                i++;
            } else if (arr[i].first < aux[j].first) {
                rhs = std::move(arr[i]);
                i++;
            } else {
                rhs = std::move(aux[j]);
                j++;
            }


            aux.emplace_back(lhs.first + rhs.first, std::make_unique<tree_node>(lhs.second, rhs.second, 0));
        }

        return std::move(aux.back().second);
    }

    void tree_to_map_dfs(const tree_ptr_t& v, alphabet_map& mp, std::vector<bool>& cur) {
        if (v->left != nullptr) {
            cur.push_back(false);
            tree_to_map_dfs(v->left, mp, cur);

            cur.back() = true;
            tree_to_map_dfs(v->right, mp, cur);

            cur.pop_back();
        } else {
            mp[v->ch] = code_sequence(cur);
        }
    }

    alphabet_map tree_to_map(const tree_ptr_t& v) {
        alphabet_map mp;
        std::vector<bool> cur;
        tree_to_map_dfs(v, mp, cur);
        return mp;
    }

    // encodes and writes count symbols from input to output
    void write_symbols(std::istream& input, obitstream& output, uint64_t count, const tree_ptr_t& tree) {
        alphabet_map mp = tree_to_map(tree);

        for (size_t i = 0; i < count; i++) {
            unsigned char ch;
            input >> std::noskipws >> ch;

            for (auto byte : mp[ch].seq) {
                output << byte;
            }
            for (size_t j = 0; j < mp[ch].in_last; j++) {
                output << mp[ch].last[CHAR_BIT - 1 - j];
            }
        }

        output.flush();
    }

    void output_dfs(const tree_ptr_t& v, std::vector<bool>& structure, std::vector<unsigned char>& chars, std::vector<bool>& cur) {
        if (v->left != nullptr) {
            structure.push_back(true);
            cur.push_back(false);
            output_dfs(v->left, structure, chars, cur);

            structure.push_back(true);
            cur.back() = true;
            output_dfs(v->right, structure, chars, cur);

            cur.pop_back();
        } else {
            chars.push_back(v->ch);
        }
        structure.push_back(false);
    }

    void write_tree(obitstream& output, const tree_ptr_t& tree) {
        std::vector<bool> cur;
        std::vector<bool> tree_structure;
        std::vector<unsigned char> alphabet;

        output_dfs(tree, tree_structure, alphabet, cur);
        auto sz = static_cast<uint32_t>(alphabet.size());

        assert(tree_structure.size() == 4 * alphabet.size() - 3);

        // write alphabet size and alphabet in dfs order
        output << sz;
        for (auto ch : alphabet) {
            output << ch;
        }

        obitstream stream(output);
        for (auto bit: tree_structure) {
            stream << bit;
        }
        stream.flush();
    }

    void write_encoded(const tree_ptr_t &tree, uint64_t count, std::istream &input, obitstream &output) {
        write_tree(output, tree);

        output << count;

        write_symbols(input, output, count, tree);
    }

    void write_encoded(const tree_ptr_t &tree, uint64_t count, std::istream &input, std::ostream &output) {
        obitstream stream(output);
        write_encoded(tree, count, input, stream);
    }

    tree_ptr_t read_tree(ibitstream& input) {
        uint32_t sz;

        input >> sz;
        if (!input) {
            throw corrupted_tree();
        }

        std::vector<unsigned char> alphabet(sz);

        for (uint32_t i = 0; i < sz; i++) {
            input >> alphabet[i];
        }

        std::stack<tree_ptr_t> stack;
        stack.push(std::make_unique<tree_node>());
        size_t ptr = 0;

        size_t i = (4 * sz - 3 + 7) / 8 * 8;
        bool bit;
        while (i != 0 && input >> bit) {
            if (bit) {
                stack.push(std::make_unique<tree_node>());
            } else {
                if (stack.top()->left == nullptr) {
                    stack.top()->ch = alphabet[ptr];
                    ptr++;
                }
                tree_ptr_t tmp = std::move(stack.top());
                stack.pop();

                if (stack.empty()) {
                    stack.push(std::move(tmp));
                } else {
                    if (stack.top()->left == nullptr) {
                        stack.top()->left = std::move(tmp);
                    } else {
                        stack.top()->right = std::move(tmp);
                    }
                }
            }

            --i;
        }

        if (!input) {
            throw corrupted_tree();
        }

        return std::move(stack.top());
    }

    void read_symbols(ibitstream& input, std::ostream& output, uint64_t count, const tree_ptr_t& tree) {
        tree_node* node = tree.get();

        bool bit;
        while (count != 0) {
            input >> bit;
            if (bit) {
                node = node->right.get();
            } else {
                node = node->left.get();
            }

            if (node->left == nullptr) {
                output << node->ch;
                node = tree.get();
                count--;
            }
        }

        if (!input) {
            throw corrupted_data();
        }
    }

    void read_encoded(ibitstream& input, std::ostream& output) {
        tree_ptr_t tr = read_tree(input);

        uint64_t count = 0;
        input >> count;

        if (!input) {
            throw corrupted_data();
        }

        read_symbols(input, output, count, tr);
    }

    void read_encoded(std::istream& input, std::ostream& output) {
        ibitstream stream(input);

        read_encoded(stream, output);
    }
}