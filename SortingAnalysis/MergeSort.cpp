#include <cstddef>
#include <vector>

using namespace std;

namespace {

void mergeSortRange(vector<int>& values, vector<int>& buffer,
                    size_t begin, size_t end) {
    if (end - begin < 2) {
        return;
    }

    const size_t middle = begin + (end - begin) / 2;
    mergeSortRange(values, buffer, begin, middle);
    mergeSortRange(values, buffer, middle, end);

    size_t left = begin;
    size_t right = middle;
    size_t next = begin;

    while (left < middle && right < end) {
        if (values[left] <= values[right]) {
            buffer[next++] = values[left++];
        } else {
            buffer[next++] = values[right++];
        }
    }
    while (left < middle) {
        buffer[next++] = values[left++];
    }
    while (right < end) {
        buffer[next++] = values[right++];
    }

    for (size_t i = begin; i < end; ++i) {
        values[i] = buffer[i];
    }
}

}  // namespace

void mergeSort(vector<int>& values) {
    if (values.size() < 2) {
        return;
    }

    vector<int> buffer(values.size());
    mergeSortRange(values, buffer, 0, values.size());
}
