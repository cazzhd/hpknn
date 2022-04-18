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
 * @file util.h
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
std::pair<T, T> min_max_value(const std::vector<std::vector<T>> &data) {
    std::vector<T> maxValues, minValues;
    for (const auto &row : data) {
        maxValues.push_back(*max_element(row.begin(), row.end()));
        minValues.push_back(*min_element(row.begin(), row.end()));
    }

    return std::make_pair(*min_element(minValues.begin(), minValues.end()),
                          *max_element(maxValues.begin(), maxValues.end()));
}

template <typename T>
std::vector<std::vector<T>> normalize(const std::vector<std::vector<T>> &data) {
    std::vector<std::vector<T>> normalizedData;
    std::pair<T, T> minMaxValue = min_max_value(data);

    for (const auto &row : data) {
        std::vector<T> normalizedRow;
        for (const auto &value : row) {
            T normalizedValue = (value - minMaxValue.first) / (minMaxValue.second - minMaxValue.first);
            normalizedRow.push_back(normalizedValue);
        }
        normalizedData.push_back(normalizedRow);
    }
    return normalizedData;
}

template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>> &original) {
    std::vector<T> aux;
    for (const auto &v : original)
        aux.insert(aux.end(), v.begin(), v.end());
    return aux;
}

template <typename T>
std::vector<std::vector<T>> sorting_by_indices_vector(const std::vector<std::vector<T>> &original, const std::vector<unsigned int> &indices) {
    std::vector<std::vector<T>> sorted;
    for (unsigned int i = 0; i < original.size(); ++i) {
        std::vector<T> row;
        for (unsigned int j = 0; j < original[i].size(); ++j) {
            row.push_back(original[i][indices[j]]);
        }
        sorted.push_back(row);
    }

    return sorted;
}

template <typename T>
std::vector<T> sorting_by_indices_vector(const std::vector<T> &original, const std::vector<unsigned int> &indices) {
    std::vector<T> sorted;
    for (unsigned int i = 0; i < original.size(); ++i) {
        sorted.push_back(original[indices[i]]);
    }

    return sorted;
}

template <typename T>
void printMatrix(const std::vector<std::vector<T>> &matrix) {
    for (const auto &row : matrix) {
        for (const auto &value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

#endif