//
// Created by rsbat on 6/1/18.
//

#ifndef HUFFMAN_BITSTREAM_H
#define HUFFMAN_BITSTREAM_H

#include <bitset>
#include <istream>

namespace huffman {

    class bitstream_base {
    public:
        bitstream_base() : state(true), buffer(0), in_buffer(0) {};
        explicit operator bool() const;
        bool fail() const;

    protected:
        static const size_t BUFFER_SIZE = 8;

        bool state;
        std::bitset<BUFFER_SIZE> buffer;
        size_t in_buffer;
    };

    class ibitstream : public bitstream_base {
    public:
        explicit ibitstream(std::istream& input) :input(input) {};
        ibitstream& operator>>(bool&);

    private:
        std::istream& input;
    };

    class obitstream : public bitstream_base {
    public:
        explicit obitstream(std::ostream& output) :output(output) {};
        obitstream& operator<<(bool);
        void flush();

    private:
        static const size_t BUFFER_SIZE = 8;

        std::ostream& output;
    };
}


#endif //HUFFMAN_BITSTREAM_H
