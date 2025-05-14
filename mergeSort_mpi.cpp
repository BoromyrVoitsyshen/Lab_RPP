//#include <mpi.h>
//#include <iostream>
//#include <vector>
//#include <fstream>
//#include <sstream>
//#include <algorithm>
//#include <chrono>
//
//using namespace std;
//
//vector<int> read_input(const string& filename) {
//    ifstream in(filename);
//    if (!in.is_open()) throw runtime_error("Can't open input file!");
//
//    int n;
//    in >> n;
//    vector<int> arr(n);
//    for (int i = 0; i < n; ++i) {
//        in >> arr[i];
//    }
//    return arr;
//}
//
//void merge(vector<int>& result, const vector<int>& left, const vector<int>& right) {
//    result.clear();
//    size_t i = 0, j = 0;
//    while (i < left.size() && j < right.size()) {
//        if (left[i] <= right[j]) result.push_back(left[i++]);
//        else result.push_back(right[j++]);
//    }
//    while (i < left.size()) result.push_back(left[i++]);
//    while (j < right.size()) result.push_back(right[j++]);
//}
//
//int main(int argc, char* argv[]) {
//    MPI_Init(&argc, &argv);
//
//    int rank, size;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//
//    vector<int> full_data;
//    int total_elements = 0;
//
//    if (rank == 0) {
//        if (argc != 2) {
//            cerr << "Usage: mpiexec -n <num_processes> <program> <input_file>\n";
//            MPI_Abort(MPI_COMM_WORLD, 1);
//        }
//
//        string filename = argv[1];
//        full_data = read_input(filename);
//        total_elements = (int)full_data.size();
//    }
//
//    // Розсилка кількості елементів
//    MPI_Bcast(&total_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//    // Розрахунок скільки елементів дістається кожному процесу
//    int base = total_elements / size;
//    int extra = total_elements % size;
//    vector<int> send_counts(size), displs(size);
//
//    int offset = 0;
//    for (int i = 0; i < size; ++i) {
//        send_counts[i] = base + (i < extra ? 1 : 0);
//        displs[i] = offset;
//        offset += send_counts[i];
//    }
//
//    vector<int> local_data(send_counts[rank]);
//
//    auto start = chrono::high_resolution_clock::now();
//
//    MPI_Scatterv(
//        full_data.data(), send_counts.data(), displs.data(), MPI_INT,
//        local_data.data(), send_counts[rank], MPI_INT,
//        0, MPI_COMM_WORLD
//    );
//
//    sort(local_data.begin(), local_data.end());
//
//    vector<int> gathered_data;
//    if (rank == 0) gathered_data.resize(total_elements);
//
//    MPI_Gatherv(
//        local_data.data(), send_counts[rank], MPI_INT,
//        gathered_data.data(), send_counts.data(), displs.data(), MPI_INT,
//        0, MPI_COMM_WORLD
//    );
//
//    if (rank == 0) {
//        // Злиття всіх відсортованих блоків вручну
//        vector<int> final_sorted = vector<int>(send_counts[0]);
//        copy(gathered_data.begin(), gathered_data.begin() + send_counts[0], final_sorted.begin());
//
//        for (int i = 1; i < size; ++i) {
//            vector<int> temp_block(send_counts[i]);
//            copy(gathered_data.begin() + displs[i], gathered_data.begin() + displs[i] + send_counts[i], temp_block.begin());
//
//            vector<int> merged;
//            merge(merged, final_sorted, temp_block);
//            final_sorted.swap(merged);
//        }
//
//        auto end = chrono::high_resolution_clock::now();
//        chrono::duration<double> elapsed = end - start;
//
//        cout << "Time: " << elapsed.count() << " seconds\n";
//        cout << "Processes: " << size << "\n";
//        cout << "First 10 sorted elements: ";
//        for (int i = 0; i < min(10, (int)final_sorted.size()); ++i)
//            cout << final_sorted[i] << " ";
//        cout << "\n";
//    }
//
//    MPI_Finalize();
//    return 0;
//}
