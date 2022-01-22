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

#include <cstring>
#include <iostream>
#include <vector>

#include "config.h"
#include "db.h"
#include "knn.h"

/********************************* Main ********************************/
/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(const int argc, const char** argv) {
    // std::vector<double> v;

    // v.push_back(1.0);
    // v.push_back(2.0);
    // v.push_back(3.0);
    // v.push_back(4.0);
    // v.push_back(5.0);

    // // Print the vector
    // for (auto& i : v) {
    //     std::cout << i << std::endl;
    // }

    // double* a = &v[0];

    // // Print the vector
    // for (int i = 0; i < 5; i++) {
    //     std::cout << a[i] << std::endl;
    // }

    // int arr[] = {1, 2, 3, 4, 5};
    // int* cp = new int[3];
    // memcpy(cp, arr + 1, sizeof(int) * 3);

    // for (int i = 0; i < 3; i++) {
    //     std::cout << cp[i] << std::endl;
    // }

    Config config(argc, argv);

    // std::cout << config << std::endl;

    CSVReader csvReader = CSVReader();
    float* dataTest = csvReader.getData(config.dbDataTest.c_str());
    float* labelsTest = csvReader.getData(config.dbLabelsTest.c_str());

    float* dataTraining = csvReader.getData(config.dbDataTraining.c_str());
    float* labelTraining = csvReader.getData(config.dbLabelsTraining.c_str());

    KNN(*dataTraining, *dataTest, euclideanDistance, config);

    // for (int i = 0; i < config.nTuples; i++) {
    //     int n;
    //     float* aux = new float[config.nFeatures];
    //     memcpy(aux, dataTest + i * config.nFeatures, sizeof(float) * config.nFeatures);

    //     std::cout << *(dataTest + config.nFeatures - 1) << std::endl;
    //     std::cout << *(aux + config.nFeatures - 1) << std::endl;

    //     std::cin >> n;
    // }

    // Point* points = new Point[config.nTuples];
    // for (int i = 0; i < config.nTuples; i++) {
    //     points[i] = Point(i, i);
    // }
    // for (int i = 0; i < config.nTuples; i++) {
    //     std::cout << points[i] << std::endl;
    // }

    delete[] dataTest;
    delete[] labelsTest;
    delete[] dataTraining;
    delete[] labelTraining;

    return EXIT_SUCCESS;
}