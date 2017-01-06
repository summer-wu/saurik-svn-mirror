#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "platform.hpp"

extern "C" uint32_t hash(const char *begin, size_t length, uint32_t init);
uint32_t Hash(const char *begin, size_t length) {
    return hash(begin, length, 0x5463244);
}

#define Hash_(value) Hash(value, sizeof(value) - 1)

struct HashTable {
    struct Slot {
        _R<String> name_;
        uint32_t hash_;
        _R<Object> value_;

        _finline bool IsValid() {
            return name_ && name_ != _not(intptr_t);
        }
    };

    size_t size_;
    size_t count_;
    Slot *data_;

    _finline HashTable(size_t size) :
        size_(size),
        count_(0),
        data_(NULL)
    {
    }

    ~HashTable() {
        if (data_) {
            for (size_t i(0); i != 1 << size_; ++i)
                data_[i].~Slot();
            drop(data_);
        }
    }

    void Allocate_() {
        size_t size = (1 << size_) * sizeof(HashTable::Slot);
        data_ = (HashTable::Slot *) allocate(size);
        memset(data_, 0, size);
    }

    Slot *Find_(uint32_t hash, const char *begin, size_t length) {
        uint32_t mask = ~(_not(uint32_t) << size_);
        Slot *hole = NULL;

        uint32_t i = hash & mask;
        uint32_t offset = 0;

        _forever {
            HashTable::Slot *slot = &data_[i];

            if (!slot->name_)
                return hole ? hole : slot;
            else if (slot->name_ == _not(intptr_t)) {
                if (!hole)
                    hole = slot;
            } else if (
                begin &&
                slot->hash_ == hash &&
                slot->name_->Equals(begin, length)
            )
                return slot;

            i = (i + ++offset) & mask;
        }
    }

    void Reallocate_() {
        HashTable::Slot *data(data_);
        size_t size(size_++);
        Allocate_();
        for (size_t i(0); i != 1 << size; ++i)
            if (data[i].IsValid()) {
                Slot *slot(Find_(data[i].hash_, NULL, 0));
                slot->name_ = data[i].name_;
                slot->hash_ = data[i].hash_;
                slot->value_ = data[i].value_;
            }

        drop(data);
    }

    Object **Set_(uint32_t hash, const String *name, bool create) {
        if (!create) {
            if (!data_)
                return false;
        } else if (!data_)
            Allocate_();
        else if (count_ * 4 >= 1 << size_)
            Reallocate_();

        Slot *slot = Find_(hash, name->data_, name->count_);

        if (!slot->name_ || slot->name_ == _not(intptr_t))
            if (!create)
                return NULL;
            else {
                String *string = new String();
                string->AssignString(name);
                slot->name_ = string;
                slot->hash_ = hash;
                ++count_;
            }

        return &slot->value_.GetValue();
    }

    _finline Object *Get(const char *begin, size_t length) {
        String name;
        name.data_ = const_cast<char *>(begin);
        name.count_ = length;

        Object **value(HashTable::Set_(Hash(begin, length), &name, false));
        return value ? *value : NULL;
    }

    _finline Object **Set(const char *begin, size_t length, bool create) {
        String name;
        name.data_ = const_cast<char *>(begin);
        name.count_ = length;

        return Set_(Hash(begin, length), &name, create);
    }

    _finline Object **Set(const String *name, bool create) {
        return Set_(Hash(name->data_, name->count_), name, create);
    }
};

#endif//HASHTABLE_HPP
