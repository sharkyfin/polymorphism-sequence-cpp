#ifndef MUTABLE_LIST_SEQUENCE_H
#define MUTABLE_LIST_SEQUENCE_H

#include "list_sequence_base.hpp"

template <class T>
class MutableListSequence : public ListSequenceBase<T> {
protected:
    ListSequenceBase<T>* Instance() override {
        return this;
    }

public:
    class Iterator {
    private:
        const MutableListSequence<T>* sequence;
        int index;

    public:
        Iterator(const MutableListSequence<T>* sequencePtr, int currentIndex)
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

    MutableListSequence() : ListSequenceBase<T>() {}

    MutableListSequence(T* items, int count) : ListSequenceBase<T>(items, count) {}

    MutableListSequence(const MutableListSequence<T>& other) : ListSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }

    Iterator begin() const {
        return Iterator(this, 0);
    }

    Iterator end() const {
        return Iterator(this, this->GetLength());
    }
};

#endif
