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
#include <omp.h>

#include <vector>

#include "db.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
template <typename T>
/**
 * @brief Function that return the maximun and minimum value of a vector
 * @param data The vector to search
 * @return std::pair<T, T> The pair of the maximun and minimum value
 */
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
/**
 * @brief Function that normalize the data of a vector
 * @param data The vector to normalize
 * @return std::vector<std::vector<T>> The normalized vector
 */
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
/**
 * @brief Convert vector of vector of T to vector of T (flatten vector)
 * @param original The original vector
 * @return std::vector<T> The flatten vector
 */
std::vector<T> flatten(const std::vector<std::vector<T>> &original) {
    std::vector<T> aux;
    for (const auto &v : original)
        aux.insert(aux.end(), v.begin(), v.end());
    return aux;
}

template <typename T>
/**
 * @brief Sort colum of vector of T by a vector of indices
 * @param original The original vector
 * @param indexes The indexes to sort the vector by column
 * @param config The configuration of project to get nFeatures
 * @return std::vector<T>
 */
std::vector<T> sorting_by_indexes_vector(const std::vector<T> &original, const std::vector<unsigned int> &indexes, const Config &config) {
    std::vector<T> sorted;
    unsigned int nTuples = original.size() / config.nFeatures;

    for (unsigned int i = 0; i < nTuples; ++i) {
        for (unsigned int j = 0; j < config.nFeatures; ++j) {
            sorted.push_back(original[i * config.nFeatures + indexes[j]]);
        }
    }

    return sorted;
}

template <typename T>
/**
 * @brief Print a vector of vector of T as a matrix
 * @param matrix The matrix to print
 */
void printMatrix(const std::vector<std::vector<T>> &matrix) {
    for (const auto &row : matrix) {
        for (const auto &value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Function that read de data from files of config and fill vectors, if use function normalize get best scores
 * @param dataTraining vector of data training
 * @param dataTest vector of data test
 * @param labelsTraining vector of labels training
 * @param labelsTest vector of labels test
 * @param MRMR vector of MRMR
 * @param config configuration of program
 */
void readDataFromFiles(std::vector<float> &dataTraining,
                       std::vector<float> &dataTest,
                       std::vector<unsigned int> &labelsTraining,
                       std::vector<unsigned int> &labelsTest,
                       std::vector<unsigned int> &MRMR,
                       Config &config) {
    CSVReader csvReader = CSVReader();

#pragma omp parallel sections
    {
#pragma omp section
        {
            dataTraining = csvReader.readData<float>(config.dbDataTraining);
        }
#pragma omp section
        {
            dataTest = csvReader.readData<float>(config.dbDataTest);
        }
#pragma omp section
        {
            labelsTraining = csvReader.readData<unsigned int>(config.dbLabelsTraining);
        }
#pragma omp section
        {
            labelsTest = csvReader.readData<unsigned int>(config.dbLabelsTest);
        }
#pragma omp section
        {
            MRMR = csvReader.readData<unsigned int>(config.MRMR);
        }
    }
}

/**
 * @brief sort dataTraining and dataTest by MRMR vector
 * @param dataTraining vector of data training
 * @param dataTest vector of data test
 * @param MRMR vector of MRMR
 * @param config configuration of program
 */
void sortFeaturesByMRMR(std::vector<float> &dataTraining, std::vector<float> &dataTest, const std::vector<unsigned int> &MRMR, const Config &config) {
#pragma omp parallel sections
    {
#pragma omp section
        {
            dataTraining = sorting_by_indexes_vector(dataTraining, MRMR, config);
        }
#pragma omp section
        {
            dataTest = sorting_by_indexes_vector(dataTest, MRMR, config);
        }
    }
}

#endif