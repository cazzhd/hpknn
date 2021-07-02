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
 * @brief Implementation of the db instance
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/

#include "db.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/********************************* Methods ********************************/

BBDD::BBDD() {
    std::ifstream db_file;
    std::string line;

    db_file.open("db/data_essex_3600_x110_plus_class.txt");

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

            if (tmp_ncols != n_cols) exit(EXIT_FAILURE);
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
        std::cout << "No esta abierto" << std::endl;
    }
}

BBDD::~BBDD() {
    delete[] this->db;
}

unsigned int BBDD::getnRows() {
    return this->nRows;
}

unsigned int BBDD::getnCols() {
    return this->nCols;
}

int BBDD::getsizeBBDD() {
    return this->sizeBBDD;
}

float* BBDD::getDB() {
    return this->db;
}

// float* getDB() {
//     std::ifstream db_file;
//     std::string line;

//     db_file.open("db/data_essex_3600_x110_plus_class.txt");

//     /********** Getting the database dimensions ***********/

//     if (db_file.is_open()) {
//         float dato;
//         unsigned int n_cols = 0, n_rows = 1;

//         getline(db_file, line);
//         std::stringstream ss(line);
//         while (ss >> dato) {
//             ++n_cols;
//         }

//         while (getline(db_file, line)) {
//             ++n_rows;
//             std::stringstream ss(line);
//             unsigned tmp_ncols = 0;
//             while (ss >> dato) {
//                 ++tmp_ncols;
//             }

//             if (tmp_ncols != n_cols) exit(EXIT_FAILURE);
//         }

//         int db_size = n_rows * n_cols;
//         float* data_db = new float[db_size];
//         db_file.clear();
//         db_file.seekg(0);
//         for (int i = 0; i < db_size; ++i) {
//             db_file >> data_db[i];
//         }

//         std::cout << "ROWS: " << n_rows << std::endl;
//         std::cout << "COLS: " << n_cols << std::endl;

//         db_file.close();
//         return data_db;
//     } else {
//         std::cout << "No esta abierto" << std::endl;
//     }
// }