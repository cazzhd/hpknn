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
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "config.h"
#include "db.h"
#include "knn.h"
#include "mpi.h"
#include "omp.h"
#include "util.h"
template <typename T>
using vectorOfVectorData = vector<vector<T>>;
template <typename T>
using vectorOfData = vector<T>;

/**
 * @brief Function that read de data from files of config and fill vectors, if use function normalize get best scores
 * @param dataTraining vector of data training
 * @param dataTest vector of data test
 * @param labelsTraining vector of labels training
 * @param labelsTest vector of labels test
 * @param MRMR vector of MRMR
 * @param config configuration of program
 */
void readDataFromFiles(vectorOfVectorData<float>& dataTraining,
                       vectorOfVectorData<float>& dataTest,
                       vectorOfData<unsigned int>& labelsTraining,
                       vectorOfData<unsigned int>& labelsTest,
                       vectorOfData<unsigned int>& MRMR,
                       Config& config) {
    CSVReader csvReader = CSVReader();

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
}

/**
 * @brief sort dataTraining and dataTest by MRMR vector
 * @param dataTraining vector of data training
 * @param dataTest vector of data test
 * @param MRMR vector of MRMR
 */
void sortFeaturesByMRMR(vectorOfVectorData<float>& dataTraining, vectorOfVectorData<float>& dataTest, const vectorOfData<unsigned int>& MRMR) {
#pragma omp parallel sections
    {
#pragma omp section
        {
            dataTraining = sorting_by_indices_vector(dataTraining, MRMR);
        }
#pragma omp section
        {
            dataTest = sorting_by_indices_vector(dataTest, MRMR);
        }
    }
}

/**
 * @brief Create a Vector Of Points object, best structure for my implementation of KNN
 * @param dataTrainingPoints vector of points training to fill
 * @param dataTestPoints vector of points test to fill
 * @param dataTraining vector of data training
 * @param dataTest vector of data test
 * @param labelsTraining vector of labels training
 * @param labelsTest vector of labels test
 */
void createVectorOfPoints(vectorOfData<Point>& dataTrainingPoints,
                          vectorOfData<Point>& dataTestPoints,
                          const vectorOfVectorData<float>& dataTraining,
                          const vectorOfVectorData<float>& dataTest,
                          const vectorOfData<unsigned int>& labelsTraining,
                          const vectorOfData<unsigned int>& labelsTest) {
    dataTrainingPoints.resize(dataTraining.size());
    dataTestPoints.resize(dataTest.size());

#pragma omp parallel for
    for (unsigned int i = 0; i < dataTraining.size(); ++i) {
        Point trainingPoint(dataTraining[i], labelsTraining[i]);
        Point testPoint(dataTest[i], labelsTest[i]);
        dataTrainingPoints[i] = trainingPoint;
        dataTestPoints[i] = testPoint;
    }
}
/********************************* Main ********************************/
/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(const int argc, const char** argv) {
    int size, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI::Init_thread(MPI_THREAD_MULTIPLE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    Config config(argc, argv);

    // 1. Read data from files
    vectorOfVectorData<float> dataTraining, dataTest;
    vectorOfData<unsigned int> labelsTraining, labelsTest, MRMR;
    double start, end;
    if (!rank) {
        start = MPI_Wtime();
        readDataFromFiles(dataTraining, dataTest, labelsTraining, labelsTest, MRMR, config);
        end = MPI_Wtime();
        std::cout << "Time to read data: " << end - start << std::endl;
    }

    // 2. Sorting by best features (MRMR), get best scores
    if (!rank) {
        start = MPI_Wtime();
        sortFeaturesByMRMR(dataTraining, dataTest, MRMR);
        end = MPI_Wtime();
        std::cout << "Time to sort data with MRMR: " << end - start << std::endl;
    }

    // 3. Flatten data
    vectorOfData<float> dataTrainingLast, dataTestLast;
    // Print size
    if (!rank) {
        dataTrainingLast = flatten(dataTraining);
        dataTestLast = flatten(dataTest);
    }

    // Recomendación propagar los vectores de puntos

    // 4. Get the best k and number of features to use
    // floor(sqrt(config.nTuples)) // Recommended
    if (!rank) {
        start = MPI_Wtime();
        std::pair<unsigned int, unsigned int> bestHyperParams = getBestHyperParams(1, config.nTuples, dataTrainingLast, dataTestLast, labelsTraining, labelsTest, euclideanDistance, config);
        end = MPI_Wtime();
        std::cout << "Best value of k: " << bestHyperParams.first << "\nBest numbers of features: " << bestHyperParams.second << std::endl;
        std::cout << "Time getBestHyperParams: " << end - start << std::endl;

        // 5. To finalize get the score of the best k and number of features
        start = MPI_Wtime();
        std::pair<vectorOfData<unsigned int>, unsigned int> scoreTest = getScoreKNN(bestHyperParams.first, dataTrainingLast, dataTestLast, labelsTraining, labelsTest, euclideanDistance, bestHyperParams.second, config);
        std::pair<vectorOfData<unsigned int>, unsigned int> scoreTraining = getScoreKNN(bestHyperParams.first, dataTrainingLast, dataTrainingLast, labelsTraining, labelsTraining, euclideanDistance, bestHyperParams.second, config);
        end = MPI_Wtime();
        std::cout << "Time KNN: " << end - start << std::endl;

        // 6. Get Confusion Matrix for test
        vectorOfVectorData<unsigned int> confusionMatrixTest = getConfusionMatrix(labelsTest, scoreTest.first, config.nClasses);
        std::cout << "Confusion Matrix Test: " << std::endl;
        printMatrix(confusionMatrixTest);

        // vectorOfVectorData<unsigned int> confusionMatrixTraining = getConfusionMatrix(labelsTraining, scoreTraining.first, config.nClasses);
        // std::cout << "Confusion Matrix Training: " << std::endl;
        // printMatrix(confusionMatrixTraining);

        std::cout << "Accuracy of K-NN classifier on training set: " << ((float)scoreTraining.second / (float)config.nTuples) << std::endl;
        std::cout << "Accuracy of K-NN classifier on test set: " << ((float)scoreTest.second / (float)config.nTuples) << std::endl;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}