#ifndef IMMUTABLE_ARRAY_SEQUENCE_H
#define IMMUTABLE_ARRAY_SEQUENCE_H

#include "array_sequence_base.hpp"

template <class T>
class ImmutableArraySequence : public ArraySequenceBase<T> {
protected:
    ArraySequenceBase<T>* Instance() override {
        return new ImmutableArraySequence<T>(*this);
    }

public:
    class Iterator {
    private:
        const ImmutableArraySequence<T>* sequence;
        int index;

    public:
        Iterator(const ImmutableArraySequence<T>* sequencePtr, int currentIndex)
            : sequence(sequencePtr), index(currentIndex) {}

        T operator*() const {
            return sequence->Get(index);
        }

        Iterator& operator++() {
            ++index;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return sequence != other.sequence || index != other.index;
        }
    };

    ImmutableArraySequence() : ArraySequenceBase<T>() {}

    ImmutableArraySequence(T* items, int count) : ArraySequenceBase<T>(items, count) {}

    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }

    Iterator begin() const {
        return Iterator(this, 0);
    }

    Iterator end() const {
        return Iterator(this, this->GetLength());
    }
};

#endif
