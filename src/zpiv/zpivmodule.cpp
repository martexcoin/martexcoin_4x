// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "zpiv/zpivmodule.h"
#include "zpivchain.h"
#include "libzerocoin/Commitment.h"
#include "libzerocoin/Coin.h"
#include "hash.h"
#include "main.h"
#include "iostream"

PublicCoinSpend::PublicCoinSpend(libzerocoin::ZerocoinParams* params, const uint8_t version,
        const CBigNum& serial, const CBigNum& randomness, const uint256& ptxHash, CPubKey* pubkey):
            pubCoin(params)
{
    this->coinSerialNumber = serial;
    this->version = version;
    this->spendType = libzerocoin::SpendType::SPEND;
    this->ptxHash = ptxHash;
    this->coinVersion = libzerocoin::ExtractVersionFromSerial(coinSerialNumber);

    if (pubkey && getCoinVersion() >= libzerocoin::PrivateCoin::PUBKEY_VERSION) {
        // pubkey available only from v2 coins onwards
        this->pubkey = *pubkey;
    }

    this->randomness = randomness;
}

template <typename Stream>
PublicCoinSpend::PublicCoinSpend(libzerocoin::ZerocoinParams* params, Stream& strm): pubCoin(params) {
    strm >> *this;
    this->spendType = libzerocoin::SpendType::SPEND;
    this->coinVersion = libzerocoin::ExtractVersionFromSerial(this->coinSerialNumber);
}

bool PublicCoinSpend::Verify() const {
    // Check that the coin is a commitment to serial and randomness.
    libzerocoin::ZerocoinParams* params = Params().GetConsensus().Zerocoin_Params(false);
    libzerocoin::Commitment comm(&params->coinCommitmentGroup, getCoinSerialNumber(), randomness);
    if (comm.getCommitmentValue() != pubCoin.getValue()) {
        return error("%s: commitments values are not equal", __func__);
    }

    // Now check that the signature validates with the serial
    if (!HasValidSignature()) {
        return error("%s: signature invalid", __func__);;
    }

    return true;
}

bool PublicCoinSpend::HasValidSignature() const
{
    if (coinVersion < libzerocoin::PrivateCoin::PUBKEY_VERSION)
        return true;

    // for spend version 3 we must check that the provided pubkey and serial number match
    CBigNum extractedSerial = libzerocoin::ExtractSerialFromPubKey(this->pubkey);
    if (extractedSerial != this->coinSerialNumber)
        return error("%s: hashedpubkey is not equal to the serial!", __func__);

    return pubkey.Verify(signatureHash(), vchSig);
}


const uint256 PublicCoinSpend::signatureHash() const
{
    CHashWriter h(0, 0);
    h << ptxHash << denomination << getCoinSerialNumber() << randomness << txHash << outputIndex << getSpendType();
    return h.GetHash();
}

namespace ZPIVModule {

    // Return stream of CoinSpend from tx input scriptsig
    CDataStream ScriptSigToSerializedSpend(const CScript& scriptSig)
    {
        std::vector<char, zero_after_free_allocator<char> > data;
        // skip opcode and data-len
        uint8_t byteskip = ((uint8_t) scriptSig[1] + 2);
        data.insert(data.end(), scriptSig.begin() + byteskip, scriptSig.end());
        return CDataStream(data, SER_NETWORK, PROTOCOL_VERSION);
    }

    PublicCoinSpend parseCoinSpend(const CTxIn &in)
    {
        libzerocoin::ZerocoinParams *params = Params().GetConsensus().Zerocoin_Params(false);
        CDataStream serializedCoinSpend = ScriptSigToSerializedSpend(in.scriptSig);
        return PublicCoinSpend(params, serializedCoinSpend);
    }

    bool parseCoinSpend(const CTxIn &in, const CTransaction &tx, const CTxOut &prevOut, PublicCoinSpend &publicCoinSpend) {
        if (!prevOut.IsZerocoinMint())
            return error("%s: invalid argument/s", __func__);

        PublicCoinSpend spend = parseCoinSpend(in);
        spend.outputIndex = in.prevout.n;
        spend.txHash = in.prevout.hash;
        CMutableTransaction txNew(tx);
        txNew.vin.clear();
        spend.setTxOutHash(txNew.GetHash());

        // Check prev out now
        CValidationState state;
        if (!TxOutToPublicCoin(prevOut, spend.pubCoin, state))
            return error("%s: cannot get mint from output", __func__);

        spend.setDenom(spend.pubCoin.getDenomination());
        publicCoinSpend = spend;
        return true;
    }

    bool validateInput(const CTxIn &in, const CTxOut &prevOut, const CTransaction &tx, PublicCoinSpend &publicSpend) {
        // Now prove that the commitment value opens to the input
        if (!parseCoinSpend(in, tx, prevOut, publicSpend)) {
            return false;
        }
        if (libzerocoin::ZerocoinDenominationToAmount(
                libzerocoin::IntToZerocoinDenomination(in.nSequence)) != prevOut.nValue) {
            return error("PublicCoinSpend validateInput :: input nSequence different to prevout value");
        }
        return publicSpend.Verify();
    }

    bool ParseZerocoinPublicSpend(const CTxIn &txIn, const CTransaction& tx, CValidationState& state, PublicCoinSpend& publicSpend)
    {
        CTxOut prevOut;
        if(!GetOutput(txIn.prevout.hash, txIn.prevout.n ,state, prevOut)){
            return state.DoS(100, error("%s: public zerocoin spend prev output not found, prevTx %s, index %d",
                                        __func__, txIn.prevout.hash.GetHex(), txIn.prevout.n));
        }
        if (!ZPIVModule::parseCoinSpend(txIn, tx, prevOut, publicSpend)) {
            return state.Invalid(error("%s: invalid public coin spend parse %s\n", __func__,
                                       tx.GetHash().GetHex()), REJECT_INVALID, "bad-txns-invalid-zpiv");
        }
        return true;
    }
}
