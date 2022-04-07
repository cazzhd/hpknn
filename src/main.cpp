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
    int size, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI::Init_thread(MPI_THREAD_MULTIPLE);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    Config config(argc, argv);
    CSVReader csvReader = CSVReader();

    // Use normalize get the data normalized and score is little better
    vectorOfVectorData<float> dataTraining, dataTest, dataTrainingSorted, dataTestSorted;
    vectorOfData<unsigned int> labelsTraining, labelsTest, MRMR;
    double start, end;
    if (!rank) {
        start = omp_get_wtime();
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
        end = omp_get_wtime();
        std::cout << "Time to read data: " << end - start << std::endl;
    }

    // Sorting by best features (MRMR)
    if (!rank) {
        start = omp_get_wtime();
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
        end = omp_get_wtime();
        std::cout << "Time to sort data with MRMR: " << end - start << std::endl;
    }

    vectorOfData<Point> dataTrainingPoints;
    vectorOfData<Point> dataTestPoints;

    if (!rank) {
        start = omp_get_wtime();
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
        end = omp_get_wtime();
        std::cout << "Time to fill vector of Point: " << end - start << std::endl;
    }

    // In this point, need to create a local training and test vector of point to use function MPI_Scatter, or all processor know vars and the processors process her own data vectors

    // Get the best k value
    // floor(sqrt(config.nTuples))
    start = omp_get_wtime();
    std::pair<unsigned int, unsigned int> bestHyperParams = getBestHyperParams(1, config.nTuples, dataTrainingPoints, dataTestPoints, euclideanDistance);
    std::cout << "Best value of k: " << bestHyperParams.first << "\nBest numbers of features: " << bestHyperParams.second << std::endl;
    end = omp_get_wtime();
    std::cout << "Time getBestHyperParams: " << end - start << std::endl;

    int counterSuccessTraining = 0, counterSuccessTest = 0;
    vectorOfData<unsigned int> labelTrainingPredicted;
    vectorOfData<unsigned int> labelsTestPredicted;
    labelTrainingPredicted.resize(dataTrainingPoints.size());
    labelsTestPredicted.resize(dataTestPoints.size());

    start = omp_get_wtime();
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
    end = omp_get_wtime();
    std::cout << "Time KNN: " << end - start << std::endl;

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

    MPI_Finalize();
    return EXIT_SUCCESS;
}