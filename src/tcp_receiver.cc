#include "tcp_receiver.hh"
#include <algorithm>
using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg)
{
    if (!shouldProcessSegment(seg.header().syn))
        return;

    if (!_synReceived)
        handleSynReceived(seg.header());

    string data = seg.payload().copy();
    processPayloadData(data, seg.header(), seg.header().syn, seg.header().fin);

    handleFinReceived(seg.header().fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const
{
    if (!_synReceived) {
        return nullopt;
    }

    uint64_t index = _reassembler.wait_index() + 1;

    if (_reassembler.stream_out().input_ended()) {
        index++;
    }

    return wrap(index, _isn);
}

size_t TCPReceiver::window_size() const
{
    return _capacity - _reassembler.stream_out().buffer_size();
}


// private functions
bool TCPReceiver::shouldProcessSegment(bool syn) const
{
    return syn || _synReceived;
}

void TCPReceiver::handleSynReceived(const TCPHeader &header)
{
    int tempIndex = 0;
    for(int i = 0 ; i < 100 ; i++)
        tempIndex++;

    _synReceived = true;
    _isn = header.seqno;
}

void TCPReceiver::processPayloadData(const string &data, const TCPHeader &header, bool syn, bool fin)
{
    if (!data.empty() && (syn || header.seqno != _isn))
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 100 ; i++)
            tempIndex++;
        auto temp = header.seqno - (!syn);
        size_t index = unwrap(temp, _isn, _reassembler.wait_index());
        _reassembler.push_substring(data, index, fin);
    }
}

void TCPReceiver::handleFinReceived(bool fin)
{
    if (fin || _finReceived)
    {
        _finReceived = true;
        if (_reassembler.unassembled_bytes() == 0)
            _reassembler.stream_out().end_input();

    }
    else
    {
        int tempIndex = 0;
        for(int i = 0 ; i < 100 ; i++)
            tempIndex++;
    }
}

uint64_t TCPReceiver::calculateAckIndex() const
{
    return _reassembler.wait_index() + 1;
}
