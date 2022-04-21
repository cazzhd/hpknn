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

#include "omp.h"
#include "tqdm.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Point::Point() : label(-1) {}

Point::Point(std::vector<float> data, unsigned int label) {
    this->data = data;
    this->label = label;
}

Point::~Point() {
    this->data.clear();
}

std::ostream& operator<<(std::ostream& os, const Point& o) {
    os << "data: " << o.data[0] << std::endl;
    os << "label: " << o.label << std::endl;

    return os;
}

std::vector<std::pair<float, unsigned int>> getDistances(std::vector<Point>& dataTraining, Point& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int), unsigned int nFeatures) {
    std::vector<std::pair<float, unsigned int>> distances;

    for (size_t i = 0; i < dataTraining.size(); ++i) {
        distances.push_back(std::make_pair(distanceFunction(dataTraining[i], dataTest, nFeatures), dataTraining[i].label));
        // dataTraining[i].distance = distanceFunction(dataTraining[i], dataTest, nFeatures);
    }

    // sort(distances.begin(), distances.end(), [](Point& a, Point& b) {
    //     return a.first < b.distance;
    // });

    std::sort(distances.begin(), distances.end(), [](const std::pair<float, unsigned int>& a, const std::pair<float, unsigned int>& b) {
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

unsigned int KNN(int k, std::vector<Point>& dataTraining, Point& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int), unsigned int nFeatures) {
    std::vector<std::pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest, distanceFunction, nFeatures);
    return getMostFrequentClass(k, distances);

    // std::cout << counters.begin()->first << std::endl;
    // std::cout << dataTest.label << std::endl;
    // std::cout << ((counters.begin()->first == dataTest.label) ? "True" : "False") << std::endl;

    // for (auto counter : counters) {
    //     std::cout << counter.first << ": " << counter.second << std::endl;
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

std::pair<unsigned int, unsigned int> getBestHyperParams(unsigned short minValueK, unsigned short maxValueK, std::vector<Point>& dataTraining, std::vector<Point>& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int)) {
    unsigned int bestK = 0;
    unsigned int bestNFeatures = 0;
    float bestAccuracy = 0;
    tqdm bar;
    bar.set_theme_braille();

    // Iterate for all features
    for (unsigned int f = 1; f < dataTraining[0].data.size(); ++f) {
        bar.progress(f, dataTraining[0].data.size());
        std::vector<float> vectorAccuracies(maxValueK - minValueK + 1, 0);
#pragma omp parallel for schedule(dynamic)
        for (unsigned int i = 0; i < dataTest.size(); ++i) {
            std::vector<std::pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest[i], distanceFunction, f);
            for (unsigned int k = minValueK; k <= maxValueK; ++k) {
                unsigned int labelPredicted = getMostFrequentClass(k, distances);
                if (labelPredicted == dataTest[i].label) {
                    vectorAccuracies[k - minValueK]++;
                }
            }
        }
// Iterate for vectorAccuracies
#pragma omp parallel for schedule(dynamic)
        for (unsigned int i = 0; i < vectorAccuracies.size(); ++i) {
            vectorAccuracies[i] /= dataTest.size();
            if (vectorAccuracies[i] > bestAccuracy) {
                bestAccuracy = vectorAccuracies[i];
                bestK = i + minValueK;
                bestNFeatures = f;
            }
        }
    }

    bar.finish();
    return std::make_pair(bestK, bestNFeatures);
}

std::vector<std::vector<unsigned int>> getConfusionMatrix(std::vector<unsigned int>& labels, std::vector<unsigned int>& labelsPredicted, unsigned int nClasses) {
    std::vector<std::vector<unsigned int>> confusionMatrix(nClasses, std::vector<unsigned int>(nClasses));

    for (unsigned int i = 0; i < labels.size(); ++i) {
        confusionMatrix[labels[i]][labelsPredicted[i]]++;
    }

    return confusionMatrix;
}

std::pair<std::vector<unsigned int>, unsigned int> getScoreKNN(int k, std::vector<Point>& dataTraining, std::vector<Point>& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int), unsigned int nFeatures) {
    unsigned int counterSuccess = 0;
    std::vector<unsigned int> labelsPredicted;
    labelsPredicted.resize(dataTraining.size());

#pragma omp parallel for
    for (unsigned int i = 0; i < dataTest.size(); ++i) {
        unsigned int labelPredicted = KNN(k, dataTraining, dataTest[i], distanceFunction, nFeatures);
        labelsPredicted[i] = labelPredicted;
        if (labelPredicted == dataTest[i].label) {
#pragma omp atomic
            counterSuccess++;
        }
    }

    return std::make_pair(labelsPredicted, counterSuccess);
}

float euclideanDistance(Point& pointTraining, Point& pointTest, unsigned int nFeatures) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (long unsigned int i = 0; i < nFeatures; ++i) {
        distance += pow((pointTraining.data[i]) - (pointTest.data[i]), 2);
    }

    return sqrt(distance);
}

float manhattanDistance(Point& pointTraining, Point& pointTest, unsigned int nFeatures) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (long unsigned int i = 0; i < nFeatures; ++i) {
        distance += abs((pointTraining.data[i]) - (pointTest.data[i]));
    }

    return distance;
}
