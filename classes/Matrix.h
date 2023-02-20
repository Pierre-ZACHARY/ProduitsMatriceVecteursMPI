//
// Created by Yukiix on 09/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MATRIX_H
#define PRODUITSMATRICEVECTEURSMPI_MATRIX_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>


template <typename T>
class Matrix {
public:
    Matrix() {
        m_rows = 0;
        m_cols = 0;
    }

    T* data() { return m_data.data(); }
    int size() const { return m_data.size(); }

    Matrix(int rows, int cols) : m_rows(rows), m_cols(cols), m_data(rows * cols) {}
    Matrix(const Matrix<T>& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_data(other.m_data) {}
//    Matrix(Matrix<T>&& other) : m_rows(other.m_rows), m_cols(other.m_cols), m_data(std::move(other.m_data)) {}

    Matrix<T> getLine(int line) const {
        Matrix<T> result(1, m_cols);
        for (int i = 0; i < m_cols; i++) {
            result(0, i) = (*this)(line, i);
        }
        return result;
    }
    T& operator()(int row, int col) { return m_data[row * m_cols + col]; }
    const T& operator()(int row, int col) const { return m_data[row * m_cols + col]; }

    void fromVector(const std::vector<T>& vec);

    void fill(const T& value);
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat) {
        for (int i = 0; i < mat.m_rows; ++i) {
            for (int j = 0; j < mat.m_cols; ++j) {
                os << mat(i, j) << " ";
            }
            os << std::endl;
        }
        return os;
    }
    bool operator==(const Matrix<T>& other) const {
        if (m_rows != other.m_rows || m_cols != other.m_cols) {
            return false;
        }
        for (int i = 0; i < m_data.size(); i++) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }
        return true;
    }

    template <typename U>
    Matrix<decltype(T{} * U{})> operator*(const Matrix<U>& other) const {
        static_assert(std::is_same_v<decltype(std::declval<T>() * std::declval<U>()), decltype(std::declval<U>() * std::declval<T>())>, "Element types do not support operator*");
        if (m_cols != other.rows()) {
            throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
        }

        Matrix<decltype(T{} * U{})> result(m_rows, other.cols());

#pragma omp parallel for
        for (int i = 0; i < m_rows; i++) {
            for (int j = 0; j < other.cols(); j++) {
                auto dot = decltype(T{} * U{}){};
                for (int k = 0; k < m_cols; k++) {
                    dot += (*this)(i, k) * other(k, j);
                }
                result(i, j) = dot;
            }
        }

        return result;
    }

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }
    const std::vector<T> &getVector() const {
        return m_data;
    }
    std::string to_string() {
        std::string s = "";
        for (int i = 0; i < m_rows; ++i) {
            for (int j = 0; j < m_cols; ++j) {
                s += std::to_string((*this)(i, j)) + " ";
            }
            s += "\n";
        }
        return s;
    }

    Matrix<T> subMatrix(int startRow, int endRow);
    void setSubMatrix(int startRow, int endRow, const Matrix<T>& subMatrix) {
        for (int i = startRow; i < endRow; ++i) {
            for (int j = 0; j < m_cols; ++j) {
                (*this)(i, j) = subMatrix(i - startRow, j);
            }
        }
    }


protected:
    int m_cols;
    std::vector<T> m_data;
    int m_rows;
};

template<typename T>
Matrix<T> Matrix<T>::subMatrix(int startRow, int endRow) {
    Matrix<T> subMatrix(endRow - startRow, m_cols);
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < m_cols; ++j) {
            subMatrix(i - startRow, j) = (*this)(i, j);
        }
    }
    return subMatrix;
}

template<typename T>
void Matrix<T>::fill(const T &value) { std::fill(m_data.begin(), m_data.end(), value); }

template<typename T>
void Matrix<T>::fromVector(const std::vector<T> &vec) {
    if (vec.size() != m_data.size()) {
        throw std::invalid_argument("Vector size does not match matrix size");
    }
    m_data = vec;

}

#endif //PRODUITSMATRICEVECTEURSMPI_MATRIX_H
