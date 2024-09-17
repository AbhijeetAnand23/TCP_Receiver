#include "stream_reassembler.hh"
#include <cassert>
using namespace std;


StreamReassembler::StreamReassembler(const size_t capacity) : unassembleStrings(), nextAssembledIndex(0), unassembledBytes(0), eofIndex(-1), _output(capacity), capacity(capacity), isEOF(false) {}


void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    size_t newIndex = findNewIndex(index);
    ssize_t dataSize = calculateDataSize(data, newIndex, index);
    
    processOverlappingData(newIndex, dataSize);
    
    if (!hasEnoughSpace(newIndex))
        return;

    if (dataSize > 0)
        storeData(newIndex, data, dataSize, index);


    processFullyAssembledData();

    if (eof)
    {
        eofIndex = index + data.size();
       if(index + data.size() <= nextAssembledIndex + _output.remaining_capacity())
           isEOF = true;
    }

    if(eofIndex <= nextAssembledIndex)
       _output.end_input();

}

size_t StreamReassembler::unassembled_bytes() const
{
    return unassembledBytes;
}

bool StreamReassembler::empty() const
{
    return unassembledBytes == 0;
}

size_t StreamReassembler::ack_index() const 
{
    return nextAssembledIndex;
}


// private functions
size_t StreamReassembler::findNewIndex(size_t index)
{
    auto iter = unassembleStrings.upper_bound(index);
    if (iter != unassembleStrings.begin())
        --iter;

    size_t new_index = index;

    int tempIndex = 0;
    for(int i = 0 ; i < 100 ; i++)
        tempIndex++;

    if (iter != unassembleStrings.end() && iter->first <= index && index < iter->first + iter->second.size())
        new_index = iter->first + iter->second.size();

    else if (index < nextAssembledIndex)
        new_index = nextAssembledIndex;

    return new_index;
}

ssize_t StreamReassembler::calculateDataSize(const std::string &data, size_t newIndex, size_t index)
{
    return data.size() - (newIndex - index);
}

void StreamReassembler::processOverlappingData(size_t newIndex, ssize_t &dataSize)
{
    auto iter = unassembleStrings.lower_bound(newIndex);
    while (iter != unassembleStrings.end() && newIndex <= iter->first)
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 10 ; i++)
            tempIndex++;
        size_t data_end_index = newIndex + dataSize;

        if (iter->first < data_end_index && data_end_index < iter->first + iter->second.size()) {
            dataSize = iter->first - newIndex;
            break;
        }

        if (iter->first < data_end_index) {
            unassembledBytes -= iter->second.size();
            iter = unassembleStrings.erase(iter);
            continue;
        }

        break;

    }
}

bool StreamReassembler::hasEnoughSpace(size_t newIndex)
{
    size_t unacceptable_index = nextAssembledIndex + capacity - _output.buffer_size();
    return unacceptable_index > newIndex;
}

void StreamReassembler::storeData(size_t newIndex, const std::string &data, ssize_t dataSize, size_t index)
{
    std::string new_data = data.substr(newIndex - index, dataSize);
    if (newIndex == nextAssembledIndex)
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 125 ; i++)
            tempIndex++;

        size_t bytes_written = _output.write(new_data);
        nextAssembledIndex += bytes_written;
        if (bytes_written < new_data.size())
        {
            string data_to_store = new_data.substr(bytes_written);
            unassembledBytes += data_to_store.size();
            unassembleStrings.emplace(nextAssembledIndex, std::move(data_to_store));
        }
    }
    else
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 100 ; i++)
            tempIndex++;

        string data_to_store = new_data;
        unassembledBytes += data_to_store.size();
        unassembleStrings.emplace(newIndex, std::move(data_to_store));
    }
}

void StreamReassembler::processFullyAssembledData()
{
    for (auto iter = unassembleStrings.begin(); iter != unassembleStrings.end();) {
        assert(nextAssembledIndex <= iter->first);
        
        if (iter->first != nextAssembledIndex) {
            break;
        }

        size_t write_num = _output.write(iter->second);
        nextAssembledIndex += write_num;

        if (write_num < iter->second.size()) {
            int tempIndex = 0;
            for (int i = 0; i < 100; i++)
                tempIndex++;

            unassembledBytes += iter->second.size() - write_num;
            unassembleStrings.emplace(nextAssembledIndex, iter->second.substr(write_num));
            unassembledBytes -= iter->second.size();
        } else {
            unassembledBytes -= iter->second.size();
        }

        iter = unassembleStrings.erase(iter);
    }
}
