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
#include <string_view>
#include <vector>

using namespace std;

void bubbleSort(vector<int>& values);
void selectionSort(vector<int>& values);
void insertionSort(vector<int>& values);
void mergeSort(vector<int>& values);

namespace {

using SortFunction = void (*)(vector<int>&);
using Clock = chrono::steady_clock;
using Microseconds = chrono::duration<double, micro>;

constexpr size_t measurementTrials = 7;
constexpr size_t warmupOperations = 256;
constexpr size_t initialOperations = 1024;
constexpr size_t maximumOperations = 65536;
constexpr Microseconds targetTrialDuration{1000.0};

struct Algorithm {
    string_view name;
    SortFunction sort;
};

struct Measurement {
    double averageMicroseconds;
    size_t operationsPerTrial;
};

struct Result {
    string_view algorithm;
    string_view inputCase;
    size_t inputSize;
    Measurement measurement;
};

vector<int> interleaveForMergeSort(const vector<int>& sorted) {
    if (sorted.size() < 2) {
        return sorted;
    }

    const size_t middle = sorted.size() / 2;
    const size_t leftRankParity = sorted.size() % 2;
    vector<int> left;
    vector<int> right;
    left.reserve(middle);
    right.reserve(sorted.size() - middle);

    // Alternate ranks, starting on the right when that half has one extra element.
    for (size_t rank = 0; rank < sorted.size(); ++rank) {
        if (rank % 2 == leftRankParity) {
            left.push_back(sorted[rank]);
        } else {
            right.push_back(sorted[rank]);
        }
    }

    auto input = interleaveForMergeSort(left);
    const auto rightInput = interleaveForMergeSort(right);
    input.insert(input.end(), rightInput.begin(), rightInput.end());
    return input;
}

Microseconds timeBatch(SortFunction sort, vector<vector<int>>& batch,
                       const vector<int>& input, const vector<int>& expected) {
    for (auto& values : batch) {
        values = input;
    }

    const auto start = Clock::now();
    for (auto& values : batch) {
        sort(values);
    }
    const auto finish = Clock::now();

    for (const auto& values : batch) {
        if (!is_sorted(values.begin(), values.end()) || values != expected) {
            throw runtime_error("A sorting operation returned an incorrect result.");
        }
    }

    return finish - start;
}

Measurement measure(SortFunction sort, const vector<int>& input,
                    const vector<int>& expected) {
    vector<vector<int>> batch(warmupOperations);
    timeBatch(sort, batch, input, expected);

    batch.resize(initialOperations);
    while (timeBatch(sort, batch, input, expected) < targetTrialDuration &&
           batch.size() < maximumOperations) {
        batch.resize(batch.size() * 2);
    }

    Microseconds total{};
    for (size_t trial = 0; trial < measurementTrials; ++trial) {
        total += timeBatch(sort, batch, input, expected);
    }

    const auto operations = measurementTrials * batch.size();
    return {total.count() / static_cast<double>(operations), batch.size()};
}

void writeResults(const vector<Result>& results) {
    ofstream csv("results.csv");
    if (!csv) {
        throw runtime_error("Could not open results.csv for writing.");
    }

    csv.imbue(locale::classic());
    csv << "Algorithm,InputCase,InputSize,AverageTimeMicroseconds,Trials,OperationsPerTrial\n"
        << fixed << setprecision(6);

    cout << left << setw(17) << "Algorithm" << setw(13) << "Input case"
              << right << setw(6) << "Size" << setw(15) << "Average (us)"
              << setw(8) << "Trials" << setw(16) << "Ops per trial" << '\n'
              << "---------------------------------------------------------------------------\n"
              << fixed << setprecision(6);

    for (const auto& result : results) {
        const auto& measurement = result.measurement;
        csv << result.algorithm << ',' << result.inputCase << ',' << result.inputSize << ','
            << measurement.averageMicroseconds << ',' << measurementTrials << ','
            << measurement.operationsPerTrial << '\n';

        cout << left << setw(17) << result.algorithm
                  << setw(13) << result.inputCase << right
                  << setw(6) << result.inputSize << setw(15) << measurement.averageMicroseconds
                  << setw(8) << measurementTrials << setw(16) << measurement.operationsPerTrial
                  << '\n';
    }

    csv.close();
    if (!csv) {
        throw runtime_error("Could not finish writing results.csv.");
    }
    cout << "\nWrote " << results.size() << " records to results.csv.\n";
}

}  // namespace

int main() {
    constexpr array<Algorithm, 4> algorithms{{
        {"Bubble Sort", bubbleSort},
        {"Selection Sort", selectionSort},
        {"Insertion Sort", insertionSort},
        {"Merge Sort", mergeSort},
    }};
    constexpr array<string_view, 2> inputCases{"Sorted", "WorstCase"};
    constexpr array<size_t, 4> inputSizes{5, 10, 50, 100};

    try {
        vector<Result> results;
        for (const auto& algorithm : algorithms) {
            for (const auto inputCase : inputCases) {
                for (const auto size : inputSizes) {
                    vector<int> sorted(size);
                    iota(sorted.begin(), sorted.end(), 1);

                    auto input = sorted;
                    if (inputCase == "WorstCase") {
                        if (algorithm.sort == mergeSort) {
                            input = interleaveForMergeSort(sorted);
                        } else {
                            reverse(input.begin(), input.end());
                        }
                    }

                    results.push_back({algorithm.name, inputCase, size,
                                       measure(algorithm.sort, input, sorted)});
                }
            }
        }

        writeResults(results);
        return 0;
    } catch (const exception& error) {
        cerr << "Benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
