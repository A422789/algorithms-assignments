# Empirical Analysis of Sorting Algorithms

This C++17 assignment compares the measured execution time of optimized Bubble Sort, Selection Sort, Insertion Sort, and recursive Merge Sort. Each algorithm sorts arrays of 5, 10, 50, and 100 integers in place through a `std::vector<int>&` interface. Merge Sort uses an auxiliary buffer for merging.

## Inputs

- **`Sorted`** uses `std::iota` to generate the ascending sequence from 1 through the input size.
- **`WorstCase`** uses descending sequences for Bubble Sort and Insertion Sort. Selection Sort also uses descending order and performs the same `n(n − 1) / 2` comparisons for any ordering of a given size. For Merge Sort, successive ranks alternate between the two halves at each recursive split, so the merge steps repeatedly compare values from both halves before either is exhausted.

## Build and run

From the `SortingAnalysis` directory, compile with g++ and run the benchmark.

```sh
g++ -std=c++17 -O2 -Wall -Wextra -pedantic BubbleSort.cpp SelectionSort.cpp InsertionSort.cpp MergeSort.cpp Benchmark.cpp -o sorting_benchmark
./sorting_benchmark
```

On Windows PowerShell, run the executable with `.\sorting_benchmark.exe`.

## Measurement and results

Each combination starts with a 256-operation warm-up. Calibration doubles the batch size from 1,024 until a batch takes at least 1 millisecond or reaches 65,536 operations. Seven measurement trials then use that same batch size. Before each batch, the program restores every vector from the original input. `std::chrono::steady_clock` times the loop over those fresh copies, including the sort calls and any allocations inside them. Input preparation, validation, and output happen outside the timed interval. After each batch, the program checks every sorted vector with `std::is_sorted` and compares it with the expected ascending sequence. Those checks consume the results of the timed work.

The program divides the total time across the seven trials by their total number of sort calls. It prints a summary table and writes `results.csv` in the working directory. The CSV contains 32 rows, one per algorithm, input case, and input size. `AverageTimeMicroseconds` is the time per sort. `Trials` and `OperationsPerTrial` give the measurement counts for that row. Timings vary with the compiler, hardware, and system load.

## Theoretical time complexity

| Algorithm | Best Case | Average Case | Worst Case |
|---|---|---|---|
| Bubble Sort (Optimized) | O(n) | O(n²) | O(n²) |
| Selection Sort | O(n²) | O(n²) | O(n²) |
| Insertion Sort | O(n) | O(n²) | O(n²) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) |
