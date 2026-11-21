#include "SortingAlgorithms.h"

#include <cstddef>
#include <utility>

using namespace std;

namespace {

template <bool CountOperations>
void sortBySelection(vector<int>& values, OperationCounts* counts) {
    for (size_t first = 0; first + 1 < values.size(); ++first) {
        // Find the smallest element in the unsorted suffix.
        size_t minimum = first;
        for (size_t i = first + 1; i < values.size(); ++i) {
            if constexpr (CountOperations) {
                ++counts->comparisons;
            }
            if (values[i] < values[minimum]) {
                minimum = i;
            }
        }

        if (minimum != first) {
            swap(values[first], values[minimum]);
            if constexpr (CountOperations) {
                ++counts->swapsOrMoves;
            }
        }
    }
}

}  // namespace

void selectionSort(vector<int>& values) {
    sortBySelection<false>(values, nullptr);
}

OperationCounts countSelectionSort(vector<int>& values) {
    OperationCounts counts;
    sortBySelection<true>(values, &counts);
    return counts;
}
