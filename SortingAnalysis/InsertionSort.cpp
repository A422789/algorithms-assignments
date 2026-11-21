#include <cstddef>
#include <vector>

using namespace std;

void insertionSort(vector<int>& values) {
    for (size_t i = 1; i < values.size(); ++i) {
        const int value = values[i];
        size_t position = i;

        while (position > 0 && values[position - 1] > value) {
            values[position] = values[position - 1];
            --position;
        }

        values[position] = value;
    }
}
