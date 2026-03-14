#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>

#include "dynamic_array.hpp"
#include "icollection.hpp"
#include "ienumerator.hpp"

template <class T>
class Sequence : public ICollection<T> {
protected:
    void AppendToResult(Sequence<T>*& sequence, const T& item) const {
        Sequence<T>* next = sequence->Append(item);
        if (next != sequence) {
            delete sequence;
            sequence = next;
        }
    }

public:
    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    virtual const T& Get(int index) const override = 0;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    virtual int GetLength() const = 0;

    int GetCount() const override {
        return GetLength();
    }

    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;

    virtual Sequence<T>* Concat(Sequence<T>* list) const = 0;
    virtual Sequence<T>* Clone() const override = 0;
    virtual Sequence<T>* CreateEmpty() const = 0;
    virtual IEnumerator<T>* GetEnumerator() const = 0;

    virtual ~Sequence() = default;

    const T& operator[](int index) const {
        return Get(index);
    }

    template <class Mapper>
    Sequence<T>* Map(Mapper mapper) const {
        Sequence<T>* result = CreateEmpty();
        try {
            for (int i = 0; i < GetLength(); ++i) {
                AppendToResult(result, mapper(Get(i)));
            }
            return result;
        } catch (...) {
            delete result;
            throw;
        }
    }

    template <class Predicate>
    Sequence<T>* Where(Predicate predicate) const {
        Sequence<T>* result = CreateEmpty();
        try {
            for (int i = 0; i < GetLength(); ++i) {
                T value = Get(i);
                if (predicate(value)) {
                    AppendToResult(result, value);
                }
            }
            return result;
        } catch (...) {
            delete result;
            throw;
        }
    }

    template <class Reducer>
    T Reduce(Reducer reducer) const {
        if (GetLength() == 0) {
            throw EmptyCollectionException("Sequence: Reduce on empty sequence");
        }

        T result = Get(0);
        for (int i = 1; i < GetLength(); ++i) {
            result = reducer(result, Get(i));
        }
        return result;
    }

    template <class TResult, class Folder>
    TResult Fold(TResult initialValue, Folder folder) const {
        TResult result = initialValue;
        for (int i = 0; i < GetLength(); ++i) {
            result = folder(result, Get(i));
        }
        return result;
    }

    template <class Predicate>
    int Find(Predicate predicate) const {
        for (int i = 0; i < GetLength(); ++i) {
            if (predicate(Get(i))) {
                return i;
            }
        }
        return -1;
    }

    template <class Predicate>
    DynamicArray<Sequence<T>*> Split(Predicate isDelimiter, bool keepEmpty = false) const {
        DynamicArray<Sequence<T>*> parts(0);
        Sequence<T>* currentPart = CreateEmpty();
        try {
            for (int i = 0; i < GetLength(); ++i) {
                T value = Get(i);
                if (isDelimiter(value)) {
                    if (keepEmpty || currentPart->GetLength() > 0) {
                        int oldSize = parts.GetSize();
                        parts.Resize(oldSize + 1);
                        parts.Set(oldSize, currentPart);
                        currentPart = nullptr;
                    } else {
                        delete currentPart;
                        currentPart = nullptr;
                    }
                    currentPart = CreateEmpty();
                } else {
                    AppendToResult(currentPart, value);
                }
            }

            if (keepEmpty || currentPart->GetLength() > 0) {
                int oldSize = parts.GetSize();
                parts.Resize(oldSize + 1);
                parts.Set(oldSize, currentPart);
                currentPart = nullptr;
            } else {
                delete currentPart;
                currentPart = nullptr;
            }

            return parts;
        } catch (...) {
            for (int i = 0; i < parts.GetSize(); ++i) {
                delete parts.Get(i);
            }
            delete currentPart;
            throw;
        }
    }

    Sequence<T>* Slice(int index, int count) const {
        return Slice(index, count, nullptr);
    }

    Sequence<T>* Slice(int index, int count, const Sequence<T>* replacement) const {
        int length = GetLength();
        if (index < 0 || count < 0 || index > length || index + count > length) {
            throw IndexOutOfRangeException("Sequence: invalid range in Slice");
        }

        Sequence<T>* result = CreateEmpty();
        try {
            for (int i = 0; i < index; ++i) {
                AppendToResult(result, Get(i));
            }

            if (replacement != nullptr) {
                for (int i = 0; i < replacement->GetLength(); ++i) {
                    AppendToResult(result, replacement->Get(i));
                }
            }

            for (int i = index + count; i < length; ++i) {
                AppendToResult(result, Get(i));
            }

            return result;
        } catch (...) {
            delete result;
            throw;
        }
    }
};

template <class T>
bool operator==(const Sequence<T>& left, const Sequence<T>& right) {
    if (left.GetLength() != right.GetLength()) {
        return false;
    }

    for (int i = 0; i < left.GetLength(); ++i) {
        if (left.Get(i) != right.Get(i)) {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator!=(const Sequence<T>& left, const Sequence<T>& right) {
    return !(left == right);
}

template <class T>
Sequence<T>* operator+(const Sequence<T>& left, const Sequence<T>& right) {
    Sequence<T>* rightCopy = right.Clone();
    try {
        Sequence<T>* result = left.Concat(rightCopy);
        delete rightCopy;
        return result;
    } catch (...) {
        delete rightCopy;
        throw;
    }
}

template <class T>
Sequence<T>* operator+(const Sequence<T>& sequence, T item) {
    Sequence<T>* result = sequence.Clone();
    try {
        Sequence<T>* next = result->Append(item);
        if (next != result) {
            delete result;
            result = next;
        }
        return result;
    } catch (...) {
        delete result;
        throw;
    }
}

template <class T>
std::ostream& operator<<(std::ostream& output, const Sequence<T>& sequence) {
    output << "[";

    IEnumerator<T>* enumerator = nullptr;
    try {
        enumerator = sequence.GetEnumerator();
        bool first = true;
        while (enumerator->MoveNext()) {
            if (!first) {
                output << ", ";
            }
            output << enumerator->Current();
            first = false;
        }
        delete enumerator;
    } catch (...) {
        delete enumerator;
        throw;
    }

    output << "]";
    return output;
}

#endif
