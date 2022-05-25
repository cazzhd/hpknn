/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpknn repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number
 * TIN2012-32039 and TIN2015-67020-P.\n Spanish 'Ministerio de Ciencia,
 * Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file energySaving.h
 * @author Francisco Rodríguez Jiménez
 * @date 25/05/2022
 * @brief Function declarations of energySaving
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

#ifndef ENERGY_SAVING_H
#define ENERGY_SAVING_H

/********************************* Includes *******************************/
#include <string>

/******************************** Constants *******************************/

/******************************** Structures ******************************/

/**
 * @brief Struct of Energy that permit set the energy saving for the program from json response
 * get from the server
 */
typedef struct Energy {
    std::string date;   /**< Date of the energy saving */
    std::string hour;   /**< Hour of the energy saving */
    bool isCheap;       /**< Flag to know if the energy saving is cheap */
    bool isUnderAvg;    /**< Flag to know if the energy saving is under average */
    std::string market; /**< Market of the energy saving */
    float price;        /**< Price of the energy saving */
    std::string units;  /**< Units of the energy saving */

    /********************************* Methods ********************************/

    /**
     * @brief Construct a new Energy object
     * @return An object containing all configuration parameters
     */
    Energy();

    /**
     * @brief Destroy the Energy object
     */
    ~Energy();

    /**
     * @brief fetch to API the energy saving and set the values of the struct
     */
    void fetchEnergyPriceNow();

    /**
     * @brief Function that check each hour if the energy saving is cheap, and sleep thread
     */
    void sleepUntilCheap();

    /**
     * @brief Overload of the operator << to print the Energy object
     * @param os The output stream
     * @param o The Energy object
     * @return std::ostream& The output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Energy& o);
} Energy;

#endif
