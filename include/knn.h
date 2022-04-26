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

/******************************** Constants *******************************/

/********************************* Methods ********************************/
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
std::vector<std::pair<float, unsigned int>> getDistances(std::vector<float>& dataTraining,
                                                         std::vector<float>& dataTest,
                                                         std::vector<unsigned int> labelsTraining,
                                                         float (*distanceFunction)(std::vector<float>&,
                                                                                   std::vector<float>&,
                                                                                   unsigned int,
                                                                                   unsigned int,
                                                                                   unsigned int),
                                                         unsigned int ptrDataTest,
                                                         unsigned int nFeatures,
                                                         const Config& config);

/**
 * @brief Create a map with
 * @param k number of neighbors used to classify
 * @param distances Vector of pairs with distance and label
 * @return the most frequent class
 */
unsigned int getMostFrequentClass(int k, std::vector<std::pair<float, unsigned int>>& distances);

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
                                                         const Config& config);

/**
 * @brief Get the Confusion Matrix object
 * @param labels The labels of the test or training data
 * @param labelsPredicted The predicted labels with KNN
 * @param nClasses The number of classes
 * @return std::vector<std::vector<unsigned int>> that contains the confusion matrix
 */
std::vector<std::vector<unsigned int>> getConfusionMatrix(std::vector<unsigned int>& labels,
                                                          std::vector<unsigned int>& labelsPredicted,
                                                          unsigned int nClasses);

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
 * @return std::pair<std::vector<unsigned int>, unsigned int> that contains the labels predicted and the counter of correct predictions
 */
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
float euclideanDistance(std::vector<float>& dataTraining,
                        std::vector<float>& dataTest,
                        unsigned int ptrDataTraining,
                        unsigned int ptrDataTest,
                        unsigned int nFeatures);

/**
 * @brief Get the Manhattan object
 * @param dataTraining The training data
 * @param dataTest The test data
 * @param ptrDataTraining The pointer to data training, where use to select one training tuple
 * @param ptrDataTest The pointer to data test, where use to select one test tuple
 * @param nFeatures The number of features to use in the distance function
 * @return float with the Manhattan Distance
 */
float manhattanDistance(std::vector<float>& dataTraining,
                        std::vector<float>& dataTest,
                        unsigned int ptrDataTraining,
                        unsigned int ptrDataTest,
                        unsigned int nFeatures);

#endif