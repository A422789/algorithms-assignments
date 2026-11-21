#include <cstddef>
#include <utility>
#include <vector>

using namespace std;

void bubbleSort(vector<int>& values) {
    for (size_t end = values.size(); end > 1; --end) {
        bool swapped = false;
        for (size_t i = 1; i < end; ++i) {
            if (values[i - 1] > values[i]) {
                swap(values[i - 1], values[i]);
                swapped = true;
            }
        }

        if (!swapped) {
            break;
        }
    }
}
