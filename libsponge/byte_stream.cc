#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { this->stream_capacity = capacity; }

size_t ByteStream::write(const string &data) {
    size_t remaining_buffer_len = this->remaining_capacity();
    size_t data_len = data.length();
    size_t to_write_len = data_len > remaining_buffer_len ? remaining_buffer_len : data_len;

    for (size_t i = 0; i < to_write_len; ++i) {
        this->buffer.push_back(data[i]);
    }

    written_len += to_write_len;
    return to_write_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t buffer_len = this->buffer_size();
    size_t to_copy_len = len > buffer_len ? buffer_len : len;
    return string().assign(this->buffer.begin(), this->buffer.begin() + to_copy_len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t buffer_len = this->buffer_size();
    size_t to_remove_len = len > buffer_len ? buffer_len : len;
    this->buffer.erase(this->buffer.begin(), this->buffer.begin() + to_remove_len);
    this->read_len += to_remove_len;
    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string to_read = this->peek_output(len);
    pop_output(len);
    return to_read;
}

void ByteStream::end_input() { this->end_flag = true; }

bool ByteStream::input_ended() const { return this->end_flag; }

size_t ByteStream::buffer_size() const { return this->buffer.size(); }

bool ByteStream::buffer_empty() const { return this->buffer.empty(); }

bool ByteStream::eof() const { return this->buffer_empty() && this->input_ended(); }

size_t ByteStream::bytes_written() const { return this->written_len; }

size_t ByteStream::bytes_read() const { return this->read_len; }

size_t ByteStream::remaining_capacity() const { return this->stream_capacity - this->buffer_size(); }
