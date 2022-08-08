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
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

/******************************** Constants *******************************/

/******************************** Structures ******************************/

/**
 * @brief Class manage the energy saving, realizate the connection with the server and get the data
 */
class EnergyAwareClientAPI {
   private:
    int sock;                  /**< Socket descriptor */
    SSL *ssl;                  /**< SSL descriptor */
    char *host;                /**< Host name */
    int port;                  /**< Port */
    struct sockaddr_in server; /**< Server address */

    /**
     * @brief Open Socket
     * @return 0 if success, -1 if error
     * @return int
     */
    int openSocket();

    /**
     * @brief Connect to the server
     * @return 0 if success, -1 if error
     * @return int
     */
    int connectToServer();

    /**
     * @brief Init the SSL
     */
    void initSSL();

    /**
     * @brief Convert host name to IP
     * @param host Host name
     * @return char*
     */
    char *hostToIp(const char *host);

   public:
    /**
     * @brief Constructor
     * @param host Host name
     * @param port Port
     */
    EnergyAwareClientAPI(const char *host, int port);

    /**
     * @brief Send package
     *
     * @param buffer Buffer to send
     * @return int
     */
    int sendPackage(const char *buffer);

    /**
     * @brief Receive package
     *
     * @return string
     */
    std::string recvPackage();

    /**
     * @brief Close the socket
     */
    void closeConnection();
};

/**
 * @brief Struct of Energy that permit set the energy saving for the program from json response
 * get from the server
 */
typedef struct Energy {
    std::string date;             /**< Date of the energy saving */
    std::string hour;             /**< Hour of the energy saving */
    bool isCheap;                 /**< Flag to know if the energy saving is cheap */
    bool isUnderAvg;              /**< Flag to know if the energy saving is under average */
    std::string market;           /**< Market of the energy saving */
    float price;                  /**< Price of the energy saving */
    std::string units;            /**< Units of the energy saving */
    EnergyAwareClientAPI *client; /**< Client to connect to the server */

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
     * @brief Function that reads the vals and determine if needs to sleep
     */
    void checkSleep();

    /**
     * @brief Check each hour doing fetch to API the energy saving and set the values of the struct
     * thread 0 check every hour if the energy saving is cheap, and sleep thread
     */
    void checkEnergyPrice();

    /**
     * @brief sleep thread one hour
     * @param isSlave if is slave sleep 5 seconds more
     */
    void sleepThread(bool isSlave = false);

    /**
     * @brief thread slace wait until master do fetch to API the energy saving and set the values of the struct
     */
    void waitUntilInitializeData();

    /**
     * @brief Overload of the operator << to print the Energy object
     * @param os The output stream
     * @param o The Energy object
     * @return std::ostream& The output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const Energy &o);
} Energy;

#endif
