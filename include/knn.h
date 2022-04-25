/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpmoon repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number
 * TIN2012-32039 and TIN2015-67020-P.\n Spanish 'Ministerio de Ciencia,
 * Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file knn.h
 * @author Francisco Rodríguez Jiménez
 * @date 26/06/2021
 * @brief Function declarations of the knn algorithm
 * @copyright Hpmoon (c) 2015 EFFICOMP
 */

#ifndef KNN_H
#define KNN_H

/********************************* Includes *******************************/
#include <fstream>
#include <map>
#include <vector>

#include "config.h"

using namespace std;

/******************************** Constants *******************************/

/********************************* Methods ********************************/
struct Point {
    vector<float> data;  //!< Pointer to the data of the point
    unsigned int label;  //!< Label of the point

    /**
     * @brief Default Construct a new Point object
     */
    Point();

    /**
     * @brief Construct a new Point object
     * @param data Pointer to the data of the point
     * @param label Label of the point
     */
    Point(vector<float> data, unsigned int label);

    /**
     * @brief Destroy the Point object
     */
    ~Point();

    /**
     * @brief Overload operator << to print in the standard output info about Point
     * @param os stream output
     * @param o object Point
     * @return std::ostream& stream with info
     */
    friend ostream& operator<<(ostream& os, const Point& o);
};

/**
 * @brief Calculates the distance from testpoint to all data
 * @param dataTraining The reference to training data
 * @param dataTest The reference to data test
 * @param labelsTraining The reference to labels of training data
 * @param distanceFunction The distance function to use
 * @param ptrDataTest The pointer to data test, where use to select one test tuple
 * @param nFeatures The number of features to use in the distance function
 * @param config The configuration of the algorithm
 * @return vectors of pairs with distance and label
 */
vector<pair<float, unsigned int>> getDistances(const vector<float>& dataTraining,
                                               const vector<float>& dataTest,
                                               const vector<unsigned int> labelsTraining,
                                               float (*distanceFunction)(const vector<float>&,
                                                                         const vector<float>&,
                                                                         const unsigned int,
                                                                         const unsigned int,
                                                                         const unsigned int),
                                               const unsigned int ptrDataTest,
                                               const unsigned int nFeatures,
                                               const Config& config);

/**
 * @brief Create a map with
 * @param k number of neighbors used to classify
 * @param distances Vector of pairs with distance and label
 * @return the most frequent class
 */
unsigned int getMostFrequentClass(int k, const vector<pair<float, unsigned int>>& distances);

/**
 * @brief Using the KNN algorithm to find the nearest neighbors
 * @param k The number of neighbors to find
 * @param dataTraining The training data
 * @param dataTest The Point to find the nearest neighbors
 * @param labelsTraining The labels of the training data
 * @param distanceFunction The distance function to use
 * @param ptrDataTest The pointer to data test, where use to select one test tuple
 * @param nFeatures The number of features to use in the distance function
 * @param config The configuration of the algorithm
 * @return label predicted
 */
unsigned int KNN(int k,
                 const vector<float>& dataTraining,
                 const vector<float>& dataTest,
                 const vector<unsigned int>& labelsTraining,
                 float (*distanceFunction)(const vector<float>&,
                                           const vector<float>&,
                                           const unsigned int,
                                           const unsigned int,
                                           const unsigned int),
                 const unsigned int ptrDataTest,
                 const unsigned int nFeatures,
                 const Config& config);

/**
 * @brief Get the Best K object
 * @param minValueK The minimum value of K with starts
 * @param maxValueK The maximum value of K with ends
 * @param dataTraining The training data
 * @param dataTest The Point to find the nearest neighbors
 * @param labelsTraining The labels of the training data
 * @param labelsTest The labels of the test data
 * @param distanceFunction The distance function to use
 * @param config The configuration of the algorithm
 * @return Pair with the best K and the best numbers of predictions
 */
pair<unsigned int, unsigned int> getBestHyperParams(const unsigned short minValueK,
                                                    const unsigned short maxValueK,
                                                    const vector<float>& dataTraining,
                                                    const vector<float>& dataTest,
                                                    const vector<unsigned int>& labelsTraining,
                                                    const vector<unsigned int>& labelsTest,
                                                    float (*distanceFunction)(const vector<float>&,
                                                                              const vector<float>&,
                                                                              const unsigned int,
                                                                              const unsigned int,
                                                                              const unsigned int),
                                                    const Config& config);

/**
 * @brief Get the Confusion Matrix object
 * @param labels The labels of the test or training data
 * @param labelsPredicted The predicted labels with KNN
 * @param nClasses The number of classes
 * @return vector<vector<unsigned int>> that contains the confusion matrix
 */
vector<vector<unsigned int>> getConfusionMatrix(const vector<unsigned int>& labels,
                                                const vector<unsigned int>& labelsPredicted,
                                                const unsigned int nClasses);

/**
 * @brief Get the Score from KNN
 *
 * @param k The number of neighbors to find
 * @param dataTraining The training data
 * @param dataTest The Point to find the nearest neighbors
 * @param labelsTraining The labels of the training data
 * @param labelsTest The labels of the test data
 * @param distanceFunction The distance function to use
 * @param nFeatures The number of features to use in the distance function
 * @param config The configuration of the algorithm
 * @return pair<vector<unsigned int>, unsigned int> that contains the labels predicted and the counter of correct predictions
 */
pair<vector<unsigned int>, unsigned int> getScoreKNN(const int k,
                                                     const vector<float>& dataTraining,
                                                     const vector<float>& dataTest,
                                                     const vector<unsigned int>& labelsTraining,
                                                     const vector<unsigned int>& labelsTest,
                                                     float (*distanceFunction)(const vector<float>&,
                                                                               const vector<float>&,
                                                                               const unsigned int,
                                                                               const unsigned int,
                                                                               const unsigned int),
                                                     const unsigned int nFeatures,
                                                     const Config& config);

/**
 * @brief Get the Euclidean Distance object
 * @param dataTraining The training data
 * @param dataTest The test data
 * @param ptrDataTraining The pointer to data training, where use to select one training tuple
 * @param ptrDataTest The pointer to data test, where use to select one test tuple
 * @param nFeatures The number of features to use in the distance function
 * @return float with the Euclidean Distance
 */
float euclideanDistance(const vector<float>& dataTraining,
                        const vector<float>& dataTest,
                        const unsigned int ptrDataTraining,
                        const unsigned int ptrDataTest,
                        const unsigned int nFeatures);

/**
 * @brief Get the Manhattan object
 * @param dataTraining The training data
 * @param dataTest The test data
 * @param ptrDataTraining The pointer to data training, where use to select one training tuple
 * @param ptrDataTest The pointer to data test, where use to select one test tuple
 * @param nFeatures The number of features to use in the distance function
 * @return float with the Manhattan Distance
 */
float manhattanDistance(const vector<float>& dataTraining,
                        const vector<float>& dataTest,
                        const unsigned int ptrDataTraining,
                        const unsigned int ptrDataTest,
                        const unsigned int nFeatures);

#endif