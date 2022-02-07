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
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "config.h"
#include "db.h"
#include "knn.h"
#include "omp.h"
#include "util.h"

template <typename T>
using vectorOfVectorData = std::vector<std::vector<T>>;
template <typename T>
using vectorOfData = std::vector<T>;

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
    vectorOfVectorData<float> dataTraining = normalize(csvReader.readData<float>(config.dbDataTraining));
    vectorOfVectorData<float> dataTest = normalize(csvReader.readData<float>(config.dbDataTest));
    vectorOfData<unsigned int> labelTraining = flatten(csvReader.readData<unsigned int>(config.dbLabelsTraining));
    vectorOfData<unsigned int> labelsTest = flatten(csvReader.readData<unsigned int>(config.dbLabelsTest));

    vectorOfData<Point> dataTrainingPoints;
    vectorOfData<Point> dataTestPoints;
    for (unsigned int i = 0; i < dataTraining.size(); ++i) {
        Point trainingPoint(dataTraining[i], labelTraining[i]);
        dataTrainingPoints.push_back(trainingPoint);
    }
    for (unsigned int i = 0; i < dataTest.size(); ++i) {
        Point testPoint(dataTest[i], labelsTest[i]);
        dataTestPoints.push_back(testPoint);
    }

    // std::cout << getBestK(1, 8, dataTrainingPoints, dataTestPoints, euclideanDistance) << std::endl;

    double begin1, end1;
    // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    begin1 = omp_get_wtime();
    bool isSuccess;
    int counterSuccess = 0;
    for (unsigned int i = 0; i < dataTestPoints.size(); ++i) {
        isSuccess = KNN(2, dataTrainingPoints, dataTestPoints[i], euclideanDistance);
        if (isSuccess) counterSuccess++;
    }
    end1 = omp_get_wtime();
    // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Accuracy = " << ((float)counterSuccess / (float)dataTestPoints.size()) << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << "[ns]" << std::endl;
    std::cout << "Time difference = " << end1 - begin1 << "(s)" << std::endl;
    // printf("Work took %f seconds\n", end1 - begin1);

    // Make with dinamic memory
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