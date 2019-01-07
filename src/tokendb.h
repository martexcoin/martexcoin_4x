
// Copyright (c) 2019-2019 The MartteXcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef TOKEN_LEVELDB_H
#define TOKEN_LEVELDB_H

#include <sstream>
#include <string>
#include <leveldb/db.h>
#include "util.h"


static leveldb::ReadOptions GetReadOptions() 
{
    leveldb::ReadOptions options;
    
    return options;
}

static leveldb::WriteOptions GetWriteOptions() 
{
    leveldb::WriteOptions options;
    
    return options;
}


class TokenDB
{
    
public:
    
TokenDB();
    
    ~TokenDB() 
    {
        LogPrintf(" \n");
    }

    void Close();

public:
    
    leveldb::DB *tdb;

    leveldb::Status statusdb;
    
public:
    
    std::string Search(char *key)
    {
        std::ostringstream response;
        
        LogPrintf(" > Token Search Init \n");
        
        LogPrintf(" .... Token Search Get Key : %c \n", key);

        std::string v;      
        
        statusdb = tdb->Get(GetReadOptions(), key, &v); 

        if ( statusdb.ok() )
        {
            
            LogPrintf(" ........ Token Search Found Value : (  %s  )\n", v);
            
            response.str(v);
        }
        else 
        {
            response.str("");
            
            LogPrintf(" ........ Token Search Not Found\n");
            
        }
        
        LogPrintf(" > Token Search End \n");
        
        return response.str();
    }

    bool Write(char *key , char *value)
    {
        LogPrintf(" > Token Wirte Init\n");
        
        LogPrintf(" .... Token Wirte Key : %c \n", key);

        std::string v;      
        
        statusdb = tdb->Get(GetReadOptions(), key, &v);       
        
        if ( statusdb.ok() )
        {
            LogPrintf(" ........ Token Wirte Exist\n");
            
            return false;
            
        }
        else 
        {
            tdb->Put(GetWriteOptions(), key, value);
            
            LogPrintf(" ........ Token Wirte Create\n");
            
            return true;
        }
        
        LogPrintf(" > Token Wirte End\n");
        
        return true;
    }
    
    bool update(char key, char upValue)
    {
        return true;
    }
    
    bool Delete(char key)
    {
        return true;
    }
};


#endif