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

/******************************** Constants *******************************/

/********************************* Methods ********************************/
double getEuclideanDistance();

double getManhattan();

double getMinkowski();

#endif