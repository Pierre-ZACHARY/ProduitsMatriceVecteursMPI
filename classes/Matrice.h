//
// Created by Yukiix on 09/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MATRICE_H
#define PRODUITSMATRICEVECTEURSMPI_MATRICE_H

#include <string>
#include <vector>


class Matrice {
    private:
        int n;
        int m;
        std::vector<std::vector<double>> data;
    public:
        Matrice(int nbLignes, int nbColonnes);
        Matrice(const std::string &path);
        ~Matrice();
        Matrice operator*(const Matrice &other) const;
        bool operator==(const Matrice &other) const;
        friend std::ostream &operator<<(std::ostream &os, const Matrice &mat);
};

#endif //PRODUITSMATRICEVECTEURSMPI_MATRICE_H
