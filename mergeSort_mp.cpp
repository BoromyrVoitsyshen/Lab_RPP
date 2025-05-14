#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <sstream>
#include <algorithm>

using namespace std;

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }

    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (int t = 0; t < temp.size(); ++t) {
        arr[left + t] = temp[t];
    }
}

void merge_sort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int mid = (left + right) / 2;
    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

void parallel_merge_sort(vector<int>& arr, int num_threads) {
    int n = arr.size();
    int chunk_size = n / num_threads;

#pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int end = (tid == num_threads - 1) ? n - 1 : (start + chunk_size - 1);

        merge_sort(arr, start, end);
    }

    for (int size = chunk_size; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = min(left + size - 1, n - 1);
            int right = min(left + 2 * size - 1, n - 1);
            if (mid < right) merge(arr, left, mid, right);
        }
    }
}

vector<int> read_input(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) throw runtime_error("Can't open input file!");

    int n;
    in >> n;
    vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        in >> arr[i];
    }
    return arr;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: <program> <num_threads> <input_file>\n";
        return 1;
    }

    int num_threads;
    istringstream(argv[1]) >> num_threads;
    string filename = argv[2];

    try {
        vector<int> data = read_input(filename);

        auto start = chrono::high_resolution_clock::now();
        parallel_merge_sort(data, num_threads);
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;

        cout << "Time: " << elapsed.count() << " seconds\n";
        cout << "Threads: " << num_threads << "\n";
        cout << "First 10 sorted elements: ";
        for (int i = 0; i < min(10, (int)data.size()); ++i)
            cout << data[i] << " ";
        cout << "\n";
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
