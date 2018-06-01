//
// Created by rsbat on 6/1/18.
//

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

    obitstream &obitstream::operator<<(bool val) {
        buffer[BUFFER_SIZE - in_buffer - 1] = val;
        ++in_buffer;

        if (in_buffer == BUFFER_SIZE) {
            uint8_t tmp = static_cast<uint8_t>(buffer.to_ulong());
            output.write(reinterpret_cast<char*>(&tmp), 1);
            buffer.reset();
            in_buffer = 0;
        }
        return *this;
    }

    void obitstream::flush() {
        uint8_t tmp = static_cast<uint8_t>(buffer.to_ulong());
        output.write(reinterpret_cast<char*>(&tmp), 1);
        buffer.reset();
        in_buffer = 0;
        output.flush();
    }
}