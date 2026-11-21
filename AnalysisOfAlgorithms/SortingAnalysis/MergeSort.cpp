#include "SortingAlgorithms.h"

#include <cstddef>

using namespace std;

namespace {

template <bool CountOperations>
void sortAndMerge(vector<int>& values, vector<int>& temporary,
                  size_t first, size_t last, OperationCounts* counts) {
    if (last - first < 2) {
        return;
    }

    const size_t middle = first + (last - first) / 2;
    sortAndMerge<CountOperations>(values, temporary, first, middle, counts);
    sortAndMerge<CountOperations>(values, temporary, middle, last, counts);

    // Compare the first remaining element of each half until one runs out.
    size_t left = first;
    size_t right = middle;
    size_t output = first;
    while (left < middle && right < last) {
        if constexpr (CountOperations) {
            ++counts->comparisons;
        }
        if (values[left] <= values[right]) {
            temporary[output++] = values[left++];
        } else {
            temporary[output++] = values[right++];
        }
    }

    while (left < middle) {
        temporary[output++] = values[left++];
    }
    while (right < last) {
        temporary[output++] = values[right++];
    }
    for (size_t i = first; i < last; ++i) {
        values[i] = temporary[i];
    }

    if constexpr (CountOperations) {
        // Every merged element is written into the buffer and copied back once.
        counts->swapsOrMoves += 2 * static_cast<uint64_t>(last - first);
    }
}

template <bool CountOperations>
void sortWithBuffer(vector<int>& values, OperationCounts* counts) {
    if (values.size() < 2) {
        return;
    }

    // Reuse one buffer for all recursive merges in this sort call.
    vector<int> temporary(values.size());
    sortAndMerge<CountOperations>(values, temporary, 0, values.size(), counts);
}

}  // namespace

void mergeSort(vector<int>& values) {
    sortWithBuffer<false>(values, nullptr);
}

OperationCounts countMergeSort(vector<int>& values) {
    OperationCounts counts;
    sortWithBuffer<true>(values, &counts);
    return counts;
}
