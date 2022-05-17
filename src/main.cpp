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

#define TAG_RESULT 0
#define TAG_ASK_FOR_JOB 1
#define TAG_JOB_DATA 2
#define TAG_STOP 3

using namespace std;

/**
 * @brief master function executed by the master process
 * managing the slaves with send jobs and receiving results using dinamyc balancing
 * @param config configuration parameters
 */
void master(const Config& config) {
    unsigned int chunkProcessed = 0;
    unsigned int bestAccuracy = 0;
    pair<unsigned int, unsigned int> bestHyperParamsGlobal = make_pair(0, 0);
    vector<unsigned int> bestHyperParamsLocal;
    bestHyperParamsLocal.resize(3);
    MPI_Status status, status2;

    // while (/* there are jobs unprocessed */ || /* there are slaves still working on jobs */) {
    while (true) {
        // Wait for incoming slave message
        printf("Master wait for request slave\n");
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // Store rank of the slave who sent the message
        int slave_rank = status.MPI_SOURCE;
        if (status.MPI_TAG == TAG_ASK_FOR_JOB) {
            printf("Master recive request\n");
            // If the slave is ready to receive a job, send it one
            if (chunkProcessed < config.maxFeatures) {
                printf("Master send job: %d \n", chunkProcessed);
                MPI_Send(&chunkProcessed, 1, MPI_UNSIGNED, slave_rank, TAG_JOB_DATA, MPI_COMM_WORLD);
                chunkProcessed += config.chunkSize;
            } else {
                // Send stop message to the slave
                printf("Master send stop\n");
                MPI_Send(NULL, 0, MPI_INT, slave_rank, TAG_STOP, MPI_COMM_WORLD);
            }
        } else {
            // If the slave sent a result, process it
            printf("Master recive result\n");
            MPI_Recv(&bestHyperParamsLocal, 3, MPI_UNSIGNED, slave_rank, TAG_RESULT, MPI_COMM_WORLD, &status2);
            if (bestHyperParamsLocal[2] > bestAccuracy) {
                bestHyperParamsGlobal = make_pair(bestHyperParamsLocal[0], bestHyperParamsLocal[1]);
                bestAccuracy = bestHyperParamsLocal[2];
            }
            printf("Master best hyperparams: %d:%d with accuracy: %d\n", bestHyperParamsGlobal.first,
                   bestHyperParamsGlobal.second, bestAccuracy);
        }
    }
}

/**
 * @brief slave function executed by the slave processes
 * receiving jobs and sending results
 * @param dataTraining data for training
 * @param dataTest data for testing
 * @param labelsTraining labels for training
 * @param labelsTest labels for testing
 * @param config configuration parameters
 */
void slave(vector<float>& dataTraining,
           vector<float>& dataTest,
           vector<unsigned int>& labelsTraining,
           vector<unsigned int>& labelsTest,
           const Config& config) {
    bool hasWork = true;
    unsigned int chunkToProcess = 0;
    MPI_Status status, status2;

    do {
        // First send message to master to ask for a job, and wait for job
        printf("Slave asking for a job\n");
        MPI_Send(NULL, 0, MPI_INT, 0, TAG_ASK_FOR_JOB, MPI_COMM_WORLD);
        printf("Slave wait for job\n");
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_JOB_DATA) {
            // Work with data received, process it
            MPI_Recv(&chunkToProcess, 1, MPI_UNSIGNED, 0, TAG_JOB_DATA, MPI_COMM_WORLD, &status2);
            printf("Slave recive job %d\n", chunkToProcess);
            vector<unsigned int> bestHyperParamsLocal = getBestHyperParamsHeterogeneous(chunkToProcess, 1, config.nTuples, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, config);
            // Print best hyperparameters
            printf("Best hyperparameters: %d %d %d\n", bestHyperParamsLocal[0], bestHyperParamsLocal[1], bestHyperParamsLocal[2]);
            // Send result to master
            printf("Slave send result to master %d\n", chunkToProcess);
            MPI_Send(&bestHyperParamsLocal, bestHyperParamsLocal.size(), MPI_UNSIGNED, 0, TAG_RESULT, MPI_COMM_WORLD);
            printf("Slave send success %d\n", chunkToProcess);
        } else {
            printf("Slave recive stop\n");
            // If the master sent a stop message, stop
            MPI_Recv(NULL, 0, MPI_INT, 0, TAG_STOP, MPI_COMM_WORLD, &status2);
            hasWork = !hasWork;
        }
    } while (hasWork);
}

/********************************* Main ********************************/
/**
 * @brief Main program
 * @param argc The number of arguments of the program
 * @param argv Arguments of the program
 */
int main(int argc, char* argv[]) {
    int size, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    // Initialize enviroment MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    // Initialize the configuration
    Config config(argc, argv);

    // Vars for use in both modes
    vector<float> dataTraining, dataTest;
    vector<unsigned int> labelsTraining, labelsTest, MRMR;
    double start, end;

    // 1. Read data from files
    readDataFromFiles(dataTraining, dataTest, labelsTraining, labelsTest, MRMR, config);

    // 2. Sorting by best features (MRMR), get best scores
    if (config.sortingByMRMR) {
        sortFeaturesByMRMR(dataTraining, dataTest, MRMR, config);
    }

    // Mode homo for homogeneous platforms, static balancing
    if (config.mode == "homo") {
        // Present each process with mpi
        printf("\nHello from process %d/%d on %s\n", rank, size, processor_name);

        // 3. Get the best k and number of features to use, floor(sqrt(config.nTuples)) // Recommended
        start = MPI_Wtime();
        pair<unsigned int, unsigned int> bestHyperParams = getBestHyperParamsHomogeneous(1, config.nTuples, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, config);
        end = MPI_Wtime();

        if (!rank) {
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

            cout << "Accuracy of K-NN classifier on training set: " << ((float)scoreTraining.second / (float)config.nTuples) << endl;
            cout << "Accuracy of K-NN classifier on test set: " << ((float)scoreTest.second / (float)config.nTuples) << endl;
        }
    } else if (config.mode == "hetero") {
        // Mode hetero for heterogeneous platforms, dynamic balancing
        if (!rank) {
            master(config);
        } else {
            slave(dataTraining, dataTest, labelsTraining, labelsTest, config);
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
