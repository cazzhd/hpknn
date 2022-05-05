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
 * @file knn.cpp
 * @author Francisco Rodríguez Jiménez
 * @date 07/07/2021
 * @brief Implementation of the KNN
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/
#include "knn.h"

#include <mpi.h>
#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#include "tqdm.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
std::vector<std::pair<float, unsigned int>> getDistances(std::vector<float>& dataTraining,
                                                         std::vector<float>& dataTestTuple,
                                                         std::vector<unsigned int> labelsTraining,
                                                         float (*distanceFunction)(std::vector<float>&,
                                                                                   std::vector<float>&,
                                                                                   unsigned int,
                                                                                   unsigned int,
                                                                                   unsigned int),
                                                         unsigned int ptrDataTest,
                                                         unsigned int nFeatures,
                                                         const Config& config) {
    std::vector<std::pair<float, unsigned int>> distances;

    unsigned int nTuples = dataTraining.size() / config.nFeatures;
    for (unsigned int i = 0; i < nTuples; ++i) {
        float distance = distanceFunction(dataTraining, dataTestTuple, i * config.nFeatures, ptrDataTest, nFeatures);
        distances.push_back(std::make_pair(distance, labelsTraining[i]));
    }

    sort(distances.begin(), distances.end(), [](const std::pair<float, unsigned int>& a, const std::pair<float, unsigned int>& b) {
        return a.first < b.first;
    });

    return distances;
}

unsigned int getMostFrequentClass(int k, std::vector<std::pair<float, unsigned int>>& distances) {
    std::map<unsigned int, int, std::greater<int>> counters;
    for (auto it = distances.begin(); it != distances.begin() + k; ++it) {
        counters[it->second]++;
    }

    return counters.begin()->first;
}

unsigned int KNN(int k,
                 std::vector<float>& dataTraining,
                 std::vector<float>& dataTest,
                 std::vector<unsigned int>& labelsTraining,
                 float (*distanceFunction)(std::vector<float>&,
                                           std::vector<float>&,
                                           unsigned int,
                                           unsigned int,
                                           unsigned int),
                 unsigned int ptrDataTest,
                 unsigned int nFeatures,
                 const Config& config) {
    std::vector<std::pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest, labelsTraining, distanceFunction, ptrDataTest, nFeatures, config);
    return getMostFrequentClass(k, distances);
}

std::pair<unsigned int, unsigned int> getBestHyperParams(unsigned short minValueK,
                                                         unsigned short maxValueK,
                                                         std::vector<float>& dataTraining,
                                                         std::vector<float>& dataTest,
                                                         std::vector<unsigned int>& labelsTraining,
                                                         std::vector<unsigned int>& labelsTest,
                                                         float (*distanceFunction)(std::vector<float>&,
                                                                                   std::vector<float>&,
                                                                                   unsigned int,
                                                                                   unsigned int,
                                                                                   unsigned int),
                                                         const Config& config) {
    unsigned int bestK = 0;
    unsigned int bestNFeatures = 0;
    unsigned int bestAccuracy = 0;
    // tqdm bar;
    // bar.set_theme_braille();

    // Get rank MPI
    int rank = MPI::COMM_WORLD.Get_rank();
    int size = MPI::COMM_WORLD.Get_size();

    // Iterate for all features
    unsigned int nTuples = dataTest.size() / config.nFeatures;
    unsigned int sizePerProcess = config.nFeatures / size;

    for (unsigned int f = 1 + rank; f < sizePerProcess; f += size) {
        // bar.progress(f, sizePerProcess);
        std::vector<unsigned int> vectorAccuracies(maxValueK - minValueK + 1, 0);
#pragma omp parallel for schedule(dynamic)
        for (unsigned int i = 0; i < nTuples; ++i) {
            std::vector<std::pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest, labelsTraining, distanceFunction, i * config.nFeatures, f, config);
            for (unsigned int k = minValueK; k <= maxValueK; ++k) {
                unsigned int labelPredicted = getMostFrequentClass(k, distances);
                if (labelPredicted == labelsTest[i]) {
                    vectorAccuracies[k - minValueK]++;
                }
            }
        }
// Iterate for vectorAccuracies
#pragma omp parallel for schedule(dynamic)
        for (unsigned int i = 0; i < vectorAccuracies.size(); ++i) {
            if (vectorAccuracies[i] > bestAccuracy) {
                bestAccuracy = vectorAccuracies[i];
                bestK = i + minValueK;
                bestNFeatures = f;
            }
        }
    }

    // The process has best accuracy send bestK and bestNFeatures to root
    std::vector<unsigned int> bestKs(size, 0);
    std::vector<unsigned int> bestNFeaturess(size, 0);
    std::vector<unsigned int> bestAccuracies(size, 0);

    // Each process print bestK and bestNFeatures
    printf("Process %d: bestK = %d, bestNFeatures = %d, bestAccuracy = %d\n", rank, bestK, bestNFeatures, bestAccuracy);

    MPI_Gather(&bestK, 1, MPI_UNSIGNED, bestKs.data(), 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    MPI_Gather(&bestNFeatures, 1, MPI_UNSIGNED, bestNFeaturess.data(), 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    MPI_Gather(&bestAccuracy, 1, MPI_UNSIGNED, bestAccuracies.data(), 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Get index of bestAccuracy
    unsigned int indexBestAccuracy = 0;
    if (!rank) {
        for (unsigned int i = 0; i < bestAccuracies.size(); ++i) {
            if (bestAccuracies[i] > bestAccuracies[indexBestAccuracy]) {
                indexBestAccuracy = i;
            }
        }
    }

    return std::make_pair(bestKs[indexBestAccuracy], bestNFeaturess[indexBestAccuracy]);
    // bar.finish();
}

std::vector<std::vector<unsigned int>> getConfusionMatrix(std::vector<unsigned int>& labels,
                                                          std::vector<unsigned int>& labelsPredicted,
                                                          unsigned int nClasses) {
    std::vector<std::vector<unsigned int>> confusionMatrix(nClasses, std::vector<unsigned int>(nClasses));

    for (unsigned int i = 0; i < labels.size(); ++i) {
        confusionMatrix[labels[i]][labelsPredicted[i]]++;
    }

    return confusionMatrix;
}

std::pair<std::vector<unsigned int>, unsigned int> getScoreKNN(int k,
                                                               std::vector<float>& dataTraining,
                                                               std::vector<float>& dataTest,
                                                               std::vector<unsigned int>& labelsTraining,
                                                               std::vector<unsigned int>& labelsTest,
                                                               float (*distanceFunction)(std::vector<float>&,
                                                                                         std::vector<float>&,
                                                                                         unsigned int,
                                                                                         unsigned int,
                                                                                         unsigned int),
                                                               unsigned int nFeatures,
                                                               const Config& config) {
    unsigned int counterSuccess = 0;
    std::vector<unsigned int> labelsPredicted;
    labelsPredicted.resize(dataTraining.size());

    unsigned int nTuples = dataTest.size() / config.nFeatures;
#pragma omp parallel for
    for (unsigned int i = 0; i < nTuples; ++i) {
        unsigned int labelPredicted = KNN(k, dataTraining, dataTest, labelsTraining, distanceFunction, i * config.nFeatures, nFeatures, config);
        labelsPredicted[i] = labelPredicted;
        if (labelPredicted == labelsTest[i]) {
#pragma omp atomic
            counterSuccess++;
        }
    }

    return make_pair(labelsPredicted, counterSuccess);
}

float euclideanDistance(std::vector<float>& dataTraining,
                        std::vector<float>& dataTest,
                        unsigned int ptrDataTraining,
                        unsigned int ptrDataTest,
                        unsigned int nFeatures) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (unsigned int i = 0; i < nFeatures; ++i) {
        distance += pow((dataTraining[ptrDataTraining + i]) - (dataTest[ptrDataTest + i]), 2);
    }

    return sqrt(distance);
}

float manhattanDistance(std::vector<float>& dataTraining,
                        std::vector<float>& dataTest,
                        unsigned int nFeatures,
                        unsigned int ptrDataTraining,
                        unsigned int ptrDataTest) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (long unsigned int i = 0; i < nFeatures; ++i) {
        distance += abs((dataTraining[ptrDataTraining + i]) - (dataTest[ptrDataTest + i]));
    }

    return distance;
}
