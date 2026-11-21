#ifndef ANALYSIS_SORTING_ALGORITHMS_H
#define ANALYSIS_SORTING_ALGORITHMS_H

#include <cstdint>
#include <vector>

struct OperationCounts {
    std::uint64_t comparisons = 0;
    std::uint64_t swapsOrMoves = 0;
};

void selectionSort(std::vector<int>& values);
void bubbleSort(std::vector<int>& values);
void quickSort(std::vector<int>& values);
void mergeSort(std::vector<int>& values);

// These functions sort the input and return counts from that single run.
OperationCounts countSelectionSort(std::vector<int>& values);
OperationCounts countBubbleSort(std::vector<int>& values);
OperationCounts countQuickSort(std::vector<int>& values);
OperationCounts countMergeSort(std::vector<int>& values);

#endif
