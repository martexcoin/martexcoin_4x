// Copyright (c) 2019-2019 The MarteXcoin Team
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "rpcserver.h"
#include "init.h"
#include "net.h"
#include "netbase.h"
#include "util.h"
#include "wallet.h"
#include "walletdb.h"
#include "main.h"
#include "tokendb.h"
#include <boost/assign/list_of.hpp>
#include "json/json_spirit_utils.h"
#include "json/json_spirit_value.h"
#include <ctime>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>


using namespace std;
using namespace boost;
using namespace boost::assign;
using namespace json_spirit;


/* createtoken(const Array& params, bool fHelp) : Object
   Create Token via RPC */
/* sendtoken(const Array& params, bool fHelp) : Object
   Send Tokens via RPC */

Value createtoken(const Array& params, bool fHelp) {
    int64_t nStart = GetTimeMillis();
    if (fHelp || params.size() != 5 || params.size() > 5)
        throw runtime_error(
            "createtoken nametoken symbol units fee\n"
            "address = Address associated of Token\n"
            "nametoken = Name of Token\n"
            "symbol = Symbol of Token\n"
            "units = Units of Tokeen\n"
            "fee = Fee of Token\n"
            "Returns new token informations.");

    //int TokenID = rand()/(float)RAND_MAX * (9999999999-1000000000) + 1000000000;

    CMarteXAddress address(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid MarteX address");

    string strTK_NAME = params[1].get_str();
    string strTK_SYMBOL = params[2].get_str();
    string strTK_UNIT = params[3].get_str();
    string strTK_FEE = params[4].get_str();

    std::time_t tTokenTimeStamp = std::time(nullptr);

    uint256 TokenID = GetRandHash();

    // Parse the account first so we don't generate a key if there's an error
    string strAccount;
    string strTokenID;

    if (!pwalletMain->IsLocked())
        pwalletMain->TopUpKeyPool();

    CPubKey newKey;
    if (!pwalletMain->GetKeyFromPool(newKey))
        throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, "Error: Keypool ran out, please call keypoolrefill first");
    CKeyID keyID = newKey.GetID();
    pwalletMain->SetAddressBookName(keyID, strAccount);
    strTokenID = HexStr(newKey.begin(), newKey.end());

    Object ret;
    Object TK_HEAD;
    //TK_HEAD.push_back(Pair("ID", strTokenID ));
    TK_HEAD.push_back(Pair("ID", TokenID.GetHex() ));
    TK_HEAD.push_back(Pair("Name", strTK_NAME ));
    TK_HEAD.push_back(Pair("Symbol", strTK_SYMBOL ));

    Object TK_CON;
    TK_CON.push_back(Pair("Units", strTK_UNIT ));
    TK_CON.push_back(Pair("Fee", strTK_FEE ));
    TK_CON.push_back(Pair("TimeStamp", tTokenTimeStamp ));
    ret.push_back(Pair("Head", TK_HEAD));
    ret.push_back(Pair("Content", TK_CON));

    //grava tokens.dat inicio
    

    TokenDB adb;
    //adb.Write(TokenID.GetHex(),strTK_NAME,strTK_SYMBOL,strTK_UNIT,strTK_FEE,tTokenTimeStamp);
    //adb.Write(tTokenTimeStamp);
    adb.Write(string("TokenIndex"), strTK_NAME);
    //adb.Write((string("TokenIndex")), TokenID.GetHex());
    //adb.Write(make_pair(string("TokenIndex"), TokenID.GetHex()), TokenID.GetHex());
    
    
    //const key = "id";
    //const value = "value";
    
    //adb.Write(strTK_SYMBOL, strTK_NAME);
    //adb.Write(make_pair(string("token"), ret), strTK_SYMBOL);
    LogPrintf("Writing token id %d to token LevelDB  %dms\n", TokenID.GetHex(), GetTimeMillis() - nStart);

    //if (!adb.Read(tTokenTimeStamp))
           //LogPrintf("Invalid or missing tokens.dat; recreating\n");
           //return false;

    //LogPrintf("Loaded %s tokens from tokens.dat  %dms\n", adb.Read(tTokenTimeStamp), GetTimeMillis() - nStart);

    //adb.Write(tTokenTimeStamp);
    //LogPrintf("Flushed token id %d addresses to tokens.dat  %dms\n", TokenID.GetHex(), GetTimeMillis() - nStart);
    //grava tokens.dat fim

    return ret;

}

Value sendtoken(const Array& params, bool fHelp) {

    if (fHelp || params.size() != 0)
        throw runtime_error(
            "sendtoken \n"
            "Returns transaction id to transfer token.");

    Array ret;
    Object obj;
    obj.push_back(Pair("New PubKey", getnewpubkey ));
    ret.push_back(obj);
    return ret;
}
