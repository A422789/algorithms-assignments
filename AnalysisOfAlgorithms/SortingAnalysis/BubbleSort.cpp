#include "SortingAlgorithms.h"

#include <cstddef>
#include <utility>

using namespace std;

namespace {

template <bool CountOperations>
void sortByBubbling(vector<int>& values, OperationCounts* counts) {
    for (size_t limit = values.size(); limit > 1; --limit) {
        bool swapped = false;
        for (size_t i = 1; i < limit; ++i) {
            if constexpr (CountOperations) {
                ++counts->comparisons;
            }
            if (values[i] < values[i - 1]) {
                swap(values[i - 1], values[i]);
                swapped = true;
                if constexpr (CountOperations) {
                    ++counts->swapsOrMoves;
                }
            }
        }

        // A pass with no exchanges means the remaining prefix is already sorted.
        if (!swapped) {
            return;
        }
    }
}

}  // namespace

void bubbleSort(vector<int>& values) {
    sortByBubbling<false>(values, nullptr);
}

OperationCounts countBubbleSort(vector<int>& values) {
    OperationCounts counts;
    sortByBubbling<true>(values, &counts);
    return counts;
}
