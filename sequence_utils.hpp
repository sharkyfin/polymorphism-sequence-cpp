#ifndef SEQUENCE_UTILS_H
#define SEQUENCE_UTILS_H

#include "mutable_array_sequence.hpp"
#include "pair.hpp"

template <class T1, class T2>
Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& first, const Sequence<T2>& second) {
    MutableArraySequence<Pair<T1, T2>>* result = new MutableArraySequence<Pair<T1, T2>>();

    IEnumerator<T1>* firstEnumerator = nullptr;
    IEnumerator<T2>* secondEnumerator = nullptr;

    try {
        firstEnumerator = first.GetEnumerator();
        secondEnumerator = second.GetEnumerator();

        while (true) {
            bool hasFirst = firstEnumerator->MoveNext();
            bool hasSecond = secondEnumerator->MoveNext();
            if (!hasFirst || !hasSecond) {
                break;
            }

            result->Append(Pair<T1, T2>(firstEnumerator->Current(), secondEnumerator->Current()));
        }

        delete firstEnumerator;
        delete secondEnumerator;
        return result;
    } catch (...) {
        delete firstEnumerator;
        delete secondEnumerator;
        delete result;
        throw;
    }
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& zipped) {
    MutableArraySequence<T1>* first = nullptr;
    MutableArraySequence<T2>* second = nullptr;
    IEnumerator<Pair<T1, T2>>* enumerator = nullptr;

    try {
        first = new MutableArraySequence<T1>();
        second = new MutableArraySequence<T2>();
        enumerator = zipped.GetEnumerator();

        while (enumerator->MoveNext()) {
            Pair<T1, T2> value = enumerator->Current();
            first->Append(value.first);
            second->Append(value.second);
        }

        delete enumerator;
        return Pair<Sequence<T1>*, Sequence<T2>*>(first, second);
    } catch (...) {
        delete enumerator;
        delete first;
        delete second;
        throw;
    }
}

#endif
