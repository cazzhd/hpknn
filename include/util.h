/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpknn repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number TIN2012-32039 and TIN2015-67020-P.\n
 * Spanish 'Ministerio de Ciencia, Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file utils.h
 * @author Francisco Rodríguez Jiménez
 * @date 23/01/2022
 * @brief Function declarations of Utils implementations
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

#ifndef UTIL_H
#define UTIL_H

/********************************* Includes *******************************/
#include <vector>

/******************************** Constants *******************************/

/********************************* Methods ********************************/
template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>> &original) {
    std::vector<T> aux;
    for (const auto &v : original)
        aux.insert(aux.end(), v.begin(), v.end());
    return aux;
}

#endif