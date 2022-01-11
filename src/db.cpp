/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpknn repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number
 * TIN2012-32039 and TIN2015-67020-P.\n Spanish 'Ministerio de Ciencia,
 * Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file db.cpp
 * @author Francisco Rodríguez Jiménez
 * @date 29/06/2021
 * @brief Implementation of the BBDD class
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/
#include "db.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "config.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
BBDD::BBDD(const char* filename, const Config& config) {
    std::ifstream db_file;
    std::string line;

    db_file.open(filename);

    /********** Getting the database dimensions ***********/
    if (db_file.is_open()) {
        float dato;
        unsigned int n_cols = 0, n_rows = 1;

        getline(db_file, line);
        std::stringstream ss(line);
        while (ss >> dato) {
            ++n_cols;
        }

        while (getline(db_file, line)) {
            ++n_rows;
            std::stringstream ss(line);
            unsigned tmp_ncols = 0;
            while (ss >> dato) {
                ++tmp_ncols;
            }

            check(tmp_ncols != n_cols, "%s\n", ERROR_DIMENSION_DB);
        }

        int db_size = n_rows * n_cols;
        float* data_db = new float[db_size];
        db_file.clear();
        db_file.seekg(0);
        for (int i = 0; i < db_size; ++i) {
            db_file >> data_db[i];
        }

        db_file.close();

        this->nRows = n_rows;
        this->nCols = n_cols;
        this->sizeBBDD = db_size;
        this->db = data_db;
    } else {
        std::cout << "The file isn't open" << std::endl;
    }
}

BBDD::~BBDD() { delete[] this->db; }

unsigned int BBDD::getRows() { return this->nRows; }

unsigned int BBDD::getCols() { return this->nCols; }

int BBDD::getSizeBBDD() { return this->sizeBBDD; }

float* BBDD::getDB() { return this->db; }

std::ostream& operator<<(std::ostream& os, const BBDD& o) {
    os << "Ncols: " << o.nCols << std::endl;
    os << "Nrows: " << o.nRows << std::endl;
    os << "Size: " << o.sizeBBDD << std::endl;

    return os;
}