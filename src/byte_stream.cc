#include "byte_stream.hh"
#include <algorithm>
using namespace std;


ByteStream::ByteStream(const size_t capa)
{
    capacity = capa;
    bytesWritten = 0;
    bytesRead = 0;
    inputEnded = false;
    _error = false;
}


size_t ByteStream::write(const string& data)
{
    if (inputEnded || _error)
    {
        _error = true;
        return 0;
    }

    size_t bytesToWrite = min(data.length(), remaining_capacity());

    for (size_t i = 0; i < bytesToWrite; i++)
    {
        buffer.push_back(data[i]);
        bytesWritten++;
    }

    return bytesToWrite;
}

string ByteStream::peek_output(const size_t len) const
{
    size_t outputLen = min(len, buffer.size());
    return string(buffer.begin(), buffer.begin() + outputLen);
}

void ByteStream::pop_output(const size_t len)
{
    size_t bytesToPop = min(len, buffer.size());

    for (size_t i = 0; i < bytesToPop; i++)
    {
        buffer.pop_front();
        bytesRead++;
    }
}

string ByteStream::read(const size_t len)
{
    string output = peek_output(len);
    pop_output(output.length());
    return output;
}

void ByteStream::end_input()
{
    inputEnded = true;
}

bool ByteStream::input_ended() const
{
    return inputEnded;
}

size_t ByteStream::buffer_size() const
{
    return buffer.size();
}

bool ByteStream::buffer_empty() const
{
    return buffer.empty();
}

bool ByteStream::eof() const
{
    return inputEnded && buffer_empty();
}

size_t ByteStream::bytes_written() const
{
    return bytesWritten;
}

size_t ByteStream::bytes_read() const
{
    return bytesRead;
}

size_t ByteStream::remaining_capacity() const
{
    return (buffer.size() <= capacity) ? capacity - buffer.size() : 0;
}