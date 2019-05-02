// Copyright (c) 2014-2019 The MarteX Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include "anonsend-client.h"

#include "wallet/coincontrol.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "init.h"
#include "masternode-payments.h"
#include "masternode-sync.h"
#include "masternodeman.h"
#include "netmessagemaker.h"
#include "script/sign.h"
#include "txmempool.h"
#include "util.h"
#include "utilmoneystr.h"

#include <memory>

CAnonSendClient anonSendClient;

void CAnonSendClient::ProcessMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman)
{
    if(fMasternodeMode) return;
    if(fLiteMode) return; // ignore all MarteX related functionality
    if(!masternodeSync.IsBlockchainSynced()) return;

    if(strCommand == NetMsgType::DSQUEUE) {
        TRY_LOCK(cs_anonsend, lockRecv);
        if(!lockRecv) return;

        if(pfrom->nVersion < MIN_ANONSEND_PEER_PROTO_VERSION) {
            LogPrint("anonsend", "DSQUEUE -- peer=%d using obsolete version %i\n", pfrom->id, pfrom->nVersion);
            connman.PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::REJECT, strCommand, REJECT_OBSOLETE,
                               strprintf("Version must be %d or greater", MIN_ANONSEND_PEER_PROTO_VERSION)));
            return;
        }

        CAnonsendQueue dsq;
        vRecv >> dsq;

        // process every dsq only once
        for (const auto& q : vecAnonsendQueue) {
            if(q == dsq) {
                // LogPrint("anonsend", "DSQUEUE -- %s seen\n", dsq.ToString());
                return;
            }
        }

        LogPrint("anonsend", "DSQUEUE -- %s new\n", dsq.ToString());

        if(dsq.IsExpired()) return;
        if(dsq.nInputCount < 0 || dsq.nInputCount > ANONSEND_ENTRY_MAX_SIZE) return;

        masternode_info_t infoMn;
        if(!mnodeman.GetMasternodeInfo(dsq.masternodeOutpoint, infoMn)) return;

        if(!dsq.CheckSignature(infoMn.pubKeyMasternode)) {
            // we probably have outdated info
            mnodeman.AskForMN(pfrom, dsq.masternodeOutpoint, connman);
            return;
        }

        // if the queue is ready, submit if we can
        if(dsq.fReady) {
            if(!infoMixingMasternode.fInfoValid) return;
            if(infoMixingMasternode.addr != infoMn.addr) {
                LogPrintf("DSQUEUE -- message doesn't match current Masternode: infoMixingMasternode=%s, addr=%s\n", infoMixingMasternode.addr.ToString(), infoMn.addr.ToString());
                return;
            }

            if(nState == POOL_STATE_QUEUE) {
                LogPrint("anonsend", "DSQUEUE -- AnonSend queue (%s) is ready on masternode %s\n", dsq.ToString(), infoMn.addr.ToString());
                SubmitDenominate(connman);
            }
        } else {
            for (const auto& q : vecAnonsendQueue) {
                if(q.masternodeOutpoint == dsq.masternodeOutpoint) {
                    // no way same mn can send another "not yet ready" dsq this soon
                    LogPrint("anonsend", "DSQUEUE -- Masternode %s is sending WAY too many dsq messages\n", infoMn.addr.ToString());
                    return;
                }
            }

            int nThreshold = infoMn.nLastDsq + mnodeman.CountEnabled(MIN_ANONSEND_PEER_PROTO_VERSION)/5;
            LogPrint("anonsend", "DSQUEUE -- nLastDsq: %d  threshold: %d  nDsqCount: %d\n", infoMn.nLastDsq, nThreshold, mnodeman.nDsqCount);
            //don't allow a few nodes to dominate the queuing process
            if(infoMn.nLastDsq != 0 && nThreshold > mnodeman.nDsqCount) {
                LogPrint("anonsend", "DSQUEUE -- Masternode %s is sending too many dsq messages\n", infoMn.addr.ToString());
                return;
            }

            if(!mnodeman.AllowMixing(dsq.masternodeOutpoint)) return;

            LogPrint("anonsend", "DSQUEUE -- new AnonSend queue (%s) from masternode %s\n", dsq.ToString(), infoMn.addr.ToString());
            if(infoMixingMasternode.fInfoValid && infoMixingMasternode.outpoint == dsq.masternodeOutpoint) {
                dsq.fTried = true;
            }
            vecAnonsendQueue.push_back(dsq);
            dsq.Relay(connman);
        }

    } else if(strCommand == NetMsgType::DSSTATUSUPDATE) {

        if(pfrom->nVersion < MIN_ANONSEND_PEER_PROTO_VERSION) {
            LogPrint("anonsend", "DSSTATUSUPDATE -- peer=%d using obsolete version %i\n", pfrom->id, pfrom->nVersion);
            connman.PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::REJECT, strCommand, REJECT_OBSOLETE,
                               strprintf("Version must be %d or greater", MIN_ANONSEND_PEER_PROTO_VERSION)));
            return;
        }

        if(!infoMixingMasternode.fInfoValid) return;
        if(infoMixingMasternode.addr != pfrom->addr) {
            //LogPrintf("DSSTATUSUPDATE -- message doesn't match current Masternode: infoMixingMasternode %s addr %s\n", infoMixingMasternode.addr.ToString(), pfrom->addr.ToString());
            return;
        }

        int nMsgSessionID;
        int nMsgState;
        int nMsgEntriesCount;
        int nMsgStatusUpdate;
        int nMsgMessageID;
        vRecv >> nMsgSessionID >> nMsgState >> nMsgEntriesCount >> nMsgStatusUpdate >> nMsgMessageID;

        if(nMsgState < POOL_STATE_MIN || nMsgState > POOL_STATE_MAX) {
            LogPrint("anonsend", "DSSTATUSUPDATE -- nMsgState is out of bounds: %d\n", nMsgState);
            return;
        }

        if(nMsgStatusUpdate < STATUS_REJECTED || nMsgStatusUpdate > STATUS_ACCEPTED) {
            LogPrint("anonsend", "DSSTATUSUPDATE -- nMsgStatusUpdate is out of bounds: %d\n", nMsgStatusUpdate);
            return;
        }

        if(nMsgMessageID < MSG_POOL_MIN || nMsgMessageID > MSG_POOL_MAX) {
            LogPrint("anonsend", "DSSTATUSUPDATE -- nMsgMessageID is out of bounds: %d\n", nMsgMessageID);
            return;
        }

        LogPrint("anonsend", "DSSTATUSUPDATE -- nMsgSessionID %d  nMsgState: %d  nEntriesCount: %d  nMsgStatusUpdate: %d  nMsgMessageID %d (%s)\n",
                nMsgSessionID, nMsgState, nEntriesCount, nMsgStatusUpdate, nMsgMessageID, CAnonSend::GetMessageByID(PoolMessage(nMsgMessageID)));

        if(!CheckPoolStateUpdate(PoolState(nMsgState), nMsgEntriesCount, PoolStatusUpdate(nMsgStatusUpdate), PoolMessage(nMsgMessageID), nMsgSessionID)) {
            LogPrint("anonsend", "DSSTATUSUPDATE -- CheckPoolStateUpdate failed\n");
        }

    } else if(strCommand == NetMsgType::DSFINALTX) {

        if(pfrom->nVersion < MIN_ANONSEND_PEER_PROTO_VERSION) {
            LogPrint("anonsend", "DSFINALTX -- peer=%d using obsolete version %i\n", pfrom->id, pfrom->nVersion);
            connman.PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::REJECT, strCommand, REJECT_OBSOLETE,
                               strprintf("Version must be %d or greater", MIN_ANONSEND_PEER_PROTO_VERSION)));
            return;
        }

        if(!infoMixingMasternode.fInfoValid) return;
        if(infoMixingMasternode.addr != pfrom->addr) {
            //LogPrintf("DSFINALTX -- message doesn't match current Masternode: infoMixingMasternode %s addr %s\n", infoMixingMasternode.addr.ToString(), pfrom->addr.ToString());
            return;
        }

        int nMsgSessionID;
        vRecv >> nMsgSessionID;
        CTransaction txNew(deserialize, vRecv);

        if(nSessionID != nMsgSessionID) {
            LogPrint("anonsend", "DSFINALTX -- message doesn't match current AnonSend session: nSessionID: %d  nMsgSessionID: %d\n", nSessionID, nMsgSessionID);
            return;
        }

        LogPrint("anonsend", "DSFINALTX -- txNew %s", txNew.ToString());

        //check to see if input is spent already? (and probably not confirmed)
        SignFinalTransaction(txNew, pfrom, connman);

    } else if(strCommand == NetMsgType::DSCOMPLETE) {

        if(pfrom->nVersion < MIN_ANONSEND_PEER_PROTO_VERSION) {
            LogPrint("anonsend", "DSCOMPLETE -- peer=%d using obsolete version %i\n", pfrom->id, pfrom->nVersion);
            connman.PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::REJECT, strCommand, REJECT_OBSOLETE,
                               strprintf("Version must be %d or greater", MIN_ANONSEND_PEER_PROTO_VERSION)));
            return;
        }

        if(!infoMixingMasternode.fInfoValid) return;
        if(infoMixingMasternode.addr != pfrom->addr) {
            LogPrint("anonsend", "DSCOMPLETE -- message doesn't match current Masternode: infoMixingMasternode=%s  addr=%s\n", infoMixingMasternode.addr.ToString(), pfrom->addr.ToString());
            return;
        }

        int nMsgSessionID;
        int nMsgMessageID;
        vRecv >> nMsgSessionID >> nMsgMessageID;

        if(nMsgMessageID < MSG_POOL_MIN || nMsgMessageID > MSG_POOL_MAX) {
            LogPrint("anonsend", "DSCOMPLETE -- nMsgMessageID is out of bounds: %d\n", nMsgMessageID);
            return;
        }

        if(nSessionID != nMsgSessionID) {
            LogPrint("anonsend", "DSCOMPLETE -- message doesn't match current AnonSend session: nSessionID: %d  nMsgSessionID: %d\n", nSessionID, nMsgSessionID);
            return;
        }

        LogPrint("anonsend", "DSCOMPLETE -- nMsgSessionID %d  nMsgMessageID %d (%s)\n", nMsgSessionID, nMsgMessageID, CAnonSend::GetMessageByID(PoolMessage(nMsgMessageID)));

        CompletedTransaction(PoolMessage(nMsgMessageID));
    }
}

void CAnonSendClient::ResetPool()
{
    nCachedLastSuccessBlock = 0;
    txMyCollateral = CMutableTransaction();
    vecMasternodesUsed.clear();
    UnlockCoins();
    keyHolderStorage.ReturnAll();
    SetNull();
}

void CAnonSendClient::SetNull()
{
    // Client side
    nEntriesCount = 0;
    fLastEntryAccepted = false;
    infoMixingMasternode = masternode_info_t();
    pendingDsaRequest = CPendingDsaRequest();

    CAnonSendBase::SetNull();
}

//
// Unlock coins after mixing fails or succeeds
//
void CAnonSendClient::UnlockCoins()
{
    while(true) {
        TRY_LOCK(pwalletMain->cs_wallet, lockWallet);
        if(!lockWallet) {MilliSleep(50); continue;}
        for (const auto& outpoint : vecOutPointLocked)
            pwalletMain->UnlockCoin(outpoint);
        break;
    }

    vecOutPointLocked.clear();
}

std::string CAnonSendClient::GetStatus()
{
    static int nStatusMessageProgress = 0;
    nStatusMessageProgress += 10;
    std::string strSuffix = "";

    if(WaitForAnotherBlock() || !masternodeSync.IsBlockchainSynced())
        return strAutoDenomResult;

    switch(nState) {
        case POOL_STATE_IDLE:
            return _("AnonSend is idle.");
        case POOL_STATE_QUEUE:
            if(     nStatusMessageProgress % 70 <= 30) strSuffix = ".";
            else if(nStatusMessageProgress % 70 <= 50) strSuffix = "..";
            else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";
            return strprintf(_("Submitted to masternode, waiting in queue %s"), strSuffix);;
        case POOL_STATE_ACCEPTING_ENTRIES:
            if(nEntriesCount == 0) {
                nStatusMessageProgress = 0;
                return strAutoDenomResult;
            } else if(fLastEntryAccepted) {
                if(nStatusMessageProgress % 10 > 8) {
                    fLastEntryAccepted = false;
                    nStatusMessageProgress = 0;
                }
                return _("AnonSend request complete:") + " " + _("Your transaction was accepted into the pool!");
            } else {
                if(     nStatusMessageProgress % 70 <= 40) return strprintf(_("Submitted following entries to masternode: %u / %d"), nEntriesCount, CAnonSend::GetMaxPoolTransactions());
                else if(nStatusMessageProgress % 70 <= 50) strSuffix = ".";
                else if(nStatusMessageProgress % 70 <= 60) strSuffix = "..";
                else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";
                return strprintf(_("Submitted to masternode, waiting for more entries ( %u / %d ) %s"), nEntriesCount, CAnonSend::GetMaxPoolTransactions(), strSuffix);
            }
        case POOL_STATE_SIGNING:
            if(     nStatusMessageProgress % 70 <= 40) return _("Found enough users, signing ...");
            else if(nStatusMessageProgress % 70 <= 50) strSuffix = ".";
            else if(nStatusMessageProgress % 70 <= 60) strSuffix = "..";
            else if(nStatusMessageProgress % 70 <= 70) strSuffix = "...";
            return strprintf(_("Found enough users, signing ( waiting %s )"), strSuffix);
        case POOL_STATE_ERROR:
            return _("AnonSend request incomplete:") + " " + strLastMessage + " " + _("Will retry...");
        case POOL_STATE_SUCCESS:
            return _("AnonSend request complete:") + " " + strLastMessage;
       default:
            return strprintf(_("Unknown state: id = %u"), nState);
    }
}

bool CAnonSendClient::GetMixingMasternodeInfo(masternode_info_t& mnInfoRet)
{
    mnInfoRet = infoMixingMasternode.fInfoValid ? infoMixingMasternode : masternode_info_t();
    return infoMixingMasternode.fInfoValid;
}

bool CAnonSendClient::IsMixingMasternode(const CNode* pnode)
{
    return infoMixingMasternode.fInfoValid && pnode->addr == infoMixingMasternode.addr;
}

//
// Check the mixing progress and send client updates if a Masternode
//
void CAnonSendClient::CheckPool()
{
    // reset if we're here for 10 seconds
    if((nState == POOL_STATE_ERROR || nState == POOL_STATE_SUCCESS) && GetTime() - nTimeLastSuccessfulStep >= 10) {
        LogPrint("anonsend", "CAnonSendClient::CheckPool -- timeout, RESETTING\n");
        UnlockCoins();
        if (nState == POOL_STATE_ERROR) {
            keyHolderStorage.ReturnAll();
        } else {
            keyHolderStorage.KeepAll();
        }
        SetNull();
    }
}

//
// Check for various timeouts (queue objects, mixing, etc)
//
void CAnonSendClient::CheckTimeout()
{
    if(fMasternodeMode) return;

    CheckQueue();

    if(!fEnableAnonSend) return;

    // catching hanging sessions
    switch(nState) {
        case POOL_STATE_ERROR:
            LogPrint("anonsend", "CAnonSendClient::CheckTimeout -- Pool error -- Running CheckPool\n");
            CheckPool();
            break;
        case POOL_STATE_SUCCESS:
            LogPrint("anonsend", "CAnonSendClient::CheckTimeout -- Pool success -- Running CheckPool\n");
            CheckPool();
            break;
        default:
            break;
    }

    int nLagTime = 10; // give the server a few extra seconds before resetting.
    int nTimeout = (nState == POOL_STATE_SIGNING) ? ANONSEND_SIGNING_TIMEOUT : ANONSEND_QUEUE_TIMEOUT;
    bool fTimeout = GetTime() - nTimeLastSuccessfulStep >= nTimeout + nLagTime;

    if(nState != POOL_STATE_IDLE && fTimeout) {
        LogPrint("anonsend", "CAnonSendClient::CheckTimeout -- %s timed out (%ds) -- resetting\n",
                (nState == POOL_STATE_SIGNING) ? "Signing" : "Session", nTimeout);
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();
        SetState(POOL_STATE_ERROR);
        strLastMessage = _("Session timed out.");
    }
}

//
// Execute a mixing denomination via a Masternode.
// This is only ran from clients
//
bool CAnonSendClient::SendDenominate(const std::vector<CTxDSIn>& vecTxDSIn, const std::vector<CTxOut>& vecTxOut, CConnman& connman)
{
    if(fMasternodeMode) {
        LogPrintf("CAnonSendClient::SendDenominate -- AnonSend from a Masternode is not supported currently.\n");
        return false;
    }

    if(txMyCollateral == CMutableTransaction()) {
        LogPrintf("CAnonSendClient:SendDenominate -- AnonSend collateral not set\n");
        return false;
    }

    // lock the funds we're going to use
    for (const auto& txin : txMyCollateral.vin)
        vecOutPointLocked.push_back(txin.prevout);

    for (const auto& txdsin : vecTxDSIn)
        vecOutPointLocked.push_back(txdsin.prevout);

    // we should already be connected to a Masternode
    if(!nSessionID) {
        LogPrintf("CAnonSendClient::SendDenominate -- No Masternode has been selected yet.\n");
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();
        return false;
    }

    if(!CheckDiskSpace()) {
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();
        fEnableAnonSend = false;
        LogPrintf("CAnonSendClient::SendDenominate -- Not enough disk space, disabling AnonSend.\n");
        return false;
    }

    SetState(POOL_STATE_ACCEPTING_ENTRIES);
    strLastMessage = "";

    LogPrintf("CAnonSendClient::SendDenominate -- Added transaction to pool.\n");

    {
        // construct a pseudo tx, for debugging purpuses only

        CMutableTransaction tx;

        for (const auto& txdsin : vecTxDSIn) {
            LogPrint("anonsend", "CAnonSendClient::SendDenominate -- txdsin=%s\n", txdsin.ToString());
            tx.vin.push_back(txdsin);
        }

        for (const CTxOut& txout : vecTxOut) {
            LogPrint("anonsend", "CAnonSendClient::SendDenominate -- txout=%s\n", txout.ToString());
            tx.vout.push_back(txout);
        }

        LogPrintf("CAnonSendClient::SendDenominate -- Submitting partial tx %s", tx.ToString());
    }

    // store our entry for later use
    CDarkSendEntry entry(vecTxDSIn, vecTxOut, txMyCollateral);
    vecEntries.push_back(entry);
    RelayIn(entry, connman);
    nTimeLastSuccessfulStep = GetTime();

    return true;
}

// Incoming message from Masternode updating the progress of mixing
bool CAnonSendClient::CheckPoolStateUpdate(PoolState nStateNew, int nEntriesCountNew, PoolStatusUpdate nStatusUpdate, PoolMessage nMessageID, int nSessionIDNew)
{
    if(fMasternodeMode) return false;

    // do not update state when mixing client state is one of these
    if(nState == POOL_STATE_IDLE || nState == POOL_STATE_ERROR || nState == POOL_STATE_SUCCESS) return false;

    strAutoDenomResult = _("Masternode:") + " " + CAnonSend::GetMessageByID(nMessageID);

    // if rejected at any state
    if(nStatusUpdate == STATUS_REJECTED) {
        LogPrintf("CAnonSendClient::CheckPoolStateUpdate -- entry is rejected by Masternode\n");
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();
        SetState(POOL_STATE_ERROR);
        strLastMessage = CAnonSend::GetMessageByID(nMessageID);
        return true;
    }

    if(nStatusUpdate == STATUS_ACCEPTED && nState == nStateNew) {
        if(nStateNew == POOL_STATE_QUEUE && nSessionID == 0 && nSessionIDNew != 0) {
            // new session id should be set only in POOL_STATE_QUEUE state
            nSessionID = nSessionIDNew;
            nTimeLastSuccessfulStep = GetTime();
            LogPrintf("CAnonSendClient::CheckPoolStateUpdate -- set nSessionID to %d\n", nSessionID);
            return true;
        }
        else if(nStateNew == POOL_STATE_ACCEPTING_ENTRIES && nEntriesCount != nEntriesCountNew) {
            nEntriesCount = nEntriesCountNew;
            nTimeLastSuccessfulStep = GetTime();
            fLastEntryAccepted = true;
            LogPrintf("CAnonSendClient::CheckPoolStateUpdate -- new entry accepted!\n");
            return true;
        }
    }

    // only situations above are allowed, fail in any other case
    return false;
}

//
// After we receive the finalized transaction from the Masternode, we must
// check it to make sure it's what we want, then sign it if we agree.
// If we refuse to sign, it's possible we'll be charged collateral
//
bool CAnonSendClient::SignFinalTransaction(const CTransaction& finalTransactionNew, CNode* pnode, CConnman& connman)
{
    if(fMasternodeMode || pnode == NULL) return false;

    finalMutableTransaction = finalTransactionNew;
    LogPrintf("CAnonSendClient::SignFinalTransaction -- finalMutableTransaction=%s", finalMutableTransaction.ToString());

    // Make sure it's BIP69 compliant
    sort(finalMutableTransaction.vin.begin(), finalMutableTransaction.vin.end(), CompareInputBIP69());
    sort(finalMutableTransaction.vout.begin(), finalMutableTransaction.vout.end(), CompareOutputBIP69());

    if(finalMutableTransaction.GetHash() != finalTransactionNew.GetHash()) {
        LogPrintf("CAnonSendClient::SignFinalTransaction -- WARNING! Masternode %s is not BIP69 compliant!\n", infoMixingMasternode.outpoint.ToStringShort());
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();
        return false;
    }

    std::vector<CTxIn> sigs;

    //make sure my inputs/outputs are present, otherwise refuse to sign
    for (const auto& entry : vecEntries) {
        for (const auto& txdsin : entry.vecTxDSIn) {
            /* Sign my transaction and all outputs */
            int nMyInputIndex = -1;
            CScript prevPubKey = CScript();
            CTxIn txin = CTxIn();

            for(unsigned int i = 0; i < finalMutableTransaction.vin.size(); i++) {
                if(finalMutableTransaction.vin[i] == txdsin) {
                    nMyInputIndex = i;
                    prevPubKey = txdsin.prevPubKey;
                    txin = txdsin;
                }
            }

            if(nMyInputIndex >= 0) { //might have to do this one input at a time?
                int nFoundOutputsCount = 0;
                CAmount nValue1 = 0;
                CAmount nValue2 = 0;

                for (const auto& txoutFinal : finalMutableTransaction.vout) {
                    for (const auto& txout: entry.vecTxOut) {
                        if(txoutFinal == txout) {
                            nFoundOutputsCount++;
                            nValue1 += txoutFinal.nValue;
                        }
                    }
                }

                for (const auto& txout : entry.vecTxOut)
                    nValue2 += txout.nValue;

                int nTargetOuputsCount = entry.vecTxOut.size();
                if(nFoundOutputsCount < nTargetOuputsCount || nValue1 != nValue2) {
                    // in this case, something went wrong and we'll refuse to sign. It's possible we'll be charged collateral. But that's
                    // better then signing if the transaction doesn't look like what we wanted.
                    LogPrintf("CAnonSendClient::SignFinalTransaction -- My entries are not correct! Refusing to sign: nFoundOutputsCount: %d, nTargetOuputsCount: %d\n", nFoundOutputsCount, nTargetOuputsCount);
                    UnlockCoins();
                    keyHolderStorage.ReturnAll();
                    SetNull();

                    return false;
                }

                const CKeyStore& keystore = *pwalletMain;

                LogPrint("anonsend", "CAnonSendClient::SignFinalTransaction -- Signing my input %i\n", nMyInputIndex);
                if(!SignSignature(keystore, prevPubKey, finalMutableTransaction, nMyInputIndex, int(SIGHASH_ALL|SIGHASH_ANYONECANPAY))) { // changes scriptSig
                    LogPrint("anonsend", "CAnonSendClient::SignFinalTransaction -- Unable to sign my own transaction!\n");
                    // not sure what to do here, it will timeout...?
                }

                sigs.push_back(finalMutableTransaction.vin[nMyInputIndex]);
                LogPrint("anonsend", "CAnonSendClient::SignFinalTransaction -- nMyInputIndex: %d, sigs.size(): %d, scriptSig=%s\n", nMyInputIndex, (int)sigs.size(), ScriptToAsmStr(finalMutableTransaction.vin[nMyInputIndex].scriptSig));
            }
        }
    }

    if(sigs.empty()) {
        LogPrintf("CAnonSendClient::SignFinalTransaction -- can't sign anything!\n");
        UnlockCoins();
        keyHolderStorage.ReturnAll();
        SetNull();

        return false;
    }

    // push all of our signatures to the Masternode
    LogPrintf("CAnonSendClient::SignFinalTransaction -- pushing sigs to the masternode, finalMutableTransaction=%s", finalMutableTransaction.ToString());
    CNetMsgMaker msgMaker(pnode->GetSendVersion());
    connman.PushMessage(pnode, msgMaker.Make(NetMsgType::DSSIGNFINALTX, sigs));
    SetState(POOL_STATE_SIGNING);
    nTimeLastSuccessfulStep = GetTime();

    return true;
}

// mixing transaction was completed (failed or successful)
void CAnonSendClient::CompletedTransaction(PoolMessage nMessageID)
{
    if(fMasternodeMode) return;

    if(nMessageID == MSG_SUCCESS) {
        LogPrintf("CompletedTransaction -- success\n");
        nCachedLastSuccessBlock = nCachedBlockHeight;
        keyHolderStorage.KeepAll();
    } else {
        LogPrintf("CompletedTransaction -- error\n");
        keyHolderStorage.ReturnAll();
    }
    UnlockCoins();
    SetNull();
    strLastMessage = CAnonSend::GetMessageByID(nMessageID);
}

bool CAnonSendClient::IsDenomSkipped(CAmount nDenomValue)
{
    return std::find(vecDenominationsSkipped.begin(), vecDenominationsSkipped.end(), nDenomValue) != vecDenominationsSkipped.end();
}

bool CAnonSendClient::WaitForAnotherBlock()
{
    if(!masternodeSync.IsMasternodeListSynced())
        return true;

    if(fAnonSendMultiSession)
        return false;

    return nCachedBlockHeight - nCachedLastSuccessBlock < nMinBlocksToWait;
}

bool CAnonSendClient::CheckAutomaticBackup()
{
    switch(nWalletBackups) {
        case 0:
            LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- Automatic backups disabled, no mixing available.\n");
            strAutoDenomResult = _("Automatic backups disabled") + ", " + _("no mixing available.");
            fEnableAnonSend = false; // stop mixing
            pwalletMain->nKeysLeftSinceAutoBackup = 0; // no backup, no "keys since last backup"
            return false;
        case -1:
            // Automatic backup failed, nothing else we can do until user fixes the issue manually.
            // There is no way to bring user attention in daemon mode so we just update status and
            // keep spamming if debug is on.
            LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- ERROR! Failed to create automatic backup.\n");
            strAutoDenomResult = _("ERROR! Failed to create automatic backup") + ", " + _("see debug.log for details.");
            return false;
        case -2:
            // We were able to create automatic backup but keypool was not replenished because wallet is locked.
            // There is no way to bring user attention in daemon mode so we just update status and
            // keep spamming if debug is on.
            LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- WARNING! Failed to create replenish keypool, please unlock your wallet to do so.\n");
            strAutoDenomResult = _("WARNING! Failed to replenish keypool, please unlock your wallet to do so.") + ", " + _("see debug.log for details.");
            return false;
    }

    if(pwalletMain->nKeysLeftSinceAutoBackup < ANONSEND_KEYS_THRESHOLD_STOP) {
        // We should never get here via mixing itself but probably smth else is still actively using keypool
        LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- Very low number of keys left: %d, no mixing available.\n", pwalletMain->nKeysLeftSinceAutoBackup);
        strAutoDenomResult = strprintf(_("Very low number of keys left: %d") + ", " + _("no mixing available."), pwalletMain->nKeysLeftSinceAutoBackup);
        // It's getting really dangerous, stop mixing
        fEnableAnonSend = false;
        return false;
    } else if(pwalletMain->nKeysLeftSinceAutoBackup < ANONSEND_KEYS_THRESHOLD_WARNING) {
        // Low number of keys left but it's still more or less safe to continue
        LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- Very low number of keys left: %d\n", pwalletMain->nKeysLeftSinceAutoBackup);
        strAutoDenomResult = strprintf(_("Very low number of keys left: %d"), pwalletMain->nKeysLeftSinceAutoBackup);

        if(fCreateAutoBackups) {
            LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- Trying to create new backup.\n");
            std::string warningString;
            std::string errorString;

            if(!AutoBackupWallet(pwalletMain, "", warningString, errorString)) {
                if(!warningString.empty()) {
                    // There were some issues saving backup but yet more or less safe to continue
                    LogPrintf("CAnonSendClient::CheckAutomaticBackup -- WARNING! Something went wrong on automatic backup: %s\n", warningString);
                }
                if(!errorString.empty()) {
                    // Things are really broken
                    LogPrintf("CAnonSendClient::CheckAutomaticBackup -- ERROR! Failed to create automatic backup: %s\n", errorString);
                    strAutoDenomResult = strprintf(_("ERROR! Failed to create automatic backup") + ": %s", errorString);
                    return false;
                }
            }
        } else {
            // Wait for smth else (e.g. GUI action) to create automatic backup for us
            return false;
        }
    }

    LogPrint("anonsend", "CAnonSendClient::CheckAutomaticBackup -- Keys left since latest backup: %d\n", pwalletMain->nKeysLeftSinceAutoBackup);

    return true;
}

//
// Passively run mixing in the background to anonymize funds based on the given configuration.
//
bool CAnonSendClient::DoAutomaticDenominating(CConnman& connman, bool fDryRun)
{
    if(fMasternodeMode) return false; // no client-side mixing on masternodes
    if(!fEnableAnonSend) return false;
    if(!pwalletMain || pwalletMain->IsLocked(true)) return false;
    if(nState != POOL_STATE_IDLE) return false;

    if(!masternodeSync.IsMasternodeListSynced()) {
        strAutoDenomResult = _("Can't mix while sync in progress.");
        return false;
    }

    if(!CheckAutomaticBackup())
        return false;

    if(GetEntriesCount() > 0) {
        strAutoDenomResult = _("Mixing in progress...");
        return false;
    }

    TRY_LOCK(cs_anonsend, lockDS);
    if(!lockDS) {
        strAutoDenomResult = _("Lock is already in place.");
        return false;
    }

    if(!fDryRun && pwalletMain->IsLocked(true)) {
        strAutoDenomResult = _("Wallet is locked.");
        return false;
    }

    if(WaitForAnotherBlock()) {
        LogPrintf("CAnonSendClient::DoAutomaticDenominating -- Last successful AnonSend action was too recent\n");
        strAutoDenomResult = _("Last successful AnonSend action was too recent.");
        return false;
    }

    if(mnodeman.size() == 0) {
        LogPrint("anonsend", "CAnonSendClient::DoAutomaticDenominating -- No Masternodes detected\n");
        strAutoDenomResult = _("No Masternodes detected.");
        return false;
    }

    CAmount nValueMin = CAnonSend::GetSmallestDenomination();

    // if there are no confirmed DS collateral inputs yet
    if(!pwalletMain->HasCollateralInputs()) {
        // should have some additional amount for them
        nValueMin += CAnonSend::GetMaxCollateralAmount();
    }

    // including denoms but applying some restrictions
    CAmount nBalanceNeedsAnonymized = pwalletMain->GetNeedsToBeAnonymizedBalance(nValueMin);

    // anonymizable balance is way too small
    if(nBalanceNeedsAnonymized < nValueMin) {
        LogPrintf("CAnonSendClient::DoAutomaticDenominating -- Not enough funds to anonymize\n");
        strAutoDenomResult = _("Not enough funds to anonymize.");
        return false;
    }

    // excluding denoms
    CAmount nBalanceAnonimizableNonDenom = pwalletMain->GetAnonymizableBalance(true);
    // denoms
    CAmount nBalanceDenominatedConf = pwalletMain->GetDenominatedBalance();
    CAmount nBalanceDenominatedUnconf = pwalletMain->GetDenominatedBalance(true);
    CAmount nBalanceDenominated = nBalanceDenominatedConf + nBalanceDenominatedUnconf;

    LogPrint("anonsend", "CAnonSendClient::DoAutomaticDenominating -- nValueMin: %f, nBalanceNeedsAnonymized: %f, nBalanceAnonimizableNonDenom: %f, nBalanceDenominatedConf: %f, nBalanceDenominatedUnconf: %f, nBalanceDenominated: %f\n",
            (float)nValueMin/COIN,
            (float)nBalanceNeedsAnonymized/COIN,
            (float)nBalanceAnonimizableNonDenom/COIN,
            (float)nBalanceDenominatedConf/COIN,
            (float)nBalanceDenominatedUnconf/COIN,
            (float)nBalanceDenominated/COIN);

    if(fDryRun) return true;

    // Check if we have should create more denominated inputs i.e.
    // there are funds to denominate and denominated balance does not exceed
    // max amount to mix yet.
    if(nBalanceAnonimizableNonDenom >= nValueMin + CAnonSend::GetCollateralAmount() && nBalanceDenominated < nAnonSendAmount*COIN)
        return CreateDenominated(connman);

    //check if we have the collateral sized inputs
    if(!pwalletMain->HasCollateralInputs())
        return !pwalletMain->HasCollateralInputs(false) && MakeCollateralAmounts(connman);

    if(nSessionID) {
        strAutoDenomResult = _("Mixing in progress...");
        return false;
    }

    // Initial phase, find a Masternode
    // Clean if there is anything left from previous session
    UnlockCoins();
    keyHolderStorage.ReturnAll();
    SetNull();

    // should be no unconfirmed denoms in non-multi-session mode
    if(!fAnonSendMultiSession && nBalanceDenominatedUnconf > 0) {
        LogPrintf("CAnonSendClient::DoAutomaticDenominating -- Found unconfirmed denominated outputs, will wait till they confirm to continue.\n");
        strAutoDenomResult = _("Found unconfirmed denominated outputs, will wait till they confirm to continue.");
        return false;
    }

    //check our collateral and create new if needed
    std::string strReason;
    if(txMyCollateral == CMutableTransaction()) {
        if(!pwalletMain->CreateCollateralTransaction(txMyCollateral, strReason)) {
            LogPrintf("CAnonSendClient::DoAutomaticDenominating -- create collateral error:%s\n", strReason);
            return false;
        }
    } else {
        if(!CAnonSend::IsCollateralValid(txMyCollateral)) {
            LogPrintf("CAnonSendClient::DoAutomaticDenominating -- invalid collateral, recreating...\n");
            if(!pwalletMain->CreateCollateralTransaction(txMyCollateral, strReason)) {
                LogPrintf("CAnonSendClient::DoAutomaticDenominating -- create collateral error: %s\n", strReason);
                return false;
            }
        }
    }

    int nMnCountEnabled = mnodeman.CountEnabled(MIN_ANONSEND_PEER_PROTO_VERSION);

    // If we've used 90% of the Masternode list then drop the oldest first ~30%
    int nThreshold_high = nMnCountEnabled * 0.9;
    int nThreshold_low = nThreshold_high * 0.7;
    LogPrint("anonsend", "Checking vecMasternodesUsed: size: %d, threshold: %d\n", (int)vecMasternodesUsed.size(), nThreshold_high);

    if((int)vecMasternodesUsed.size() > nThreshold_high) {
        vecMasternodesUsed.erase(vecMasternodesUsed.begin(), vecMasternodesUsed.begin() + vecMasternodesUsed.size() - nThreshold_low);
        LogPrint("anonsend", "  vecMasternodesUsed: new size: %d, threshold: %d\n", (int)vecMasternodesUsed.size(), nThreshold_high);
    }

    bool fUseQueue = GetRandInt(100) > 33;
    // don't use the queues all of the time for mixing unless we are a liquidity provider
    if((nLiquidityProvider || fUseQueue) && JoinExistingQueue(nBalanceNeedsAnonymized, connman))
        return true;

    // do not initiate queue if we are a liquidity provider to avoid useless inter-mixing
    if(nLiquidityProvider) return false;

    if(StartNewQueue(nValueMin, nBalanceNeedsAnonymized, connman))
        return true;

    strAutoDenomResult = _("No compatible Masternode found.");
    return false;
}

bool CAnonSendClient::JoinExistingQueue(CAmount nBalanceNeedsAnonymized, CConnman& connman)
{
    std::vector<CAmount> vecStandardDenoms = CAnonSend::GetStandardDenominations();
    // Look through the queues and see if anything matches
    for (auto& dsq : vecAnonsendQueue) {
        // only try each queue once
        if(dsq.fTried) continue;
        dsq.fTried = true;

        if(dsq.IsExpired()) continue;

        masternode_info_t infoMn;

        if(!mnodeman.GetMasternodeInfo(dsq.masternodeOutpoint, infoMn)) {
            LogPrintf("CAnonSendClient::JoinExistingQueue -- dsq masternode is not in masternode list, masternode=%s\n", dsq.masternodeOutpoint.ToStringShort());
            continue;
        }

        if(infoMn.nProtocolVersion < MIN_ANONSEND_PEER_PROTO_VERSION) continue;

        // skip next mn payments winners
        if (mnpayments.IsScheduled(infoMn, 0)) {
            LogPrintf("CAnonSendClient::JoinExistingQueue -- skipping winner, masternode=%s\n", infoMn.outpoint.ToStringShort());
            continue;
        }

        std::vector<int> vecBits;
        if(!CAnonSend::GetDenominationsBits(dsq.nDenom, vecBits)) {
            // incompatible denom
            continue;
        }

        // mixing rate limit i.e. nLastDsq check should already pass in DSQUEUE ProcessMessage
        // in order for dsq to get into vecAnonsendQueue, so we should be safe to mix already,
        // no need for additional verification here

        LogPrint("anonsend", "CAnonSendClient::JoinExistingQueue -- found valid queue: %s\n", dsq.ToString());

        CAmount nValueInTmp = 0;
        std::vector<CTxDSIn> vecTxDSInTmp;
        std::vector<COutput> vCoinsTmp;
        CAmount nMinAmount = vecStandardDenoms[vecBits.front()];
        CAmount nMaxAmount = nBalanceNeedsAnonymized;
        // nInputCount is not covered by legacy signature, require SPORK_6_NEW_SIGS to activate to use new algo
        // (to make sure nInputCount wasn't modified by some intermediary node)
        bool fNewAlgo = infoMn.nProtocolVersion > 70208 && sporkManager.IsSporkActive(SPORK_6_NEW_SIGS);

        if (fNewAlgo && dsq.nInputCount != 0) {
            nMinAmount = nMaxAmount = dsq.nInputCount * vecStandardDenoms[vecBits.front()];
        }
        // Try to match their denominations if possible, select exact number of denominations
        if(!pwalletMain->SelectCoinsByDenominations(dsq.nDenom, nMinAmount, nMaxAmount, vecTxDSInTmp, vCoinsTmp, nValueInTmp, 0, nAnonSendRounds)) {
            LogPrintf("CAnonSendClient::JoinExistingQueue -- Couldn't match %d denominations %d %d (%s)\n", dsq.nInputCount, vecBits.front(), dsq.nDenom, CAnonSend::GetDenominationsToString(dsq.nDenom));
            continue;
        }

        vecMasternodesUsed.push_back(dsq.masternodeOutpoint);

        if (connman.IsMasternodeOrDisconnectRequested(infoMn.addr)) {
            LogPrintf("CAnonSendClient::JoinExistingQueue -- skipping masternode connection, addr=%s\n", infoMn.addr.ToString());
            continue;
        }

        nSessionDenom = dsq.nDenom;
        nSessionInputCount = fNewAlgo ? dsq.nInputCount : 0;
        infoMixingMasternode = infoMn;
        pendingDsaRequest = CPendingDsaRequest(infoMn.addr, CAnonsendAccept(nSessionDenom, nSessionInputCount, txMyCollateral));
        connman.AddPendingMasternode(infoMn.addr);
        // TODO: add new state POOL_STATE_CONNECTING and bump MIN_ANONSEND_PEER_PROTO_VERSION
        SetState(POOL_STATE_QUEUE);
        nTimeLastSuccessfulStep = GetTime();
        LogPrintf("CAnonSendClient::JoinExistingQueue -- pending connection (from queue): nSessionDenom: %d (%s), nSessionInputCount: %d, addr=%s\n",
                  nSessionDenom, CAnonSend::GetDenominationsToString(nSessionDenom), nSessionInputCount, infoMn.addr.ToString());
        strAutoDenomResult = _("Trying to connect...");
        return true;
    }
    strAutoDenomResult = _("Failed to find mixing queue to join");
    return false;
}

bool CAnonSendClient::StartNewQueue(CAmount nValueMin, CAmount nBalanceNeedsAnonymized, CConnman& connman)
{
    int nTries = 0;
    int nMnCountEnabled = mnodeman.CountEnabled(MIN_ANONSEND_PEER_PROTO_VERSION);

    // ** find the coins we'll use
    std::vector<CTxIn> vecTxIn;
    CAmount nValueInTmp = 0;
    if(!pwalletMain->SelectCoinsDark(nValueMin, nBalanceNeedsAnonymized, vecTxIn, nValueInTmp, 0, nAnonSendRounds)) {
        // this should never happen
        LogPrintf("CAnonSendClient::StartNewQueue -- Can't mix: no compatible inputs found!\n");
        strAutoDenomResult = _("Can't mix: no compatible inputs found!");
        return false;
    }

    // otherwise, try one randomly
    while(nTries < 10) {
        masternode_info_t infoMn = mnodeman.FindRandomNotInVec(vecMasternodesUsed, MIN_ANONSEND_PEER_PROTO_VERSION);

        if(!infoMn.fInfoValid) {
            LogPrintf("CAnonSendClient::StartNewQueue -- Can't find random masternode!\n");
            strAutoDenomResult = _("Can't find random Masternode.");
            return false;
        }

        // skip next mn payments winners
        if (mnpayments.IsScheduled(infoMn, 0)) {
            LogPrintf("CAnonSendClient::StartNewQueue -- skipping winner, masternode=%s\n", infoMn.outpoint.ToStringShort());
            nTries++;
            continue;
        }

        vecMasternodesUsed.push_back(infoMn.outpoint);

        if(infoMn.nLastDsq != 0 && infoMn.nLastDsq + nMnCountEnabled/5 > mnodeman.nDsqCount) {
            LogPrintf("CAnonSendClient::StartNewQueue -- Too early to mix on this masternode!"
                        " masternode=%s  addr=%s  nLastDsq=%d  CountEnabled/5=%d  nDsqCount=%d\n",
                        infoMn.outpoint.ToStringShort(), infoMn.addr.ToString(), infoMn.nLastDsq,
                        nMnCountEnabled/5, mnodeman.nDsqCount);
            nTries++;
            continue;
        }

        if (connman.IsMasternodeOrDisconnectRequested(infoMn.addr)) {
            LogPrintf("CAnonSendClient::StartNewQueue -- skipping masternode connection, addr=%s\n", infoMn.addr.ToString());
            nTries++;
            continue;
        }

        LogPrintf("CAnonSendClient::StartNewQueue -- attempt %d connection to Masternode %s\n", nTries, infoMn.addr.ToString());

        std::vector<CAmount> vecAmounts;
        pwalletMain->ConvertList(vecTxIn, vecAmounts);
        // try to get a single random denom out of vecAmounts
        while(nSessionDenom == 0) {
            nSessionDenom = CAnonSend::GetDenominationsByAmounts(vecAmounts);
        }

        // Count available denominations.
        // Should never really fail after this point, since we just selected compatible inputs ourselves.
        std::vector<int> vecBits;
        if (!CAnonSend::GetDenominationsBits(nSessionDenom, vecBits)) {
            return false;
        }

        CAmount nValueInTmp = 0;
        std::vector<CTxDSIn> vecTxDSInTmp;
        std::vector<COutput> vCoinsTmp;
        std::vector<CAmount> vecStandardDenoms = CAnonSend::GetStandardDenominations();

        bool fSelected = pwalletMain->SelectCoinsByDenominations(nSessionDenom, vecStandardDenoms[vecBits.front()], vecStandardDenoms[vecBits.front()] * ANONSEND_ENTRY_MAX_SIZE, vecTxDSInTmp, vCoinsTmp, nValueInTmp, 0, nAnonSendRounds);
        if (!fSelected) {
            return false;
        }

        // nInputCount is not covered by legacy signature, require SPORK_6_NEW_SIGS to activate to use new algo
        // (to make sure nInputCount wasn't modified by some intermediary node)
        bool fNewAlgo = infoMn.nProtocolVersion > 70208 && sporkManager.IsSporkActive(SPORK_6_NEW_SIGS);
        nSessionInputCount = fNewAlgo
                ? std::min(vecTxDSInTmp.size(), size_t(5 + GetRand(ANONSEND_ENTRY_MAX_SIZE - 5 + 1)))
                : 0;
        infoMixingMasternode = infoMn;
        connman.AddPendingMasternode(infoMn.addr);
        pendingDsaRequest = CPendingDsaRequest(infoMn.addr, CAnonsendAccept(nSessionDenom, nSessionInputCount, txMyCollateral));
        // TODO: add new state POOL_STATE_CONNECTING and bump MIN_ANONSEND_PEER_PROTO_VERSION
        SetState(POOL_STATE_QUEUE);
        nTimeLastSuccessfulStep = GetTime();
        LogPrintf("CAnonSendClient::StartNewQueue -- pending connection, nSessionDenom: %d (%s), nSessionInputCount: %d, addr=%s\n",
                nSessionDenom, CAnonSend::GetDenominationsToString(nSessionDenom), nSessionInputCount, infoMn.addr.ToString());
        strAutoDenomResult = _("Trying to connect...");
        return true;
    }
    strAutoDenomResult = _("Failed to start a new mixing queue");
    return false;
}

void CAnonSendClient::ProcessPendingDsaRequest(CConnman& connman)
{
    if (!pendingDsaRequest) return;

    bool fDone = connman.ForNode(pendingDsaRequest.GetAddr(), [&](CNode* pnode) {
        LogPrint("anonsend", "-- processing dsa queue for addr=%s\n", pnode->addr.ToString());
        nTimeLastSuccessfulStep = GetTime();
        // TODO: this vvvv should be here after new state POOL_STATE_CONNECTING is added and MIN_ANONSEND_PEER_PROTO_VERSION is bumped
        // SetState(POOL_STATE_QUEUE);
        strAutoDenomResult = _("Mixing in progress...");
        CNetMsgMaker msgMaker(pnode->GetSendVersion());
        connman.PushMessage(pnode, msgMaker.Make(NetMsgType::DSACCEPT, pendingDsaRequest.GetDSA()));
        return true;
    });

    if (fDone) {
        pendingDsaRequest = CPendingDsaRequest();
    } else if (pendingDsaRequest.IsExpired()) {
        LogPrint("anonsend", "CAnonSendClient::%s -- failed to connect to %s\n", __func__, pendingDsaRequest.GetAddr().ToString());
        SetNull();
    }
}

bool CAnonSendClient::SubmitDenominate(CConnman& connman)
{
    std::string strError;
    std::vector<CTxDSIn> vecTxDSInRet;
    std::vector<CTxOut> vecTxOutRet;

    // Submit transaction to the pool if we get here
    if (nLiquidityProvider) {
        // Try to use only inputs with the same number of rounds starting from the lowest number of rounds possible
        for(int i = 0; i< nAnonSendRounds; i++) {
            if(PrepareDenominate(i, i + 1, strError, vecTxDSInRet, vecTxOutRet)) {
                LogPrintf("CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for %d rounds, success\n", i);
                return SendDenominate(vecTxDSInRet, vecTxOutRet, connman);
            }
            LogPrint("anonsend", "CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for %d rounds, error: %s\n", i, strError);
        }
    } else {
        // Try to use only inputs with the same number of rounds starting from the highest number of rounds possible
        for(int i = nAnonSendRounds; i > 0; i--) {
            if(PrepareDenominate(i - 1, i, strError, vecTxDSInRet, vecTxOutRet)) {
                LogPrintf("CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for %d rounds, success\n", i);
                return SendDenominate(vecTxDSInRet, vecTxOutRet, connman);
            }
            LogPrint("anonsend", "CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for %d rounds, error: %s\n", i, strError);
        }
    }

    // We failed? That's strange but let's just make final attempt and try to mix everything
    if(PrepareDenominate(0, nAnonSendRounds, strError, vecTxDSInRet, vecTxOutRet)) {
        LogPrintf("CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for all rounds, success\n");
        return SendDenominate(vecTxDSInRet, vecTxOutRet, connman);
    }

    // Should never actually get here but just in case
    LogPrintf("CAnonSendClient::SubmitDenominate -- Running AnonSend denominate for all rounds, error: %s\n", strError);
    strAutoDenomResult = strError;
    return false;
}

bool CAnonSendClient::PrepareDenominate(int nMinRounds, int nMaxRounds, std::string& strErrorRet, std::vector<CTxDSIn>& vecTxDSInRet, std::vector<CTxOut>& vecTxOutRet)
{
    if(!pwalletMain) {
        strErrorRet = "Wallet is not initialized";
        return false;
    }

    if (pwalletMain->IsLocked(true)) {
        strErrorRet = "Wallet locked, unable to create transaction!";
        return false;
    }

    if (GetEntriesCount() > 0) {
        strErrorRet = "Already have pending entries in the AnonSend pool";
        return false;
    }

    // make sure returning vectors are empty before filling them up
    vecTxDSInRet.clear();
    vecTxOutRet.clear();

    // ** find the coins we'll use
    std::vector<CTxDSIn> vecTxDSIn;
    std::vector<COutput> vCoins;
    CAmount nValueIn = 0;

    /*
        Select the coins we'll use

        if nMinRounds >= 0 it means only denominated inputs are going in and coming out
    */
    std::vector<int> vecBits;
    if (!CAnonSend::GetDenominationsBits(nSessionDenom, vecBits)) {
        strErrorRet = "Incorrect session denom";
        return false;
    }
    std::vector<CAmount> vecStandardDenoms = CAnonSend::GetStandardDenominations();
    bool fSelected = pwalletMain->SelectCoinsByDenominations(nSessionDenom, vecStandardDenoms[vecBits.front()], vecStandardDenoms[vecBits.front()] * ANONSEND_ENTRY_MAX_SIZE, vecTxDSIn, vCoins, nValueIn, nMinRounds, nMaxRounds);
    if (nMinRounds >= 0 && !fSelected) {
        strErrorRet = "Can't select current denominated inputs";
        return false;
    }

    LogPrintf("CAnonSendClient::PrepareDenominate -- max value: %f\n", (double)nValueIn/COIN);

    {
        LOCK(pwalletMain->cs_wallet);
        for (const auto& txin : vecTxDSIn) {
            pwalletMain->LockCoin(txin.prevout);
        }
    }

    CAmount nValueLeft = nValueIn;

    // Try to add every needed denomination, repeat up to 5-ANONSEND_ENTRY_MAX_SIZE times.
    // NOTE: No need to randomize order of inputs because they were
    // initially shuffled in CWallet::SelectCoinsByDenominations already.
    int nStep = 0;
    int nStepsMax = nSessionInputCount != 0 ? nSessionInputCount : (5 + GetRandInt(ANONSEND_ENTRY_MAX_SIZE - 5 + 1));

    while (nStep < nStepsMax) {
        for (const auto& nBit : vecBits) {
            CAmount nValueDenom = vecStandardDenoms[nBit];
            if (nValueLeft - nValueDenom < 0) continue;

            // Note: this relies on a fact that both vectors MUST have same size
            std::vector<CTxDSIn>::iterator it = vecTxDSIn.begin();
            std::vector<COutput>::iterator it2 = vCoins.begin();
            while (it2 != vCoins.end()) {
                // we have matching inputs
                if ((*it2).tx->tx->vout[(*it2).i].nValue == nValueDenom) {
                    // add new input in resulting vector
                    vecTxDSInRet.push_back(*it);
                    // remove corresponding items from initial vectors
                    vecTxDSIn.erase(it);
                    vCoins.erase(it2);

                    CScript scriptDenom = keyHolderStorage.AddKey(pwalletMain);

                    // add new output
                    CTxOut txout(nValueDenom, scriptDenom);
                    vecTxOutRet.push_back(txout);

                    // subtract denomination amount
                    nValueLeft -= nValueDenom;

                    // step is complete
                    break;
                }
                ++it;
                ++it2;
            }
        }
        nStep++;
        if(nValueLeft == 0) break;
    }

    {
        // unlock unused coins
        LOCK(pwalletMain->cs_wallet);
        for (const auto& txin : vecTxDSIn) {
            pwalletMain->UnlockCoin(txin.prevout);
        }
    }

    if (CAnonSend::GetDenominations(vecTxOutRet) != nSessionDenom || (nSessionInputCount != 0 && vecTxOutRet.size() != nSessionInputCount)) {
        {
            // unlock used coins on failure
            LOCK(pwalletMain->cs_wallet);
            for (const auto& txin : vecTxDSInRet) {
                pwalletMain->UnlockCoin(txin.prevout);
            }
        }
        keyHolderStorage.ReturnAll();
        strErrorRet = "Can't make current denominated outputs";
        return false;
    }

    // We also do not care about full amount as long as we have right denominations
    return true;
}

// Create collaterals by looping through inputs grouped by addresses
bool CAnonSendClient::MakeCollateralAmounts(CConnman& connman)
{
    std::vector<CompactTallyItem> vecTally;
    if(!pwalletMain->SelectCoinsGrouppedByAddresses(vecTally, false)) {
        LogPrint("anonsend", "CAnonSendClient::MakeCollateralAmounts -- SelectCoinsGrouppedByAddresses can't find any inputs!\n");
        return false;
    }

    // First try to use only non-denominated funds
    for (const auto& item : vecTally) {
        if(!MakeCollateralAmounts(item, false, connman)) continue;
        return true;
    }

    // There should be at least some denominated funds we should be able to break in pieces to continue mixing
    for (const auto& item : vecTally) {
        if(!MakeCollateralAmounts(item, true, connman)) continue;
        return true;
    }

    // If we got here then smth is terribly broken actually
    LogPrintf("CAnonSendClient::MakeCollateralAmounts -- ERROR: Can't make collaterals!\n");
    return false;
}

// Split up large inputs or create fee sized inputs
bool CAnonSendClient::MakeCollateralAmounts(const CompactTallyItem& tallyItem, bool fTryDenominated, CConnman& connman)
{
    LOCK2(cs_main, pwalletMain->cs_wallet);

    // denominated input is always a single one, so we can check its amount directly and return early
    if(!fTryDenominated && tallyItem.vecOutPoints.size() == 1 && CAnonSend::IsDenominatedAmount(tallyItem.nAmount))
        return false;

    CWalletTx wtx;
    CAmount nFeeRet = 0;
    int nChangePosRet = -1;
    std::string strFail = "";
    std::vector<CRecipient> vecSend;

    // make our collateral address
    CReserveKey reservekeyCollateral(pwalletMain);
    // make our change address
    CReserveKey reservekeyChange(pwalletMain);

    CScript scriptCollateral;
    CPubKey vchPubKey;
    assert(reservekeyCollateral.GetReservedKey(vchPubKey, false)); // should never fail, as we just unlocked
    scriptCollateral = GetScriptForDestination(vchPubKey.GetID());

    vecSend.push_back((CRecipient){scriptCollateral, CAnonSend::GetMaxCollateralAmount(), false});

    // try to use non-denominated and not mn-like funds first, select them explicitly
    CCoinControl coinControl;
    coinControl.fAllowOtherInputs = false;
    coinControl.fAllowWatchOnly = false;
    // send change to the same address so that we were able create more denoms out of it later
    coinControl.destChange = tallyItem.txdest;
    for (const auto& outpoint : tallyItem.vecOutPoints)
        coinControl.Select(outpoint);

    bool fSuccess = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
            nFeeRet, nChangePosRet, strFail, &coinControl, true, ONLY_NONDENOMINATED);
    if(!fSuccess) {
        LogPrintf("CAnonSendClient::MakeCollateralAmounts -- ONLY_NONDENOMINATED: %s\n", strFail);
        // If we failed then most likely there are not enough funds on this address.
        if(fTryDenominated) {
            // Try to also use denominated coins (we can't mix denominated without collaterals anyway).
            if(!pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
                                nFeeRet, nChangePosRet, strFail, &coinControl, true, ALL_COINS)) {
                LogPrintf("CAnonSendClient::MakeCollateralAmounts -- ALL_COINS Error: %s\n", strFail);
                reservekeyCollateral.ReturnKey();
                return false;
            }
        } else {
            // Nothing else we can do.
            reservekeyCollateral.ReturnKey();
            return false;
        }
    }

    reservekeyCollateral.KeepKey();

    LogPrintf("CAnonSendClient::MakeCollateralAmounts -- txid=%s\n", wtx.GetHash().GetHex());

    // use the same nCachedLastSuccessBlock as for DS mixing to prevent race
    CValidationState state;
    if(!pwalletMain->CommitTransaction(wtx, reservekeyChange, &connman, state)) {
        LogPrintf("CAnonSendClient::MakeCollateralAmounts -- CommitTransaction failed! Reason given: %s\n", state.GetRejectReason());
        return false;
    }

    nCachedLastSuccessBlock = nCachedBlockHeight;

    return true;
}

// Create denominations by looping through inputs grouped by addresses
bool CAnonSendClient::CreateDenominated(CConnman& connman)
{
    LOCK2(cs_main, pwalletMain->cs_wallet);

    std::vector<CompactTallyItem> vecTally;
    if(!pwalletMain->SelectCoinsGrouppedByAddresses(vecTally)) {
        LogPrint("anonsend", "CAnonSendClient::CreateDenominated -- SelectCoinsGrouppedByAddresses can't find any inputs!\n");
        return false;
    }

    bool fCreateMixingCollaterals = !pwalletMain->HasCollateralInputs();

    for (const auto& item : vecTally) {
        if(!CreateDenominated(item, fCreateMixingCollaterals, connman)) continue;
        return true;
    }

    LogPrintf("CAnonSendClient::CreateDenominated -- failed!\n");
    return false;
}

// Create denominations
bool CAnonSendClient::CreateDenominated(const CompactTallyItem& tallyItem, bool fCreateMixingCollaterals, CConnman& connman)
{
    std::vector<CRecipient> vecSend;
    CKeyHolderStorage keyHolderStorageDenom;

    CAmount nValueLeft = tallyItem.nAmount;
    nValueLeft -= CAnonSend::GetCollateralAmount(); // leave some room for fees

    LogPrintf("CreateDenominated0: %s nValueLeft: %f\n", CBitcoinAddress(tallyItem.txdest).ToString(), (float)nValueLeft/COIN);

    // ****** Add an output for mixing collaterals ************ /

    if(fCreateMixingCollaterals) {
        CScript scriptCollateral = keyHolderStorageDenom.AddKey(pwalletMain);
        vecSend.push_back((CRecipient){ scriptCollateral, CAnonSend::GetMaxCollateralAmount(), false });
        nValueLeft -= CAnonSend::GetMaxCollateralAmount();
    }

    // ****** Add outputs for denoms ************ /

    // try few times - skipping smallest denoms first if there are too many of them already, if failed - use them too
    int nOutputsTotal = 0;
    bool fSkip = true;
    do {
        std::vector<CAmount> vecStandardDenoms = CAnonSend::GetStandardDenominations();

        BOOST_REVERSE_FOREACH(CAmount nDenomValue, vecStandardDenoms) {

            if(fSkip) {
                // Note: denoms are skipped if there are already DENOMS_COUNT_MAX of them
                // and there are still larger denoms which can be used for mixing

                // check skipped denoms
                if(IsDenomSkipped(nDenomValue)) continue;

                // find new denoms to skip if any (ignore the largest one)
                if(nDenomValue != vecStandardDenoms.front() && pwalletMain->CountInputsWithAmount(nDenomValue) > DENOMS_COUNT_MAX) {
                    strAutoDenomResult = strprintf(_("Too many %f denominations, removing."), (float)nDenomValue/COIN);
                    LogPrintf("CAnonSendClient::CreateDenominated -- %s\n", strAutoDenomResult);
                    vecDenominationsSkipped.push_back(nDenomValue);
                    continue;
                }
            }

            int nOutputs = 0;

            // add each output up to 11 times until it can't be added again
            while(nValueLeft - nDenomValue >= 0 && nOutputs <= 10) {
                CScript scriptDenom = keyHolderStorageDenom.AddKey(pwalletMain);

                vecSend.push_back((CRecipient){ scriptDenom, nDenomValue, false });

                //increment outputs and subtract denomination amount
                nOutputs++;
                nValueLeft -= nDenomValue;
                LogPrintf("CreateDenominated1: totalOutputs: %d, nOutputsTotal: %d, nOutputs: %d, nValueLeft: %f\n", nOutputsTotal + nOutputs, nOutputsTotal, nOutputs, (float)nValueLeft/COIN);
            }

            nOutputsTotal += nOutputs;
            if(nValueLeft == 0) break;
        }
        LogPrintf("CreateDenominated2: nOutputsTotal: %d, nValueLeft: %f\n", nOutputsTotal, (float)nValueLeft/COIN);
        // if there were no outputs added, start over without skipping
        fSkip = !fSkip;
    } while (nOutputsTotal == 0 && !fSkip);
    LogPrintf("CreateDenominated3: nOutputsTotal: %d, nValueLeft: %f\n", nOutputsTotal, (float)nValueLeft/COIN);

    // if we have anything left over, it will be automatically send back as change - there is no need to send it manually

    CCoinControl coinControl;
    coinControl.fAllowOtherInputs = false;
    coinControl.fAllowWatchOnly = false;
    // send change to the same address so that we were able create more denoms out of it later
    coinControl.destChange = tallyItem.txdest;
    for (const auto& outpoint : tallyItem.vecOutPoints)
        coinControl.Select(outpoint);

    CWalletTx wtx;
    CAmount nFeeRet = 0;
    int nChangePosRet = -1;
    std::string strFail = "";
    // make our change address
    CReserveKey reservekeyChange(pwalletMain);

    bool fSuccess = pwalletMain->CreateTransaction(vecSend, wtx, reservekeyChange,
            nFeeRet, nChangePosRet, strFail, &coinControl, true, ONLY_NONDENOMINATED);
    if(!fSuccess) {
        LogPrintf("CAnonSendClient::CreateDenominated -- Error: %s\n", strFail);
        keyHolderStorageDenom.ReturnAll();
        return false;
    }

    keyHolderStorageDenom.KeepAll();

    CValidationState state;
    if(!pwalletMain->CommitTransaction(wtx, reservekeyChange, &connman, state)) {
        LogPrintf("CAnonSendClient::CreateDenominated -- CommitTransaction failed! Reason given: %s\n", state.GetRejectReason());
        return false;
    }

    // use the same nCachedLastSuccessBlock as for DS mixing to prevent race
    nCachedLastSuccessBlock = nCachedBlockHeight;
    LogPrintf("CAnonSendClient::CreateDenominated -- txid=%s\n", wtx.GetHash().GetHex());

    return true;
}

void CAnonSendClient::RelayIn(const CDarkSendEntry& entry, CConnman& connman)
{
    if(!infoMixingMasternode.fInfoValid) return;

    connman.ForNode(infoMixingMasternode.addr, [&entry, &connman](CNode* pnode) {
        LogPrintf("CAnonSendClient::RelayIn -- found master, relaying message to %s\n", pnode->addr.ToString());
        CNetMsgMaker msgMaker(pnode->GetSendVersion());
        connman.PushMessage(pnode, msgMaker.Make(NetMsgType::DSVIN, entry));
        return true;
    });
}

void CAnonSendClient::SetState(PoolState nStateNew)
{
    LogPrintf("CAnonSendClient::SetState -- nState: %d, nStateNew: %d\n", nState, nStateNew);
    nState = nStateNew;
}

void CAnonSendClient::UpdatedBlockTip(const CBlockIndex *pindex)
{
    nCachedBlockHeight = pindex->nHeight;
    LogPrint("anonsend", "CAnonSendClient::UpdatedBlockTip -- nCachedBlockHeight: %d\n", nCachedBlockHeight);

}

//TODO: Rename/move to core
void ThreadCheckAnonSendClient(CConnman& connman)
{
    if(fLiteMode) return; // disable all MarteX specific functionality
    if(fMasternodeMode) return; // no client-side mixing on masternodes

    static bool fOneThread;
    if(fOneThread) return;
    fOneThread = true;

    // Make this thread recognisable as the AnonSend thread
    RenameThread("martex-ps-client");

    unsigned int nTick = 0;
    unsigned int nDoAutoNextRun = nTick + ANONSEND_AUTO_TIMEOUT_MIN;

    while (true)
    {
        MilliSleep(1000);

        if(masternodeSync.IsBlockchainSynced() && !ShutdownRequested()) {
            nTick++;
            anonSendClient.CheckTimeout();
            anonSendClient.ProcessPendingDsaRequest(connman);
            if(nDoAutoNextRun == nTick) {
                anonSendClient.DoAutomaticDenominating(connman);
                nDoAutoNextRun = nTick + ANONSEND_AUTO_TIMEOUT_MIN + GetRandInt(ANONSEND_AUTO_TIMEOUT_MAX - ANONSEND_AUTO_TIMEOUT_MIN);
            }
        }
    }
}
