#include "SortingAlgorithms.h"

#include <cstddef>
#include <utility>

using namespace std;

namespace {

template <bool CountOperations>
size_t lomutoPartition(vector<int>& values, size_t first,
                            size_t last, OperationCounts* counts) {
    // The fixed pivot is the final element of the half-open range [first, last).
    const size_t pivotIndex = last - 1;
    const int pivot = values[pivotIndex];
    size_t boundary = first;

    // Grow the region of elements less than or equal to the pivot.
    for (size_t i = first; i < pivotIndex; ++i) {
        if constexpr (CountOperations) {
            ++counts->comparisons;
        }
        if (values[i] <= pivot) {
            if (i != boundary) {
                swap(values[i], values[boundary]);
                if constexpr (CountOperations) {
                    ++counts->swapsOrMoves;
                }
            }
            ++boundary;
        }
    }

    if (boundary != pivotIndex) {
        swap(values[boundary], values[pivotIndex]);
        if constexpr (CountOperations) {
            ++counts->swapsOrMoves;
        }
    }
    return boundary;
}

template <bool CountOperations>
void sortPartitions(vector<int>& values, size_t first,
                    size_t last, OperationCounts* counts) {
    if (last - first < 2) {
        return;
    }

    const auto pivot = lomutoPartition<CountOperations>(values, first, last, counts);
    sortPartitions<CountOperations>(values, first, pivot, counts);
    sortPartitions<CountOperations>(values, pivot + 1, last, counts);
}

}  // namespace

void quickSort(vector<int>& values) {
    sortPartitions<false>(values, 0, values.size(), nullptr);
}

OperationCounts countQuickSort(vector<int>& values) {
    OperationCounts counts;
    sortPartitions<true>(values, 0, values.size(), &counts);
    return counts;
}
