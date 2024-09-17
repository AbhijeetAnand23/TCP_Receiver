#ifndef SPONGE_LIBSPONGE_TCP_RECEIVER_HH
#define SPONGE_LIBSPONGE_TCP_RECEIVER_HH

#include "byte_stream.hh"
#include "stream_reassembler.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"
#include <optional>

class TCPReceiver
{
    StreamReassembler _reassembler;
    size_t _capacity;
    bool _synReceived;
    bool _finReceived;
    WrappingInt32 _isn;

    private:
        bool shouldProcessSegment(bool syn) const;
        void handleSynReceived(const TCPHeader &header);
        void processPayloadData(const string &data, const TCPHeader &header, bool syn, bool fin);
        void handleFinReceived(bool fin);
        uint64_t calculateAckIndex() const;

    public:
        TCPReceiver(const size_t capacity) : _reassembler(capacity), _capacity(capacity), _synReceived(false), _finReceived(false), _isn(0) {}

        std::optional<WrappingInt32> ackno() const;

        size_t window_size() const;

        size_t unassembled_bytes() const { return _reassembler.unassembled_bytes(); }

        void segment_received(const TCPSegment &seg);

        ByteStream &stream_out() { return _reassembler.stream_out(); }

        const ByteStream &stream_out() const { return _reassembler.stream_out(); }
};

#endif
