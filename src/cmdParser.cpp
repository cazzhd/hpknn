/**
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of Hpmoon repository.
 *
 * This work has been funded by:
 *
 * Spanish 'Ministerio de Economía y Competitividad' under grants number TIN2012-32039 and TIN2015-67020-P.\n
 * Spanish 'Ministerio de Ciencia, Innovación y Universidades' under grant number PGC2018-098813-B-C31.\n
 * European Regional Development Fund (ERDF).
 *
 * @file cmdParser.cpp
 * @author Juan José Escobar Pérez
 * @date 02/01/2018
 * @brief Implementation of the command line arguments parser
 * @copyright Hpmoon (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/

#include "cmdParser.h"
#include <cstdlib> // atof, atoi...
#include <cstring> // std::strcpy...
#include <stdio.h> // fprintf...

/********************************* Methods ********************************/

/**
 * @brief The constructor with parameters
 * @param description A short description about the meaning of this argument
 * @param requireValue If this argument needs a value or not
 * @return An object containing this argument information
 */
CmdParser::Argument::Argument(const char *const description, const bool requireValue) {

	this -> description = (description != NULL) ? description : "";
	this -> requireValue = requireValue;
	this -> set = false;
}


/**
 * @brief Get the description of this argument
 * @return A string containing the description of this argument
 */
std::string CmdParser::Argument::getDescription() {

	return this -> description;
}


/**
 * @brief Check if this argument needs a value or not
 * @return True if this argument needs a value or false otherwise
 */
bool CmdParser::Argument::isValueRequired() {

	return this -> requireValue;
}


/**
 * @brief Set the value for this argument
 * @param value The value
 * @param reset If this argument must be reset
 * @return True if this argument has been set or false otherwise
 */
bool CmdParser::Argument::setValue(const char *const value, const bool reset) {

	this -> value = value;
	this -> set = !reset;
	return true;
}


/**
 * @brief Check if this argument has been set
 * @see requireValue
 * @return True if this argument has been set or false otherwise
 */
bool CmdParser::Argument::isSet() {

	return this -> set;
}


/**
 * @brief Get the value of this argument
 * @return The value
 */
std::string CmdParser::Argument::getValue() {

	return this -> value;
}


/**
 * @brief The constructor with parameters
 * @param overview General description of the program
 * @param syntax Generic syntax of the program
 * @param footer Short text to end the program information
 * @return An object containing the program information
 */
CmdParser::CmdParser(const char *const overview, const char *const syntax, const char *const footer) {

	this -> overview = (overview != NULL) ? overview : "";
	this -> syntax = (syntax != NULL) ? syntax : "";
	this -> footer = (footer != NULL) ? footer : "";
}


/**
 * @brief Add an example to the parser
 * @param example The string containing the example to be added
 * @return True if the example has been added or false otherwise
 */
bool CmdParser::addExample(const char *const example) {

	bool success = (example != NULL && strcmp(example, "") != 0);
	if (success) {
		this -> examples.push_back(example);
	}
	return success;
}


/**
 * @brief Add a new argument to the parser
 * @param arg The string containing the new argument to be added
 * @param requireValue If this argument needs a value or not
 * @param description A short description about the meaning of the argument
 * @return True if the argument has been added or false otherwise
 */
bool CmdParser::addArg(const char *const arg, const bool requireValue, const char *const description) {

	bool success = (arg != NULL && strcmp(arg, "") != 0);
	if (success) {
		const auto ret = this -> arguments.insert(std::pair<std::string, Argument>(arg, Argument(description, requireValue)));
		success = ret.second;
	}
	return success;
}


/**
 * @brief Erase an argument from the parser
 * @param arg The string containing the argument to be erased
 * @return True if the argument has been erased or false otherwise
 */
bool CmdParser::eraseArg(const char *const arg) {

	return (arg != NULL && this -> arguments.erase(arg) > 0);
}


/**
 * @brief Parse the command line string (only arguments which require a value)
 * @param argv The command line string to be parsed
 * @param argc The number of components in the string (arguments + values)
 * @return True if the operation has been successful or false if the parser is empty or the value of some argument is missing
 */
bool CmdParser::parse(const char *const *const argv, const int argc) {

	bool success = (!this -> arguments.empty() && argv != NULL && argc > 0);
	int parsed = 0;
	for(auto it = this -> arguments.begin(); it != this -> arguments.end() && success; ++it) {

		// Delete a possible previous parse;
		it -> second.setValue("", true);
		for (int i = 0; i < argc && argv[i] != NULL && success; ++i) {

			// Check if this argument has been passed in the command line string and needs a value
			if (it -> first.compare(argv[i]) == 0) {
				if (it -> second.isValueRequired()) {
					success = (i == argc - 1 || this -> find(argv[i + 1]) || strcmp(argv[i + 1], "") == 0) ? false : it -> second.setValue(argv[i + 1]);
				}
				else {
					success = it -> second.setValue("");
				}
				++parsed;
			}
		}
	}

	return (success && parsed > 0);
}


/**
 * @brief Check if this argument has been added to the parser
 * @param arg The argument to be checked
 * @return True if the argument already exists or false otherwise
 */
bool CmdParser::find(const char *const arg) {

	return (arg != NULL && this -> arguments.find(arg) != this -> arguments.end());
}


/**
 * @brief Check if the argument is set
 * @param arg The argument to be checked
 * @return True if the argument is set or false otherwise
 */
bool CmdParser::isSet(const char *const arg) {

	return (this -> find(arg)) ? this -> arguments.find(arg) -> second.isSet() : false;
}


/**
 * @brief Print the help
 */
void CmdParser::printHelp() {

	std::string out;
	if (!this -> overview.empty()) {
		out += '\n' + this -> overview + '\n';
	}
	if (!this -> syntax.empty()) {
		out += "\nSYNTAX: " + this -> syntax + '\n';
	}
	if (!this -> arguments.empty()) {
		out += "\nARGS:\n";

		// Find the longest argument and save its length
		int maxLength = 0;
		for(const auto &it : this -> arguments) {
			maxLength = std::max((int) it.first.length(), maxLength);
		}

		// Format the arguments
		for(auto &it : this -> arguments) {
			int blankSpaces = maxLength - it.first.length() + 3;
			out += "\n" + it.first;
			if (it.second.isValueRequired()) {
				out += " VAL";
			}
			else {
				blankSpaces += 4;
			}
			for (int i = 0; i < blankSpaces; ++i) {
				out += ' ';
			}
			out += it.second.getDescription() + '\n';
		}

		if (!this -> examples.empty()) {
			out += "\nEXAMPLES:\n\n";
			for (const auto &it : this -> examples) {
				out += it + '\n';
			}
		}
	}

	if (!this -> footer.empty()) {
		out += '\n' + this -> footer + '\n';
	}
	if (!out.empty()) {
		out += '\n';
		fprintf(stdout, "%s", out.c_str());
	}
}


/**
 * @brief Get the value of the specified argument
 * @param arg The required argument
 * @return The value
 */
template<> char* CmdParser::getValue(const char *const arg) {

	std::string aux = (this -> find(arg)) ? this -> arguments.find(arg) -> second.getValue() : "";
	char *value = new char[aux.length()];
	strcpy(value, aux.c_str());
	return (aux.empty()) ? NULL : value;
}


template<> int CmdParser::getValue(const char *const arg) {

	int aux = (this -> find(arg)) ? atoi(this -> arguments.find(arg) -> second.getValue().c_str()) : -1;
	return aux;
}


template<> float CmdParser::getValue(const char *const arg) {

	float aux = (this -> find(arg)) ? atof(this -> arguments.find(arg) -> second.getValue().c_str()) : -1.0f;
	return aux;
}


template<> double CmdParser::getValue(const char *const arg) {

	double aux = (this -> find(arg)) ? atof(this -> arguments.find(arg) -> second.getValue().c_str()) : -1.0;
	return aux;
}
