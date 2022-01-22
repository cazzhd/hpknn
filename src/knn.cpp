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

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

/******************************** Constants *******************************/

/********************************* Methods ********************************/

void KNN(const float& dataTraining, const float& dataTest, float (*distanceFunction)(const float&, const float&, const Config&), const Config& config) {
    for (int i = 0; i < config.nTuples; ++i) {
        float* actualTupleTest = new float[config.nFeatures];
        memcpy(actualTupleTest, &dataTest + i * config.nFeatures, config.nFeatures * sizeof(float));
        float* vectorDistances = new float[config.nTuples];
        for (int j = 0; j < config.nTuples; ++j) {
            float* actualTupleTraining = new float[config.nFeatures];
            memcpy(actualTupleTraining, &dataTraining + j * config.nFeatures, config.nFeatures * sizeof(float));
            float distance = distanceFunction(*actualTupleTraining, *actualTupleTest, config);
            vectorDistances[j] = distance;
        }
        std::sort(vectorDistances, vectorDistances + config.nTuples);
    }
}

float euclideanDistance(const float& tupleTraining, const float& tupleTest, const Config& config) {
    float distance = 0;
    for (int i = 0; i < config.nFeatures; ++i) {
        distance += pow((tupleTraining + i) - (tupleTest + i), 2);
    }
    return sqrt(distance);
}

float manhattanDistance() {}

float minkowskiDistance() {}
