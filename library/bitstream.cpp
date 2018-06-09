//
// Created by rsbat on 6/1/18.
//

#include <cassert>
#include <limits>
#include "bitstream.h"

namespace huffman {

    ibitstream& ibitstream::operator>>(bool& val) {
        if (in_buffer == 0) {
            char tmp;
            input.read(&tmp, 1);
            buffer = tmp;
            in_buffer = BUFFER_SIZE;
        }

        val = buffer[in_buffer - 1];
        --in_buffer;

        return *this;
    }

    ibitstream& ibitstream::operator>>(uint32_t& val) {
        assert(in_buffer == 0); // unaligned read not supported

        val = 0;
        input.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
        return *this;
    }

    ibitstream& ibitstream::operator>>(uint64_t& val) {
        assert(in_buffer == 0); // unaligned write is not supported

        val = 0;
        input.read(reinterpret_cast<char*>(&val), sizeof(uint64_t));

        return *this;
    }

    ibitstream& ibitstream::operator>>(unsigned char& val) {
        assert(in_buffer == 0);// unaligned read not supported

        val = 0;
        input.read(reinterpret_cast<char*>(&val), 1);

        return *this;
    }

    ibitstream::operator bool() const {
        return static_cast<bool>(input);
    }

    obitstream& obitstream::operator<<(bool val) {
        buffer[BUFFER_SIZE - in_buffer - 1] = val;
        ++in_buffer;

        if (in_buffer == BUFFER_SIZE) {
            auto tmp = static_cast<char>(buffer.to_ulong());
            output.write(&tmp, 1);
            buffer.reset();
            in_buffer = 0;
        }
        return *this;
    }

    obitstream& obitstream::operator<<(uint32_t val) {
        assert(in_buffer == 0); // unaligned write is not supported

        output.write(reinterpret_cast<char*>(&val), sizeof(val));

        return *this;
    }

    obitstream& obitstream::operator<<(uint64_t val) {
        assert(in_buffer == 0); // unaligned write is not supported

        output.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));

        return *this;
    }

    obitstream& obitstream::operator<<(unsigned char val) {
        //assert(in_buffer == 0); // unaligned write is not supported

        if (in_buffer == 0) {
            output.write(reinterpret_cast<const char*>(&val), 1);
        } else {
            unsigned char mask = std::numeric_limits<unsigned char>::max();
            mask <<= in_buffer;
            buffer = static_cast<char>(buffer.to_ulong()) | ((val & mask) >> in_buffer);

            auto tmp = static_cast<char>(buffer.to_ulong());
            output.write(&tmp, 1);

            buffer = val << (CHAR_BIT - in_buffer);
        }

        return *this;
    }

    void obitstream::flush() {
        auto tmp = static_cast<char>(buffer.to_ulong());
        output.write(&tmp, 1);
        buffer.reset();
        in_buffer = 0;
        output.flush();
    }
}