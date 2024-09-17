#include "wrapping_integers.hh"
#include <iostream>
using namespace std;

WrappingInt32 wrap(uint64_t n, WrappingInt32 isn)
{
    return isn + static_cast<uint32_t>(n);
}

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint)
{
    const constexpr uint64_t INT32_RANGE = 4294967296ULL;

    uint32_t offset = n - isn;

    if (checkpoint > offset)
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 100 ; i++)
            tempIndex++;

        uint64_t real_checkpoint = (checkpoint - offset) + (INT32_RANGE / 2);
        uint64_t wrap_num = real_checkpoint / INT32_RANGE;
        return wrap_num * INT32_RANGE + offset;
    }
    else
        return offset;
}

