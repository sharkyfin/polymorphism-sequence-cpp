#ifndef SEQUENCE_UTILS_H
#define SEQUENCE_UTILS_H

#include "mutable_array_sequence.hpp"
#include "pair.hpp"

template <class T1, class T2>
Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& first, const Sequence<T2>& second) {
    MutableArraySequence<Pair<T1, T2>>* result = new MutableArraySequence<Pair<T1, T2>>();

    int minLength = first.GetLength();
    if (second.GetLength() < minLength) {
        minLength = second.GetLength();
    }

    try {
        for (int i = 0; i < minLength; ++i) {
            result->Append(Pair<T1, T2>(first.Get(i), second.Get(i)));
        }
        return result;
    } catch (...) {
        delete result;
        throw;
    }
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& zipped) {
    MutableArraySequence<T1>* first = nullptr;
    MutableArraySequence<T2>* second = nullptr;

    try {
        first = new MutableArraySequence<T1>();
        second = new MutableArraySequence<T2>();

        for (int i = 0; i < zipped.GetLength(); ++i) {
            Pair<T1, T2> value = zipped.Get(i);
            first->Append(value.first);
            second->Append(value.second);
        }

        return Pair<Sequence<T1>*, Sequence<T2>*>(first, second);
    } catch (...) {
        delete first;
        delete second;
        throw;
    }
}

#endif
