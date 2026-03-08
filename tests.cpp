#include "tests.hpp"

#include <iostream>
#include <string>

#include "dynamic_array.hpp"
#include "immutable_array_sequence.hpp"
#include "immutable_list_sequence.hpp"
#include "linked_list.hpp"
#include "mutable_array_sequence.hpp"
#include "mutable_list_sequence.hpp"
#include "pair.hpp"
#include "sequence_utils.hpp"

class TestFailedException {
private:
    std::string message;

public:
    explicit TestFailedException(const std::string& msg) : message(msg) {}

    const std::string& GetMessage() const {
        return message;
    }
};

void Assert(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailedException(message);
    }
}

void AssertIntSequenceEquals(const Sequence<int>* sequence,
                             const int* expected,
                             int expectedLength,
                             const std::string& messagePrefix) {
    Assert(sequence != nullptr, messagePrefix + ": sequence is null");
    Assert(sequence->GetLength() == expectedLength, messagePrefix + ": wrong length");

    for (int i = 0; i < expectedLength; ++i) {
        Assert(sequence->Get(i) == expected[i],
               messagePrefix + ": wrong value at index " + std::to_string(i));
    }
}

void TestDynamicArrayCreateAndGet() {
    int items[3] = {1, 2, 3};
    DynamicArray<int> arr(items, 3);

    Assert(arr.GetSize() == 3, "DynamicArray: wrong size after construction");
    Assert(arr.Get(0) == 1, "DynamicArray: wrong element at index 0");
    Assert(arr.Get(2) == 3, "DynamicArray: wrong element at index 2");
}

void TestDynamicArrayCopy() {
    int items[2] = {10, 20};
    DynamicArray<int> a(items, 2);
    DynamicArray<int> b(a);

    a.Set(0, 99);
    Assert(b.Get(0) == 10, "DynamicArray: copy is not deep");
}

void TestDynamicArrayResizeSetGet() {
    DynamicArray<int> arr(2);
    arr.Set(0, 5);
    arr.Set(1, 6);

    arr.Resize(4);
    arr.Set(2, 7);
    arr.Set(3, 8);

    Assert(arr.GetSize() == 4, "DynamicArray: wrong size after Resize up");
    Assert(arr.Get(0) == 5 && arr.Get(3) == 8, "DynamicArray: wrong values after Resize up");

    arr.Resize(1);
    Assert(arr.GetSize() == 1, "DynamicArray: wrong size after Resize down");
    Assert(arr.Get(0) == 5, "DynamicArray: wrong value after Resize down");
}

void TestDynamicArrayIndexErrors() {
    DynamicArray<int> arr(1);

    bool caught = false;
    try {
        arr.Get(1);
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "DynamicArray: out-of-range Get must throw");
}

void TestLinkedListAppendPrependInsertGet() {
    LinkedList<int> list;
    list.Append(2);
    list.Prepend(1);
    list.InsertAt(3, 2);
    list.InsertAt(100, 1);

    Assert(list.GetLength() == 4, "LinkedList: wrong length");
    Assert(list.GetFirst() == 1, "LinkedList: wrong first element");
    Assert(list.GetLast() == 3, "LinkedList: wrong last element");
    Assert(list.Get(1) == 100, "LinkedList: wrong inserted element");
}

void TestLinkedListSubListAndConcat() {
    int itemsA[4] = {1, 2, 3, 4};
    int itemsB[2] = {5, 6};

    LinkedList<int> a(itemsA, 4);
    LinkedList<int> b(itemsB, 2);

    LinkedList<int>* sub = a.GetSubList(1, 3);
    Assert(sub->GetLength() == 3, "LinkedList: wrong sublist length");
    Assert(sub->Get(0) == 2 && sub->Get(2) == 4, "LinkedList: wrong sublist values");

    LinkedList<int>* c = a.Concat(&b);
    Assert(c->GetLength() == 6, "LinkedList: wrong concat length");
    Assert(c->Get(4) == 5 && c->Get(5) == 6, "LinkedList: wrong concat values");

    delete sub;
    delete c;
}

void TestSequenceInterfaceSubsequenceAndConcat() {
    Sequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(10);
    seq->Append(20);
    seq->Append(30);

    Sequence<int>* sub = seq->GetSubsequence(1, 2);
    Assert(sub->GetLength() == 2, "Sequence: wrong subsequence length");
    Assert(sub->Get(0) == 20 && sub->Get(1) == 30, "Sequence: wrong subsequence values");

    Sequence<int>* con = seq->Concat(sub);
    Assert(con->GetLength() == 5, "Sequence: wrong concat length");
    Assert(con->Get(3) == 20 && con->Get(4) == 30, "Sequence: wrong concat values");

    delete seq;
    delete sub;
    delete con;
}

void TestMutableSemantics() {
    MutableArraySequence<int> seq;
    Sequence<int>* returned = seq.Append(1);

    Assert(returned == &seq, "MutableSequence: Append must return same object");
    Assert(seq.GetLength() == 1 && seq.Get(0) == 1, "MutableSequence: object not modified");
}

void TestImmutableArraySemantics() {
    ImmutableArraySequence<int> seq;
    Sequence<int>* s1 = seq.Append(10);

    Assert(seq.GetLength() == 0, "ImmutableArraySequence: original changed after Append");
    Assert(s1 != &seq, "ImmutableArraySequence: Append must return new object");
    Assert(s1->GetLength() == 1 && s1->Get(0) == 10, "ImmutableArraySequence: wrong new sequence");

    Sequence<int>* s2 = s1->Prepend(5);
    Assert(s1->GetLength() == 1, "ImmutableArraySequence: previous object changed after Prepend");
    Assert(s2->GetLength() == 2 && s2->Get(0) == 5 && s2->Get(1) == 10,
           "ImmutableArraySequence: wrong Prepend result");

    delete s1;
    delete s2;
}

void TestImmutableListSemantics() {
    ImmutableListSequence<int> seq;
    Sequence<int>* s1 = seq.Append(7);

    Assert(seq.GetLength() == 0, "ImmutableListSequence: original changed after Append");
    Assert(s1 != &seq, "ImmutableListSequence: Append must return new object");

    Sequence<int>* s2 = s1->InsertAt(9, 1);
    Assert(s1->GetLength() == 1, "ImmutableListSequence: previous object changed after InsertAt");
    Assert(s2->GetLength() == 2 && s2->Get(1) == 9, "ImmutableListSequence: wrong InsertAt result");

    delete s1;
    delete s2;
}

void TestImmutableSubsequence() {
    int items[4] = {4, 5, 6, 7};
    ImmutableArraySequence<int> seq(items, 4);

    Sequence<int>* sub = seq.GetSubsequence(1, 2);
    Assert(sub->GetLength() == 2, "ImmutableArraySequence: wrong subsequence length");
    Assert(sub->Get(0) == 5 && sub->Get(1) == 6, "ImmutableArraySequence: wrong subsequence values");
    Assert(seq.GetLength() == 4, "ImmutableArraySequence: original changed after GetSubsequence");

    delete sub;
}

void TestICollectionContract() {
    int items[3] = {11, 22, 33};
    ICollection<int>* collection = new MutableArraySequence<int>(items, 3);

    Assert(collection->GetCount() == 3, "ICollection: wrong count");
    Assert(collection->Get(1) == 22, "ICollection: wrong value from Get");

    ICollection<int>* clone = collection->Clone();
    Assert(clone->GetCount() == 3, "ICollection: wrong clone count");
    Assert(clone->Get(2) == 33, "ICollection: wrong clone value");

    delete collection;
    delete clone;
}

void TestOperatorIndex() {
    int items[3] = {7, 8, 9};
    MutableListSequence<int> seq(items, 3);

    Assert(seq[0] == 7, "operator[]: wrong value at index 0");
    Assert(seq[2] == 9, "operator[]: wrong value at index 2");

    bool caught = false;
    try {
        int unused = seq[3];
        (void)unused;
    } catch (const LabException&) {
        caught = true;
    }
    Assert(caught, "operator[]: out-of-range must throw");
}

void TestOperatorEqualAndNotEqual() {
    int itemsA[3] = {1, 2, 3};
    int itemsB[3] = {1, 2, 3};
    int itemsC[3] = {1, 2, 4};

    MutableArraySequence<int> a(itemsA, 3);
    ImmutableListSequence<int> b(itemsB, 3);
    MutableListSequence<int> c(itemsC, 3);

    Assert(a == b, "operator==: equal sequences must be equal");
    Assert(a != c, "operator!=: different sequences must be not equal");
}

void TestOperatorPlus() {
    int leftItems[2] = {1, 2};
    int rightItems[2] = {3, 4};

    MutableArraySequence<int> left(leftItems, 2);
    MutableListSequence<int> right(rightItems, 2);

    Sequence<int>* concat = left + right;
    int expectedConcat[4] = {1, 2, 3, 4};
    AssertIntSequenceEquals(concat, expectedConcat, 4, "operator+ sequence");

    Sequence<int>* plusElement = left + 9;
    int expectedPlusElement[3] = {1, 2, 9};
    AssertIntSequenceEquals(plusElement, expectedPlusElement, 3, "operator+ element");

    Assert(left.GetLength() == 2, "operator+: left side must not be changed");

    delete concat;
    delete plusElement;
}

void TestOperatorOutput() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    std::ostream& output = (std::cout << seq);
    std::cout << "\n";
    Assert(&output == &std::cout, "operator<<: must return output stream reference");
}

void TestMap() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    Sequence<int>* mapped = seq.Map([](int value) { return value * 3; });
    int expected[3] = {3, 6, 9};
    AssertIntSequenceEquals(mapped, expected, 3, "Map");
    Assert(seq.Get(0) == 1, "Map: source sequence must stay unchanged");

    delete mapped;
}

void TestReduce() {
    int items[4] = {1, 2, 3, 4};
    ImmutableArraySequence<int> seq(items, 4);

    int sum = seq.Reduce([](int left, int right) { return left + right; });
    Assert(sum == 10, "Reduce: wrong sum");
}

void TestFold() {
    int items[3] = {2, 3, 4};
    MutableArraySequence<int> seq(items, 3);

    int folded = seq.Fold(1, [](int acc, int value) { return acc * value; });
    Assert(folded == 24, "Fold: wrong result");
}

void TestWhereAndNoMatches() {
    int items[5] = {1, 2, 3, 4, 5};
    MutableListSequence<int> seq(items, 5);

    Sequence<int>* even = seq.Where([](int value) { return value % 2 == 0; });
    int expectedEven[2] = {2, 4};
    AssertIntSequenceEquals(even, expectedEven, 2, "Where even");

    Sequence<int>* none = seq.Where([](int value) { return value > 100; });
    Assert(none->GetLength() == 0, "Where: no matches should produce empty sequence");

    delete even;
    delete none;
}

void TestFind() {
    int items[4] = {5, 10, 15, 20};
    ImmutableListSequence<int> seq(items, 4);

    int found = seq.Find([](int value) { return value == 15; });
    int notFound = seq.Find([](int value) { return value == 99; });

    Assert(found == 2, "Find: wrong index for existing value");
    Assert(notFound == -1, "Find: must return -1 if no value found");
}

void TestIteration() {
    int items[4] = {1, 2, 3, 4};
    MutableArraySequence<int> arraySeq(items, 4);
    MutableListSequence<int> listSeq(items, 4);

    int arraySum = 0;
    for (int value : arraySeq) {
        arraySum += value;
    }

    int listSum = 0;
    for (int value : listSeq) {
        listSum += value;
    }

    Assert(arraySum == 10, "Iteration: wrong sum for array sequence");
    Assert(listSum == 10, "Iteration: wrong sum for list sequence");
}

void TestSplitWithoutDelimiters() {
    int items[3] = {1, 2, 3};
    MutableArraySequence<int> seq(items, 3);

    DynamicArray<Sequence<int>*> parts = seq.Split([](int value) { return value == 0; });
    Assert(parts.GetSize() == 1, "Split: without delimiters there must be one part");

    int expected[3] = {1, 2, 3};
    AssertIntSequenceEquals(parts.Get(0), expected, 3, "Split: wrong part content");

    delete parts.Get(0);
}

void TestSplitBoundaries() {
    int items[5] = {0, 1, 0, 2, 0};
    MutableArraySequence<int> seq(items, 5);

    DynamicArray<Sequence<int>*> parts = seq.Split([](int value) { return value == 0; }, false);
    Assert(parts.GetSize() == 2, "Split: wrong count when delimiter at start/end and keepEmpty=false");

    int expectedA[1] = {1};
    int expectedB[1] = {2};
    AssertIntSequenceEquals(parts.Get(0), expectedA, 1, "Split part 0");
    AssertIntSequenceEquals(parts.Get(1), expectedB, 1, "Split part 1");

    for (int i = 0; i < parts.GetSize(); ++i) {
        delete parts.Get(i);
    }

    DynamicArray<Sequence<int>*> partsWithEmpty = seq.Split([](int value) { return value == 0; }, true);
    Assert(partsWithEmpty.GetSize() == 4,
           "Split: wrong count when delimiter at start/end and keepEmpty=true");

    Assert(partsWithEmpty.Get(0)->GetLength() == 0, "Split keepEmpty: first part should be empty");
    Assert(partsWithEmpty.Get(1)->GetLength() == 1 && partsWithEmpty.Get(1)->Get(0) == 1,
           "Split keepEmpty: second part wrong");
    Assert(partsWithEmpty.Get(2)->GetLength() == 1 && partsWithEmpty.Get(2)->Get(0) == 2,
           "Split keepEmpty: third part wrong");
    Assert(partsWithEmpty.Get(3)->GetLength() == 0, "Split keepEmpty: last part should be empty");

    for (int i = 0; i < partsWithEmpty.GetSize(); ++i) {
        delete partsWithEmpty.Get(i);
    }
}

void TestSliceCountZero() {
    int items[3] = {4, 5, 6};
    MutableListSequence<int> seq(items, 3);

    Sequence<int>* sliced = seq.Slice(1, 0);
    int expected[3] = {4, 5, 6};
    AssertIntSequenceEquals(sliced, expected, 3, "Slice count=0");

    delete sliced;
}

void TestSliceRemoveAll() {
    int items[3] = {4, 5, 6};
    ImmutableArraySequence<int> seq(items, 3);

    Sequence<int>* sliced = seq.Slice(0, 3);
    Assert(sliced->GetLength() == 0, "Slice remove all: result must be empty");

    delete sliced;
}

void TestSliceWithReplacement() {
    int items[4] = {1, 2, 3, 4};
    MutableArraySequence<int> seq(items, 4);

    int replacementItems[2] = {9, 8};
    ImmutableListSequence<int> replacement(replacementItems, 2);

    Sequence<int>* sliced = seq.Slice(1, 2, &replacement);
    int expected[4] = {1, 9, 8, 4};
    AssertIntSequenceEquals(sliced, expected, 4, "Slice with replacement");

    delete sliced;
}

void TestZipDifferentLength() {
    int leftItems[3] = {1, 2, 3};
    int rightItems[2] = {10, 20};

    MutableArraySequence<int> left(leftItems, 3);
    MutableListSequence<int> right(rightItems, 2);

    Sequence<Pair<int, int>>* zipped = Zip(left, right);
    Assert(zipped->GetLength() == 2, "Zip: length must be min(len1, len2)");

    Pair<int, int> first = zipped->Get(0);
    Pair<int, int> second = zipped->Get(1);
    Assert(first.first == 1 && first.second == 10, "Zip: wrong first pair");
    Assert(second.first == 2 && second.second == 20, "Zip: wrong second pair");

    Pair<Sequence<int>*, Sequence<int>*> unzipped = Unzip(*zipped);
    int expectedFirst[2] = {1, 2};
    int expectedSecond[2] = {10, 20};
    AssertIntSequenceEquals(unzipped.first, expectedFirst, 2, "Unzip first");
    AssertIntSequenceEquals(unzipped.second, expectedSecond, 2, "Unzip second");

    delete zipped;
    delete unzipped.first;
    delete unzipped.second;
}

void TestUnzipEmpty() {
    MutableArraySequence<Pair<int, int>> zipped;
    Pair<Sequence<int>*, Sequence<int>*> unzipped = Unzip(zipped);

    Assert(unzipped.first->GetLength() == 0, "Unzip empty: first sequence must be empty");
    Assert(unzipped.second->GetLength() == 0, "Unzip empty: second sequence must be empty");

    delete unzipped.first;
    delete unzipped.second;
}

void TestReduceOnEmpty() {
    MutableArraySequence<int> empty;

    bool caught = false;
    try {
        int reduced = empty.Reduce([](int left, int right) { return left + right; });
        (void)reduced;
    } catch (const LabException&) {
        caught = true;
    }

    Assert(caught, "Reduce: empty sequence must throw");
}

bool RunAllTests() {
    int passed = 0;
    int failed = 0;

    struct TestEntry {
        const char* name;
        void (*func)();
    };

    TestEntry tests[] = {
        {"DynamicArray: create/get", TestDynamicArrayCreateAndGet},
        {"DynamicArray: copy", TestDynamicArrayCopy},
        {"DynamicArray: resize/set/get", TestDynamicArrayResizeSetGet},
        {"DynamicArray: index errors", TestDynamicArrayIndexErrors},
        {"LinkedList: append/prepend/insert/get", TestLinkedListAppendPrependInsertGet},
        {"LinkedList: sublist/concat", TestLinkedListSubListAndConcat},
        {"Sequence: abstraction", TestSequenceInterfaceSubsequenceAndConcat},
        {"MutableSequence: semantics", TestMutableSemantics},
        {"ImmutableArraySequence: semantics", TestImmutableArraySemantics},
        {"ImmutableListSequence: semantics", TestImmutableListSemantics},
        {"ImmutableSequence: subsequence", TestImmutableSubsequence},
        {"ICollection: contract", TestICollectionContract},
        {"Operators: []", TestOperatorIndex},
        {"Operators: == / !=", TestOperatorEqualAndNotEqual},
        {"Operators: +", TestOperatorPlus},
        {"Operators: <<", TestOperatorOutput},
        {"Map", TestMap},
        {"Reduce", TestReduce},
        {"Fold", TestFold},
        {"Where", TestWhereAndNoMatches},
        {"Find", TestFind},
        {"Iteration", TestIteration},
        {"Split: no delimiters", TestSplitWithoutDelimiters},
        {"Split: boundaries", TestSplitBoundaries},
        {"Slice: count=0", TestSliceCountZero},
        {"Slice: remove all", TestSliceRemoveAll},
        {"Slice: replacement", TestSliceWithReplacement},
        {"Zip", TestZipDifferentLength},
        {"Unzip: empty", TestUnzipEmpty},
        {"Reduce: empty", TestReduceOnEmpty},
    };

    int testCount = static_cast<int>(sizeof(tests) / sizeof(tests[0]));

    for (int i = 0; i < testCount; ++i) {
        try {
            tests[i].func();
            std::cout << "[PASS] " << tests[i].name << "\n";
            ++passed;
        } catch (const TestFailedException& e) {
            std::cout << "[FAIL] " << tests[i].name << " -> " << e.GetMessage() << "\n";
            ++failed;
        } catch (const LabException& e) {
            std::cout << "[FAIL] " << tests[i].name << " -> unexpected LabException: "
                      << e.GetMessage() << "\n";
            ++failed;
        } catch (...) {
            std::cout << "[FAIL] " << tests[i].name << " -> unknown exception\n";
            ++failed;
        }
    }

    std::cout << "\nTests passed: " << passed << "\n";
    std::cout << "Tests failed: " << failed << "\n";

    return failed == 0;
}

#ifdef TESTS_MAIN
int main() {
    bool ok = RunAllTests();
    return ok ? 0 : 1;
}
#endif
