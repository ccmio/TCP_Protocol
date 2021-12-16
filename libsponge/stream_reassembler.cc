#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof && this->empty()) {
        this->_output.end_input();
    }
    if (index >= this->first_unread_pos + this->_capacity || index + data.size() <= this->first_unread_pos) {
        return;
    }

    size_t left_bound = max(index, this->first_unread_pos);
    size_t right_bound = min(index + data.size(), this->first_unread_pos + this->_capacity);
    string str = data.substr(left_bound - index, right_bound - left_bound);    
    segment seg(str, left_bound, eof);

    if (left_bound + data.size() <= this->first_unread_pos)
    assemble(seg);
    if (index == this->first_unread_pos) {
        read_out(seg);
    } else {
        this->buffer.insert(seg);
        this->unassembled_count += seg.data.size();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return this->unassembled_count; }

bool StreamReassembler::empty() const { return this->unassembled_count == 0; }

void StreamReassembler::assemble(segment &seg) {
    auto next_it = this->buffer.lower_bound(seg);  // next_it指向当前大于等于该seg的最小一个

    // 向左融合
    if (next_it != this->buffer.begin()) {
        auto pre_it = prev(next_it, 1);  // pre_it指向小于该seg的最大一个
        if (pre_it->index + pre_it->data.size() >= seg.index + seg.data.size()) {
            seg = *pre_it;
        } else if (pre_it->index + pre_it->data.size() >= seg.index) {
            seg.index = pre_it->index;
            seg.data = pre_it->data.substr(0, seg.index - pre_it->index) + seg.data;
        }
        this->unassembled_count -= pre_it->data.size();
        this->buffer.erase(pre_it);
    }

    // 向右融合
    while (seg.index + seg.data.size() >= next_it->index) {
        if (seg.index + seg.data.size() < next_it->index + next_it->data.size()) {
            seg.data = seg.data.substr(0, next_it->index - seg.index) + next_it->data;
        }
        this->unassembled_count -= next_it->data.size();
        next_it = this->buffer.erase(next_it);
    }
}

void StreamReassembler::read_out(segment seg) {
    this->_output.write(seg.data);
    this->first_unread_pos += seg.data.size();
}
