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
#include "config.h"

/******************************** Constants *******************************/
const char* const ERROR_DIMENSION_DB =
    "Error: Number of columns of the database are irregular.";

/********************************* Methods ********************************/
/**
 * @brief Class of BBDD that permit read the EGG Database
 */
class BBDD {
   private:
    /********** Attributes ***********/
    unsigned int nRows; /**< Number of rows of the database */
    unsigned int nCols; /**< Number of columns of the database */
    int sizeBBDD;       /**< Size of the database */
    float* db;          /**< Database */

   public:
    /********** Methods ***********/
    /**
     * @brief Constructor of the class
     * @param filename The name of the file to read
     * @param config The configuration of the program
    */
    BBDD(const char* filename, const Config& config);

    /**
     * @brief Destructor of the class
    */
    ~BBDD();

    /**
     * @brief Get nRows of the database
     * @return unsigned int with the number of rows
    */
    unsigned int getRows();

    /**
     * @brief Get nCols of the database
     * @return unsigned int with the number of cols
    */
    unsigned int getCols();

    /**
     * @brief Get sizeBBDD of the database
     * @return int with the size of the database
    */
    int getSizeBBDD();

    /**
     * @brief Get the database
     * @return float* pointing to the database
    */
    float* getDB();

    /**
     * @brief Overload operator << to print in the standard output info about BBDD
     * @param os stream output
     * @param o object BBDD
     * @return std::ostream& stream with info
    */
    friend std::ostream& operator<<(std::ostream& os, const BBDD& o);
};

#endif
