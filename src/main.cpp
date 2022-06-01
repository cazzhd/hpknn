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
#include "energySaving.h"
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
 * @return pair of the best k and the best accuracy
 */
pair<unsigned int, unsigned int> master(const Config& config) {
    const unsigned int TAM = 3;
    unsigned int chunkProcessed = 0, slavesDone = 0, bestAccuracy = 0;
    pair<unsigned int, unsigned int> bestHyperParamsGlobal = make_pair(0, 0);
    vector<unsigned int> bestHyperParamsLocal;
    bestHyperParamsLocal.resize(TAM);

    MPI_Status status;
    unsigned int totalSlaves = MPI::COMM_WORLD.Get_size() - 1;

    // while (/* there are jobs unprocessed */ || /* there are slaves still working on jobs */) {
    while (slavesDone != totalSlaves) {
        // Wait for incoming slave message
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        // Store rank of the slave who sent the message
        int slaveRank = status.MPI_SOURCE;

        if (status.MPI_TAG == TAG_ASK_FOR_JOB) {
            // If the slave is ready to receive a job, send it one
            MPI_Recv(NULL, 0, MPI_INT, slaveRank, TAG_ASK_FOR_JOB, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (chunkProcessed < config.maxFeatures) {
                MPI_Send(&chunkProcessed, 1, MPI_UNSIGNED, slaveRank, TAG_JOB_DATA, MPI_COMM_WORLD);
                chunkProcessed += config.chunkSize;
            } else {
                // Send stop message to the slave
                MPI_Send(NULL, 0, MPI_INT, slaveRank, TAG_STOP, MPI_COMM_WORLD);
            }
        } else if (status.MPI_TAG == TAG_RESULT) {
            // If the slave sent a result, process it
            MPI_Recv(&bestHyperParamsLocal[0], TAM, MPI_UNSIGNED, slaveRank, TAG_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (bestHyperParamsLocal[2] > bestAccuracy) {
                bestHyperParamsGlobal = make_pair(bestHyperParamsLocal[0], bestHyperParamsLocal[1]);
                bestAccuracy = bestHyperParamsLocal[2];
            }
        } else {
            MPI_Recv(NULL, 0, MPI_INT, slaveRank, TAG_STOP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            slavesDone++;
        }
    }

    return bestHyperParamsGlobal;
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
    MPI_Status status;

    do {
        // First send message to master to ask for a job, and wait for job
        MPI_Send(NULL, 0, MPI_INT, 0, TAG_ASK_FOR_JOB, MPI_COMM_WORLD);
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_JOB_DATA) {
            // Work with data received, process it
            MPI_Recv(&chunkToProcess, 1, MPI_INT, 0, TAG_JOB_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<unsigned int> bestHyperParamsLocal = getBestHyperParamsHeterogeneous(chunkToProcess, 1, config.nTuples, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, config);
            // Print best hyperparameters
            // Send result to master
            MPI_Send(&bestHyperParamsLocal[0], bestHyperParamsLocal.size(), MPI_UNSIGNED, 0, TAG_RESULT, MPI_COMM_WORLD);
        } else {
            // If the master sent a stop message, stop
            MPI_Recv(NULL, 0, MPI_INT, 0, TAG_STOP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(NULL, 0, MPI_INT, 0, TAG_STOP, MPI_COMM_WORLD);
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

    // Initialize the energy to save the energy consumption
    Energy saving;

#pragma omp parallel num_threads(2) if (config.savingEnergy)
    {
        // printf thread id
        if (omp_get_thread_num() == 0 && config.savingEnergy) {
            // Initialize the energy saving to save the energy consumption
            saving.checkEnergyPrice();
        }

        if (config.savingEnergy) {
            saving.checkSleep();
        }
        // Vars for use in both modes
        vector<float> dataTraining, dataTest;
        vector<unsigned int> labelsTraining, labelsTest, MRMR;
        pair<unsigned int, unsigned int> bestHyperParams;
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
            // printf("\nHello from process %d/%d on %s\n", rank, size, processor_name);

            // 3. Get the best k and number of features to use, floor(sqrt(config.nTuples)) // Recommended
            start = MPI_Wtime();
            bestHyperParams = getBestHyperParamsHomogeneous(1, config.nTuples, dataTraining, dataTest, labelsTraining, labelsTest, euclideanDistance, config);
            end = MPI_Wtime();

        } else if (config.mode == "hetero") {
            // Mode hetero for heterogeneous platforms, dynamic balancing
            if (!rank) {
                start = MPI_Wtime();
                bestHyperParams = master(config);
                end = MPI_Wtime();
            } else {
                slave(dataTraining, dataTest, labelsTraining, labelsTest, config);
            }
        }

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
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
