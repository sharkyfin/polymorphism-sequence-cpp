#ifndef MUTABLE_ARRAY_SEQUENCE_H
#define MUTABLE_ARRAY_SEQUENCE_H

#include "array_sequence_base.hpp"

template <class T>
class MutableArraySequence : public ArraySequenceBase<T> {
protected:
    ArraySequenceBase<T>* Instance() override {
        return this;
    }

public:
    class Iterator {
    private:
        const MutableArraySequence<T>* sequence;
        int index;

    public:
        Iterator(const MutableArraySequence<T>* sequencePtr, int currentIndex)
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

    MutableArraySequence() : ArraySequenceBase<T>() {}

    MutableArraySequence(T* items, int count) : ArraySequenceBase<T>(items, count) {}

    MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }

    Iterator begin() const {
        return Iterator(this, 0);
    }

    Iterator end() const {
        return Iterator(this, this->GetLength());
    }
};

#endif
