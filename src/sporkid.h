// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2019 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORKID_H
#define SPORKID_H

/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/

enum SporkId : int32_t {
    SPORK_2_FASTSEND_ENABLED                    = 10001,
    SPORK_3_FASTSEND_BLOCK_FILTERING            = 10002,
    SPORK_5_FASTSEND_MAX_VALUE                  = 10004,
    SPORK_6_NEW_SIGS                            = 10005,
    SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT      = 10007,
    SPORK_9_SUPERBLOCKS_ENABLED                 = 10008,
    SPORK_10_MASTERNODE_PAY_UPDATED_NODES       = 10009,
    SPORK_12_RECONSIDER_BLOCKS                  = 10011,
    SPORK_13_ENABLE_SUPERBLOCKS                 = 10012,
    SPORK_14_REQUIRE_SENTINEL_FLAG              = 10013,
    SPORK_15_NEW_PROTOCOL_ENFORCEMENT           = 10014,
    SPORK_16_ZEROCOIN_MAINTENANCE_MODE          = 10016,
    SPORK_17_COLDSTAKING_ENFORCEMENT            = 10017,
    SPORK_18_ZEROCOIN_PUBLICSPEND_V4            = 10018,
    SPORK_19_ECOFUND_PAYMENT_ENFORCEMENT        = 10019,
    SPORK_20_FORCE_ENABLED_MASTERNODE           = 10020,

    SPORK_INVALID                               = -1
};

// Default values
struct CSporkDef
{
    CSporkDef(): sporkId(SPORK_INVALID), defaultValue(0) {}
    CSporkDef(SporkId id, int64_t val, std::string n): sporkId(id), defaultValue(val), name(n) {}
    SporkId sporkId;
    int64_t defaultValue;
    std::string name;
};

#endif
