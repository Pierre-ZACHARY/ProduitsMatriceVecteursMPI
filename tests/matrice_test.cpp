//
// Created by Yukiix on 09/02/2023.
//
#include <gtest/gtest.h>
#include "classes/Matrix.h"
using namespace std;

TEST(MatriceTest, BasicAssertions) {
    Matrix<int> matrice(2, 2);
    matrice.fill(6);
    ASSERT_EQ(matrice(0, 0), 6);
    ASSERT_EQ(matrice(0, 1), 6);
    matrice(0, 0) = 5;
    ASSERT_EQ(matrice(0, 0), 5);
    Matrix<float> matrice2(2, 2);
    matrice2.fill(6.0f);
    ASSERT_EQ(matrice2(0, 0), 6.0f);
    cout << matrice2 <<endl;

    Matrix<int> matrice3 = matrice * matrice;
    ASSERT_EQ(matrice3(0, 0), matrice(0,0)*matrice(0,0)+matrice(0,1)*matrice(1,0));
    Matrix<float> matrice4 = matrice * matrice2; // test matrix int * matrix float
    ASSERT_EQ(matrice4(0, 0), matrice(0,0)*matrice2(0,0)+matrice(0,1)*matrice2(1,0));
}