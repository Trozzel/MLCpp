//
// Created by George Ford on 12/31/22.
//

#ifndef YOUTUBE_DATAHANDLER_HPP
#define YOUTUBE_DATAHANDLER_HPP

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <span>
#include <set>
#include <algorithm>
#include <random>

#include "Matrix2D.hpp"
#include "Features.hpp"
#include "DataLoader.hpp"

enum class DataSetType {
    Training,
    Test,
    Validation,
	NormTraining,
	NormTest,
	NormValidation
};

namespace fs = std::filesystem;
using std::vector;
using std::shared_ptr;

template<typename T, typename LabelType = uint8_t>
class DataHandler {
	using Features_t = Features<T, LabelType>;
private:
    float _trainSetPercent {0.8f };
    float _testSetPercent  {0.15f};
    float _valSetPercent   {0.05f};

    shared_ptr<vector<Features_t>>     _allData {new vector<Features<T, LabelType>>};
    shared_ptr<vector<Features<float, LabelType>>> _normAllData {new vector<Features<float, LabelType>>};

    std::span<Features_t> _trainingData;
    std::span<Features<float, LabelType>> _normTrainingData;
    std::span<Features_t> _testData;
    std::span<Features<float, LabelType>> _normTestData;
    std::span<Features_t> _validationData;
    std::span<Features<float, LabelType>> _normValidationData;

private:
    // Return span of given dataset type
    std::span<Features_t> dataSet(DataSetType dataSetType) {
        switch (dataSetType) {
            case DataSetType::Training:
                return _trainingData;
            case DataSetType::Test:
                return _testData;
            case DataSetType::Validation:
                return _validationData;
            case DataSetType::NormTraining:
                return _normTrainingData;
            case DataSetType::NormTest:
                return _normTestData;
            case DataSetType::NormValidation:
                return _normValidationData;
        }
    }

    // Convert bytes array to N-byte int
    template<typename Int_t>
        static Int_t toBigEndian(const uint8_t *bytes);
    template<>
        static int16_t toBigEndian(const uint8_t *bytes) {
            return int16_t (
                    bytes[0] << 8 |
                    bytes[1]
            );
    }
    template<>
        static int32_t toBigEndian(const uint8_t *bytes) {
            return int32_t (
                    bytes[0] << 24 |
                    bytes[1] << 16 |
					bytes[2] << 8  |
					bytes[3]
            );
    }
    template<>
        static int64_t toBigEndian(const uint8_t *bytes) {
            return int64_t (
                    bytes[0] << 56 |
                    bytes[1] << 48 |
                    bytes[2] << 40 |
                    bytes[3] << 32 |
					bytes[4] << 24 |
					bytes[5] << 16 |
					bytes[6] << 8  |
					bytes[7]
                );
    }

    public:
    // CTORs
    explicit DataHandler(std::tuple<uint, uint, uint> splitPercentages = std::make_tuple(80, 15, 5)) {
        assert(std::get<0>(splitPercentages) + std::get<1>(splitPercentages)
                + std::get<2>(splitPercentages) == 100);
        _trainSetPercent = (float) std::get<0>(splitPercentages) / 100.f;
        _testSetPercent  = (float) std::get<1>(splitPercentages) / 100.f;
        _valSetPercent   = (float) std::get<2>(splitPercentages) / 100.f;
    }
    ~DataHandler() = default;

    // Load dataSet from files
	void loadCsvData(const fs::path& filePath, bool includeLabels, char delim = ',') {
        // LOAD DATA FROM CSV FILE
        std::fstream ifs(filePath);
        if(!ifs.is_open()) {
            std::cerr << "Problem opening file, " << filePath << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Loading: "<< filePath << "..." << std::endl;
        // Initialize Feature vector
        std::string line;
        // GET HEADER AND IGNORE
        std::getline(ifs, line);

        // ITERATE THROUGH EVERY LINE AND GET DATA
        while (std::getline(ifs, line)) {
                _allData->template emplace_back();
                std::stringstream ss(line);
                std::string valStr{};
                bool firstVal{true};
                // ITERATE THROUGH SINGLE LINE
                while (getline(ss, valStr, delim)) {
                    std::stringstream valSs(valStr);
                    T val;
                    valSs >> val;
                    if (firstVal && includeLabels) {
                        _allData->back().label(val);
						_allData->back().labelRep(static_cast<LabelType>(val));
                    } else {
                        _allData->back().push_back(val);
                    }
                    firstVal = false;
                }
            }
	}
    void loadMnistData(const fs::path& filePath) {
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen(filePath.c_str(), "rb"), &fclose);
        if(fp == nullptr) {
            std::cerr << "Error opening file, " << filePath << std::endl;
            exit(EXIT_FAILURE);
        }
        printf("Loading %s...\n", filePath.c_str());
        // 1. READ 1st 4 int32_t
        // header[0]: magic number
        // header[1]: total number of images in dataset
        // header[2]: number of rows
        // header[3]: number of cols
        int32_t header[4]{};
        uint8_t headerBytes[4]{};
        for (int32_t & i : header) {
            fread(&headerBytes[0], sizeof(headerBytes), 1, fp.get());
            i = toBigEndian<int32_t>(headerBytes);
        }
        int32_t numImages = header[1];
        int32_t numRows = header[2];
        int32_t numCols = header[3];

        // 2. READ THE REST OF THE DATA
        for(int i = 0; i < numImages  && !feof(fp.get()); ++i) {
            _allData->template emplace_back();
            for (int j = 0; j < numRows * numCols && !feof(fp.get()); ++j) {
                _allData->back().push_back(static_cast<uint8_t>(fgetc(fp.get())));
            }
        }
    }
    void loadMnistLabels(const fs::path& filePath) {
        std::unique_ptr<FILE, int(*)(FILE*)> fp(fopen(filePath.c_str(), "rb"), &fclose);
        if(fp == nullptr) {
            fprintf(stderr, "Error opening file, %s\n", filePath.c_str());
            exit(EXIT_FAILURE);
        }
        printf("Loading %s...\n", filePath.c_str());
        // 1. READ 1st 2 int32_t
        // header[0]: magic number
        // header[1]: total number of items
        int32_t header[2];
        uint8_t headerBytes[4];
        for (int32_t & i : header) {
            fread(&headerBytes[0], sizeof(headerBytes), 1, fp.get());
            i = toBigEndian<int32_t>(headerBytes);
        }
        // 2. READ ALL LABELS
        int32_t numLabels = header[1];
        static int startIdx = 0;
        for (int i = startIdx; i < numLabels + startIdx && !feof(fp.get()); ++i) {
            _allData->at(i).label(static_cast<uint8_t>(fgetc(fp.get())));
			_allData->at(i).labelRep(static_cast<LabelType>(_allData->at(i).label()));
        }
        startIdx += numLabels;
    }
    void loadIrisData(const fs::path& filePath, char delim = ',') {
        std::ifstream ifs(filePath);
        if(!ifs.is_open()) {
            std::cerr << "Error opening file, " << fs::absolute(filePath) << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string line;
        std::set<std::string> labelStrings;
		int idx = 0;
        while(std::getline(ifs, line)) {
			if(line.empty()) break;
            std::stringstream ss(line);
            T val{};
            std::string elem;
            // 1. READ THE FLOAT VALUES
            _allData->emplace_back();
            std::getline(ss, elem, delim); // Clear first 0
            for (int i = 0; i < 4; ++i, std::getline(ss, elem, delim)) {
                std::stringstream elemSs(elem);
                elemSs >> val;
                _allData->back().push_back(val);
            }
            // 2. HANDLE CATEGORY STRING
            std::getline(ss, elem, delim);
			_allData->back().labelRep(elem);
            labelStrings.emplace(elem);
            _allData->back().label(std::distance(labelStrings.begin(), labelStrings.find(elem)));
			++idx;
        }
		auto randGen = std::mt19937(std::random_device{}());
		std::shuffle(_allData->begin(), _allData->end(), randGen);
    }

	/// Split the dataSet into % Training, % Testing and % Validation
    void splitData() {
        assert(_trainSetPercent + _testSetPercent + _valSetPercent == 1.0f);
		auto beginTest = (size_t)(_allData->size() * _trainSetPercent);
        auto amtTest = (size_t)(_allData->size() * _testSetPercent);
        auto amtVal = (size_t)(_allData->size() * _valSetPercent);

        _trainingData = std::span<Features_t>(*_allData).first(beginTest);
        _testData = std::span<Features_t>(*_allData).subspan(beginTest, amtTest);
        _validationData = std::span<Features_t>(*_allData).last(amtVal);
	}
    void splitPercentage(std::tuple<uint, uint, uint> percentage) {
        assert(std::get<0>(percentage) + std::get<1>(percentage)
                       + std::get<2>(percentage) == 100);
        _trainSetPercent = (float) std::get<0>(percentage) / 100.f;
        _testSetPercent  = (float) std::get<1>(percentage) / 100.f;
        _valSetPercent   = (float) std::get<2>(percentage) / 100.f;
    }
    [[nodiscard]]
        std::tuple<int, int, int>  splitPercentage() const {
        return std::make_tuple((int)_trainSetPercent, (int)_testSetPercent, (int)_valSetPercent);
    }

    /// Create normalized dataset
    void normalize(float normMin = 0.0f, float normMax = 1.0f) {
        puts("Normalizing all data...");
        _normAllData->resize(_allData->size());
        // 1. Find min and max across every feature vector
        auto min = std::numeric_limits<float>::max();
        auto max = std::numeric_limits<float>::min();
        for (const auto & features : *_allData) {
            auto tmpMin = static_cast<float>(*std::min_element(features.begin(), features.end()));
            min = (tmpMin < min) ? tmpMin : min;
            auto tmpMax = static_cast<float>(*std::max_element(features.begin(), features.end()));
            max = (tmpMax > max) ? tmpMax : max;
        }
        // 2. Normalize every element in every feature vector
        for (size_t i = 0; const auto & features : *_allData) {
            std::transform(features.begin(), features.end(), std::back_inserter(_normAllData->at(i)),
				   [=](T elem) -> float { // Assuming T is not bigger than sizeof(T*)
					  return ( (static_cast<float>(elem) - min) / (max - min) ) * (normMax - normMin)
								+ normMin;
				   });
            _normAllData->at(i).label(features.label());
            _normAllData->at(i++).labelRep(features.labelRep());
        }
        // 3. Assign training, test, and validation normalized std::span's
        assert(_trainSetPercent + _testSetPercent + _valSetPercent == 1.0f);
        auto beginTest = (size_t)(_normAllData->size() * _trainSetPercent);
        auto amtTest = (size_t)(_normAllData->size() * _testSetPercent);
        auto amtVal = (size_t)(_normAllData->size() * _valSetPercent);

        _normTrainingData = std::span<Features<float, LabelType>>(*_normAllData).first(beginTest);
        _normTestData = std::span<Features<float, LabelType>>(*_normAllData).subspan(beginTest, amtTest);
        _normValidationData = std::span<Features<float, LabelType>>(*_normAllData).last(amtVal);
    }

    // Access to datasets
    inline std::span<Features_t> allData() const {return std::span(*_allData);}
    void allData(shared_ptr<vector<Features_t>> featuresVec) {
        _allData = featuresVec;
    }
    inline std::span<Features<float, LabelType>> normAllData() const {return std::span(*_normAllData);}
    void normAllData(shared_ptr<vector<Features<float, LabelType>>> featuresVec) {
        _normAllData = featuresVec;
    }
    inline std::span<Features_t> trainingData()   const {return _trainingData;}
    inline std::span<Features_t> testData()       const {return _testData;}
    inline std::span<Features_t> validationData() const {return _validationData;}
    inline std::span<Features<float, LabelType>> normTrainingData()   const {return _normTrainingData;}
    inline std::span<Features<float, LabelType>> normTestData()       const {return _normTestData;}
    inline std::span<Features<float, LabelType>> normValidationData() const {return _normValidationData;}

	std::set<uint8_t> classes() {
		static std::set<uint8_t> classLabels;
		static bool performedYet = false;
		if(!performedYet) {
			for(const auto & feature : *_allData) {
				classLabels.insert(feature.label());
			}
			performedYet = true;
		}

		return classLabels;
	}
};


#endif //YOUTUBE_DATAHANDLER_HPP
