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
 * @file main.cpp
 * @author Francisco Rodríguez Jiménez
 * @date 26/06/2021
 * @brief a Parallel and Distributed K-NN for Energyaware Heterogeneous
 * Platforms
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/

#include <stdlib.h>

#include <iostream>

#include "config.h"
#include "db.h"

/********************************* Main ********************************/

/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(const int argc, const char** argv) {
    Config config(argc, argv);

    BBDD dbTrain = BBDD(config.dbFilenameTrain.c_str(), config);
    BBDD dbTest = BBDD(config.dbFilenameTest.c_str(), config);

    std::cout << "Info dbTrain" << std::endl;
    std::cout << "Ncols: " << dbTrain.getnCols() << std::endl;
    std::cout << "Nrows: " << dbTrain.getnRows() << std::endl;
    std::cout << "Size: " << dbTrain.getsizeBBDD() << std::endl;

    std::cout << "\nInfo dbTest" << std::endl;
    std::cout << "Ncols: " << dbTest.getnCols() << std::endl;
    std::cout << "Nrows: " << dbTest.getnRows() << std::endl;
    std::cout << "Size: " << dbTest.getsizeBBDD() << std::endl;

    return EXIT_SUCCESS;
}