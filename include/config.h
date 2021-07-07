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

/******************************** Structures ******************************/
typedef struct Config {
    std::string dbFilenameTest;
    std::string dbFilenameTrain;
    int K;
    int nFeatures;

    /********************************* Methods ********************************/
    Config();
    ~Config();
    friend std::ostream& operator<<(std::ostream& os, const Config& o);
} Config;

#endif