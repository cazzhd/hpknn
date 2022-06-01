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

#include <cpr/cpr.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>

#include "struct_mapping/struct_mapping.h"

/******************************** Constants *******************************/

/********************************* Methods ********************************/
Energy::Energy() {
    struct_mapping::reg(&Energy::date, "date");
    struct_mapping::reg(&Energy::hour, "hour");
    struct_mapping::reg(&Energy::isCheap, "is-cheap");
    struct_mapping::reg(&Energy::isUnderAvg, "is-under-avg");
    struct_mapping::reg(&Energy::market, "market");
    struct_mapping::reg(&Energy::price, "price");
    struct_mapping::reg(&Energy::units, "units");
}

Energy::~Energy() {}

void Energy::fetchEnergyPriceNow() {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.preciodelaluz.org/v1/prices/now"},
                               cpr::Parameters{{"zone", "PCB"}});
    std::istringstream is(r.text);
    struct_mapping::map_json_to_struct(*this, is);
}

void Energy::checkSleep() {
    // Print value date with printf
    printf("Thread slave: Date: %s\n", date.c_str());
    if (!(this->isCheap && this->isUnderAvg)) {
        this->sleepThread(true);
    }
}

void Energy::checkEnergyPrice() {
    while (true) {
        printf("Thread main checking energy price\n");
        this->fetchEnergyPriceNow();
        sleepThread();
    }
}

void Energy::sleepThread(bool isSlave) {
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm* ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm, "%X") << '\n';

    ++ptm->tm_hour;
    ptm->tm_min = 0;
    ptm->tm_sec = isSlave ? 5 : 0;
    std::cout << "Waiting for: " << std::put_time(ptm, "%X") << '\n';
    std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
}

std::ostream& operator<<(std::ostream& os, const Energy& o) {
    os << "date: " << o.date << std::endl;
    os << "hour: " << o.hour << std::endl;
    os << "isCheap: " << o.isCheap << std::endl;
    os << "isUnderAvg: " << o.isUnderAvg << std::endl;
    os << "market: " << o.market << std::endl;
    os << "price: " << o.price << std::endl;
    os << "units: " << o.units << std::endl;
    return os;
}