#include "byte_stream.hh"
#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <map>
#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
using namespace std;

class StreamReassembler
{
    private:
        std::map<size_t, std::string> unassembleStrings;
        size_t nextAssembledIndex;
        size_t unassembledBytes;
        size_t eofIndex;
        size_t capacity;
        bool isEOF;
        ByteStream _output;

        size_t findNewIndex(size_t index);
        ssize_t calculateDataSize(const std::string &data, size_t newIndex, size_t index);
        void processOverlappingData(size_t newIndex, ssize_t &dataSize);
        bool hasEnoughSpace(size_t newIndex);
        void storeData(size_t newIndex, const std::string &data, ssize_t dataSize, size_t index);
        void processFullyAssembledData();

    public:
        StreamReassembler(const size_t capacity);

        void push_substring(const std::string &data, const uint64_t index, const bool eof);

        const ByteStream &stream_out() const { return _output; }

        ByteStream &stream_out() { return _output; }

        size_t unassembled_bytes() const;

        size_t wait_index() const { return nextAssembledIndex; }

        bool empty() const;

        size_t ack_index() const;
};

#endif
