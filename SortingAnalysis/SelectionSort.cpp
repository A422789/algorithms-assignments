#include <cstddef>
#include <utility>
#include <vector>

using namespace std;

void selectionSort(vector<int>& values) {
    for (size_t first = 0; first + 1 < values.size(); ++first) {
        size_t smallest = first;
        for (size_t i = first + 1; i < values.size(); ++i) {
            if (values[i] < values[smallest]) {
                smallest = i;
            }
        }

        if (smallest != first) {
            swap(values[first], values[smallest]);
        }
    }
}
