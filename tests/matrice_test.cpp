//
// Created by Yukiix on 09/02/2023.
//
#include <gtest/gtest.h>
#include "classes/Matrice.h"
using namespace std;

TEST(MatriceTest, BasicAssertions) {
    Matrice m1(2, 2);
    Matrice m2(2, 2);
    ASSERT_EQ(m1, m2);
    Matrice m3 = m1 * m2;
    Matrice m4 = Matrice("./matrice_input_test");
    cout << m3 << endl;
    ASSERT_EQ(m3, m4);
}