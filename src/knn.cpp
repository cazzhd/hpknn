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
Point::Point() : data(nullptr), distance(0), label(-1) {}

Point::Point(float& data, float distance, unsigned int label, const Config& config) {
    float* data_ = new float[config.nFeatures];

    // Copy data
    for (unsigned int i = 0; i < config.nFeatures; ++i) {
        data_[i] = (data + i);
    }

    this->data = data_;
    this->distance = distance;
    this->label = label;
}

Point::~Point() {
    if (this->data != nullptr) {
        delete[] this->data;
    }
}

std::ostream& operator<<(std::ostream& os, const Point& o) {
    os << "dataPointer: " << *o.data << std::endl;
    os << "distance: " << o.distance << std::endl;
    os << "label: " << o.label << std::endl;

    return os;
}

void KNN(int k, const Point& dataTraining, const Point& dataTest, float (*distanceFunction)(const float&, const float&, const Config&), const Config& config) {
    float* actualTupleTest = new float[config.nFeatures];
    float* actualTupleTraining = new float[config.nFeatures];

    for (int i = 0; i < config.nTuples; ++i) {
        memcpy(actualTupleTest, &dataTest + i * config.nFeatures, config.nFeatures * sizeof(float));
        float* vectorDistances = new float[config.nTuples];
        for (int j = 0; j < config.nTuples; ++j) {
            memcpy(actualTupleTraining, &dataTraining + j * config.nFeatures, config.nFeatures * sizeof(float));
            std::cout << actualTupleTraining << std::endl;
            float distance = distanceFunction(*actualTupleTraining, *actualTupleTest, config);
            vectorDistances[j] = distance;
        }
        std::sort(vectorDistances, vectorDistances + config.nTuples);
        delete[] vectorDistances;
    }

    delete[] actualTupleTest;
    delete[] actualTupleTraining;
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
