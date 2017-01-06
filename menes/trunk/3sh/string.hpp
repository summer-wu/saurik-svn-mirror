#ifndef STRING_HPP
#define STRING_HPP

struct String {
    size_t count_;
    size_t size_;
    char *data_;

    _finline String() :
        count_(0),
        size_(0),
        data_(NULL)
    {
    }

    _finline ~String() {
        if (size_ != 0)
            drop(data_);
    }

    _finline void Clear() {
        count_ = 0;
    }

    _finline void Share_(char *data, size_t count) {
        if (size_)
            drop(data_);

        count_ = count;
        size_ = 0;
        data_ = data;
    }

    _finline void Share(char *data) {
        Share_(data, strlen(data));
    }

    void Reallocate_(size_t size) {
        if (count_ > size)
            count_ = size;
        char *data = (char *) allocate(size);
        memcpy(data, data_, count_);
        if (size_ != 0)
            drop(data_);
        size_ = size;
        data_ = data;
    }

    void InsertLast(char value) {
        if (count_ + 1 >= size_)
            Reallocate_((size_ + 4) * 2);

        data_[count_++] = value;
        data_[count_] = '\0';
    }

    bool Equals(const char *begin, size_t length) const {
        return count_ == length && memcmp(data_, begin, length) == 0;
    }

    template <size_t Length_>
    bool Equals(const char (&begin)[Length_]) const {
        return Equals(begin, Length_ - 1);
    }

    void Reserve(size_t size) {
        if (size_ < size)
            Reallocate_(size);
    }

    _finline void Increase(size_t size) {
        Reserve(count_ + size);
    }

    void AssignRange(const char *begin, size_t length) {
        if (length == _not(size_t))
            length = strlen(begin);

        Reserve(length + 1);
        memcpy(data_, begin, length);
        count_ = length;
        data_[count_] = '\0';
    }

    // XXX: make this be reference awesome (note this is somewhat critical now)
    _finline void AssignString(const String *value) {
        AssignRange(value->data_, value->count_);
    }

    void CopyRangeLast(const char *begin, size_t length) {
        if (length == _not(size_t))
            length = strlen(begin);
        if (length == 0)
            return;

        if (count_ + length >= size_)
            Reallocate_((count_ + length + 4) * 2);

        memcpy(data_ + count_, begin, length);
        count_ += length;
        data_[count_] = '\0';
    }

    _finline void CopyStringLast(const String *string) {
        CopyRangeLast(string->data_, string->count_);
    }
};

#endif//STRING_HPP
