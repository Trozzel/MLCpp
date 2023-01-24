//
// Created by George Ford on 1/11/23.
//

#ifndef DATAHANDLERLIBRARY_MATRIX2D_HPP
#define DATAHANDLERLIBRARY_MATRIX2D_HPP
#include <vector>
#include <iostream>
#include <cmath>
#include <numeric>
#include <iterator>
#include <fstream>

template<typename T, uint32_t Rows, uint32_t Cols>
class Matrix2D ;

template<typename T, uint32_t Cols>
class Array {
    T* _array{};
    const size_t _cols {Cols};

public:
    Array() = default;
    explicit Array(T* ptr) : _array(ptr) {}
    Array(const Array& arr) : _array(arr._array) {
        std::cout << "In Array Copy Constructor" << std::endl;
    }
    Array& operator=(const Array& arr) {
        if(this == &arr) return *this;
        _array = arr._array;
        return *this;
    }

    class iterator {
        Array& _arr;
        size_t _pos;
    public:
        iterator(Array& arr, size_t pos) : _pos(pos), _arr(arr){}
        bool operator!=(const iterator& it) {
            return _pos != it._pos;
        }
        iterator& operator++() {
            _pos++ ;
            return *this;
        }
        // postfix
        iterator& operator++ (int) {
            ++_pos;
            return *this;
        }
        iterator& operator--() {
            assert(_pos != 0);
            _pos--;
            return *this;
        }
        // postfix
        iterator& operator--(int) {
            ++_pos;
            return *this;
        }
        iterator& operator+(size_t num) {
            assert(_pos + num <= _arr.cols());
            _pos += num;
            return *this;
        }
        iterator& operator-(size_t num) {
            assert(_pos - num >= 0);
            _pos -= num;
            return *this;
        }
        T* operator->() {
            return &_arr[_pos];
        }
        T operator*() {
            return _arr[_pos];
        }
    };

    iterator begin() {
        return iterator(*this, 0);
    }

    iterator end() {
        return iterator(*this, _cols);
    }

    T& operator[](size_t col) { return _array[col];}
    T operator[](size_t col) const {return _array[col];}

    constexpr size_t size() { return _cols; }
    constexpr size_t cols() { return _cols; }

    void setAddress(T* t) { _array = t; }
};


template<typename T, uint32_t Rows, uint32_t Cols>
class Matrix2D {
    bool _valid{true};
    T* _matrix = new T[Rows * Cols];
    Array<T, Cols>* _rowPtr = new Array<T, Cols>[Rows];
    const size_t _rows = Rows;
    const size_t _cols = Cols;

public:
    Matrix2D() {
        for (int i=0; i<Rows; i++) {
            _rowPtr[i].setAddress(&_matrix[0] + i * Cols);
        }
    }
    Matrix2D(const Matrix2D& mat)
    {
        std::cout << "Copying matrix..." << std::endl;
        std::copy(&mat._matrix[0], &mat._matrix[_rows * _cols], &_matrix[0]);
        std::copy(&mat._rowPtr[0], &mat._rowPtr[_rows],         &_rowPtr[0]);
    }
    Matrix2D(Matrix2D && mat)  noexcept {
        _matrix = &mat._matrix[0];
        _rowPtr = &mat._rowPtr[0];
        mat._matrix = nullptr;
        mat._rowPtr = nullptr;
        mat._valid = false;
    }
    ~Matrix2D() {
        if (_valid) {
            delete[] _rowPtr;
            delete[] _matrix;
        }
    }

    Matrix2D& operator=(const Matrix2D& mat) {
        if(&mat == this) return *this;
        std::cout << "Copying matrix..." << std::endl;
        std::copy(&mat._matrix[0], &mat._matrix[_cols * _cols], &_matrix[0]);
        std::copy(&mat._rowPtr[0], &mat._rowPtr[_cols * _cols], &_rowPtr[0]);
        return *this;
    }

    Array<T, Cols>& operator[](size_t rowNum) {
        return _rowPtr[rowNum];
    }
    Array<T, Cols> operator[](size_t rowNum) const {
        return _rowPtr[rowNum];
    }

    constexpr size_t size() { return _rows; }
    constexpr std::tuple<size_t, size_t> shape() {
        return std::make_tuple(Rows, Cols);
    }

    class iterator{
        Matrix2D _matrix;
        size_t _pos;
    public:
        iterator(Matrix2D& matrix, size_t pos) : _matrix(matrix), _pos(pos){}
        iterator& operator++() {
            ++_pos;
            return *this;
        }
        const Matrix2D::iterator  operator++(int) {
            ++_pos;
            return *this;
        }
        iterator& operator--() {
            --_pos;
            return *this;
        }
        iterator& operator--(int) {
            --_pos;
            return *this;
        }
        iterator& operator+(size_t n) {
            _pos+=n;
            return *this;
        }
        iterator& operator-(size_t n) {
            _pos-=n;
            return *this;
        }
        Array<T, Cols>* operator->() {
            return &_matrix[_pos];
        }
        Array<T, Cols>* operator->() const {
            return &_matrix[_pos];
        }
        Array<T, Cols>& operator*() {
            return _matrix[_pos];
        }
        Array<T, Cols>& operator*() const {
            return _matrix[_pos];
        }
    };

    iterator begin() {
        return iterator(*this, 0);
    }
    iterator end() {
        return iterator(*this, _rows);
    }
};
#endif //DATAHANDLERLIBRARY_MATRIX2D_HPP
