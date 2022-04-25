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

using namespace std;

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Point::Point() : label(-1) {}

Point::Point(vector<float> data, unsigned int label) {
    // this->data = data;
    this->label = label;
#pragma omp critical
    {
        copy(data.begin(), data.end(), this->data);
    }
}

Point::~Point() {
    // this->data.clear();
}

ostream& operator<<(ostream& os, const Point& o) {
    os << "data: " << o.data[0] << endl;
    os << "label: " << o.label << endl;

    return os;
}

vector<pair<float, unsigned int>> getDistances(vector<float>& dataTraining,
                                               vector<float>& dataTestTuple,
                                               vector<unsigned int> labelsTraining,
                                               float (*distanceFunction)(std::vector<float>&, std::vector<float>&, unsigned int, unsigned int, unsigned int),
                                               unsigned int ptrDataTest,
                                               unsigned int nFeatures) {
    vector<pair<float, unsigned int>> distances;

    // Recorrer dataTraining
    for (unsigned int i = 0; i < dataTraining.size() / 3600; ++i) {
        float distance = distanceFunction(dataTraining, dataTestTuple, i * 3600, ptrDataTest, nFeatures);
        distances.push_back(make_pair(distance, labelsTraining[i]));
    }

    // for (size_t i = 0; i < dataTestTuple.size(); ++i) {
    // vector<float> dataTrainingTuple(dataTraining.begin() + i * nFeatures, dataTraining.begin() + (i + 1) * nFeatures);
    // distances.push_back(make_pair(distanceFunction(dataTrainingTuple, dataTestTuple, nFeatures), labelsTraining[i]));
    // dataTraining[i].distance = distanceFunction(dataTraining[i], dataTest, nFeatures);
    // }

    // sort(distances.begin(), distances.end(), [](Point& a, Point& b) {
    //     return a.first < b.distance;
    // });

    sort(distances.begin(), distances.end(), [](const pair<float, unsigned int>& a, const pair<float, unsigned int>& b) {
        return a.first < b.first;
    });

    return distances;
}

unsigned int getMostFrequentClass(int k, vector<pair<float, unsigned int>>& distances) {
    map<unsigned int, int, greater<int>> counters;
    for (auto it = distances.begin(); it != distances.begin() + k; ++it) {
        counters[it->second]++;
    }

    return counters.begin()->first;
}

unsigned int KNN(int k,
                 vector<float>& dataTraining,
                 vector<float>& dataTest,
                 vector<unsigned int>& labelsTraining,
                 float (*distanceFunction)(vector<float>&, vector<float>&, unsigned int, unsigned int, unsigned int),
                 unsigned int ptrDataTest,
                 unsigned int nFeatures) {
    vector<pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest, labelsTraining, distanceFunction, ptrDataTest, nFeatures);
    return getMostFrequentClass(k, distances);

    // cout << counters.begin()->first << endl;
    // cout << dataTest.label << endl;
    // cout << ((counters.begin()->first == dataTest.label) ? "True" : "False") << endl;

    // for (auto counter : counters) {
    //     cout << counter.first << ": " << counter.second << endl;
    // }

    // float* actualTupleTest = new float[config.nFeatures];
    // float* actualTupleTraining = new float[config.nFeatures];

    // for (int i = 0; i < config.nTuples; ++i) {
    //     memcpy(actualTupleTest, &dataTest + i * config.nFeatures, config.nFeatures * sizeof(float));
    //     float* vectorDistances = new float[config.nTuples];
    //     for (int j = 0; j < config.nTuples; ++j) {
    //         memcpy(actualTupleTraining, &dataTraining + j * config.nFeatures, config.nFeatures * sizeof(float));
    //         cout << actualTupleTraining << endl;
    //         float distance = distanceFunction(*actualTupleTraining, *actualTupleTest, config);
    //         vectorDistances[j] = distance;
    //     }
    //     sort(vectorDistances, vectorDistances + config.nTuples);
    //     delete[] vectorDistances;
    // }

    // delete[] actualTupleTest;
    // delete[] actualTupleTraining;
}

pair<unsigned int, unsigned int> getBestHyperParams(unsigned short minValueK,
                                                    unsigned short maxValueK,
                                                    vector<float>& dataTraining,
                                                    vector<float>& dataTest,
                                                    vector<unsigned int>& labelsTraining,
                                                    vector<unsigned int>& labelsTest,
                                                    float (*distanceFunction)(vector<float>&, vector<float>&, unsigned int, unsigned int, unsigned int),
                                                    const Config& config) {
    unsigned int bestK = 0;
    unsigned int bestNFeatures = 0;
    float bestAccuracy = 0;
    tqdm bar;
    bar.set_theme_braille();

    // Iterate for all features
    for (unsigned int f = 1; f < 500; ++f) {
        bar.progress(f, 500);
        vector<float> vectorAccuracies(maxValueK - minValueK + 1, 0);
#pragma omp parallel for schedule(dynamic)
        for (unsigned int i = 0; i < dataTest.size() / config.nFeatures; ++i) {
            vector<pair<float, unsigned int>> distances = getDistances(dataTraining, dataTest, labelsTraining, distanceFunction, i * config.nFeatures, f);
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
            vectorAccuracies[i] /= dataTest.size();
            if (vectorAccuracies[i] > bestAccuracy) {
                bestAccuracy = vectorAccuracies[i];
                bestK = i + minValueK;
                bestNFeatures = f;
            }
        }
    }

    bar.finish();
    return make_pair(bestK, bestNFeatures);
}

vector<vector<unsigned int>> getConfusionMatrix(vector<unsigned int>& labels, vector<unsigned int>& labelsPredicted, unsigned int nClasses) {
    vector<vector<unsigned int>> confusionMatrix(nClasses, vector<unsigned int>(nClasses));

    for (unsigned int i = 0; i < labels.size(); ++i) {
        confusionMatrix[labels[i]][labelsPredicted[i]]++;
    }

    return confusionMatrix;
}

pair<vector<unsigned int>, unsigned int> getScoreKNN(int k,
                                                     vector<float>& dataTraining,
                                                     vector<float>& dataTest,
                                                     vector<unsigned int>& labelsTraining,
                                                     vector<unsigned int>& labelsTest,
                                                     float (*distanceFunction)(vector<float>&, vector<float>&, unsigned int, unsigned int, unsigned int),
                                                     unsigned int nFeatures,
                                                     const Config& config) {
    unsigned int counterSuccess = 0;
    vector<unsigned int> labelsPredicted;
    labelsPredicted.resize(dataTraining.size());

#pragma omp parallel for
    for (unsigned int i = 0; i < dataTest.size() / config.nFeatures; ++i) {
        unsigned int labelPredicted = KNN(k, dataTraining, dataTest, labelsTraining, distanceFunction, i * config.nFeatures, nFeatures);
        labelsPredicted[i] = labelPredicted;
        if (labelPredicted == labelsTest[i]) {
#pragma omp atomic
            counterSuccess++;
        }
    }

    return make_pair(labelsPredicted, counterSuccess);
}

float euclideanDistance(std::vector<float>& pointTraining,
                        std::vector<float>& pointTest,
                        unsigned int ptrDataTraining,
                        unsigned int ptrDataTest,
                        unsigned int nFeatures) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (unsigned int i = 0; i < nFeatures; ++i) {
        distance += pow((pointTraining[ptrDataTraining + i]) - (pointTest[ptrDataTest + i]), 2);
    }

    return sqrt(distance);
}

float manhattanDistance(vector<float>& pointTraining, vector<float>& pointTest, unsigned int nFeatures) {
    float distance = 0;

    // #pragma omp parallel for simd reduction(+: distance)
    for (long unsigned int i = 0; i < nFeatures; ++i) {
        distance += abs((pointTraining[i]) - (pointTest[i]));
    }

    return distance;
}
