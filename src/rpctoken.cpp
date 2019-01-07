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

Value createtoken(const Array& params, bool fHelp) 
{
    
    int64_t nStart = GetTimeMillis();
 
    LogPrintf(" > Create Token Init \n");
    
    if (fHelp || params.size() != 5 || params.size() > 5)
        throw runtime_error(
            "createtoken nametoken symbol units fee\n"
            "address = Address associated of Token\n"
            "nametoken = Name of Token\n"
            "symbol = Symbol of Token\n"
            "units = Units of Tokeen\n"
            "fee = Fee of Token\n"
            "Returns new token informations.");

    CMarteXAddress address(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid MarteX address");

    string strTK_NAME = params[1].get_str();
    string strTK_SYMBOL = params[2].get_str();
    string strTK_UNIT = params[3].get_str();
    string strTK_FEE = params[4].get_str();

    std::time_t tTokenTimeStamp = std::time(nullptr);

    std::stringstream ss;
    ss << tTokenTimeStamp;
    std::string strTokenTimeStamp = ss.str();


    uint256 TokenID = GetRandHash();

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
    Object TK_CON;
    Object OP_RETURN;
    
    TokenDB tdb;
    
    std::string hexKey = "";
    std::string hexValue = "";
    std::string valueArr = "" ;
    
     valueArr.append("ID:");
     valueArr.append(strTokenID);
     valueArr.append(",NAME:");
     valueArr.append(strTK_NAME);
     valueArr.append(",SYMBOL:");
     valueArr.append(strTK_SYMBOL);
     valueArr.append(",UNITS:");
     valueArr.append(strTK_UNIT);
     valueArr.append(",FEE:");
     valueArr.append(strTK_FEE);
     valueArr.append(",TIMESTAMP:");
     valueArr.append(strTokenTimeStamp);
    
    hexKey = HexStr(strTK_SYMBOL.begin(), strTK_SYMBOL.end());
    
    hexValue = HexStr(valueArr.begin(), valueArr.end());

    if ( tdb.Write(hexKey.c_str(), hexValue.c_str()) )
    {
        
       ////



 ///// NAo mexer estou fazendo propagacao.
  /*

         std::string tokenOpReturn;
        
        tokenOpReturn.append("544b4e");
        tokenOpReturn.append(hexValue.c_str());
        
        OP_RETURN.push_back(Pair(0, "")); 
        OP_RETURN.push_back(Pair(1, "{'miWZSvnyQSzw8B4WyAFfiprghpjePCVsY4':0.00015}"));
        OP_RETURN.push_back(Pair(2, "5"));
        OP_RETURN.push_back(Pair(3, "544b4e"));


        string strAccount = AccountFromValue(params[0]);
        Object sendTo = params[1].get_obj();
        int nMinDepth = 1;
        if (params.size() > 2)
                nMinDepth = params[2].get_int();

        CWalletTx wtx;
        wtx.strFromAccount = strAccount;

        set<CMarteXAddress> setAddress;
        vector<pair<CScript, int64_t> > vecSend;

        if (params.size() > 3 && params[3].type() != null_type && !params[3].get_str().empty())
        {
                wtx.mapValue["comment"] = params[3].get_str();
                
                std::vector<unsigned char> data = ParseHexV(params[3].get_str(),"Data");
	CScript scriptN = CScript() << OP_RETURN << data;
                vecSend.push_back(make_pair(scriptN, 0));
        }

        int64_t totalAmount = 0;
        BOOST_FOREACH(const Pair& s, sendTo)
        {
                    CMarteXAddress address(s.name_);
                    if (!address.IsValid())
                            throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, string("Invalid MarteX address: ")+s.name_);

                    if (setAddress.count(address))
                            throw JSONRPCError(RPC_INVALID_PARAMETER, string("Invalid parameter, duplicated address: ")+s.name_);
                    setAddress.insert(address);

                    CScript scriptPubKey;
                    scriptPubKey.SetDestination(address.Get());
                    CAmount nAmount = AmountFromValue(s.value_);
                    totalAmount += nAmount;
                    vecSend.push_back(make_pair(scriptPubKey, nAmount));
        }

        EnsureWalletIsUnlocked();

        // Check funds
        int64_t nBalance = GetAccountBalance(strAccount, nMinDepth, ISMINE_SPENDABLE);
        if (totalAmount > nBalance)
                throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Account has insufficient funds");

        // Send
        CReserveKey keyChange(pwalletMain);
        int64_t nFeeRequired = 0;
        int nChangePos;
        std::string strFailReason;
        bool fCreated = pwalletMain->CreateTransaction(vecSend, wtx, keyChange, nFeeRequired, nChangePos, strFailReason);
        if (!fCreated)
        {
                if (totalAmount + nFeeRequired > pwalletMain->GetBalance())
                        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");
                throw JSONRPCError(RPC_WALLET_ERROR, "Transaction creation failed");
        }
        if (!pwalletMain->CommitTransaction(wtx, keyChange))
                throw JSONRPCError(RPC_WALLET_ERROR, "Transaction commit failed");

        return wtx.GetHash().GetHex();
        }
        */
////
        
        //sendmany "" '{"miWZSvnyQSzw8B4WyAFfiprghpjePCVsY4":0.00015}' 5 "546573746520544f4b454e204d5854"
        
        TK_HEAD.push_back(Pair("Status", "CREATE" ));
        TK_HEAD.push_back(Pair("ID", TokenID.GetHex() ));
        TK_HEAD.push_back(Pair("Name", strTK_NAME ));
        TK_HEAD.push_back(Pair("Symbol", strTK_SYMBOL ));

        TK_CON.push_back(Pair("Units", strTK_UNIT ));
        TK_CON.push_back(Pair("Fee", strTK_FEE ));
        TK_CON.push_back(Pair("TimeStamp", tTokenTimeStamp ));
        
        ret.push_back(Pair("Head", TK_HEAD));
        
        ret.push_back(Pair("Content", TK_CON));
    }
    else
    {
        TK_HEAD.push_back(Pair("Status", "ERROR" ));
        TK_HEAD.push_back(Pair("err", "Token ( " +  strTK_SYMBOL + " ) Existe."));
        
        ret.push_back(Pair("Head", TK_HEAD));
    }

    LogPrintf(" > Create Token End ( %dms ) \n", GetTimeMillis() - nStart);

    return ret;

}

Value searchtoken(const Array& params, bool fHelp) 
{

    if (fHelp || params.size() != 1)
        throw runtime_error(
            "searchtoken <tknsymbol>\n"
            "Returns details token symbol.");

    TokenDB tdb;
    
    Object ret;
    
    std::string key = params[0].get_str();
    
    std::string hexKey = HexStr(key.begin(), key.end());
    
    LogPrintf("Search token SYMBOL %s \n", key);
    
    std::string value = tdb.Search(hexKey.c_str());
    
    LogPrintf("Retorno HEX token SYMBOL %s \n", value.c_str()); 
    
    std::string strHex(value.c_str());
    std::string res;
    res.reserve(strHex.size() / 2);
    
    for (int i = 0; i < strHex.size(); i += 2)
    {
        std::istringstream iss(strHex.substr(i, 2));
        int cr;
        iss >> std::hex >> cr;
        res += static_cast<char>(cr);
    }

    std::string s = res;
    std::string delimiter = ",";
    std::string delimiter2 = ":";
    size_t pos = 0;
    std::string param;
    while ((pos = s.find(delimiter)) != std::string::npos) 
    {
        param = s.substr(0, pos);
        
        size_t pos2 = param.find(delimiter2);
        
        ret.push_back(Pair(param.substr(0, pos2), param.substr(pos2+1, param.size()) ) );

        s.erase(0, pos + delimiter.length());
    }
    //ret = res;
    
    return ret;
}
