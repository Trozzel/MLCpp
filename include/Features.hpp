//
// Created by George Ford on 12/31/22.
//

#ifndef YOUTUBE_FEATURES_HPP
#define YOUTUBE_FEATURES_HPP

#include <utility>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>
#include <utility>
#include <map>

using std::vector;

template<typename T, typename LabelType>
class Features {
private:
    vector<T> _features {};
    uint8_t   _label{};
	LabelType _labelRep{};

public:
    // Apply vector<T>::iterator iterator_traits to Features
    using iterator = typename vector<T>::iterator;
    using const_iterator = typename vector<T>::const_iterator;
	using value_type = T;
	using pointer_type = T*;
	using reference_type = T&;

    // CTOR
    /**************************************************************************/
    Features() = default;
    Features(const Features& other) : 
		_features(other._features), // make copy of vector<T>
        _label(other._label), _labelRep(other._labelRep)
	{}
	Features(Features&& other) noexcept : 
		_features(std::move(other._features)),  // invalidate previous vector<T>
		_label(std::move(other._label)),
		_labelRep(std::move(other._labelRep))
	{
	}
    Features& operator=(const Features& other) {
        if(this == &other) return *this;
        _features = other._features;
        _label = other._label;
        _labelRep = other._labelRep;
        return *this;
    }
    Features& operator=(Features&& other) noexcept {
        if(&other == this) return *this;
        _features = std::move(other._features); // copy other vector<T>
        _label = std::move(other._label);
		_labelRep = std::move(other._labelRep);
        return *this;
    }
    ~Features() = default;

	//
    /**************************************************************************/
    const_iterator begin() const {
        return _features.begin();
    }
    iterator begin() {
        return _features.begin();
    }
    const_iterator end() const {
        return _features.end();
    }
    iterator end() {
        return _features.end();
    }
    auto cbegin() const {
        return _features.cbegin();
    }
    auto cend() const {
        return _features.cend();
    }

    // Access operators
    /**************************************************************************/
    T at(size_t i) const {
        return _features.at(i);
    }

    T& operator[](size_t i)   {
        return _features[i];
    }
    const T& operator[](size_t i) const {
        return _features[i];
    }
    inline void push_back(T feature) {
        _features.push_back(feature);
    }
    inline void emplace_back(T feature) {
        _features.emplace_back(feature);
    }
    T& back()  { return _features.back(); }
    const T& back()  const { return _features.back(); }
    T& front() { return _features.front(); }
    const T& front() const { return _features.front(); }

    // 
    /**************************************************************************/
    [[nodiscard]]
		inline const std::vector<T>* features() const {
			return *_features;
		}
    inline void features(std::vector<T> * featureVector) {
        _features = featureVector;
    }

    [[nodiscard]]
		inline uint8_t label() const {
			return _label;
		}
    inline void label(uint8_t label) {
        _label = label;
    }

	// Label representation (i.e. int, string, etc)
	[[nodiscard]]
		const inline LabelType& labelRep () const { 
			return _labelRep;
		}
	inline void labelRep(const LabelType& labelRep) {
		_labelRep = labelRep;
	}

    //
    /**************************************************************************/
    [[nodiscard]]
    inline auto size() const {
        return _features.size();
    }
};


#endif //YOUTUBE_FEATURES_HPP
