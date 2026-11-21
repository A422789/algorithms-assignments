# Empirical Analysis of Sorting Algorithms

- **Course** · Analysis of Algorithms
- **Language** · C++17

This assignment compares four sorting algorithms by execution time, element operations, and theoretical auxiliary space. It examines how ascending and descending inputs affect their behavior.

## Algorithms

- **Selection Sort** finds the minimum of the unsorted suffix and moves it into position. Its comparison count depends only on the input size.
- **Bubble Sort** exchanges adjacent elements that are out of order. It stops as soon as a pass makes no swaps.
- **Quick Sort** uses Lomuto partitioning with the last element as the fixed pivot. For distinct ascending and descending inputs, each partition leaves one side empty, giving quadratic time in both tested orders.
- **Merge Sort** splits the input recursively and merges sorted halves into a shared temporary buffer before copying them back.

## Test cases

Each algorithm runs these four cases for a total of 16 results. The program generates each sequence with `std::iota` and reverses it for the descending cases.

| Input size | Input order | Values |
|---:|---|---|
| 5 | Sorted | 1 through 5 |
| 5 | Reverse Sorted | 5 through 1 |
| 100 | Sorted | 1 through 100 |
| 100 | Reverse Sorted | 100 through 1 |

## Time measurement

Each case starts with 256 warm-up sorts. Calibration doubles the repetitions from 512 until a batch takes at least 1 ms or reaches 131,072 repetitions. The program then runs three independent trials with the selected batch size. Batching spreads the cost of two `std::chrono::steady_clock` readings across many sorts because a single sort can take less than a microsecond.

Each batch uses freshly copied inputs. The clock measures the loop and sort calls, including any allocation performed by the algorithm. Input generation, copying, validation, and reporting occur outside the timed interval. Validation compares every output with the expected ascending sequence, which also consumes the results of the timed work. The optimized build compiles the sort implementations as separate translation units. Compile-time branches remove counting code from the timed variants.

Average time is the sum of the three measured batch durations in microseconds divided by the total number of measured sort calls. Warm-up and calibration batches are excluded. All combinations use the same executable, machine, and optimization settings.

## Operation counts

One separate instrumented sort per case supplies the counts. Both timed and instrumented runs must produce the same expected output. A comparison is a comparison between two element values, including comparisons against the Quick Sort pivot. A swap exchanges two distinct positions and counts once. Merge Sort records one move for each element written into the merge buffer or copied back into the input array. Loop checks, self-swaps, buffer initialization, and benchmark copies are excluded from the counts.

## Theoretical complexity

The space classifications describe the algorithms. The benchmark does not profile memory usage.

| Algorithm | Best time | Average time | Worst time | Auxiliary space | In-place |
|---|---|---|---|---|---|
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1), no extra array or recursion | Yes |
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1), no extra array or recursion | Yes |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) average and O(n) worst for recursion, no extra array | Yes, excluding recursion stack |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) overall, with an O(n) buffer and O(log n) recursion stack | No |

## Compile and run

From `AnalysisOfAlgorithms/SortingAnalysis`, compile all five source files with g++.

```sh
g++ -std=c++17 -O2 -Wall -Wextra -pedantic SelectionSort.cpp BubbleSort.cpp QuickSort.cpp MergeSort.cpp Benchmark.cpp -o sorting_benchmark
```

On Linux or macOS, run the executable as follows.

```sh
./sorting_benchmark
```

On Windows PowerShell, use a g++ toolchain that produces Windows executables.

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic SelectionSort.cpp BubbleSort.cpp QuickSort.cpp MergeSort.cpp Benchmark.cpp -o sorting_benchmark.exe
.\sorting_benchmark.exe
```

## Output

The program prints one table per algorithm and generates `results.csv` in the working directory. The CSV has 16 data rows.

| Column | Meaning |
|---|---|
| `Algorithm` | Sorting method |
| `InputSize` | Number of integers |
| `InputOrder` | `Sorted` or `Reverse Sorted` |
| `AverageTimeMicroseconds` | Average elapsed microseconds per sort |
| `Trials` | Number of independent timing trials |
| `OperationsPerTrial` | Number of sort calls in each timed trial |
| `Comparisons` | Element comparisons in one instrumented sort |
| `SwapsOrMoves` | Swaps in one instrumented sort, or moves for Merge Sort |
| `AuxiliarySpace` | Theoretical extra space used by the algorithm |
| `InPlace` | Whether the algorithm sorts in place, with the recursion-stack qualification for Quick Sort |
