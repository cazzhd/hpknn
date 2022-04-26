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

/******************************** Constants *******************************/
template std::vector<float> CSVReader::readData(std::string filename);
template std::vector<unsigned int> CSVReader::readData(std::string filename);

/********************************* Methods ********************************/
CSVReader::CSVReader(const char delimiter) : delimiter(delimiter) {}

template <typename T>
std::vector<T> CSVReader::readData(std::string filename) {
    std::vector<T> dataDb;
    std::ifstream dbFile;
    std::string line;

    dbFile.open(filename);

    /********** Getting the database dimensions ***********/
    if (dbFile.is_open()) {
        std::string data;
        unsigned int nCols = 0, nRows = 1;

        getline(dbFile, line);
        std::stringstream ss(line);
        while (std::getline(ss, data, this->delimiter)) {
            ++nCols;
        }

        while (getline(dbFile, line)) {
            ++nRows;
            std::stringstream ss(line);
            unsigned tmpNcols = 0;
            while (std::getline(ss, data, this->delimiter)) {
                ++tmpNcols;
            }

            check(tmpNcols != nCols, "%s\n", ERROR_DIMENSION_DB);
        }

        dbFile.clear();
        dbFile.seekg(0);
        while (getline(dbFile, line)) {
            std::stringstream ss(line);
            while (std::getline(ss, data, this->delimiter)) {
                dataDb.push_back(std::stof(data));
            }
        }

        dbFile.close();
    } else {
        check(true, "%s\n", ERROR_OPEN_DB);
    }

    return dataDb;
}
