//
// Created by rsbat on 6/1/18.
//

#include <cassert>
#include "bitstream.h"

namespace huffman {

    bitstream_base::operator bool() const {
        return !fail();
    }

    bool bitstream_base::fail() const {
        return !state;
    }

    ibitstream& ibitstream::operator>>(bool& val) {
        if (in_buffer == 0) {
            uint8_t tmp;
            input.read(reinterpret_cast<char*>(&tmp), 1);
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
        assert(in_buffer == 0); // unaligned write is not supported

        output.write(reinterpret_cast<const char*>(&val), 1);

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