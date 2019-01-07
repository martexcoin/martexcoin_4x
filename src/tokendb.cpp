// Copyright (c) 2019-2019 The MartteXcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#include "tokendb.h"
#include <sstream>
#include <string>
#include "util.h"
#include <leveldb/db.h>
#include "leveldb/cache.h"

using namespace std;
using namespace boost;

leveldb::DB *tokendb;

static leveldb::Options GetOpenOptions() 
{
    leveldb::Options options;
    options.create_if_missing = true;
    //options.block_cache = leveldb::NewLRUCache(1 * 1048576);  // 1MB cache
    options.block_cache = leveldb::NewLRUCache(1 * 1024);  // 1KB cache
    return options;
}

bool init_tokendb(leveldb::Options& options)
{
    
    filesystem::path dbDir = GetDataDir() / "tokendb";
    
    leveldb::Status status = leveldb::DB::Open(options, dbDir.string(), &tokendb);

    if (status.ok())
    { 
        LogPrintf(".... Token OpenDB Path : %s \n", dbDir.string());
        
        return true;
        
    }
    else
    {
        LogPrintf(".... Token OpenDB Error : %s \n", status.ToString());
        
        return false;
        
    }
    
    
}

TokenDB::TokenDB()
{  
    
    LogPrintf(" > Token Init \n");
    
    if (tokendb) 
    {
        tdb = tokendb;
        
        LogPrintf(".... Token OpenDB OK \n");
        
        LogPrintf(".... Token Init End \n");
        
        return;
    }
    
    
    LogPrintf(" > Token OpenDB Init\n");

    leveldb::Options options = GetOpenOptions();

    if ( init_tokendb(options) )
    { 
        tdb = tokendb;
        
        LogPrintf(".... Token OpenDB OK \n");  
        
    }
    
    LogPrintf(" > Token OpenDB End\n");
    
    
    LogPrintf(" > Token Init End \n");

}

