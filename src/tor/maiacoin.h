/* Copyright (c) 2014, Martexcoin Developers */
/* See LICENSE for licensing information */

/**
 * \file Martexcoin.h
 * \brief Headers for Martexcoin.cpp
 **/

#ifndef TOR_Martexcoin_H
#define TOR_Martexcoin_H

#ifdef __cplusplus
extern "C" {
#endif

    char const* Martexcoin_tor_data_directory(
    );

    char const* Martexcoin_service_directory(
    );

    int check_interrupted(
    );

    void set_initialized(
    );

    void wait_initialized(
    );

#ifdef __cplusplus
}
#endif

#endif

