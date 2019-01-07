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

Value sendtoken(const Array& params, bool fHelp) {

    if (fHelp || params.size() != 0)
        throw runtime_error(
            "sendtoken \n"
            "Returns transaction id to transfer token.");

    Array ret;
    Object obj;
    obj.push_back(Pair("New PubKey", getnewpubkey));
    ret.push_back(obj);
    return ret;
}

Value searchtoken(const Array& params, bool fHelp) {

    if (fHelp || params.size() != 0)
        throw runtime_error(
            "sendtoken \n"
            "Returns transaction id to transfer token.");

    TokenDB tdb;
    
    std::string key = params[0].get_str();
    
    std::string value = tdb.Search(key.c_str());
    
    char valueToHash = value.c_str();
    
    
    LogPrintf("Writing token HASH %s \n", value.c_str());
    
    
    
    CDataStream ssTokens(SER_DISK, CLIENT_VERSION);
    
    unsigned char pchMsgTmp[4];
    ssTokens << FLATDATA(Params().MessageStart());
    //ssTokens << FLATDATA(value.c_str());
    
    ssTokens << valueToHash;
    
    uint256 hash = Hash(ssTokens.begin(), ssTokens.end());
    
    ssTokens << hash;
    
    char v;
    
    ssTokens >> v;
    
    LogPrintf("Writing token VALUE %c \n", v);
    
    Object ret;
    
    ret.push_back(Pair("value", v));
    
    return ret;
}

bool checkToken(const Array& params) {

    TokenDB tdb;
    
    std::string key = params[0].get_str();
    
    std::string value = tdb.Search(key.c_str());
    
    char valueToHash = value.c_str();
    
    if (valueToHash.size() >  0)
        return true;
    else
        
    
    return true;
}

/*

//
// TokenDB
//

TokenDB::TokenDB()
{
    pathAddr = GetDataDir() / "tokens.dat";
}

bool TokenDB::Write(char key)
{
    // Generate random temporary filename
    unsigned short randv = 0;
    GetRandBytes((unsigned char *)&randv, sizeof(randv));
    std::string tmpfn = strprintf("tokens.dat.%04x", randv);

    // serialize tokens, checksum data up to that point, then append csum
    CDataStream ssTokens(SER_DISK, CLIENT_VERSION);
    ssTokens << FLATDATA(Params().MessageStart());
    ssTokens << key;
    uint256 hash = Hash(ssTokens.begin(), ssTokens.end());
    ssTokens << hash;

    // open temp output file, and associate with CAutoFile
    boost::filesystem::path pathTmp = GetDataDir() / tmpfn;
    FILE *file = fopen(pathTmp.string().c_str(), "wb");
    CAutoFile fileout = CAutoFile(file, SER_DISK, CLIENT_VERSION);
    if (!fileout)
        return error("CAddrman::Write() : open failed");

    // Write and commit header, data
    try {
        fileout << ssTokens;
    }
    catch (std::exception &e) {
        return error("CAddrman::Write() : I/O error");
    }
    FileCommit(fileout);
    fileout.fclose();

    // replace existing peers.dat, if any, with new peers.dat.XXXX
    if (!RenameOver(pathTmp, pathAddr))
        return error("CAddrman::Write() : Rename-into-place failed");

    return true;
}

TokenDB::Read(char key)
{
    // open input file, and associate with CAutoFile
    FILE *file = fopen(pathAddr.string().c_str(), "rb");
    CAutoFile filein = CAutoFile(file, SER_DISK, CLIENT_VERSION);
    if (!filein)
        return error("CAddrman::Read() : open failed");

    // use file size to size memory buffer
    int fileSize = boost::filesystem::file_size(pathAddr);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if ( dataSize < 0 )
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char *)&vchData[0], dataSize);
        filein >> hashIn;
    }
    catch (std::exception &e) {
        return error("CAddrman::Read() 2 : I/O error or stream data corrupted");
    }
    filein.fclose();

    CDataStream ssTokens(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssTokens.begin(), ssTokens.end());
    if (hashIn != hashTmp)
        return error("CAddrman::Read() : checksum mismatch; data corrupted");

    unsigned char pchMsgTmp[4];
    try {
        // de-serialize file header (network specific magic number) and ..
        ssTokens >> FLATDATA(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp)))
            return error("CAddrman::Read() : invalid network magic number");

        // de-serialize address data into one CAddrMan object
        ssTokens >> key;
    }
    catch (std::exception &e) {
        return error("CAddrman::Read() : I/O error or stream data corrupted");
    }

    //return true;
    //std::string s(std::istreambuf_iterator<char>(ssTokens), {});
    //return key;
    LogPrintf("  %s\n", key);
}

*/
