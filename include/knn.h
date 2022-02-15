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
 * @param k The number of neighbors to find
 * @param dataTraining The training data
 * @param dataTest The Point to find the nearest neighbors
 * @param distanceFunction The distance function to use
 * @param nFeatures The number of features to use in the distance function
 * @return label predicted
 */
unsigned int KNN(int k, std::vector<Point>& dataTraining, Point& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int), unsigned int nFeatures);

/**
 * @brief Get the Best K object
 * @param minValueK The minimum value of K with starts
 * @param maxValueK The maximum value of K with ends
 * @param dataTraining The training data
 * @param dataTest The Point to find the nearest neighbors
 * @param distanceFunction The distance function to use
 * @return Pair with the best K and the best numbers of predictions
 */
std::pair<unsigned int, unsigned int> getBestK(unsigned short minValueK, unsigned short maxValueK, std::vector<Point>& dataTraining, std::vector<Point>& dataTest, float (*distanceFunction)(Point&, Point&, unsigned int));

/**
 * @brief Get the Confusion Matrix object
 * @param labels The labels of the test or training data
 * @param labelsPredicted The predicted labels with KNN
 * @param nClasses The number of classes
 * @return std::vector<std::vector<unsigned int>> that contains the confusion matrix
 */
std::vector<std::vector<unsigned int>> getConfusionMatrix(std::vector<unsigned int>& labels, std::vector<unsigned int>& labelsPredicted, unsigned int nClasses);

/**
 * @brief Get the Euclidean Distance object
 * @param pointTraining The training data
 * @param pointTest The test data
 * @param nFeatures The number of features to use in the distance function
 * @return float with the Euclidean Distance
 */
float euclideanDistance(Point& pointTraining, Point& pointTest, unsigned int nFeatures);

/**
 * @brief Get the Manhattan object
 * @param pointTraining The training data
 * @param pointTest The test data
 * @return float with the Manhattan Distance
 */
float manhattanDistance(Point& pointTraining, Point& pointTest);

/**
 * @brief Get the Minkowski object
 * @param pointTraining The training data
 * @param pointTest The test data
 * @return float with the Manhattan Distance
 */
float minkowskiDistance(Point& pointTraining, Point& pointTest);

#endif