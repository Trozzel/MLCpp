#ifndef __KNN_HPP__
#define __KNN_HPP__

#include <vector>
#include <numeric>   // for std::accumulate
#include <algorithm> // for std::transform
#include <future>
#include <set>
#include "Features.hpp"
#include "DataHandler.hpp"

enum class DistanceType {
    Euclidean,
    Manhattan
};

constexpr DistanceType defaultDistanceType = DistanceType::Euclidean;

template<typename T, int K = 5>
class Knn {
private:
	int _k = K;
	std::span<Features<T>>    _trainingData;
	std::span<Features<T>>    _testData;
	std::span<Features<T>>    _validationData;
    DistanceType _distanceType {defaultDistanceType};
    std::mutex _stdoutMtx;

private:
    std::span<Features<T>> dataSet(DataSetType dataSetType) {
        switch (dataSetType) {
            case DataSetType::Training:
                return _trainingData;
            case DataSetType::Test:
                return _testData;
            case DataSetType::Validation:
                return _validationData;
        }
    }

    // Euclidean distance
    static double _euclidean(const Features<T>& queryPt, const Features<T>& input)  {
        double total = 0.0;
        for(int i = 0; i < queryPt.size(); ++i) {
            total += std::pow(queryPt[i] - input[i], 2);
        }
        return std::sqrt(total);
    }

    // Manhattan distance
    static double _manhattan(const Features<T>& queryPt, const Features<T>& input)  {
        double total = 0.0;
        for (int i = 0; i < queryPt.size(); ++i) {
            total += std::abs(queryPt[i] - input[i]);
        }
        return total;
    }

    /// Calculate total performance of given dataset
    /// @details spawns 'hardware_concurrency()' threads and performs nearest neighbors and
    /// compares their labels to the label of queryPt
    /// @param pointer to Features<T> against which to test
    /// @param dsType Dataset to calculate performance on
    double _calcPerformance(DataSetType dsType){
        // 1. ASSIGN THE CORRECT DATASET
        std::span<Features<T>> ds = dataSet(dsType);
        char dsString[20];
        switch  (dsType) {
            case DataSetType::Training:
                strcpy(dsString, "training");
                break;
            case DataSetType::Test:
                strcpy(dsString, "test");
                break;
            case DataSetType::Validation:
                strcpy(dsString, "validation");
                break;
        }

        // 3. LAMBDA FOR CALCULATING PREDICTION ACCURACY
        auto performance = [=, &ds](size_t beg, size_t end) -> double {
            int numCorrect{0};
            int dsIdx {0};
            size_t arraySize = end - beg;

            std::vector<Features<T>*> neighbors(_k);
            for (; beg < end; ++beg) {
                neighbors = findKNearest(ds[beg], neighbors);
                if(predict(neighbors) == ds[beg].label()) {
                    ++numCorrect;
                }
                // Display updated accuracy
                if((dsIdx+1) % 100 == 0 && dsIdx != 0) {
                    std::unique_lock<std::mutex> lck(_stdoutMtx);
                    std::cout << dsString << "[" << dsIdx << "] - Current performance for thread["
                              << std::this_thread::get_id() << "]: " << std::fixed << std::setprecision(3)
                              << (double)(100. * (double) numCorrect / (double) (dsIdx+1)) << "%\n";
//                    printf("%s[%05d] - Current performance for thread[%lu]: %6.3f%%\n",
//                           dsString,
//                           dsIdx,
//                           std::this_thread::get_id(),
//                           100. * (double) numCorrect / (double) (dsIdx+1));
                }
                ++dsIdx;
            }
            return (double)numCorrect / (double)(arraySize);
        };

        // 4. LAUNCH THREADS FOR PERFORMING CALCULATIONS
        const unsigned int numFutures = std::thread::hardware_concurrency();
        size_t N = ds.size();
        std::future<double> futures[numFutures];
        for (int i = 0; i < numFutures; ++i) {
            futures[i] = std::async(performance, size_t(N/numFutures)*i, size_t(N/numFutures)*(i+1));
        }
        using namespace std::chrono;
        auto start  = steady_clock::now();
        double accuracy = 0.0;
        for(auto & future : futures) {
            accuracy += future.get();
        }
        accuracy /= numFutures;
        auto finish = steady_clock::now();

        printf("%s accuracy for K = %d: %6.3f %%\n",
               dsString, _k, 100. * accuracy);
        printf("Elapsed time for calculating %lu feature arrays: %.3f seconds.\n",
               ds.size(),
               (float)duration_cast<milliseconds>(finish-start).count() / 1000.f);
        return accuracy;
    }

public:
	// CONSTRUCTORS
    /**************************************************************************/
	Knn() = default;
    ~Knn() = default;

	// Data setters / getters
    /**************************************************************************/

    auto trainingData() const { return _trainingData; }
	inline void trainingData(std::span<Features<T>> ds)   { _trainingData = ds;}
    auto testData() const { return _testData; }
	inline void testData(std::span<Features<T>>  ds)       { _testData = ds;}
    auto validationData() const { return _validationData; }
	inline void validationData(std::span<Features<T>>  ds) { _validationData = ds;}

    int kValue() { return _k; }
	inline void kValue(int k) { _k = k;}
    [[nodiscard]]
    DistanceType distanceType() const {return _distanceType;}
    inline void distanceType(DistanceType distanceType) { _distanceType = distanceType; }

	// PERFORM CALCULATIONS
    /// Find the k nearest neighbors
    /// For use on single queryPt  and not for use within overall analysis since
    /// it returns a copied vector of pointers on each call
    /// @param queryPt the set of features
    /// @return copy of vector\<Features\<T\>*\>
	auto findKNearest(const Features<T>& queryPt) {
        std::vector<Features<T>*> neighbors(_k);
        double distMinPrev = -1.;
        // Find K amount of closest neighbors
        for(int kIdx=0; kIdx < _k; ++kIdx) {
            int idx = 0;
            double distMin = std::numeric_limits<double>::max();
            for(int i = 0; i < _trainingData.size(); ++i) {
                double dist = calcDistance(_trainingData[i], queryPt);
                if (dist < distMin && dist > distMinPrev) {
                    idx = i;
                    distMin = dist;
                }
            }
            distMinPrev = distMin;
            neighbors[kIdx] = &_trainingData[idx];
        }
        return neighbors;
    }

    /// Find the k nearest neighbors
    /// @param queryPt the set of features
    /// @param reference to neighbors vector with preallocated memory
    /// @return reference of vector\<Features\<T\>*\>
    auto & findKNearest(const Features<T>& queryPt, std::vector<Features<T>*>& neighbors) {
        double distMinPrev = -1.;
        // Find K amount of closest neighbors
        for(int kIdx=0; kIdx < _k; ++kIdx) {
            int idx = 0;
            double distMin = std::numeric_limits<double>::max();
            for(int i = 0; i < _trainingData.size(); ++i) {
                double dist = calcDistance(_trainingData[i], queryPt);
                if (dist < distMin && dist > distMinPrev) {
                    idx = i;
                    distMin = dist;
                }
            }
            distMinPrev = distMin;
            neighbors[kIdx] = &_trainingData[idx];
        }
        return neighbors;
    }

    /// Predict which class the given set of features belongs to
    /// @details Compares the max element of label of k-neighbors to the actual label
    /// @param neighbors vector of pointers which access the k-neighbors Features sets
    static int predict(const std::vector<Features<T>*>& neighbors) {
        std::map<T, int> classFreq;
        for(const auto & neighbor : neighbors) {
            classFreq[neighbor->label()]++;
        }
        auto iter = std::max_element(classFreq.begin(), classFreq.end(),
                                     [](const auto & kv1, const auto & kv2) {
                                         return kv1.second < kv2.second;
                                     });

        return iter->first;
    }

    double calcDistance(const Features<T>& queryPt, const Features<T>& input)
    {
        assert(queryPt.size() == input.size());
        switch (_distanceType) {
            case DistanceType::Euclidean: {
                return _euclidean(queryPt, input);
            }
            case DistanceType::Manhattan: {
                return _manhattan(queryPt, input);
            }
        }
    }

	double trainSetPerformance(DistanceType distanceType = defaultDistanceType) {
        _distanceType = distanceType;
        return _calcPerformance(DataSetType::Training);
    }

    double valSetPerformance(DistanceType distanceType = defaultDistanceType) {
        _distanceType = distanceType;
        return _calcPerformance(DataSetType::Validation);
    }

	double testSetPerformance(DistanceType distanceType = defaultDistanceType) {
        _distanceType = distanceType;
        return _calcPerformance(DataSetType::Test);
    }
};

#endif //  __KNN_HPP__
