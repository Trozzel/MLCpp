#include <iostream>
#include <chrono>
#include <type_traits>
#include <iterator>

#include "DataHandler.hpp"
#include "knn.hpp"

namespace fs = std::filesystem;

using namespace std;

auto getNow() {
	return chrono::steady_clock::now();
}

auto getDuration(chrono::steady_clock::time_point t2, 
				 chrono::steady_clock::time_point t1) {
	return chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
}

// Floating point display number
template<typename T, uint32_t Rows_Cols = 28>
void displayNumber(const Features<T>& features, enable_if_t<is_floating_point_v<T>, void**> = nullptr) {
    for (int i = 0; i < Rows_Cols; ++i) {
        for (int j = 0; j < Rows_Cols; ++j) {
            cout << setw(4) << (int)(features[i * Rows_Cols +j ] * 255);
        } cout << endl;
    }
}

// Integral point display
template<typename T, uint32_t Rows_Cols = 28>
void displayNumber(const Features<T>& features, enable_if_t<is_integral_v<T>, void**> = nullptr) {
    for (int i = 0; i < Rows_Cols; ++i) {
        for (int j = 0; j < Rows_Cols; ++j) {
            cout << setw(4) << (int)features[i * Rows_Cols + j] << " ";
        } cout << endl;
    }
    cout << string(5 * Rows_Cols, '-') << endl;
}

int main(){
    // 1. LOAD DATA INTO DATA HANDLER
    const fs::path dataPath = "../../data/";
	auto start = getNow();
	DataHandler<uint8_t> dh;

    dh.loadMnistData(dataPath   / "train-images-idx3-ubyte");
    dh.loadMnistLabels(dataPath / "train-labels-idx1-ubyte");
    dh.loadMnistData(dataPath   / "t10k-images-idx3-ubyte");
    dh.loadMnistLabels(dataPath / "t10k-labels-idx1-ubyte");
    auto finish = getNow();
    cout << "Total time reading dataSet: " << getDuration(getNow(), start)
         << " ms" << endl;
    // 2. SPLIT DATA INTO 3 DATA SETS
    dh.splitData();

    // 3. ASSIGN KNN TO APPROPRIATE DATASETS
    Knn<uint8_t, 5> knn;
    knn.trainingData(dh.trainingData());
    knn.testData(dh.testData());
    knn.validationData(dh.validationData());

    cout << string (80, '*') << endl;
    cout << "SIZE OF DATASETS:\n";
    cout << "Size of training data:   " << knn.trainingData().size() << endl;
    cout << "Size of test data:       " << knn.testData().size() << endl;
    cout << "Size of validation data: " << knn.validationData().size() << endl;

    // 4. RUN VALIDATION PERFORMANCE TESTING
    cout << endl;
    cout << string(80, '*' ) << endl;
    cout << "BEGINNING VALIDATION SET VALIDATION..." << endl;
    knn.valSetPerformance(DistanceType::Euclidean);
}

