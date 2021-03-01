// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2019 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "messagesigner.h"
#include "anonsend-relay.h"

CAnonSendRelay::CAnonSendRelay() :
        CSignedMessage(),
        vchSig2(),
        vinMasternode(),
        nBlockHeight(0),
        nRelayType(0),
        in(),
        out()
{ }

CAnonSendRelay::CAnonSendRelay(CTxIn& vinMasternodeIn,
                                     std::vector<unsigned char>& vchSigIn,
                                     int nBlockHeightIn,
                                     int nRelayTypeIn,
                                     CTxIn& in2,
                                     CTxOut& out2):
        CSignedMessage(),
        vchSig2(),
        vinMasternode(vinMasternodeIn),
        nBlockHeight(nBlockHeightIn),
        nRelayType(nRelayTypeIn),
        in(in2),
        out(out2)
{
    SetVchSig(vchSigIn);
}

std::string CAnonSendRelay::ToString()
{
    std::ostringstream info;

    info << "vin: " << vinMasternode.ToString() << " nBlockHeight: " << (int)nBlockHeight << " nRelayType: " << (int)nRelayType << " in " << in.ToString() << " out " << out.ToString();

    return info.str();
}

uint256 CAnonSendRelay::GetSignatureHash() const
{
    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << nMessVersion << in << out;
    return ss.GetHash();
}

std::string CAnonSendRelay::GetStrMessage() const
{
    return in.ToString() + out.ToString();
}

void CAnonSendRelay::Relay()
{
    int nCount = std::min(mnodeman.CountEnabled(ActiveProtocol()), 20);
    int nRank1 = (rand() % nCount) + 1;
    int nRank2 = (rand() % nCount) + 1;

    //keep picking another second number till we get one that doesn't match
    while (nRank1 == nRank2)
        nRank2 = (rand() % nCount) + 1;

    //printf("rank 1 - rank2 %d %d \n", nRank1, nRank2);

    //relay this message through 2 separate nodes for redundancy
    RelayThroughNode(nRank1);
    RelayThroughNode(nRank2);
}

void CAnonSendRelay::RelayThroughNode(int nRank)
{
    CMasternode* pmn = mnodeman.GetMasternodeByRank(nRank, nBlockHeight, ActiveProtocol());

    if (pmn != NULL) {
        //printf("RelayThroughNode %s\n", pmn->addr.ToString().c_str());
        CNode* pnode = ConnectNode((CAddress)pmn->addr, NULL, false, true);
        if (pnode) {
            //printf("Connected\n");
            pnode->PushMessage("dsr", (*this));
            pnode->Release();
            return;
        }
    } else {
        //printf("RelayThroughNode NULL\n");
    }
}
