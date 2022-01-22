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

#include "config.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
/**
 * @brief Using the KNN algorithm to find the nearest neighbors
 * @param K The number of neighbors to find
 * @param dataTraining The training data
 * @param dataTest The test data
 * @param distanceFunction The distance function to use
 */
void KNN(const float& dataTraining, const float& dataTest, float (*distanceFunction)(const float&, const float&, const Config&), const Config& config);

/**
 * @brief Get the Euclidean Distance object
 * @param tupleTraining The training data
 * @param tupleTest The test data
 * @return float with the Euclidean Distance
 */
float euclideanDistance(const float& tupleTraining, const float& tupleTest, const Config& config);

/**
 * @brief Get the Manhattan object
 * @return float with the Manhattan Distance
 */
float manhattanDistance();

/**
 * @brief Get the Minkowski object
 * @return float with the Manhattan Distance
 */
float minkowskiDistance();

#endif