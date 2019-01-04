// Copyright (c) 2019-2019 The MartteXcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#include <map>

#include <boost/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <leveldb/env.h>
#include <leveldb/cache.h>
#include <leveldb/filter_policy.h>
#include <memenv/memenv.h>

#include "kernel.h"
#include "checkpoints.h"
#include "tokendb.h"
#include "util.h"
#include "main.h"
#include "chainparams.h"
//#include "serialize.h"

using namespace std;
using namespace boost;

leveldb::DB *tokendb; // global pointer for LevelDB object instance

static leveldb::Options GetOptions() {
    leveldb::Options options;
    int nCacheSizeMB = GetArg("-dbcache", 100);
    options.block_cache = leveldb::NewLRUCache(nCacheSizeMB * 1048576);
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);
    return options;
}

void init_tokenindex(leveldb::Options& options, bool fRemoveOld = false) {
    // First time init.
    filesystem::path directory = GetDataDir() / "tokenleveldb";

    if (fRemoveOld) {
        filesystem::remove_all(directory); // remove directory
        unsigned int nFile = 1;

        while (true)
        {
            filesystem::path strBlockFile = GetDataDir() / strprintf("blk%04u.dat", nFile);

            // Break if no such file
            if( !filesystem::exists( strBlockFile ) )
                break;

            filesystem::remove(strBlockFile);

            nFile++;
        }
    }

    filesystem::create_directory(directory);
    LogPrintf("Opening Token LevelDB in %s\n", directory.string());
    leveldb::Status status = leveldb::DB::Open(options, directory.string(), &tokendb);
    if (!status.ok()) {
        throw runtime_error(strprintf("init_tokenindex(): error opening database environment %s", status.ToString()));
    }
}

// CDB subclasses are created and destroyed VERY OFTEN. That's why
// we shouldn't treat this as a free operations.
TokenDB::TokenDB(const char* pszMode)
{
    assert(pszMode);
    activeTokenBatch = NULL;
    fReadOnly = (!strchr(pszMode, '+') && !strchr(pszMode, 'w'));

    if (tokendb) {
        pdb = tokendb;
        return;
    }

    bool fCreate = strchr(pszMode, 'c');

    options = GetOptions();
    options.create_if_missing = true; //options.create_if_missing = fCreate;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);

    init_tokenindex(options); // Init directory
    pdb = tokendb;

    if (Exists(string("version")))
    {
        TokenReadVersion(nVersion);
        LogPrintf("Token index version is %d\n", nVersion);

        if (nVersion < DATABASE_VERSION)
        {
            LogPrintf("Required index version is %d, removing old database\n", DATABASE_VERSION);

            // Leveldb instance destruction
            delete tokendb;
            tokendb = pdb = NULL;
            delete activeTokenBatch;
            activeTokenBatch = NULL;

            init_tokenindex(options, true); // Remove directory and create new database
            pdb = tokendb;

            bool fTmp = fReadOnly;
            fReadOnly = false;
            TokenWriteVersion(DATABASE_VERSION); // Save transaction index version
            fReadOnly = fTmp;
        }
    }
    else if (fCreate)
    {
        bool fTmp = fReadOnly;
        fReadOnly = false;
        TokenWriteVersion(DATABASE_VERSION);
        fReadOnly = fTmp;
    }

    LogPrintf("Opened Token LevelDB successfully\n");
}

void TokenDB::Close()
{
    delete tokendb;
    tokendb = pdb = NULL;
    delete options.filter_policy;
    options.filter_policy = NULL;
    delete options.block_cache;
    options.block_cache = NULL;
    delete activeTokenBatch;
    activeTokenBatch = NULL;
}

bool TokenDB::TokenBegin()
{
    assert(!activeTokenBatch);
    activeTokenBatch = new leveldb::WriteBatch();
    return true;
}

bool TokenDB::TokenCommit()
{
    assert(activeTokenBatch);
    leveldb::Status status = pdb->Write(leveldb::WriteOptions(), activeTokenBatch);
    delete activeTokenBatch;
    activeTokenBatch = NULL;
    if (!status.ok()) {
        LogPrintf("Token LevelDB batch commit failure: %s\n", status.ToString());
        return false;
    }
    return true;
}

class cBatchTokenScanner : public leveldb::WriteBatch::Handler {
public:
    std::string needle;
    bool *deleted;
    std::string *foundValue;
    bool foundEntry;

    cBatchTokenScanner() : foundEntry(false) {}

    virtual void Put(const leveldb::Slice& key, const leveldb::Slice& value) {
        if (key.ToString() == needle) {
            foundEntry = true;
            *deleted = false;
            *foundValue = value.ToString();
        }
    }

    virtual void Delete(const leveldb::Slice& key) {
        if (key.ToString() == needle) {
            foundEntry = true;
            *deleted = true;
        }
    }
};

// When performing a read, if we have an active batch we need to check it first
// before reading from the database, as the rest of the code assumes that once
// a database transaction begins reads are consistent with it. It would be good
// to change that assumption in future and avoid the performance hit, though in
// practice it does not appear to be large.
bool TokenDB::ScanTokenBatch(const CDataStream &key, string *value, bool *deleted) const {
    assert(activeTokenBatch);
    *deleted = false;
    cBatchTokenScanner scanner;
    scanner.needle = key.str();
    scanner.deleted = deleted;
    scanner.foundValue = value;
    leveldb::Status status = activeTokenBatch->Iterate(&scanner);
    if (!status.ok()) {
        throw runtime_error(status.ToString());
    }
    return scanner.foundEntry;
}

