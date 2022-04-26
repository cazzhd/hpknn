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
#include <mpi.h>
#include <omp.h>
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
#include "util.h"

using namespace std;

/********************************* Main ********************************/
/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(int argc, char* argv[]) {
    int size, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    Config config(argc, argv);
    const unsigned int TAM = config.nTuples * config.nFeatures;

    // 1. Read data from files
    vector<float> dataTraining, dataTest;
    vector<unsigned int> labelsTraining, labelsTest, MRMR;
    // labelsTraining.resize(config.nTuples);
    double start, end;

    // Present each process with mpi
    printf("\nHybrid: Hello from process %d/%d on %s\n", rank, size, processor_name);

    if (!rank) {
        start = MPI_Wtime();
        readDataFromFiles(dataTraining, dataTest, labelsTraining, labelsTest, MRMR, config);
        end = MPI_Wtime();
        cout << "Time to read data: " << end - start << endl;
    }

    // 2. Sorting by best features (MRMR), get best scores
    if (!rank) {
        start = MPI_Wtime();
        sortFeaturesByMRMR(dataTraining, dataTest, MRMR, config);
        end = MPI_Wtime();
        cout << "Time to sort data with MRMR: " << end - start << endl;
    }

    // // dataTraining bcast to all processes
    // MPI_Bcast(&dataTraining[0], dataTraining.size(), MPI_FLOAT, 0, MPI_COMM_WORLD);
    // MPI_Bcast(&labelsTraining[0], labelsTraining.size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // 3. Get the best k and number of features to use
    // floor(sqrt(config.nTuples)) // Recommended
    if (!rank) {
        start = MPI_Wtime();
        pair<unsigned int, unsigned int> bestHyperParams = getBestHyperParams(1, config.nTuples, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, config);
        end = MPI_Wtime();
        cout << "Best value of k: " << bestHyperParams.first << "\nBest numbers of features: " << bestHyperParams.second << endl;
        cout << "Time getBestHyperParams: " << end - start << endl;

        // 4. To finalize get the score of the best k and number of features
        start = MPI_Wtime();
        pair<vector<unsigned int>, unsigned int> scoreTest = getScoreKNN(bestHyperParams.first, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, bestHyperParams.second, config);
        pair<vector<unsigned int>, unsigned int> scoreTraining = getScoreKNN(bestHyperParams.first, dataTraining, dataTraining, labelsTraining, labelsTraining, euclideanDistance, bestHyperParams.second, config);
        end = MPI_Wtime();
        cout << "Time KNN: " << end - start << endl;

        // 5. Get Confusion Matrix for test
        vector<vector<unsigned int>> confusionMatrixTest = getConfusionMatrix(labelsTest, scoreTest.first, config.nClasses);
        cout << "Confusion Matrix Test: " << endl;
        printMatrix(confusionMatrixTest);

        // vectorOfVectorData<unsigned int> confusionMatrixTraining = getConfusionMatrix(labelsTraining, scoreTraining.first, config.nClasses);
        // cout << "Confusion Matrix Training: " << endl;
        // printMatrix(confusionMatrixTraining);

        cout << "Accuracy of K-NN classifier on training set: " << ((float)scoreTraining.second / (float)config.nTuples) << endl;
        cout << "Accuracy of K-NN classifier on test set: " << ((float)scoreTest.second / (float)config.nTuples) << endl;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}