// Copyright (c) 2019-2019 The MartteXcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef TOKEN_LEVELDB_H
#define TOKEN_LEVELDB_H

#include "main.h"
#include "version.h"
#include "clientversion.h"
#include <map>
#include <string>
#include <vector>
#include <db_cxx.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

// Class that provides access to a LevelDB. Note that this class is frequently
// instantiated on the stack and then destroyed again, so instantiation has to
// be very cheap. Unfortunately that means, a TokenDB instance is actually just a
// wrapper around some global state.
//
// A LevelDB is a key/value store that is optimized for fast usage on hard
// disks. It prefers long read/writes to seeks and is based on a series of
// sorted key/value mapping files that are stacked on top of each other, with
// newer files overriding older files. A background thread compacts them
// together when too many files stack up.
//
// Learn more: http://code.google.com/p/leveldb/
class TokenDB
{
public:
    TokenDB(const char* pszMode="r+");
    ~TokenDB() {
        // Note that this is not the same as Close() because it deletes only
        // data scoped to this TxDB object.
        delete activeTokenBatch;
    }

    // Destroys the underlying shared global state accessed by this TxDB.
    void Close();

private:
    leveldb::DB *pdb;  // Points to the global instance.

    // A batch stores up writes and deletes for atomic application. When this
    // field is non-NULL, writes/deletes go there instead of directly to disk.
    leveldb::WriteBatch *activeTokenBatch;
    leveldb::Options options;
    bool fReadOnly;
    int nVersion;

public:
    // Returns true and sets (value,false) if activeTokenBatch contains the given key
    // or leaves value alone and sets deleted = true if activeTokenBatch contains a
    // delete for it.
    bool ScanTokenBatch(const CDataStream &key, std::string *value, bool *deleted) const;

    template<typename K, typename T>
    bool Read(const K& key, T& value)
    {
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(1000);
        ssKey << key;
        std::string strValue;

        bool readFromDb = true;
        if (activeTokenBatch) {
            // First we must search for it in the currently pending set of
            // changes to the db. If not found in the batch, go on to read disk.
            bool deleted = false;
            readFromDb = ScanTokenBatch(ssKey, &strValue, &deleted) == false;
            if (deleted) {
                return false;
            }
        }
        if (readFromDb) {
            leveldb::Status status = pdb->Get(leveldb::ReadOptions(),
                                              ssKey.str(), &strValue);
            if (!status.ok()) {
                if (status.IsNotFound())
                    return false;
                // Some unexpected error.
                LogPrintf("LevelDB read failure: %s\n", status.ToString());
                return false;
            }
        }
        // Unserialize value
        try {
            CDataStream ssValue(strValue.data(), strValue.data() + strValue.size(),
                                SER_DISK, CLIENT_VERSION);
            ssValue >> value;
        }
        catch (std::exception &e) {
            return false;
        }
        return true;
    }

    template<typename K, typename T>
    bool Write(const K& key , const T& value)
    {
        if (fReadOnly)
            assert(!"Write called on token database in read-only mode");

        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(1000);
        ssKey << key;
        CDataStream ssValue(SER_DISK, CLIENT_VERSION);
        ssValue.reserve(10000);
        ssValue << value;

        if (activeTokenBatch) {
            activeTokenBatch->Put(ssKey.str(), ssValue.str());
            return true;
        }
	leveldb::Status status = pdb->Put(leveldb::WriteOptions(), ssKey.str(), ssValue.str());
        if (!status.ok()) {
            LogPrintf("Token LevelDB write failure: %s\n", status.ToString());
            return false;
        }
        return true;
    }

    template<typename K>
    bool Erase(const K& key)
    {
        if (!pdb)
            return false;
        if (fReadOnly)
            assert(!"Erase called on token database in read-only mode");

        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(1000);
        ssKey << key;
        if (activeTokenBatch) {
            activeTokenBatch->Delete(ssKey.str());
            return true;
        }
        leveldb::Status status = pdb->Delete(leveldb::WriteOptions(), ssKey.str());
        return (status.ok() || status.IsNotFound());
    }

    template<typename K>
    bool Exists(const K& key)
    {
        CDataStream ssKey(SER_DISK, CLIENT_VERSION);
        ssKey.reserve(1000);
        ssKey << key;
        std::string unused;


        leveldb::Status status = pdb->Get(leveldb::ReadOptions(), ssKey.str(), &unused);
        return status.IsNotFound() == false;
    }


public:
    bool TokenBegin();
    bool TokenCommit();
    bool TokenAbort()
    {
        delete activeTokenBatch;
        activeTokenBatch = NULL;
        return true;
    }

    bool TokenReadVersion(char nVersion)
    {
        nVersion = 0;
	return Read(std::string("version"), nVersion);
        //return 1;
    }

    bool TokenWriteVersion(char nVersion)
    {
        //nVersion = "0";
        //return 1;
	return Write(std::string("version"), nVersion);
    }
};


#endif // TOKEN_DB_H
