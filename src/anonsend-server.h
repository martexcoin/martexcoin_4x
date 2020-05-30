// Copyright (c) 2014-2020 The MarteX Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ANONSENDSERVER_H
#define ANONSENDSERVER_H

#include "net.h"
#include "anonsend.h"

class CAnonSendServer;

// The main object for accessing mixing
extern CAnonSendServer anonSendServer;

/** Used to keep track of current status of mixing pool
 */
class CAnonSendServer : public CAnonSendBase
{
private:
    // Mixing uses collateral transactions to trust parties entering the pool
    // to behave honestly. If they don't it takes their money.
    std::vector<CTransactionRef> vecSessionCollaterals;

    bool fUnitTest;

    /// Add a clients entry to the pool
    bool AddEntry(const CAnonSendEntry& entryNew, PoolMessage& nMessageIDRet);
    /// Add signature to a txin
    bool AddScriptSig(const CTxIn& txin);

    /// Charge fees to bad actors (Charge clients a fee if they're abusive)
    void ChargeFees(CConnman& connman);
    /// Rarely charge fees to pay miners
    void ChargeRandomFees(CConnman& connman);

    /// Check for process
    void CheckPool(CConnman& connman);

    void CreateFinalTransaction(CConnman& connman);
    void CommitFinalTransaction(CConnman& connman);

    /// Is this nDenom and txCollateral acceptable?
    bool IsAcceptableDSA(const CAnonsendAccept& dsa, PoolMessage &nMessageIDRet);
    bool CreateNewSession(const CAnonsendAccept& dsa, PoolMessage &nMessageIDRet, CConnman& connman);
    bool AddUserToExistingSession(const CAnonsendAccept& dsa, PoolMessage &nMessageIDRet);
    /// Do we have enough users to take entries?
    bool IsSessionReady() { return (int)vecSessionCollaterals.size() >= CAnonSend::GetMaxPoolTransactions(); }

    /// Check that all inputs are signed. (Are all inputs signed?)
    bool IsSignaturesComplete();
    /// Check to make sure a given input matches an input in the pool and its scriptSig is valid
    bool IsInputScriptSigValid(const CTxIn& txin);
    /// Are these outputs compatible with other client in the pool?
    bool IsOutputsCompatibleWithSessionDenom(const std::vector<CTxOut>& vecTxOut);

    // Set the 'state' value, with some logging and capturing when the state changed
    void SetState(PoolState nStateNew);

    /// Relay mixing Messages
    void RelayFinalTransaction(const CTransaction& txFinal, CConnman& connman);
    void PushStatus(CNode* pnode, PoolStatusUpdate nStatusUpdate, PoolMessage nMessageID, CConnman& connman);
    void RelayStatus(PoolStatusUpdate nStatusUpdate, CConnman& connman, PoolMessage nMessageID = MSG_NOERR);
    void RelayCompletedTransaction(PoolMessage nMessageID, CConnman& connman);

    void SetNull();

public:
    CAnonSendServer() :
        fUnitTest(false) { SetNull(); }

    void ProcessMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman);

    void CheckTimeout(CConnman& connman);
    void CheckForCompleteQueue(CConnman& connman);
};

void ThreadCheckAnonSendServer(CConnman& connman);

#endif
