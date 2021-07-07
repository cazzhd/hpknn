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
 * @file cmdParser.h
 * @author Juan José Escobar Pérez
 * @date 02/01/2018
 * @brief Function declarations of the command line arguments parser
 * @copyright Hpmoon (c) 2015 EFFICOMP
 */

#ifndef CMDPARSER_H
#define CMDPARSER_H

/********************************* Includes *******************************/

#include <map> // std::map...
#include <string> // std::string...
#include <vector> // std::vector...

/********************************* Classes ********************************/

/**
 * @brief Structure containing the command line arguments and the program information
 */
class CmdParser {

	private:

		/**
		 * @brief Structure containing the attributes of each argument
		 */
		class Argument {

			private:

				/**
				 * @brief A short description about the meaning of this argument
				 */
				std::string description;


				/**
				 * @brief If this argument needs a value or not
				 */
				bool requireValue;


				/**
				 * @brief If this argument has been properly set (according to the requireValue parameter)
				 */
				bool set;


				/**
				 * @brief The value of this argument
				 */
				std::string value;


			public:

				/**
				 * @brief The constructor with parameters
				 * @param description A short description about the meaning of this argument
				 * @param requireValue If this argument needs a value or not
				 * @return An object containing the argument information
				 */
				Argument(const char *const description, const bool requireValue);


				/**
				 * @brief Get the description of this argument
				 * @return A string containing the description of this argument
				 */
				std::string getDescription();


				/**
				 * @brief Check if this argument needs a value or not
				 * @return True if this argument needs a value or false otherwise
				 */
				bool isValueRequired();


				/**
				 * @brief Set the value for this argument
				 * @param value The value
				 * @param reset If this argument must be reset
				 * @return True if this argument has been set or false otherwise
				 */
				bool setValue(const char *const value = "", const bool reset = false);


				/**
				 * @brief Check if this argument is set
				 * @see requireValue
				 * @return True if this argument is set or false otherwise
				 */
				bool isSet();


				/**
				 * @brief Get the value of this argument
				 * @return The value
				 */
				std::string getValue();

		};


		/**
		 * @brief General description of the program
		 */
		std::string overview;


		/**
		 * @brief Generic syntax of the program
		 */
		std::string syntax;


		/**
		 * @brief Examples of how to call to the program
		 */
		std::vector<std::string> examples;


		/**
		 * @brief Short text to end the program information
		 */
		std::string footer;


		/**
		 * @brief List of arguments to be parsed
		 */
		std::map<std::string, Argument> arguments;


	public:

		/**
		 * @brief The constructor with parameters
		 * @param overview General description of the program
		 * @param syntax Generic syntax of the program
		 * @param footer Short text to end the program information
		 * @return An object containing the program information
		 */
		CmdParser(const char *const overview = "", const char *const syntax = "", const char *const footer = "");


		/**
		 * @brief Add an example to the parser
		 * @param example The string containing the example to be added
		 * @return True if the example has been added or false otherwise
		 */
		bool addExample(const char *const example);


		/**
		 * @brief Add a new argument to the parser
		 * @param arg The string containing the new argument to be added
		 * @param requireValue If this argument needs a value or not
		 * @param description A short description about the meaning of the argument
		 * @return True if the argument has been added or false otherwise
		 */
		bool addArg(const char *const arg, const bool requireValue, const char *const description = "");


		/**
		 * @brief Erase an argument from the parser
		 * @param arg The string containing the argument to be erased
		 * @return True if the argument has been erased or false otherwise
		 */
		bool eraseArg(const char *const arg);


		/**
		 * @brief Parse the command line string
		 * @param argv The command line string to be parsed
		 * @param argc The number of components in the string (arguments + values)
		 * @return True if the operation has been successful or false if the parser is empty or the value of some argument is missing
		 */
		bool parse(const char *const *const argv, const int argc);


		/**
		 * @brief Check if this argument has been added to the parser
		 * @param arg The argument to be checked
		 * @return True if the argument already exists or false otherwise
		 */
		bool find(const char *const arg);


		/**
		 * @brief Check if the argument is set
		 * @param arg The argument to be checked
		 * @return True if the argument is set or false otherwise
		 */
		bool isSet(const char *const arg);


		/**
		 * @brief Print the help
		 */
		void printHelp();


		/**
		 * @brief Get the value of the specified argument
		 * @param arg The required argument
		 * @return The value
		 */
		template <typename T>
		T getValue(const char *const arg);

};

#endif
