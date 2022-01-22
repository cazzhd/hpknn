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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "config.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
BBDD::BBDD(const char* filename) {
    std::ifstream dbFile;
    std::string line;

    dbFile.open(filename);

    /********** Getting the database dimensions ***********/
    if (dbFile.is_open()) {
        float data;
        unsigned int nCols = 0, nRows = 1;

        getline(dbFile, line);
        std::stringstream ss(line);
        while (ss >> data) {
            ++nCols;
        }

        while (getline(dbFile, line)) {
            ++nRows;
            std::stringstream ss(line);
            unsigned tmpNcols = 0;
            while (ss >> data) {
                ++tmpNcols;
            }

            check(tmpNcols != nCols, "%s\n", ERROR_DIMENSION_DB);
        }

        int dbSize = nRows * nCols;
        float* dataDb = new float[dbSize];
        dbFile.clear();
        dbFile.seekg(0);
        for (int i = 0; i < dbSize; ++i) {
            dbFile >> dataDb[i];
        }

        dbFile.close();

        this->nRows = nRows;
        this->nCols = nCols;
        this->sizeBBDD = dbSize;
        this->db = dataDb;
    } else {
        check(true, "%s\n", ERROR_OPEN_DB);
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

CSVReader::CSVReader(const char delimiter) : delimiter(delimiter) {}

float* CSVReader::getData(const char* filename) {
    float* dataDb;
    std::ifstream dbFile;
    std::string line;

    dbFile.open(filename);

    /********** Getting the database dimensions ***********/
    if (dbFile.is_open()) {
        float data;
        unsigned int nCols = 0, nRows = 1;

        getline(dbFile, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        while (ss >> data) {
            ++nCols;
        }

        while (getline(dbFile, line)) {
            ++nRows;
            std::replace(line.begin(), line.end(), ',', ' ');
            std::stringstream ss(line);
            unsigned tmpNcols = 0;
            while (ss >> data) {
                ++tmpNcols;
            }

            check(tmpNcols != nCols, "%s\n", ERROR_DIMENSION_DB);
        }

        int dbSize = nRows * nCols;
        dataDb = new float[dbSize];
        dbFile.clear();
        dbFile.seekg(0);

        unsigned int i = 0;
        while (getline(dbFile, line)) {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::stringstream ss(line);
            while (ss >> dataDb[i]) {
                ++i;
            }
        }

        dbFile.close();
    } else {
        check(true, "%s\n", ERROR_OPEN_DB);
    }
    return dataDb;
}