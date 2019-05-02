// Copyright (c) 2014-2019 The MarteX Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "dsnotificationinterface.h"
#include "fastsend.h"
#include "governance.h"
#include "masternodeman.h"
#include "masternode-payments.h"
#include "masternode-sync.h"
#include "anonsend.h"
#ifdef ENABLE_WALLET
#include "anonsend-client.h"
#endif // ENABLE_WALLET

void CDSNotificationInterface::InitializeCurrentBlockTip()
{
    LOCK(cs_main);
    UpdatedBlockTip(chainActive.Tip(), NULL, IsInitialBlockDownload());
}

void CDSNotificationInterface::AcceptedBlockHeader(const CBlockIndex *pindexNew)
{
    masternodeSync.AcceptedBlockHeader(pindexNew);
}

void CDSNotificationInterface::NotifyHeaderTip(const CBlockIndex *pindexNew, bool fInitialDownload)
{
    masternodeSync.NotifyHeaderTip(pindexNew, fInitialDownload, connman);
}

void CDSNotificationInterface::UpdatedBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload)
{
    if (pindexNew == pindexFork) // blocks were disconnected without any new ones
        return;

    masternodeSync.UpdatedBlockTip(pindexNew, fInitialDownload, connman);

    // Update global DIP0001 activation status
    fDIP0001ActiveAtTip = pindexNew->nHeight >= Params().GetConsensus().DIP0001Height;

    if (fInitialDownload)
        return;

    if (fLiteMode)
        return;

    mnodeman.UpdatedBlockTip(pindexNew);
    CAnonSend::UpdatedBlockTip(pindexNew);
#ifdef ENABLE_WALLET
    anonSendClient.UpdatedBlockTip(pindexNew);
#endif // ENABLE_WALLET
    fastsend.UpdatedBlockTip(pindexNew);
    mnpayments.UpdatedBlockTip(pindexNew, connman);
    governance.UpdatedBlockTip(pindexNew, connman);
}

void CDSNotificationInterface::SyncTransaction(const CTransaction &tx, const CBlockIndex *pindex, int posInBlock)
{
    fastsend.SyncTransaction(tx, pindex, posInBlock);
    CAnonSend::SyncTransaction(tx, pindex, posInBlock);
}
