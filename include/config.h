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
 * @file config.h
 * @author Francisco Rodríguez Jiménez
 * @date 01/07/2021
 * @brief Function declarations of Config
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

#ifndef CONFIG_H
#define CONFIG_H

/********************************* Includes *******************************/
#include <fstream>

/******************************** Constants *******************************/
const char* const ERROR_PARSE_ARGUMENTS =
    "Error: Missing required value of the argument or nothing to parse, please use -h for more information.";

/******************************** Structures ******************************/

/**
 * @brief Struct of Config that permit set the configuration of the program from json file
 */
typedef struct Config {
    std::string dbDataTest;       /**< Filename of the dataset to test */
    std::string dbLabelsTest;     /**< Filename of the dataset labels to test */
    std::string dbDataTraining;   /**< Filename of the dataset to train */
    std::string dbLabelsTraining; /**< Filename of the dataset labels to train */
    std::string MRMR;              /**< Filename of the MRMR file */
    int nTuples;                  /**< Number of tuples of the dataset */
    int nFeatures;                /**< Number of features of the dataset */
    int nClasses;                 /**< Number of classes of the dataset */

    /********************************* Methods ********************************/
    /**
     * @brief The constructor with parameters
     * @param argc Number of arguments
     * @param argv The command-line parameters
     * @return An object containing all configuration parameters
     */
    Config(const int argc, const char** argv);

    /**
     * @brief Destroy the Config:: Config object
     *
     */
    ~Config();

    /**
     * @brief Overload operator << to print in the standard output info about Config
     * @param os stream output
     * @param o object Config
     * @return std::ostream& stream with info
     */
    friend std::ostream& operator<<(std::ostream& os, const Config& o);
} Config;

/**
 * @brief Check the condition. If it is true, a message is showed and the
 * program will abort
 * @param cond The condition to be evaluated
 * @param format The format of the arguments
 * @param ... The corresponding messages to be showed in error case
 */
void check(const bool cond, const char* const format, ...);

#endif