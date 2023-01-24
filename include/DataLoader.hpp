//
// Created by George Ford on 12/30/22.
//

#ifndef YOUTUBE_DATALOADER_HPP
#define YOUTUBE_DATALOADER_HPP

#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include <sstream>

namespace fs = std::filesystem;

enum class FileType {
	Csv,
	Mnist
};

struct csv{};
struct iris{};
struct mnist{};

/**
 * Base DataLoader class implementation
 * Must call either CSVDataLoader or MnistDataLoader to instantiate
 * @note The caller of the DataLoader is completely responsible for deleting the
 * pointers to the vector<Features*>*
 */ 
template<typename T>
class DataLoader {
protected:
    std::ifstream _ifs{};
    fs::path      _filePath{};

public:
public:
    explicit DataLoader(const fs::path &filePath) :
		_filePath(filePath), _ifs(std::ifstream(filePath))
    {}
    virtual ~DataLoader() { if(_ifs.is_open()) _ifs.close(); }

	/**
	 * Return a PtrDataSet (vector<vector<T>*>*) from file
	 * Must be implemented within child classes
	 */
    virtual std::vector<std::vector<T>*>* const getData() = 0;

	[[nodiscard]] const fs::path& filePath() const { return _filePath; }

    /// Manually close ifstream object
    void close() noexcept {
		if(_ifs.is_open()) {
			_ifs.close();
		}
		else {
			fprintf(stderr, "File object was not open and could not be closed\n");
		}

	}
};


template<typename T>
class CsvDataLoader : public DataLoader<T> {
public:
	using filetype = csv;
public:

    // CTOR
	explicit CsvDataLoader(const fs::path& filePath) :
        DataLoader<T>::DataLoader(filePath)
	{
	}

    // GETDATA IMPLEMENTATION FOR CSV
    std::vector<std::vector<T>*>* getData() override {
		auto pData = new std::vector<std::vector<T>*>;
		std::string line;

		// Read and ignore header
		getline(this->_ifs, line);

		while(this->_ifs) {
			getline(this->_ifs, line);
			std::stringstream ss(line);
			pData->emplace_back(new std::vector<T>*);
			while(ss.good()) {
				std::string valStr;
				getline(ss, valStr, ',');
				std::stringstream ssNum;
				T pixVal;
				ssNum << valStr;
				ssNum >> pixVal;
				pData->back().push_back(pixVal);
			}
		}
		// Remove last empty vector
		pData->pop_back();

		return pData;
	}

};


#endif //YOUTUBE_DATALOADER_HPP
