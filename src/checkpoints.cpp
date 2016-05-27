// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        //( 0,      hashGenesisBlock )
        ( 0,      uint256("0x81f998298dc023c13c9b7948f75cd1ab90b30d7f9d753e7dbdbfb3b00b193e3b") ) // Params().HashGenesisBlock())
        ( 50,     uint256("0x00000bdca326c2ee986d9ed245da755616f546c6fb5386a283a4fca707ac5283") )
        ( 100,    uint256("0x0000007abc10b795577d308e90bf672417127ea05fa33f546e77fdd5ad2d3f6d") )
        ( 150,    uint256("0x0000000e631c50d54c482dacc62a6a622ac79b300d841a61dd426440166cd700") )
        ( 1000,   uint256("0x000000000ce309ed986a29fa90519b4678933c9faa2965c94690d77ae1715572") )
        ( 1500,   uint256("0x0000000004e177cff321dc904d433fa675c6666634a3f4a8e8bac3877d482f99") )
        ( 2000,   uint256("0x00000000211da7b37ff620fb63263bb953e75d000eaaf7887bb65b7b9f5991c1") )
        ( 2500,   uint256("0x000000008f26985293c82a7c13e5178260522bd74073c064a4c7742cad2de780") )
        ( 3000,   uint256("0x000000004c89688c20ea90440bedda62cc97507d371cc628c794cdfe0220e8ca") )
        ( 3500,   uint256("0x000000002f1220236cef1dc73570be35b5eb7730fdad8be4e9e3886bd9cde73a") )
        ( 4000,   uint256("0x000000000392aab7575305f0bb9959c3e046e780d62eb64370cf47d37bb2ee6f") )
        ( 4500,   uint256("0x385f3591b597d9f7949074ab81fd254ce66b87ba2faa16fe5beef175f3cb5285") )
        ( 5000,   uint256("0x00000000cb9d4a55ff04ead3eec0350ce019eef3c4c15c1e458cc381eb63faae") )
        ( 10000,  uint256("0x2a74ed86b2a3b4914254bc413d55872f867f59ab47400a54f03a47554a769ad2") )
        ( 50000,  uint256("0xe98b11dd3847c151a838c0efa1537c29030c807aa7563120dde1773c95cd88af") )
        ( 100000, uint256("0xa3596b81674a7784effa7fad0b1011fa7dc52265394ab966087bea9d24c6e0b6") )
        ( 150000, uint256("0x2bffd94a59cd319566803096f6c20198881d3dbca093eb2c75e18797e9a61523") )
        ( 200000, uint256("0x9b3f3ceb7ffa4055e57939d283eb3552163b0d3a07ff0769be4ca02d72baf2c8") )
        ( 250000, uint256("0xc098de7f2e32508b66687d08b0c739be903d6bc69c8df7126a648811c8132504") )
        ( 300000, uint256("0x000000076aa4dc0c9042e360d634a2265b252a40813e3de285860d5cc0cb1982") )
        ( 350000, uint256("0x0000001351223366422ef946cb266aab7e01e397bfb811c85c53d40e299b0a31") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 0, hashGenesisBlockTestNet )
    ;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // MartexCoin: synchronized checkpoint (centrally broadcasted)
    uint256 hashSyncCheckpoint = 0;
    uint256 hashPendingCheckpoint = 0;
    CSyncCheckpoint checkpointMessage;
    CSyncCheckpoint checkpointMessagePending;
    uint256 hashInvalidCheckpoint = 0;
    CCriticalSection cs_hashSyncCheckpoint;

    // MartexCoin: get last synchronized checkpoint
    CBlockIndex* GetLastSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            error("GetSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        else
            return mapBlockIndex[hashSyncCheckpoint];
        return NULL;
    }

    // MartexCoin: only descendant of current sync-checkpoint is allowed
    bool ValidateSyncCheckpoint(uint256 hashCheckpoint)
    {
        if (!mapBlockIndex.count(hashSyncCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for current sync-checkpoint %s", hashSyncCheckpoint.ToString().c_str());
        if (!mapBlockIndex.count(hashCheckpoint))
            return error("ValidateSyncCheckpoint: block index missing for received sync-checkpoint %s", hashCheckpoint.ToString().c_str());

        CBlockIndex* pindexSyncCheckpoint = mapBlockIndex[hashSyncCheckpoint];
        CBlockIndex* pindexCheckpointRecv = mapBlockIndex[hashCheckpoint];

        if (pindexCheckpointRecv->nHeight <= pindexSyncCheckpoint->nHeight)
        {
            // Received an older checkpoint, trace back from current checkpoint
            // to the same height of the received checkpoint to verify
            // that current checkpoint should be a descendant block
            CBlockIndex* pindex = pindexSyncCheckpoint;
            while (pindex->nHeight > pindexCheckpointRecv->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("ValidateSyncCheckpoint: pprev null - block index structure failure");
            if (pindex->GetBlockHash() != hashCheckpoint)
            {
                hashInvalidCheckpoint = hashCheckpoint;
                return error("ValidateSyncCheckpoint: new sync-checkpoint %s is conflicting with current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
            }
            return false; // ignore older checkpoint
        }

        // Received checkpoint should be a descendant block of the current
        // checkpoint. Trace back to the same height of current checkpoint
        // to verify.
        CBlockIndex* pindex = pindexCheckpointRecv;
        while (pindex->nHeight > pindexSyncCheckpoint->nHeight)
            if (!(pindex = pindex->pprev))
                return error("ValidateSyncCheckpoint: pprev2 null - block index structure failure");
        if (pindex->GetBlockHash() != hashSyncCheckpoint)
        {
            hashInvalidCheckpoint = hashCheckpoint;
            return error("ValidateSyncCheckpoint: new sync-checkpoint %s is not a descendant of current sync-checkpoint %s", hashCheckpoint.ToString().c_str(), hashSyncCheckpoint.ToString().c_str());
        }
        return true;
    }

    bool WriteSyncCheckpoint(const uint256& hashCheckpoint)
    {
        CTxDB txdb;
        txdb.TxnBegin();
        if (!txdb.WriteSyncCheckpoint(hashCheckpoint))
        {
            txdb.TxnAbort();
            return error("WriteSyncCheckpoint(): failed to write to db sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }
        if (!txdb.TxnCommit())
            return error("WriteSyncCheckpoint(): failed to commit to db sync checkpoint %s", hashCheckpoint.ToString().c_str());

        Checkpoints::hashSyncCheckpoint = hashCheckpoint;
        return true;
    }

    bool AcceptPendingSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint != 0 && mapBlockIndex.count(hashPendingCheckpoint))
        {
            if (!ValidateSyncCheckpoint(hashPendingCheckpoint))
            {
                hashPendingCheckpoint = 0;
                checkpointMessagePending.SetNull();
                return false;
            }

            CTxDB txdb;
            CBlockIndex* pindexCheckpoint = mapBlockIndex[hashPendingCheckpoint];
            if (!pindexCheckpoint->IsInMainChain())
            {
                CBlock block;
                if (!block.ReadFromDisk(pindexCheckpoint))
                    return error("AcceptPendingSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                if (!block.SetBestChain(txdb, pindexCheckpoint))
                {
                    hashInvalidCheckpoint = hashPendingCheckpoint;
                    return error("AcceptPendingSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
                }
            }

            if (!WriteSyncCheckpoint(hashPendingCheckpoint))
                return error("AcceptPendingSyncCheckpoint(): failed to write sync checkpoint %s", hashPendingCheckpoint.ToString().c_str());
            hashPendingCheckpoint = 0;
            checkpointMessage = checkpointMessagePending;
            checkpointMessagePending.SetNull();
            printf("AcceptPendingSyncCheckpoint : sync-checkpoint at %s\n", hashSyncCheckpoint.ToString().c_str());
            // relay the checkpoint
            if (!checkpointMessage.IsNull())
            {
                BOOST_FOREACH(CNode* pnode, vNodes)
                    checkpointMessage.RelayTo(pnode);
            }
            return true;
        }
        return false;
    }

    // Automatically select a suitable sync-checkpoint 
    uint256 AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && (pindex->GetBlockTime() + CHECKPOINT_MAX_SPAN > pindexBest->GetBlockTime() || pindex->nHeight + 8 > pindexBest->nHeight))
            pindex = pindex->pprev;
        return pindex->GetBlockHash();
    }

    // Check against synchronized checkpoint
    bool CheckSync(const uint256& hashBlock, const CBlockIndex* pindexPrev)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        int nHeight = pindexPrev->nHeight + 1;

        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];

        if (nHeight > pindexSync->nHeight)
        {
            // trace back to same height as sync-checkpoint
            const CBlockIndex* pindex = pindexPrev;
            while (pindex->nHeight > pindexSync->nHeight)
                if (!(pindex = pindex->pprev))
                    return error("CheckSync: pprev null - block index structure failure");
            if (pindex->nHeight < pindexSync->nHeight || pindex->GetBlockHash() != hashSyncCheckpoint)
                return false; // only descendant of sync-checkpoint can pass check
        }
        if (nHeight == pindexSync->nHeight && hashBlock != hashSyncCheckpoint)
            return false; // same height with sync-checkpoint
        if (nHeight < pindexSync->nHeight && !mapBlockIndex.count(hashBlock))
            return false; // lower height than sync-checkpoint
        return true;
    }

    bool WantedByPendingSyncCheckpoint(uint256 hashBlock)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (hashPendingCheckpoint == 0)
            return false;
        if (hashBlock == hashPendingCheckpoint)
            return true;
        if (mapOrphanBlocks.count(hashPendingCheckpoint) 
            && hashBlock == WantedByOrphan(mapOrphanBlocks[hashPendingCheckpoint]))
            return true;
        return false;
    }

    // MartexCoin: reset synchronized checkpoint to last hardened checkpoint
    bool ResetSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        const uint256& hash = mapCheckpoints.rbegin()->second;
        if (mapBlockIndex.count(hash) && !mapBlockIndex[hash]->IsInMainChain())
        {
            // checkpoint block accepted but not yet in main chain
            printf("ResetSyncCheckpoint: SetBestChain to hardened checkpoint %s\n", hash.ToString().c_str());
            CTxDB txdb;
            CBlock block;
            if (!block.ReadFromDisk(mapBlockIndex[hash]))
                return error("ResetSyncCheckpoint: ReadFromDisk failed for hardened checkpoint %s", hash.ToString().c_str());
            if (!block.SetBestChain(txdb, mapBlockIndex[hash]))
            {
                return error("ResetSyncCheckpoint: SetBestChain failed for hardened checkpoint %s", hash.ToString().c_str());
            }
        }
        else if(!mapBlockIndex.count(hash))
        {
            // checkpoint block not yet accepted
            hashPendingCheckpoint = hash;
            checkpointMessagePending.SetNull();
            printf("ResetSyncCheckpoint: pending for sync-checkpoint %s\n", hashPendingCheckpoint.ToString().c_str());
        }

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, mapCheckpoints)
        {
            const uint256& hash = i.second;
            if (mapBlockIndex.count(hash) && mapBlockIndex[hash]->IsInMainChain())
            {
                if (!WriteSyncCheckpoint(hash))
                    return error("ResetSyncCheckpoint: failed to write sync checkpoint %s", hash.ToString().c_str());
                printf("ResetSyncCheckpoint: sync-checkpoint reset to %s\n", hashSyncCheckpoint.ToString().c_str());
                return true;
            }
        }

        return false;
    }

    void AskForPendingSyncCheckpoint(CNode* pfrom)
    {
        LOCK(cs_hashSyncCheckpoint);
        if (pfrom && hashPendingCheckpoint != 0 && (!mapBlockIndex.count(hashPendingCheckpoint)) && (!mapOrphanBlocks.count(hashPendingCheckpoint)))
            pfrom->AskFor(CInv(MSG_BLOCK, hashPendingCheckpoint));
    }

    bool SetCheckpointPrivKey(std::string strPrivKey)
    {
        // Test signing a sync-checkpoint with genesis block
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = !fTestNet ? hashGenesisBlock : hashGenesisBlockTestNet;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        std::vector<unsigned char> vchPrivKey = ParseHex(strPrivKey);
        CKey key;
        key.SetPrivKey(CPrivKey(vchPrivKey.begin(), vchPrivKey.end())); // if key is not correct openssl may crash
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return false;

        // Test signing successful, proceed
        CSyncCheckpoint::strMasterPrivKey = strPrivKey;
        return true;
    }

    bool SendSyncCheckpoint(uint256 hashCheckpoint)
    {
        CSyncCheckpoint checkpoint;
        checkpoint.hashCheckpoint = hashCheckpoint;
        CDataStream sMsg(SER_NETWORK, PROTOCOL_VERSION);
        sMsg << (CUnsignedSyncCheckpoint)checkpoint;
        checkpoint.vchMsg = std::vector<unsigned char>(sMsg.begin(), sMsg.end());

        if (CSyncCheckpoint::strMasterPrivKey.empty())
            return error("SendSyncCheckpoint: Checkpoint master key unavailable.");
        std::vector<unsigned char> vchPrivKey = ParseHex(CSyncCheckpoint::strMasterPrivKey);
        CKey key;
        key.SetPrivKey(CPrivKey(vchPrivKey.begin(), vchPrivKey.end())); // if key is not correct openssl may crash
        if (!key.Sign(Hash(checkpoint.vchMsg.begin(), checkpoint.vchMsg.end()), checkpoint.vchSig))
            return error("SendSyncCheckpoint: Unable to sign checkpoint, check private key?");

        if(!checkpoint.ProcessSyncCheckpoint(NULL))
        {
            printf("WARNING: SendSyncCheckpoint: Failed to process checkpoint.\n");
            return false;
        }

        // Relay checkpoint
        {
            LOCK(cs_vNodes);
            BOOST_FOREACH(CNode* pnode, vNodes)
                checkpoint.RelayTo(pnode);
        }
        return true;
    }

    // Is the sync-checkpoint outside maturity window?
    bool IsMatureSyncCheckpoint()
    {
        LOCK(cs_hashSyncCheckpoint);
        // sync-checkpoint should always be accepted block
        assert(mapBlockIndex.count(hashSyncCheckpoint));
        const CBlockIndex* pindexSync = mapBlockIndex[hashSyncCheckpoint];
        return (nBestHeight >= pindexSync->nHeight + nCoinbaseMaturity ||
                pindexSync->GetBlockTime() + nStakeMinAge < GetAdjustedTime());
    }
}

// MartexCoin: sync-checkpoint master key
const std::string CSyncCheckpoint::strMasterPubKey = "0x04fcb557b10ca5b18d9d8a60786d2e5420480ce5548f546150ae790dc7ecaaab0a37f9bb0baf459aca639ce3d72a92310e89b9b34f576cce86f452cff861502938";

std::string CSyncCheckpoint::strMasterPrivKey = "";

// MartexCoin: verify signature of sync-checkpoint message
bool CSyncCheckpoint::CheckSignature()
{
    CKey key;
    if (!key.SetPubKey(ParseHex(CSyncCheckpoint::strMasterPubKey)))
        return error("CSyncCheckpoint::CheckSignature() : SetPubKey failed");
    if (!key.Verify(Hash(vchMsg.begin(), vchMsg.end()), vchSig))
        return error("CSyncCheckpoint::CheckSignature() : verify signature failed");

    // Now unserialize the data
    CDataStream sMsg(vchMsg, SER_NETWORK, PROTOCOL_VERSION);
    sMsg >> *(CUnsignedSyncCheckpoint*)this;
    return true;
}

// MartexCoin: process synchronized checkpoint
bool CSyncCheckpoint::ProcessSyncCheckpoint(CNode* pfrom)
{
    if (!CheckSignature())
        return false;

    LOCK(Checkpoints::cs_hashSyncCheckpoint);
    if (!mapBlockIndex.count(hashCheckpoint))
    {
        // We haven't received the checkpoint chain, keep the checkpoint as pending
        Checkpoints::hashPendingCheckpoint = hashCheckpoint;
        Checkpoints::checkpointMessagePending = *this;
        printf("ProcessSyncCheckpoint: pending for sync-checkpoint %s\n", hashCheckpoint.ToString().c_str());
        // Ask this guy to fill in what we're missing
        if (pfrom)
        {
            pfrom->PushGetBlocks(pindexBest, hashCheckpoint);
            // ask directly as well in case rejected earlier by duplicate
            // proof-of-stake because getblocks may not get it this time
            pfrom->AskFor(CInv(MSG_BLOCK, mapOrphanBlocks.count(hashCheckpoint)? WantedByOrphan(mapOrphanBlocks[hashCheckpoint]) : hashCheckpoint));
        }
        return false;
    }

    if (!Checkpoints::ValidateSyncCheckpoint(hashCheckpoint))
        return false;

    CTxDB txdb;
    CBlockIndex* pindexCheckpoint = mapBlockIndex[hashCheckpoint];
    if (!pindexCheckpoint->IsInMainChain())
    {
        // checkpoint chain received but not yet main chain
        CBlock block;
        if (!block.ReadFromDisk(pindexCheckpoint))
            return error("ProcessSyncCheckpoint: ReadFromDisk failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        if (!block.SetBestChain(txdb, pindexCheckpoint))
        {
            Checkpoints::hashInvalidCheckpoint = hashCheckpoint;
            return error("ProcessSyncCheckpoint: SetBestChain failed for sync checkpoint %s", hashCheckpoint.ToString().c_str());
        }
    }

    if (!Checkpoints::WriteSyncCheckpoint(hashCheckpoint))
        return error("ProcessSyncCheckpoint(): failed to write sync checkpoint %s", hashCheckpoint.ToString().c_str());
    Checkpoints::checkpointMessage = *this;
    Checkpoints::hashPendingCheckpoint = 0;
    Checkpoints::checkpointMessagePending.SetNull();
    printf("ProcessSyncCheckpoint: sync-checkpoint at %s\n", hashCheckpoint.ToString().c_str());
    return true;
}
