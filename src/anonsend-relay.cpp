
#include "anonsend-relay.h"


CAnonSendRelay::CAnonSendRelay()
{
    vinMasternode = CTxIn();
    nBlockHeight = 0;
    nRelayType = 0;
    in = CTxIn();
    out = CTxOut();
}

CAnonSendRelay::CAnonSendRelay(CTxIn& vinMasternodeIn, vector<unsigned char>& vchSigIn, int nBlockHeightIn, int nRelayTypeIn, CTxIn& in2, CTxOut& out2)
{
    vinMasternode = vinMasternodeIn;
    vchSig = vchSigIn;
    nBlockHeight = nBlockHeightIn;
    nRelayType = nRelayTypeIn;
    in = in2;
    out = out2;
}

std::string CAnonSendRelay::ToString()
{
    std::ostringstream info;

    info << "vin: " << vinMasternode.ToString() <<
        " nBlockHeight: " << (int)nBlockHeight <<
        " nRelayType: "  << (int)nRelayType <<
        " in " << in.ToString() <<
        " out " << out.ToString();
        
    return info.str();   
}

bool CAnonSendRelay::Sign(std::string strSharedKey)
{
    std::string strMessage = in.ToString() + out.ToString();

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if(!anonSendSigner.SetKey(strSharedKey, errorMessage, key2, pubkey2))
    {
        LogPrintf("CAnonSendRelay():Sign - ERROR: Invalid shared key: '%s'\n", errorMessage.c_str());
        return false;
    }

    if(!anonSendSigner.SignMessage(strMessage, errorMessage, vchSig2, key2)) {
        LogPrintf("CAnonSendRelay():Sign - Sign message failed\n");
        return false;
    }

    if(!anonSendSigner.VerifyMessage(pubkey2, vchSig2, strMessage, errorMessage)) {
        LogPrintf("CAnonSendRelay():Sign - Verify message failed\n");
        return false;
    }

    return true;
}

bool CAnonSendRelay::VerifyMessage(std::string strSharedKey)
{
    std::string strMessage = in.ToString() + out.ToString();

    CKey key2;
    CPubKey pubkey2;
    std::string errorMessage = "";

    if(!anonSendSigner.SetKey(strSharedKey, errorMessage, key2, pubkey2))
    {
        LogPrintf("CAnonSendRelay()::VerifyMessage - ERROR: Invalid shared key: '%s'\n", errorMessage.c_str());
        return false;
    }

    if(!anonSendSigner.VerifyMessage(pubkey2, vchSig2, strMessage, errorMessage)) {
        LogPrintf("CAnonSendRelay()::VerifyMessage - Verify message failed\n");
        return false;
    }

    return true;
}

void CAnonSendRelay::Relay()
{
    int nCount = std::min(mnodeman.CountEnabled(), 20);
    int nRank1 = (rand() % nCount)+1; 
    int nRank2 = (rand() % nCount)+1; 

    //keep picking another second number till we get one that doesn't match
    while(nRank1 == nRank2) nRank2 = (rand() % nCount)+1;

    //printf("rank 1 - rank2 %d %d \n", nRank1, nRank2);

    //relay this message through 2 separate nodes for redundancy
    RelayThroughNode(nRank1);
    RelayThroughNode(nRank2);
}

void CAnonSendRelay::RelayThroughNode(int nRank)
{
    CMasternode* pmn = mnodeman.GetMasternodeByRank(nRank, nBlockHeight, MIN_POOL_PEER_PROTO_VERSION);

    if(pmn != NULL){
        //printf("RelayThroughNode %s\n", pmn->addr.ToString().c_str());
        if(ConnectNode((CAddress)pmn->addr, NULL, true)){
            //printf("Connected\n");
            CNode* pNode = FindNode(pmn->addr);
            if(pNode)
            {
                //printf("Found\n");
                pNode->PushMessage("dsr", (*this));
                return;
            }
        }
    } else {
        //printf("RelayThroughNode NULL\n");
    }
}
