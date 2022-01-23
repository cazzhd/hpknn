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
#include <vector>

#include "config.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
struct Point {
    std::vector<float> data;  //!< Pointer to the data of the point
    float distance;           //!< Distance to the point
    unsigned int label;       //!< Label of the point

    /**
	 * @brief Default Construct a new Point object
	 */
    Point();

    /**
	 * @brief Construct a new Point object
	 * @param data Pointer to the data of the point
	 * @param label Label of the point
	 */
    Point(std::vector<float> data, unsigned int label);

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
 * @param dataPointTest The test data
 * @param distanceFunction The distance function to use
 */
void KNN(int k, std::vector<Point>& dataTraining, Point& dataTest, float (*distanceFunction)(Point&, Point&));

/**
 * @brief Get the Euclidean Distance object
 * @param pointTraining The training data
 * @param pointTest The test data
 * @return float with the Euclidean Distance
 */
float euclideanDistance(Point& pointTraining, Point& pointTest);

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