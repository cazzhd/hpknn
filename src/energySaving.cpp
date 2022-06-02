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
 * @file energySaving.cpp
 * @author Francisco Rodríguez Jiménez
 * @date 25/05/2022
 * @brief Implementation of the energySaving
 * @copyright Hpknn (c) 2015 EFFICOMP
 */

/********************************* Includes *******************************/
#include "energySaving.h"

#include <string.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>

#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
int EnergyAwareClientAPI::sendPackage(const char *buffer) {
    int len = SSL_write(this->ssl, buffer, strlen(buffer));
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
            case SSL_ERROR_WANT_WRITE:
                return 0;
            case SSL_ERROR_WANT_READ:
                return 0;
            case SSL_ERROR_ZERO_RETURN:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
            default:
                return -1;
        }
    }
    return len;
}

std::string EnergyAwareClientAPI::recvPackage() {
    int len = 0;
    std::string allBuffer = "";
    do {
        char buf[1];
        len = SSL_read(this->ssl, buf, 1);

        // Only copy when start in "{" and end in "}"
        if (allBuffer == "" && buf[0] == '{') {
            allBuffer += buf[0];
        } else if (allBuffer != "") {
            allBuffer += buf[0];
            if (buf[0] == '}') {
                break;
            }
        }
    } while (len > 0);
    return allBuffer;
}

void EnergyAwareClientAPI::initSSL() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *meth = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(meth);
    this->ssl = SSL_new(ctx);

    if (!this->ssl) {
        perror("Error creating SSL.\n");
        exit(EXIT_FAILURE);
    }

    SSL_get_fd(this->ssl);
    SSL_set_fd(this->ssl, this->sock);
    int err = SSL_connect(this->ssl);
    if (err <= 0) {
        printf("Error creating SSL connection.  err=%x\n", err);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    // printf("SSL connection using %s\n", SSL_get_cipher(ssl));
}

char *EnergyAwareClientAPI::hostToIp(const char *host) {
    hostent *hostname = gethostbyname(host);
    if (hostname) {
        return inet_ntoa(*(struct in_addr *)hostname->h_addr);
    }
    fprintf(stderr, "ERROR, no such host\n");
    exit(EXIT_FAILURE);
}

int EnergyAwareClientAPI::openSocket() {
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock < -1) {
        perror("Error creating socket.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int EnergyAwareClientAPI::connectToServer() {
    this->server.sin_family = AF_INET;
    this->server.sin_addr.s_addr = inet_addr(this->host);
    this->server.sin_port = htons(this->port);

    socklen_t socklen = sizeof(this->server);
    if (connect(this->sock, (struct sockaddr *)&this->server, socklen)) {
        perror("Error connecting to server.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

EnergyAwareClientAPI::EnergyAwareClientAPI(const char *host, int port) {
    this->host = this->hostToIp(host);
    this->port = port;

    this->openSocket();
    this->connectToServer();
    this->initSSL();
}

Energy::Energy() {
    struct_mapping::reg(&Energy::date, "date");
    struct_mapping::reg(&Energy::hour, "hour");
    struct_mapping::reg(&Energy::isCheap, "is-cheap");
    struct_mapping::reg(&Energy::isUnderAvg, "is-under-avg");
    struct_mapping::reg(&Energy::market, "market");
    struct_mapping::reg(&Energy::price, "price");
    struct_mapping::reg(&Energy::units, "units");

    client = new EnergyAwareClientAPI("api.preciodelaluz.org", 443);
}

Energy::~Energy() {}

void Energy::fetchEnergyPriceNow() {
    std::string request = "GET /v1/prices/now?zone=PCB HTTP/1.1\r\nHost: api.preciodelaluz.org\r\nConnection: close\r\n\r\n";
    client->sendPackage(request.c_str());
    std::istringstream is(client->recvPackage());
    struct_mapping::map_json_to_struct(*this, is);
}

void Energy::waitUntilInitializeData() {
    while (this->date == "")
        ;
}

void Energy::checkSleep() {
    // Print value date with printf
    this->waitUntilInitializeData();
    // printf("Thread slave: Date: %s\n", date.c_str());
    if (!(this->isCheap && this->isUnderAvg)) {
        this->sleepThread(true);
    }
}

void Energy::checkEnergyPrice() {
    while (true) {
        // printf("Thread main checking energy price\n");
        this->fetchEnergyPriceNow();
        sleepThread();
    }
}

void Energy::sleepThread(bool isSlave) {
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm *ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm, "%X") << '\n';

    ++ptm->tm_hour;
    ptm->tm_min = 0;
    ptm->tm_sec = isSlave ? 5 : 0;
    std::cout << "Waiting for: " << std::put_time(ptm, "%X") << '\n';
    std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
}

std::ostream &operator<<(std::ostream &os, const Energy &o) {
    os << "date: " << o.date << std::endl;
    os << "hour: " << o.hour << std::endl;
    os << "isCheap: " << o.isCheap << std::endl;
    os << "isUnderAvg: " << o.isUnderAvg << std::endl;
    os << "market: " << o.market << std::endl;
    os << "price: " << o.price << std::endl;
    os << "units: " << o.units << std::endl;
    return os;
}