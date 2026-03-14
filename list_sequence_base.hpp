#ifndef LIST_SEQUENCE_BASE_H
#define LIST_SEQUENCE_BASE_H

#include "linked_list.hpp"
#include "sequence.hpp"

template <class T>
class ListSequenceBase : public Sequence<T> {
protected:
    LinkedList<T> data;

    virtual ListSequenceBase<T>* Instance() = 0;

    Sequence<T>* AppendInternal(T item) {
        data.Append(item);
        return this;
    }

    Sequence<T>* PrependInternal(T item) {
        data.Prepend(item);
        return this;
    }

    Sequence<T>* InsertAtInternal(T item, int index) {
        data.InsertAt(item, index);
        return this;
    }

public:
    ListSequenceBase() : data() {}

    ListSequenceBase(T* items, int count) : data(items, count) {}

    ListSequenceBase(const ListSequenceBase<T>& other) : data(other.data) {}

    const T& GetFirst() const override {
        return data.GetFirst();
    }

    const T& GetLast() const override {
        return data.GetLast();
    }

    const T& Get(int index) const override {
        return data.Get(index);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        LinkedList<T>* subList = nullptr;
        Sequence<T>* result = nullptr;
        try {
            subList = data.GetSubList(startIndex, endIndex);
            result = this->CreateEmpty();

            for (int i = 0; i < subList->GetLength(); ++i) {
                this->AppendToResult(result, subList->Get(i));
            }

            delete subList;
            return result;
        } catch (...) {
            delete subList;
            delete result;
            throw;
        }
    }

    int GetLength() const override {
        return data.GetLength();
    }

    IEnumerator<T>* GetEnumerator() const override {
        return data.GetEnumerator();
    }

    Sequence<T>* Append(T item) override {
        ListSequenceBase<T>* target = Instance();
        try {
            return target->AppendInternal(item);
        } catch (...) {
            if (target != this) {
                delete target;
            }
            throw;
        }
    }

    Sequence<T>* Prepend(T item) override {
        ListSequenceBase<T>* target = Instance();
        try {
            return target->PrependInternal(item);
        } catch (...) {
            if (target != this) {
                delete target;
            }
            throw;
        }
    }

    Sequence<T>* InsertAt(T item, int index) override {
        ListSequenceBase<T>* target = Instance();
        try {
            return target->InsertAtInternal(item, index);
        } catch (...) {
            if (target != this) {
                delete target;
            }
            throw;
        }
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        if (list == nullptr) {
            throw InvalidArgumentException("ListSequenceBase: null pointer in Concat");
        }

        Sequence<T>* result = this->Clone();
        try {
            for (int i = 0; i < list->GetLength(); ++i) {
                this->AppendToResult(result, list->Get(i));
            }
            return result;
        } catch (...) {
            delete result;
            throw;
        }
    }
};

#endif
