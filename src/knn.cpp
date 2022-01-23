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
Point::Point() : distance(0), label(-1) {}

Point::Point(std::vector<float> data, unsigned int label) {
    this->data = data;
    this->distance = 0;
    this->label = label;
}

Point::~Point() {}

std::ostream& operator<<(std::ostream& os, const Point& o) {
    os << "data: " << o.data[0] << std::endl;
    os << "distance: " << o.distance << std::endl;
    os << "label: " << o.label << std::endl;

    return os;
}

void KNN(int k, std::vector<Point>& dataTraining, Point& dataTest, float (*distanceFunction)(Point&, Point&)) {
    for (size_t i = 0; i < dataTraining.size(); ++i) {
        dataTraining[i].distance = distanceFunction(dataTraining[i], dataTest);
    }

    sort(dataTraining.begin(), dataTraining.end(), [](Point& a, Point& b) {
        return a.distance < b.distance;
    });

    // for (unsigned short i = 0; i < k; ++i) {
    //     std::cout << dataTraining[i] << std::endl;
    // }

    // float* actualTupleTest = new float[config.nFeatures];
    // float* actualTupleTraining = new float[config.nFeatures];

    // for (int i = 0; i < config.nTuples; ++i) {
    //     memcpy(actualTupleTest, &dataTest + i * config.nFeatures, config.nFeatures * sizeof(float));
    //     float* vectorDistances = new float[config.nTuples];
    //     for (int j = 0; j < config.nTuples; ++j) {
    //         memcpy(actualTupleTraining, &dataTraining + j * config.nFeatures, config.nFeatures * sizeof(float));
    //         std::cout << actualTupleTraining << std::endl;
    //         float distance = distanceFunction(*actualTupleTraining, *actualTupleTest, config);
    //         vectorDistances[j] = distance;
    //     }
    //     std::sort(vectorDistances, vectorDistances + config.nTuples);
    //     delete[] vectorDistances;
    // }

    // delete[] actualTupleTest;
    // delete[] actualTupleTraining;
}

float euclideanDistance(Point& pointTraining, Point& pointTest) {
    float distance = 0;

    for (int i = 0; i < pointTraining.data.size(); ++i) {
        distance += pow((pointTraining.data[i]) - (pointTest.data[i]), 2);
    }

    return sqrt(distance);
}

float manhattanDistance() {}

float minkowskiDistance() {}
