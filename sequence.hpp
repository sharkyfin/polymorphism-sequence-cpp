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
        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            while (enumerator->MoveNext()) {
                AppendToResult(result, mapper(enumerator->Current()));
            }
            delete enumerator;
            return result;
        } catch (...) {
            delete enumerator;
            delete result;
            throw;
        }
    }

    template <class Predicate>
    Sequence<T>* Where(Predicate predicate) const {
        Sequence<T>* result = CreateEmpty();
        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            while (enumerator->MoveNext()) {
                T value = enumerator->Current();
                if (predicate(value)) {
                    AppendToResult(result, value);
                }
            }
            delete enumerator;
            return result;
        } catch (...) {
            delete enumerator;
            delete result;
            throw;
        }
    }

    template <class Reducer>
    T Reduce(Reducer reducer) const {
        if (GetLength() == 0) {
            throw EmptyCollectionException("Sequence: Reduce on empty sequence");
        }

        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            if (!enumerator->MoveNext()) {
                delete enumerator;
                throw EmptyCollectionException("Sequence: Reduce on empty sequence");
            }

            T result = enumerator->Current();
            while (enumerator->MoveNext()) {
                result = reducer(result, enumerator->Current());
            }

            delete enumerator;
            return result;
        } catch (...) {
            delete enumerator;
            throw;
        }
    }

    template <class TResult, class Folder>
    TResult Fold(TResult initialValue, Folder folder) const {
        TResult result = initialValue;
        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            while (enumerator->MoveNext()) {
                result = folder(result, enumerator->Current());
            }
            delete enumerator;
            return result;
        } catch (...) {
            delete enumerator;
            throw;
        }
    }

    template <class Predicate>
    int Find(Predicate predicate) const {
        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            int index = 0;
            while (enumerator->MoveNext()) {
                if (predicate(enumerator->Current())) {
                    delete enumerator;
                    return index;
                }
                ++index;
            }
            delete enumerator;
            return -1;
        } catch (...) {
            delete enumerator;
            throw;
        }
    }

    template <class Predicate>
    DynamicArray<Sequence<T>*> Split(Predicate isDelimiter, bool keepEmpty = false) const {
        DynamicArray<Sequence<T>*> parts(0);
        Sequence<T>* currentPart = CreateEmpty();
        IEnumerator<T>* enumerator = nullptr;
        try {
            enumerator = GetEnumerator();
            while (enumerator->MoveNext()) {
                T value = enumerator->Current();
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

            delete enumerator;
            return parts;
        } catch (...) {
            delete enumerator;
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
        IEnumerator<T>* sourceEnumerator = nullptr;
        IEnumerator<T>* replacementEnumerator = nullptr;

        try {
            sourceEnumerator = GetEnumerator();
            bool insertedReplacement = false;
            int position = 0;

            while (sourceEnumerator->MoveNext()) {
                if (!insertedReplacement && position == index) {
                    if (replacement != nullptr) {
                        replacementEnumerator = replacement->GetEnumerator();
                        while (replacementEnumerator->MoveNext()) {
                            AppendToResult(result, replacementEnumerator->Current());
                        }
                        delete replacementEnumerator;
                        replacementEnumerator = nullptr;
                    }
                    insertedReplacement = true;
                }

                T value = sourceEnumerator->Current();
                if (position < index || position >= index + count) {
                    AppendToResult(result, value);
                }
                ++position;
            }

            if (!insertedReplacement && replacement != nullptr) {
                replacementEnumerator = replacement->GetEnumerator();
                while (replacementEnumerator->MoveNext()) {
                    AppendToResult(result, replacementEnumerator->Current());
                }
                delete replacementEnumerator;
                replacementEnumerator = nullptr;
            }

            delete sourceEnumerator;
            return result;
        } catch (...) {
            delete sourceEnumerator;
            delete replacementEnumerator;
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

    IEnumerator<T>* leftEnumerator = nullptr;
    IEnumerator<T>* rightEnumerator = nullptr;

    try {
        leftEnumerator = left.GetEnumerator();
        rightEnumerator = right.GetEnumerator();

        while (true) {
            bool hasLeft = leftEnumerator->MoveNext();
            bool hasRight = rightEnumerator->MoveNext();

            if (hasLeft != hasRight) {
                delete leftEnumerator;
                delete rightEnumerator;
                return false;
            }

            if (!hasLeft) {
                break;
            }

            if (leftEnumerator->Current() != rightEnumerator->Current()) {
                delete leftEnumerator;
                delete rightEnumerator;
                return false;
            }
        }

        delete leftEnumerator;
        delete rightEnumerator;
        return true;
    } catch (...) {
        delete leftEnumerator;
        delete rightEnumerator;
        throw;
    }
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
