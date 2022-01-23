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
struct Point {
    float* data;         //!< Pointer to the data of the point
    float distance;      //!< Distance to the point
    unsigned int label;  //!< Label of the point

    /**
	 * @brief Default Construct a new Point object
	 */
    Point();

    /**
	 * @brief Construct a new Point object
	 * @param data Pointer to the data of the point
	 * @param distance Distance to the point
	 * @param label Label of the point
	 * @param config Configuration of the problem
	 */
    Point(float& data, float distance, unsigned int label, const Config& config);

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
    friend std::ostream& operator<<(std::ostream& os, const Point& o);
};

/**
 * @brief Using the KNN algorithm to find the nearest neighbors
 * @param K The number of neighbors to find
 * @param dataTraining The training data
 * @param dataTest The test data
 * @param distanceFunction The distance function to use
 */
void KNN(int k, const Point& dataTraining, const Point& dataTest, float (*distanceFunction)(const float&, const float&, const Config&), const Config& config);

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