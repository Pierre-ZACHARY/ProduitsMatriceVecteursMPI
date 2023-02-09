//
// Created by Yukiix on 09/02/2023.
//

#include <fstream>
#include <iostream>
#include "Matrice.h"
using namespace std;

Matrice::Matrice(int n, int m) : n(n), m(m) {
    data.resize(n, vector<double>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] = i * m + j;
        }
    }
}

Matrice::Matrice(const std::string &path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << path << endl;
        exit(1);
    }

    file >> n >> m;
    data.resize(n, std::vector<double>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file >> data[i][j];
        }
    }
    file.close();
}

Matrice::~Matrice() {
    data.clear();
}

Matrice Matrice::operator*(const Matrice &other) const {
    if (m != other.n) {
        std::cerr << "Incompatible matrices for multiplication" << std::endl;
        exit(1);
    }

    Matrice result(n, other.m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < other.m; j++) {
            for (int k = 0; k < m; k++) {
                result.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
    }
    return result;
}

std::ostream &operator<<(std::ostream &os, const Matrice &mat) {
    for (const auto& ligne : mat.data) {
        for (auto val : ligne) {
            os << val << " ";
        }
        os << std::endl;
    }
    return os;
}

bool Matrice::operator==(const Matrice &other) const {
if (n != other.n || m != other.m) {
        return false;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (data[i][j] != other.data[i][j]) {
                return false;
            }
        }
    }
    return true;
}
