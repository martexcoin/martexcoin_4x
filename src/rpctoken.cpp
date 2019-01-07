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
//#include <charconv>


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

    //int TokenID = rand()/(float)RAND_MAX * (9999999999-1000000000) + 1000000000;

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
    Object TK_CON;
    
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
    
    //unsigned int hexValue;

    //std::stringstream valueToString;
    
    //std::string valueToString;
    
    //valueToString << std::hex << value.c_str();
    
    //valueToString >> value; takalepau
    
    //valueToString = strtoul(value.c_str(), NULL, 16);    
    
    //valueToString >> hexValue;

    std::string str(value.c_str());
    std::string res;
    res.reserve(str.size() / 2);
    for (int i = 0; i < str.size(); i += 2)
    {
        std::istringstream iss(str.substr(i, 2));
        int temp;
        iss >> std::hex >> temp;
        res += static_cast<char>(temp);
    }
    std::cout << res;
    ret.push_back(Pair("Response", res ));
    //conpila pra v c vai
    return ret;
}


/*
template<typename T>
std::vector<T> hexstr_to_vec(const std::string& str, unsigned char chars_per_num = 2)
{
    std::vector<T> out(str.size() / chars_per_num, 0);

    T value;
    for (int i = 0; i < str.size() / chars_per_num; i++) {
        std::from_chars<T>(
            str.data() + (i * chars_per_num),
            str.data() + (i * chars_per_num) + chars_per_num,
            value,
            16
        );
        out[i] = value;
    }

    return out;
}

/*bool checkToken(const Array& params) {

    TokenDB tdb;
    
    std::string key = params[0].get_str();
    
    std::string value = tdb.Search(key.c_str());
    
    char valueToHash = value.c_str();
    
    if (valueToHash.size() >  0)
        return true;
    else
        
    
    return true;
}*/
