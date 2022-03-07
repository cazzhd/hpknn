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
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

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
#ifdef WITHMPI
#include "mpi.h"
#endif
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
    // #ifdef WITHMPI
    //     MPI::Init_thread(MPI_THREAD_MULTIPLE);
    //     int rank = MPI::COMM_WORLD.Get_rank();
    //     int size = MPI::COMM_WORLD.Get_size();
    //     std::cout << "Rank: " << rank << " Size: " << size << std::endl;
    //     MPI::Finalize();
    //     exit(0);
    // #endif

    Config config(argc, argv);
    // std::cout << config << std::endl;

    CSVReader csvReader = CSVReader();

    // Use normalize get the data normalized and score is little better
    vectorOfVectorData<float> dataTraining, dataTest, dataTrainingSorted, dataTestSorted;
    vectorOfData<unsigned int> labelsTraining, labelsTest, MRMR;
#ifdef _OPENMP
    double start, end;
    start = omp_get_wtime();
#else
    std::chrono::steady_clock::time_point start, end;
    start = std::chrono::steady_clock::now();
#endif
#pragma omp parallel sections
    {
#pragma omp section
        {
            dataTraining = csvReader.readData<float>(config.dbDataTraining);
        }
#pragma omp section
        {
            dataTest = csvReader.readData<float>(config.dbDataTest);
        }
#pragma omp section
        {
            labelsTraining = flatten(csvReader.readData<unsigned int>(config.dbLabelsTraining));
        }
#pragma omp section
        {
            labelsTest = flatten(csvReader.readData<unsigned int>(config.dbLabelsTest));
        }
#pragma omp section
        {
            MRMR = flatten(csvReader.readData<unsigned int>(config.MRMR));
        }
    }
#ifdef _OPENMP
    end = omp_get_wtime();
    std::cout << "Time to read data: " << end - start << std::endl;
#else
    end = std::chrono::steady_clock::now();
    std::cout << "Time to read data: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
#endif

    // Sorting by best features (MRMR)
#ifdef _OPENMP
    start = omp_get_wtime();
#else
    start = std::chrono::steady_clock::now();
#endif
#pragma omp parallel sections
    {
#pragma omp section
        {
            dataTrainingSorted = sorting_by_indices_vector(dataTraining, MRMR);
        }
#pragma omp section
        {
            dataTestSorted = sorting_by_indices_vector(dataTest, MRMR);
        }
    }
#ifdef _OPENMP
    end = omp_get_wtime();
    std::cout << "Time to sort data with MRMR: " << end - start << std::endl;
#else
    end = std::chrono::steady_clock::now();
    std::cout << "Time to sort data with MRMR: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
#endif

    vectorOfData<Point> dataTrainingPoints;
    vectorOfData<Point> dataTestPoints;

#ifdef _OPENMP
    start = omp_get_wtime();
#else
    start = std::chrono::steady_clock::now();
#endif
    dataTrainingPoints.resize(dataTrainingSorted.size());
    dataTestPoints.resize(dataTestSorted.size());
#pragma omp parallel for
    for (unsigned int i = 0; i < dataTrainingSorted.size(); ++i) {
        Point trainingPoint(dataTrainingSorted[i], labelsTraining[i]);
        Point testPoint(dataTestSorted[i], labelsTest[i]);
        dataTrainingPoints[i] = trainingPoint;
        dataTestPoints[i] = testPoint;
        // dataTrainingPoints.push_back(trainingPoint);
    }
#ifdef _OPENMP
    end = omp_get_wtime();
    std::cout << "Time to fill vector of Point: " << end - start << std::endl;
#else
    end = std::chrono::steady_clock::now();
    std::cout << "Time to fill vector of Point: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
#endif

// Get the best k value
// floor(sqrt(config.nTuples))
#ifdef _OPENMP
    start = omp_get_wtime();
#else
    start = std::chrono::steady_clock::now();
#endif
    std::pair<unsigned int, unsigned int> bestHyperParams = getBestHyperParams(1, config.nTuples, dataTrainingPoints, dataTestPoints, euclideanDistance);
    std::cout << "Best value of k: " << bestHyperParams.first << "\nBest numbers of features: " << bestHyperParams.second << std::endl;
#ifdef _OPENMP
    end = omp_get_wtime();
    std::cout << "Time getBestHyperParams: " << end - start << std::endl;
#else
    end = std::chrono::steady_clock::now();
    std::cout << "Time getBestHyperParams: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
#endif

    int counterSuccessTraining = 0, counterSuccessTest = 0;
    vectorOfData<unsigned int> labelTrainingPredicted;
    vectorOfData<unsigned int> labelsTestPredicted;
    labelTrainingPredicted.resize(dataTrainingPoints.size());
    labelsTestPredicted.resize(dataTestPoints.size());

#ifdef _OPENMP
    start = omp_get_wtime();
#else
    start = std::chrono::steady_clock::now();
#endif
#pragma omp parallel for
    for (unsigned int i = 0; i < dataTestPoints.size(); ++i) {
        unsigned int labelPredicted = KNN(bestHyperParams.first, dataTrainingPoints, dataTestPoints[i], euclideanDistance, bestHyperParams.second);
        labelsTestPredicted[i] = labelPredicted;
        if (labelPredicted == dataTestPoints[i].label) {
#pragma omp atomic
            counterSuccessTest++;
        }
    }

#pragma omp parallel for
    for (unsigned int i = 0; i < dataTrainingPoints.size(); ++i) {
        unsigned int labelPredicted = KNN(bestHyperParams.first, dataTrainingPoints, dataTrainingPoints[i], euclideanDistance, bestHyperParams.second);
        labelTrainingPredicted[i] = labelPredicted;
        if (labelPredicted == dataTrainingPoints[i].label) {
#pragma omp atomic
            counterSuccessTraining++;
        }
    }
#ifdef _OPENMP
    end = omp_get_wtime();
    std::cout << "Time KNN: " << end - start << std::endl;
#else
    end = std::chrono::steady_clock::now();
    std::cout << "Time KNN: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
#endif

    // Get Confusion Matrix
    vectorOfVectorData<unsigned int> confusionMatrix = getConfusionMatrix(labelsTest, labelsTestPredicted, config.nClasses);
    // Print Confusion Matrix
    std::cout << "Confusion Matrix Test: " << std::endl;
    for (unsigned int i = 0; i < confusionMatrix.size(); ++i) {
        for (unsigned int j = 0; j < confusionMatrix[i].size(); ++j) {
            std::cout << confusionMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Accuracy of K-NN classifier on training set: " << ((float)counterSuccessTraining / (float)dataTrainingPoints.size()) << std::endl;
    std::cout << "Accuracy of K-NN classifier on test set: " << ((float)counterSuccessTest / (float)dataTestPoints.size()) << std::endl;

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