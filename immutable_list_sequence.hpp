#ifndef IMMUTABLE_LIST_SEQUENCE_H
#define IMMUTABLE_LIST_SEQUENCE_H

#include "list_sequence_base.hpp"

template <class T>
class ImmutableListSequence : public ListSequenceBase<T> {
protected:
    ListSequenceBase<T>* Instance() override {
        return new ImmutableListSequence<T>(*this);
    }

public:
    class Iterator {
    private:
        const ImmutableListSequence<T>* sequence;
        int index;

    public:
        Iterator(const ImmutableListSequence<T>* sequencePtr, int currentIndex)
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

    ImmutableListSequence() : ListSequenceBase<T>() {}

    ImmutableListSequence(T* items, int count) : ListSequenceBase<T>(items, count) {}

    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequenceBase<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    Sequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }

    Iterator begin() const {
        return Iterator(this, 0);
    }

    Iterator end() const {
        return Iterator(this, this->GetLength());
    }
};

#endif
