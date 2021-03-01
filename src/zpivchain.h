// Copyright (c) 2018 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PIVX_ZPIVCHAIN_H
#define PIVX_ZPIVCHAIN_H

#include "libzerocoin/Coin.h"
#include "libzerocoin/Denominations.h"
#include "libzerocoin/CoinSpend.h"
#include <list>
#include <string>

class CBlock;
class CBigNum;
class CTxIn;
class CTxOut;
class CValidationState;
class CZerocoinMint;

bool BlockToMintValueVector(const CBlock& block, const libzerocoin::CoinDenomination denom, std::vector<CBigNum>& vValues);
bool BlockToZerocoinMintList(const CBlock& block, std::list<CZerocoinMint>& vMints, bool fFilterInvalid);
std::string ReindexZerocoinDB();
libzerocoin::CoinSpend TxInToZerocoinSpend(const CTxIn& txin);
bool TxOutToPublicCoin(const CTxOut& txout, libzerocoin::PublicCoin& pubCoin, CValidationState& state);
std::list<libzerocoin::CoinDenomination> ZerocoinSpendListFromBlock(const CBlock& block, bool fFilterInvalid);


#endif //PIVX_ZPIVCHAIN_H
