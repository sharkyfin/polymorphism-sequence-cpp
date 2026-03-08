#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "exceptions.hpp"

template <class T>
class DynamicArray {
private:
    T* data;
    int size;

public:
    DynamicArray(T* items, int count) : data(nullptr), size(0) {
        if (count < 0) {
            throw InvalidArgumentException("DynamicArray: negative count");
        }
        if (count > 0 && items == nullptr) {
            throw InvalidArgumentException("DynamicArray: null items pointer with positive count");
        }

        size = count;
        if (size > 0) {
            T* newData = new T[size];
            try {
                for (int i = 0; i < size; ++i) {
                    newData[i] = items[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
            data = newData;
        }
    }

    explicit DynamicArray(int newSize) : data(nullptr), size(0) {
        if (newSize < 0) {
            throw InvalidArgumentException("DynamicArray: negative size");
        }

        size = newSize;
        if (size > 0) {
            data = new T[size];
        }
    }

    DynamicArray(const DynamicArray<T>& other) : data(nullptr), size(other.size) {
        if (size > 0) {
            T* newData = new T[size];
            try {
                for (int i = 0; i < size; ++i) {
                    newData[i] = other.data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
            data = newData;
        }
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other) {
        if (this == &other) {
            return *this;
        }

        T* newData = nullptr;
        if (other.size > 0) {
            newData = new T[other.size];
            try {
                for (int i = 0; i < other.size; ++i) {
                    newData[i] = other.data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
        }

        delete[] data;
        data = newData;
        size = other.size;
        return *this;
    }

    ~DynamicArray() {
        delete[] data;
    }

    T Get(int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("DynamicArray: index out of range");
        }
        return data[index];
    }

    int GetSize() const {
        return size;
    }

    void Set(int index, T value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("DynamicArray: index out of range");
        }
        data[index] = value;
    }

    void Resize(int newSize) {
        if (newSize < 0) {
            throw InvalidArgumentException("DynamicArray: negative size in Resize");
        }

        T* newData = nullptr;
        if (newSize > 0) {
            newData = new T[newSize];

            int copyCount = (newSize < size) ? newSize : size;
            try {
                for (int i = 0; i < copyCount; ++i) {
                    newData[i] = data[i];
                }
            } catch (...) {
                delete[] newData;
                throw;
            }
        }

        delete[] data;
        data = newData;
        size = newSize;
    }
};

#endif
