// Copyright (c) 2018-2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "zpivchain.h"
#include "zpiv/zpivmodule.h"
#include "main.h"
#include "txdb.h"
#include "guiinterface.h"

// 6 comes from OPCODE (1) + vch.size() (1) + BIGNUM size (4)
#define SCRIPT_OFFSET 6

bool BlockToMintValueVector(const CBlock& block, const libzerocoin::CoinDenomination denom, std::vector<CBigNum>& vValues)
{
    for (const CTransaction& tx : block.vtx) {
        if(!tx.HasZerocoinMintOutputs())
            continue;

        for (const CTxOut& txOut : tx.vout) {
            if(!txOut.IsZerocoinMint())
                continue;

            CValidationState state;
            libzerocoin::PublicCoin coin(Params().GetConsensus().Zerocoin_Params(false));
            if(!TxOutToPublicCoin(txOut, coin, state))
                return false;

            if (coin.getDenomination() != denom)
                continue;

            vValues.push_back(coin.getValue());
        }
    }

    return true;
}

//return a list of zerocoin mints contained in a specific block
bool BlockToZerocoinMintList(const CBlock& block, std::list<CZerocoinMint>& vMints, bool fFilterInvalid)
{
    for (const CTransaction& tx : block.vtx) {
        if(!tx.HasZerocoinMintOutputs())
            continue;

        uint256 txHash = tx.GetHash();
        for (unsigned int i = 0; i < tx.vout.size(); i++) {
            const CTxOut txOut = tx.vout[i];
            if(!txOut.IsZerocoinMint())
                continue;

            CValidationState state;
            libzerocoin::PublicCoin pubCoin(Params().GetConsensus().Zerocoin_Params(false));
            if(!TxOutToPublicCoin(txOut, pubCoin, state))
                return false;

            //version should not actually matter here since it is just a reference to the pubcoin, not to the privcoin
            uint8_t version = 1;
            CZerocoinMint mint = CZerocoinMint(pubCoin.getDenomination(), pubCoin.getValue(), 0, 0, false, version, nullptr);
            mint.SetTxHash(tx.GetHash());
            vMints.push_back(mint);
        }
    }

    return true;
}

std::string ReindexZerocoinDB()
{
    if (!zerocoinDB->WipeCoins("spends") || !zerocoinDB->WipeCoins("mints")) {
        return _("Failed to wipe zerocoinDB");
    }

    uiInterface.ShowProgress(_("Reindexing zerocoin database..."), 0);

    const int zc_start_height = Params().GetConsensus().height_start_ZC;
    CBlockIndex* pindex = chainActive[zc_start_height];
    std::vector<std::pair<libzerocoin::CoinSpend, uint256> > vSpendInfo;
    std::vector<std::pair<libzerocoin::PublicCoin, uint256> > vMintInfo;
    while (pindex) {
        uiInterface.ShowProgress(_("Reindexing zerocoin database..."), std::max(1, std::min(99, (int)((double)(pindex->nHeight - zc_start_height) / (double)(chainActive.Height() - zc_start_height) * 100))));

        if (pindex->nHeight % 1000 == 0)
            LogPrintf("Reindexing zerocoin : block %d...\n", pindex->nHeight);

        CBlock block;
        if (!ReadBlockFromDisk(block, pindex)) {
            return _("Reindexing zerocoin failed");
        }

        for (const CTransaction& tx : block.vtx) {
            for (unsigned int i = 0; i < tx.vin.size(); i++) {
                if (tx.IsCoinBase())
                    break;

                if (tx.ContainsZerocoins()) {
                    uint256 txid = tx.GetHash();
                    //Record Serials
                    if (tx.HasZerocoinSpendInputs()) {
                        for (auto& in : tx.vin) {
                            if (!in.IsZerocoinSpend())
                                continue;
                            libzerocoin::CoinSpend spend = TxInToZerocoinSpend(in);
                            vSpendInfo.push_back(std::make_pair(spend, txid));
                        }
                    }

                    //Record mints
                    if (tx.HasZerocoinMintOutputs()) {
                        for (auto& out : tx.vout) {
                            if (!out.IsZerocoinMint())
                                continue;
                            CValidationState state;
                            libzerocoin::PublicCoin coin(Params().GetConsensus().Zerocoin_Params(pindex->nHeight < Params().GetConsensus().height_start_ZC_SerialsV2));
                            TxOutToPublicCoin(out, coin, state);
                            vMintInfo.push_back(std::make_pair(coin, txid));
                        }
                    }
                }
            }
        }

        // Flush the zerocoinDB to disk every 100 blocks
        if (pindex->nHeight % 100 == 0) {
            if ((!vSpendInfo.empty() && !zerocoinDB->WriteCoinSpendBatch(vSpendInfo)) || (!vMintInfo.empty() && !zerocoinDB->WriteCoinMintBatch(vMintInfo)))
                return _("Error writing zerocoinDB to disk");
            vSpendInfo.clear();
            vMintInfo.clear();
        }

        pindex = chainActive.Next(pindex);
    }
    uiInterface.ShowProgress("", 100);

    // Final flush to disk in case any remaining information exists
    if ((!vSpendInfo.empty() && !zerocoinDB->WriteCoinSpendBatch(vSpendInfo)) || (!vMintInfo.empty() && !zerocoinDB->WriteCoinMintBatch(vMintInfo)))
        return _("Error writing zerocoinDB to disk");

    uiInterface.ShowProgress("", 100);

    return "";
}

libzerocoin::CoinSpend TxInToZerocoinSpend(const CTxIn& txin)
{
    CDataStream serializedCoinSpend = ZPIVModule::ScriptSigToSerializedSpend(txin.scriptSig);
    return libzerocoin::CoinSpend(serializedCoinSpend);
}

bool TxOutToPublicCoin(const CTxOut& txout, libzerocoin::PublicCoin& pubCoin, CValidationState& state)
{
    CBigNum publicZerocoin;
    std::vector<unsigned char> vchZeroMint;
    vchZeroMint.insert(vchZeroMint.end(), txout.scriptPubKey.begin() + SCRIPT_OFFSET,
                       txout.scriptPubKey.begin() + txout.scriptPubKey.size());
    publicZerocoin.setvch(vchZeroMint);

    libzerocoin::CoinDenomination denomination = libzerocoin::AmountToZerocoinDenomination(txout.nValue);
    LogPrint("zero", "%s ZCPRINT denomination %d pubcoin %s\n", __func__, denomination, publicZerocoin.GetHex());
    if (denomination == libzerocoin::ZQ_ERROR)
        return state.DoS(100, error("TxOutToPublicCoin : txout.nValue is not correct"));

    libzerocoin::PublicCoin checkPubCoin(Params().GetConsensus().Zerocoin_Params(false), publicZerocoin, denomination);
    pubCoin = checkPubCoin;

    return true;
}

//return a list of zerocoin spends contained in a specific block, list may have many denominations
std::list<libzerocoin::CoinDenomination> ZerocoinSpendListFromBlock(const CBlock& block, bool fFilterInvalid)
{
    std::list<libzerocoin::CoinDenomination> vSpends;
    for (const CTransaction& tx : block.vtx) {
        if (!tx.HasZerocoinSpendInputs())
            continue;

        for (const CTxIn& txin : tx.vin) {
            if (!txin.IsZerocoinSpend())
                continue;

            libzerocoin::CoinDenomination c = libzerocoin::IntToZerocoinDenomination(txin.nSequence);
            vSpends.push_back(c);
        }
    }
    return vSpends;
}

