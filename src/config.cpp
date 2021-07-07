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
 * @file config.cpp
 * @author Francisco Rodríguez Jiménez
 * @date 02/07/2021
 * @brief Implementation of the Config struct
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/

#include "config.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Config::Config() {
    std::string filename = "config.json";

    struct_mapping::reg(&Config::dbFilenameTest, "dbFilenameTest");
    struct_mapping::reg(&Config::dbFilenameTrain, "dbFilenameTrain");
    struct_mapping::reg(&Config::K, "K");
    struct_mapping::reg(&Config::nFeatures, "nFeatures");

    std::ifstream fileConfig(filename.c_str());
    std::stringstream buffer;
    std::string line;
    while (getline(fileConfig, line)) buffer << line << "\r\n";

    struct_mapping::map_json_to_struct(*this, buffer);
}

Config::~Config() {}

std::ostream& operator<<(std::ostream& os, const Config& o) {
    os << "dbFilenameTest: " << o.dbFilenameTest << std::endl;
    os << "dbFilenameTrain: " << o.dbFilenameTrain << std::endl;
    os << "K: " << o.K << std::endl;
    os << "nFeatures: " << o.nFeatures << std::endl;

    return os;
}