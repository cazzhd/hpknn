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

#include <stdarg.h>

#include <cstdarg>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cmdParser.h"
#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Config::Config(const int argc, const char** argv) {
    /************ Init the parser ***********/
    CmdParser parser(
        "HPKNN is a Parallel and Distributed K-NN for Energyaware "
        "Heterogeneous Platforms",
        "mpirun [MPI OPTIONS] ./bin/hpknn [ARGS]", "Hpknn(c) 2015 EFFICOMP");
    parser.addExample("./bin/hpknn -h");
    parser.addExample("./bin/hpknn -conf \"config.json\"");

    /************ Add arguments ***********/
    parser.addArg("-h", false, "Display usage instructions.");
    parser.addArg("-conf", true, "Name of the file containing the JSON configuration file.");

    /************ Parse and check the missing arguments ***********/
    check(!parser.parse(argv, argc), "%s\n", ERROR_PARSE_ARGUMENTS);

    /************ Check important parameters ***********/
    if (parser.isSet("-h")) {
        parser.printHelp();
        exit(EXIT_SUCCESS);
    }

    /************ Get the JSON/command-line parameters ***********/
    std::string filename = parser.getValue<char*>("-conf");

    struct_mapping::reg(&Config::dbDataTest, "dbDataTest");
    struct_mapping::reg(&Config::dbLabelsTest, "dbLabelsTest");
    struct_mapping::reg(&Config::dbDataTraining, "dbDataTraining");
    struct_mapping::reg(&Config::dbLabelsTraining, "dbLabelsTraining");
    struct_mapping::reg(&Config::nTuples, "nTuples");
    struct_mapping::reg(&Config::nFeatures, "nFeatures");
    struct_mapping::reg(&Config::nClasses, "nClasses");
    struct_mapping::reg(&Config::K, "K");

    std::ifstream fileConfig(filename.c_str());
    std::stringstream buffer;
    std::string line;
    while (getline(fileConfig, line)) buffer << line << "\r\n";

    struct_mapping::map_json_to_struct(*this, buffer);
}

Config::~Config() {}

std::ostream& operator<<(std::ostream& os, const Config& o) {
    os << "dbDataTest: " << o.dbDataTest << std::endl;
    os << "dbLabelsTest: " << o.dbLabelsTest << std::endl;
    os << "dbDataTraining: " << o.dbDataTraining << std::endl;
    os << "dbLabelsTraining: " << o.dbLabelsTraining << std::endl;
    os << "nTuples: " << o.nTuples << std::endl;
    os << "nFeatures: " << o.nFeatures << std::endl;
    os << "nClasses: " << o.nClasses << std::endl;
    os << "K: " << o.K << std::endl;

    return os;
}

void check(const bool cond, const char* const format, ...) {
    if (cond) {
        va_list args;
        va_start(args, format);
        // fprintf(stderr, "Process %d: ", MPI::COMM_WORLD.Get_rank());
        vfprintf(stderr, format, args);
        va_end(args);
        // MPI::COMM_WORLD.Abort(-1);
        exit(EXIT_FAILURE);
    }
}