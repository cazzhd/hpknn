/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpmoon repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number
 * TIN2012-32039 and TIN2015-67020-P.\n Spanish 'Ministerio de Ciencia,
 * Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file db.h
 * @author Francisco Rodríguez Jiménez
 * @date 26/06/2021
 * @brief Function declarations of database
 * @copyright Hpmoon (c) 2015 EFFICOMP
 */

#ifndef BD_H
#define BD_H

/********************************* Includes *******************************/
#include <vector>

#include "config.h"

/******************************** Constants *******************************/
const char* const ERROR_DIMENSION_DB = "Error: Number of columns of the database are irregular.";
const char* const ERROR_OPEN_DB = "Error: Cannot open database file.";

/********************************* Methods ********************************/
/**
 * @brief Class of CSVReader that permit read the EGG Database with format CSV
 */
class CSVReader {
   private:
    char delimiter; /**< Delimiter of the CSV file */

   public:
    /********** Methods ***********/
    /**
     * @brief Constructor of the class
     * @param delimiter The delimiter of the CSV file, default ','
     */
    CSVReader(const char delimiter = ',');

    /**
     * @brief Read the Data from the CSV file in a vector of vector of float
     * @param filename The name of the file to read
     */
    template <typename T>
    std::vector<T> readData(std::string filename);
};

#endif
