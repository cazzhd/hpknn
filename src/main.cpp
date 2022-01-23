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
 * @copyr#include <chrono>
ight Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/
#include <stdlib.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>

#include "config.h"
#include "db.h"
#include "knn.h"
#include "util.h"

/********************************* Main ********************************/
/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(const int argc, const char** argv) {
    Config config(argc, argv);
    // std::cout << config << std::endl;

    CSVReader csvReader = CSVReader();
    std::vector<std::vector<float>> dataTraining = csvReader.readData(config.dbDataTraining);
    std::vector<std::vector<float>> dataTest = csvReader.readData(config.dbDataTest);
    std::vector<float> labelTraining = flatten(csvReader.readData(config.dbLabelsTraining));
    std::vector<float> labelsTest = flatten(csvReader.readData(config.dbLabelsTest));

    std::vector<Point> dataTrainingPoints;
    for (int i = 0; i < config.nTuples; ++i) {
        Point pointTraining(dataTraining[i], labelTraining[i]);
        dataTrainingPoints.push_back(pointTraining);
    }

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int i = 0; i < config.nTuples; ++i) {
        Point pointTest(dataTest[i], labelsTest[0]);
        KNN(11, dataTrainingPoints, pointTest, euclideanDistance);
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << "[ns]" << std::endl;

    // float* dataTest = csvReader.readData(config.dbDataTest.c_str());
    // float* labelsTest = csvReader.readData(config.dbLabelsTest.c_str());
    // float* dataTraining = csvReader.readData(config.dbDataTraining.c_str());
    // float* labelTraining = csvReader.readData(config.dbLabelsTraining.c_str());

    // Process dataTraining to create Point objects
    // Point* pointsTraining = new Point[config.nTuples];
    // float* tmpTupleTraining;
    // for (int i = 0; i < config.nTuples; ++i) {
    //     tmpTupleTraining = new float[config.nFeatures];
    //     memcpy(tmpTupleTraining, dataTraining + i * config.nFeatures, config.nFeatures * sizeof(float));
    //     pointsTraining[i] = Point(*tmpTupleTraining, 0, labelTraining[i], config);
    // }
    // delete[] tmpTupleTraining;

    // delete[] dataTest;
    // delete[] labelsTest;
    // delete[] dataTraining;
    // delete[] labelTraining;

    return EXIT_SUCCESS;
}