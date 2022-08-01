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

#include <mpi.h>
#include <stdarg.h>

#include <cstdarg>
#include <iostream>
#include <sstream>

#include "cmdParser.h"
#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Config::Config(int argc, char** argv) {
    /************ Init the parser ***********/
    CmdParser parser(
        "HPKNN is a Parallel and Distributed K-NN for Energyaware "
        "Heterogeneous Platforms",
        "mpirun [MPI OPTIONS] ./bin/hpknn [ARGS]", "Hpknn(c) 2015 EFFICOMP");
    parser.addExample("./bin/hpknn -h");
    parser.addExample("./bin/hpknn -conf \"config.json\"");
    parser.addExample("./bin/hpknn -mode [homo,hetero] -conf \"config.json\"");

    /************ Add arguments ***********/
    parser.addArg("-h", false, "Display usage instructions.");
    parser.addArg("-mode", true,
                  "Two modes [homo,hetero] for heterogeneous platforms or homogeneous platforms.");
    parser.addArg("-conf", true, "Name of the file containing the JSON configuration file.");

    /************ Parse and check the missing arguments ***********/
    check(!parser.parse(argv, argc), "%s\n", ERROR_PARSE_ARGUMENTS);

    /************ Check important parameters ***********/
    if (parser.isSet("-h")) {
        if (!MPI::COMM_WORLD.Get_rank()) {
            parser.printHelp();
        }
        MPI_Finalize();
        exit(EXIT_SUCCESS);
    }

    /************ Get the JSON/command-line parameters ***********/
    std::string filename = parser.getValue<char*>("-conf");
    this->mode = parser.getValue<char*>("-mode");

    // Check if mode is valid
    check(!(this->mode.compare("homo") || this->mode.compare("hetero")), "%s\n", ERROR_MODE);

    struct_mapping::reg(&Config::dbDataTest, "dbDataTest");
    struct_mapping::reg(&Config::dbLabelsTest, "dbLabelsTest");
    struct_mapping::reg(&Config::dbDataTraining, "dbDataTraining");
    struct_mapping::reg(&Config::dbLabelsTraining, "dbLabelsTraining");
    struct_mapping::reg(&Config::MRMR, "MRMR");
    struct_mapping::reg(&Config::nTuples, "nTuples");
    struct_mapping::reg(&Config::nFeatures, "nFeatures");
    struct_mapping::reg(&Config::nClasses, "nClasses");
    struct_mapping::reg(&Config::normalize, "normalize");
    struct_mapping::reg(&Config::sortingByMRMR, "sortingByMRMR");
    struct_mapping::reg(&Config::maxFeatures, "maxFeatures");
    struct_mapping::reg(&Config::chunkSize, "chunkSize");
    struct_mapping::reg(&Config::savingEnergy, "savingEnergy");
    struct_mapping::reg(&Config::stridedHomo, "stridedHomo");

    std::ifstream fileConfig(filename.c_str());
    std::stringstream buffer;
    std::string line;
    while (std::getline(fileConfig, line)) buffer << line << "\r\n";

    struct_mapping::map_json_to_struct(*this, buffer);
    this->TAM = this->nTuples * this->nFeatures;
    this->TAM_MAX_FEATURES = this->nTuples * this->maxFeatures;

    /************ Checks for both modes ***********/
    /************ Check if in mode hetero have min two process ***********/
    if (this->mode.compare("hetero") == 0) {
        check(MPI::COMM_WORLD.Get_size() < 2, "%s\n", ERROR_NPROCESS_HETERO);
        check(this->TAM_MAX_FEATURES % this->chunkSize, "%s\n", ERROR_CHUNKSIZE_HETERO);
    } else {
        /************ Check if size of data is divisible by the number of processors ***********/
        check(this->maxFeatures % MPI::COMM_WORLD.Get_size(), "%s\n", ERROR_NPROCESS_HOMO);
    }
}

Config::~Config() {}

std::ostream& operator<<(std::ostream& os, const Config& o) {
    os << "dbDataTest: " << o.dbDataTest << std::endl;
    os << "dbLabelsTest: " << o.dbLabelsTest << std::endl;
    os << "dbDataTraining: " << o.dbDataTraining << std::endl;
    os << "dbLabelsTraining: " << o.dbLabelsTraining << std::endl;
    os << "MRMR: " << o.MRMR << std::endl;
    os << "nTuples: " << o.nTuples << std::endl;
    os << "nFeatures: " << o.nFeatures << std::endl;
    os << "TAM: " << o.TAM << std::endl;
    os << "TAM_MAX_FEATURES: " << o.TAM_MAX_FEATURES << std::endl;
    os << "nClasses: " << o.nClasses << std::endl;
    os << "normalize: " << o.normalize << std::endl;
    os << "maxFeatures: " << o.maxFeatures << std::endl;
    os << "chunkSize: " << o.chunkSize << std::endl;
    os << "savingEnergy: " << o.savingEnergy << std::endl;
    os << "stridedHomo: " << o.stridedHomo << std::endl;

    return os;
}

void check(const bool cond, const char* const format, ...) {
    if (cond) {
        va_list args;
        va_start(args, format);
        fprintf(stderr, "Process %d: ", MPI::COMM_WORLD.Get_rank());
        vfprintf(stderr, format, args);
        va_end(args);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
}