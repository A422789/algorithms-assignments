#include "SortingAlgorithms.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <numeric>
#include <ratio>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace std;

namespace {

using Clock = chrono::steady_clock;
using Microseconds = chrono::duration<double, micro>;

constexpr size_t trialCount = 3;
constexpr size_t warmUpRepetitions = 256;
constexpr size_t initialRepetitions = 512;
constexpr size_t maximumRepetitions = 131072;
constexpr auto targetDuration = chrono::milliseconds(1);

struct Algorithm {
    string_view name;
    void (*sort)(vector<int>&);
    OperationCounts (*sortAndCount)(vector<int>&);
    string_view auxiliarySpace;
    string_view inPlace;
};

constexpr array<Algorithm, 4> algorithms{{
    {"Selection Sort", selectionSort, countSelectionSort, "O(1)", "Yes"},
    {"Bubble Sort", bubbleSort, countBubbleSort, "O(1)", "Yes"},
    {"Quick Sort", quickSort, countQuickSort, "O(log n) average, O(n) worst",
     "Yes, excluding recursion stack"},
    {"Merge Sort", mergeSort, countMergeSort, "O(n)", "No"},
}};

struct TestCase {
    string_view order;
    vector<int> original;
    vector<int> expected;
};

using TestCases = array<TestCase, 4>;

struct Timing {
    double averageMicroseconds;
    size_t repetitions;
};

struct CaseResult {
    size_t size;
    string_view order;
    Timing timing;
    OperationCounts counts;
};

struct AlgorithmResults {
    Algorithm algorithm;
    array<CaseResult, 4> cases;
};

TestCase makeTestCase(size_t size, bool reverseSorted) {
    TestCase test{reverseSorted ? "Reverse Sorted" : "Sorted", {}, vector<int>(size)};
    iota(test.expected.begin(), test.expected.end(), 1);
    test.original = test.expected;
    if (reverseSorted) {
        reverse(test.original.begin(), test.original.end());
    }
    return test;
}

void verifyOutput(const Algorithm& algorithm, const TestCase& test,
                  const vector<int>& output) {
    if (output != test.expected) {
        throw runtime_error(string(algorithm.name) + " returned incorrect output for " +
                                 string(test.order) + " N=" +
                                 to_string(test.original.size()));
    }
}

Clock::duration timeSortBatch(const Algorithm& algorithm, const TestCase& test,
                              size_t repetitions) {
    // Prepare a fresh input for every sort before starting the timer.
    vector<vector<int>> batch(repetitions, test.original);
    const auto start = Clock::now();
    for (auto& values : batch) {
        algorithm.sort(values);
    }
    const auto elapsed = Clock::now() - start;

    // Validate every result and make the effects of each sort observable.
    for (const auto& values : batch) {
        verifyOutput(algorithm, test, values);
    }
    return elapsed;
}

Timing measureTime(const Algorithm& algorithm, const TestCase& test) {
    timeSortBatch(algorithm, test, warmUpRepetitions);

    size_t repetitions = initialRepetitions;
    while (repetitions < maximumRepetitions &&
           timeSortBatch(algorithm, test, repetitions) < targetDuration) {
        repetitions *= 2;
    }

    auto totalTime = Clock::duration::zero();
    for (size_t trial = 0; trial < trialCount; ++trial) {
        totalTime += timeSortBatch(algorithm, test, repetitions);
    }
    return {Microseconds(totalTime).count() / static_cast<double>(trialCount * repetitions),
            repetitions};
}

AlgorithmResults benchmark(const Algorithm& algorithm, const TestCases& tests) {
    AlgorithmResults results{algorithm, {}};
    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        const auto timing = measureTime(algorithm, test);

        // Count operations on one separate copy and verify it against the same expected result.
        auto countedOutput = test.original;
        const auto counts = algorithm.sortAndCount(countedOutput);
        verifyOutput(algorithm, test, countedOutput);

        results.cases[i] = {test.original.size(), test.order, timing, counts};
    }
    return results;
}

void writeCsv(const array<AlgorithmResults, algorithms.size()>& results) {
    ofstream csv("results.csv");
    if (!csv) {
        throw runtime_error("Could not open results.csv for writing");
    }
    csv.imbue(locale::classic());
    csv << "Algorithm,InputSize,InputOrder,AverageTimeMicroseconds,Trials,OperationsPerTrial,"
           "Comparisons,SwapsOrMoves,AuxiliarySpace,InPlace\n"
        << fixed << setprecision(6);

    for (const auto& group : results) {
        for (const auto& row : group.cases) {
            csv << group.algorithm.name << ',' << row.size << ',' << row.order << ','
                << row.timing.averageMicroseconds << ',' << trialCount << ',' << row.timing.repetitions
                << ',' << row.counts.comparisons << ',' << row.counts.swapsOrMoves
                << ",\"" << group.algorithm.auxiliarySpace << "\",\"" << group.algorithm.inPlace
                << "\"\n";
        }
    }

    csv.close();
    if (!csv) {
        throw runtime_error("Could not finish writing results.csv");
    }
}

void printTable(const AlgorithmResults& results) {
    cout << '\n' << results.algorithm.name << '\n'
              << left << setw(17) << "Input order" << right << setw(5) << "N"
              << setw(13) << "Mean (us)" << setw(8) << "Trials"
              << setw(12) << "Reps/trial" << setw(14) << "Comparisons"
              << setw(14) << "Swaps/moves" << '\n'
              << string(83, '-') << '\n' << fixed << setprecision(6);

    for (const auto& row : results.cases) {
        cout << left << setw(17) << row.order << right << setw(5) << row.size
                  << setw(13) << row.timing.averageMicroseconds << setw(8) << trialCount
                  << setw(12) << row.timing.repetitions << setw(14) << row.counts.comparisons
                  << setw(14) << row.counts.swapsOrMoves << '\n';
    }
}

}  // namespace

int main() {
    try {
        const TestCases tests{makeTestCase(5, false), makeTestCase(5, true),
                              makeTestCase(100, false), makeTestCase(100, true)};
        array<AlgorithmResults, algorithms.size()> results{};
        for (size_t i = 0; i < algorithms.size(); ++i) {
            results[i] = benchmark(algorithms[i], tests);
        }

        writeCsv(results);
        for (const auto& group : results) {
            printTable(group);
        }
        cout << "\nWrote " << algorithms.size() * tests.size() << " verified results to results.csv.\n";
        return 0;
    } catch (const exception& error) {
        cerr << "Benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
