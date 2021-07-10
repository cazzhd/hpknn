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

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdarg.h>

#include "cmdParser.h"
#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/

/**
 * @brief The constructor with parameters
 * @param argc Number of arguments
 * @param argv The command-line parameters
 * @return An object containing all configuration parameters
 */
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
    parser.addArg("-conf", true,
                  "Name of the file containing the JSON configuration file.");

    /************ Parse and check the missing arguments ***********/
    check(!parser.parse(argv, argc), "%s\n", CFG_ERROR_PARSE_ARGUMENTS);

    /************ Check important parameters ***********/
    if (parser.isSet("-h")) {
        parser.printHelp();
        exit(EXIT_SUCCESS);
    }

    /************ Get the JSON/command-line parameters ***********/

    std::string filename = parser.getValue<char*>("-conf");

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

/**
 * @brief Destroy the Config:: Config object
 *
 */
Config::~Config() {}

/**
 * @brief Overload operator << to print in the standard output info about Config
 * @param os stream output
 * @param o object Config
 * @return std::ostream& stream with info
 */
std::ostream& operator<<(std::ostream& os, const Config& o) {
    os << "dbFilenameTest: " << o.dbFilenameTest << std::endl;
    os << "dbFilenameTrain: " << o.dbFilenameTrain << std::endl;
    os << "K: " << o.K << std::endl;
    os << "nFeatures: " << o.nFeatures << std::endl;

    return os;
}

/**
 * @brief Check the condition. If it is true, a message is showed and the
 * program will abort
 * @param cond The condition to be evaluated
 * @param format The format of the arguments
 * @param ... The corresponding messages to be showed in error case
 */
void check(const bool cond, const char* const format, ...) {
    if (cond) {
        va_list args;
        va_start(args, format);
        // // fprintf(stderr, "Process %d: ", MPI::COMM_WORLD.Get_rank());
        vfprintf(stderr, format, args);
        va_end(args);
        // MPI::COMM_WORLD.Abort(-1);
        exit(EXIT_FAILURE);
    }
}