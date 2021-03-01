// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2020 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_H
#define BITCOIN_WALLET_H

#include "addressbook.h"
#include "amount.h"
#include "base58.h"
#include "consensus/tx_verify.h"
#include "crypter.h"
#include "kernel.h"
#include "key.h"
#include "keystore.h"
#include "main.h"
#include "pairresult.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "zpiv/zerocoin.h"
#include "guiinterface.h"
#include "util.h"
#include "util/memory.h"
#include "validationinterface.h"
#include "wallet/wallet_ismine.h"
#include "wallet/walletdb.h"
#include "zpiv/zpivmodule.h"

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

/**
 * Settings
 */
extern CFeeRate payTxFee;
extern CAmount maxTxFee;
extern unsigned int nTxConfirmTarget;
extern bool bSpendZeroConfChange;
extern bool bdisableSystemnotifications;
extern bool fSendFreeTransactions;
extern bool fPayAtLeastCustomFee;

//! -paytxfee default
static const CAmount DEFAULT_TRANSACTION_FEE = 0;
//! -paytxfee will warn if called with a higher fee than this amount (in satoshis) per KB
static const CAmount nHighTransactionFeeWarning = 0.1 * COIN;
//! -maxtxfee default
static const CAmount DEFAULT_TRANSACTION_MAXFEE = 1 * COIN;
//! -maxtxfee will warn if called with a higher fee than this amount (in satoshis)
static const CAmount nHighTransactionMaxFeeWarning = 100 * nHighTransactionFeeWarning;
//! Largest (in bytes) free transaction we're willing to create
static const unsigned int MAX_FREE_TRANSACTION_CREATE_SIZE = 1000;
//! if set, all keys will be derived by using BIP32
static const bool DEFAULT_USE_HD_WALLET = false;
//! if set, will show warning if the wallet is a hd wallet and is unencrypted
static const bool DEFAULT_ENABLE_WARN_ENCRYPTHD = false;
//! -custombackupthreshold default
static const int DEFAULT_CUSTOMBACKUPTHRESHOLD = 1;

class CAccountingEntry;
class CCoinControl;
class COutput;
class CReserveKey;
class CScript;
class CWalletTx;

/** (client) version numbers for particular wallet features */
enum WalletFeature {
    FEATURE_BASE = 10500, // the earliest version new wallets supports (only useful for getinfo's clientversion output)

    FEATURE_WALLETCRYPT = 40000, // wallet encryption
    FEATURE_COMPRPUBKEY = 60000, // compressed public keys
    FEATURE_HD = 120200,    // Hierarchical key derivation after BIP32 (HD Wallet), BIP44 (multi-coin), BIP39 (mnemonic)
    // which uses on-the-fly private key derivation,change this to 4000000 once version changes are made

    FEATURE_PRE_SPLIT_KEYPOOL = 169900, // Upgraded to HD SPLIT and can have a pre-split keypool

    FEATURE_LATEST = FEATURE_PRE_SPLIT_KEYPOOL
};

enum AvailableCoinsType {
    ALL_COINS = 1,
    ONLY_DENOMINATED = 2,
    ONLY_NOT5000IFMN = 3,
    ONLY_NONDENOMINATED_NOT5000IFMN = 4,           // ONLY_NONDENOMINATED and not 5000 MXT at the same time
    ONLY_5000 = 5,                                 // find masternode outputs including locked ones (use with caution)
    STAKEABLE_COINS = 6                             // UTXO's that are valid for staking
};

struct CompactTallyItem {
    CBitcoinAddress address;
    CAmount nAmount;
    std::vector<CTxIn> vecTxIn;
    CompactTallyItem()
    {
        nAmount = 0;
    }
};

/** A key pool entry */
class CKeyPool
{
public:
    //! The time at which the key was generated. Set in AddKeypoolPubKeyWithDB
    int64_t nTime;
    //! The public key
    CPubKey vchPubKey;
    bool fInternal; // for change outputs
    bool m_pre_split; // For keys generated before keypool split upgrade

    CKeyPool();
    CKeyPool(const CPubKey& vchPubKeyIn, bool fInternalIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(nTime);
        READWRITE(vchPubKey);
        if (ser_action.ForRead()) {
            try {
                READWRITE(fInternal);
            }
            catch (std::ios_base::failure&) {
                /* flag as external address if we can't read the internal boolean
                   (this will be the case for any wallet before the HD chain split version) */
                fInternal = false;
            }
            try {
                READWRITE(m_pre_split);
            }
            catch (std::ios_base::failure&) {
                /* flag as postsplit address if we can't read the m_pre_split boolean
                   (this will be the case for any wallet that upgrades to HD chain split)*/
                m_pre_split = false;
            }
        }
        else {
            READWRITE(fInternal);
            READWRITE(m_pre_split);
        }
    }
};

/** Record info about last stake attempt:
 *  - tipBlock       index of the block on top of which last stake attempt was made
 *  - nTime          time slot of last attempt
 *  - nTries         number of UTXOs hashed during last attempt
 *  - nCoins         number of stakeable utxos during last attempt
**/
class CStakerStatus
{
private:
    const CBlockIndex* tipBlock{nullptr};
    int64_t nTime{0};
    int nTries{0};
    int nCoins{0};

public:
    // Get
    const CBlockIndex* GetLastTip() const { return tipBlock; }
    uint256 GetLastHash() const { return (GetLastTip() == nullptr ? UINT256_ZERO : GetLastTip()->GetBlockHash()); }
    int GetLastHeight() const { return (GetLastTip() == nullptr ? 0 : GetLastTip()->nHeight); }
    int GetLastCoins() const { return nCoins; }
    int GetLastTries() const { return nTries; }
    int64_t GetLastTime() const { return nTime; }
    // Set
    void SetLastCoins(const int coins) { nCoins = coins; }
    void SetLastTries(const int tries) { nTries = tries; }
    void SetLastTip(const CBlockIndex* lastTip) { tipBlock = lastTip; }
    void SetLastTime(const uint64_t lastTime) { nTime = lastTime; }
    void SetNull()
    {
        SetLastCoins(0);
        SetLastTries(0);
        SetLastTip(nullptr);
        SetLastTime(0);
    }
    // Check whether staking status is active (last attempt earlier than 30 seconds ago)
    bool IsActive() const { return (nTime + 30) >= GetTime(); }
};

/**
 * A CWallet is an extension of a keystore, which also maintains a set of transactions and balances,
 * and provides the ability to create new transactions.
 */
class CWallet : public CCryptoKeyStore, public CValidationInterface
{
private:
    bool SelectCoins(const CAmount& nTargetValue, std::set<std::pair<const CWalletTx*, unsigned int> >& setCoinsRet, CAmount& nValueRet, const CCoinControl* coinControl = NULL, AvailableCoinsType coin_type = ALL_COINS, bool useIX = true, bool fIncludeColdStaking=false, bool fIncludeDelegated=true) const;
    //it was public bool SelectCoins(int64_t nTargetValue, std::set<std::pair<const CWalletTx*,unsigned int> >& setCoinsRet, int64_t& nValueRet, const CCoinControl *coinControl = NULL, AvailableCoinsType coin_type=ALL_COINS, bool useIX = true) const;

    CWalletDB* pwalletdbEncryption;
    //! keeps track of whether Unlock has run a thorough check before
    bool fDecryptionThoroughlyChecked{false};

    //! the current wallet version: clients below this version are not able to load the wallet
    int nWalletVersion;

    //! the maximum wallet format version: memory-only variable that specifies to what version this wallet may be upgraded
    int nWalletMaxVersion;

    int64_t nNextResend;
    int64_t nLastResend;

    /**
     * Used to keep track of spent outpoints, and
     * detect and report conflicts (double-spends or
     * mutated transactions where the mutant gets mined).
     */
    typedef std::multimap<COutPoint, uint256> TxSpends;
    TxSpends mapTxSpends;
    void AddToSpends(const COutPoint& outpoint, const uint256& wtxid);
    void AddToSpends(const uint256& wtxid);

    /* Mark a transaction (and its in-wallet descendants) as conflicting with a particular block. */
    void MarkConflicted(const uint256& hashBlock, const uint256& hashTx);

    void SyncMetaData(std::pair<TxSpends::iterator, TxSpends::iterator>);
    /* HD derive new child key (on internal or external chain) */
    void DeriveNewChildKey(const CKeyMetadata& metadata, CKey& secretRet, uint32_t nAccountIndex, bool fInternal /*= false*/);


public:

    //TODO move to private after createwalletfromfile has been backported
    void MarkPreSplitKeys();

    static const int DEFAULT_STAKE_SPLIT_THRESHOLD = 2000;

    bool StakeableCoins(std::vector<COutput>* pCoins = nullptr);
    bool IsCollateralAmount(CAmount nInputAmount) const;

    /*
     * Main wallet lock.
     * This lock protects all the fields added by CWallet
     *   except for:
     *      fFileBacked (immutable after instantiation)
     *      strWalletFile (immutable after instantiation)
     */
    mutable RecursiveMutex cs_wallet;

    bool fFileBacked;
    bool fWalletUnlockStaking;
    std::string strWalletFile;

    void LoadKeyPool(int nIndex, const CKeyPool &keypool)
    {
        if (keypool.fInternal) {
            setInternalKeyPool.insert(nIndex);
        } else {
            setExternalKeyPool.insert(nIndex);
        }

        // If no metadata exists yet, create a default with the pool key's
        // creation time. Note that this may be overwritten by actually
        // stored metadata for that key later, which is fine.
        CKeyID keyid = keypool.vchPubKey.GetID();
        if (mapKeyMetadata.count(keyid) == 0)
            mapKeyMetadata[keyid] = CKeyMetadata(keypool.nTime);
    }

    std::set<int64_t> setInternalKeyPool;
    std::set<int64_t> setExternalKeyPool;
    std::set<int64_t> set_pre_split_keypool;
    std::map<CKeyID, CKeyMetadata> mapKeyMetadata;

    typedef std::map<unsigned int, CMasterKey> MasterKeyMap;
    MasterKeyMap mapMasterKeys;
    unsigned int nMasterKeyMaxID;

    // Stake split threshold
    CAmount nStakeSplitThreshold;
    // Staker status (last hashed block and time)
    CStakerStatus* pStakerStatus = nullptr;

    //MultiSend
    std::vector<std::pair<std::string, int> > vMultiSend;
    bool fMultiSendStake;
    bool fMultiSendMasternodeReward;
    bool fMultiSendNotify;
    std::string strMultiSendChangeAddress;
    int nLastMultiSendHeight;
    std::vector<std::string> vDisabledAddresses;

    //Auto Combine Inputs
    bool fCombineDust;
    CAmount nAutoCombineThreshold;

    CWallet();
    CWallet(std::string strWalletFileIn);
    ~CWallet();
    void SetNull();
    bool isMultiSendEnabled();
    void setMultiSendDisabled();

    std::map<uint256, CWalletTx> mapWallet;
    std::list<CAccountingEntry> laccentries;

    typedef std::pair<CWalletTx*, CAccountingEntry*> TxPair;
    typedef std::multimap<int64_t, TxPair > TxItems;
    TxItems wtxOrdered;

    int64_t nOrderPosNext;
    std::map<uint256, int> mapRequestCount;

    std::map<CTxDestination, AddressBook::CAddressBookData> mapAddressBook;

    std::set<COutPoint> setLockedCoins;

    int64_t nTimeFirstKey;
    std::map<CKeyID, CHDPubKey> mapHdPubKeys; //<! memory map of HD extended pubkeys

    const CWalletTx* GetWalletTx(const uint256& hash) const;

    std::vector<CWalletTx> getWalletTxs();
    std::string GetUniqueWalletBackupName() const;

    //! check whether we are allowed to upgrade (or already support) to the named feature
    bool CanSupportFeature(enum WalletFeature wf);

    bool AvailableCoins(std::vector<COutput>* pCoins, bool fOnlyConfirmed = true, const CCoinControl* coinControl = NULL, bool fIncludeZeroValue = false, AvailableCoinsType nCoinType = ALL_COINS, bool fUseIX = false, int nWatchonlyConfig = 1, bool fIncludeColdStaking=false, bool fIncludeDelegated=true) const;

    // Get available p2cs utxo
    void GetAvailableP2CSCoins(std::vector<COutput>& vCoins) const;

    std::map<CBitcoinAddress, std::vector<COutput> > AvailableCoinsByAddress(bool fConfirmed = true, CAmount maxCoinValue = 0);
    bool SelectCoinsMinConf(const CAmount& nTargetValue, int nConfMine, int nConfTheirs, std::vector<COutput> vCoins, std::set<std::pair<const CWalletTx*, unsigned int> >& setCoinsRet, CAmount& nValueRet) const;

    /// Get 5000 MXT output and keys which can be used for the Masternode
    bool GetMasternodeVinAndKeys(CTxIn& txinRet, CPubKey& pubKeyRet, CKey& keyRet, std::string strTxHash = "", std::string strOutputIndex = "");
    /// Extract txin information and keys from output
    bool GetVinAndKeysFromOutput(COutput out, CTxIn& txinRet, CPubKey& pubKeyRet, CKey& keyRet, bool fColdStake = false);

    bool IsSpent(const uint256& hash, unsigned int n) const;

    bool IsLockedCoin(const uint256& hash, unsigned int n) const;
    void LockCoin(const COutPoint& output);
    void UnlockCoin(const COutPoint& output);
    void UnlockAllCoins();
    void ListLockedCoins(std::vector<COutPoint>& vOutpts);

    //  keystore implementation

    // Generate a new key
    CPubKey GenerateNewKey(uint32_t nAccountIndex, bool fInternal /*= false*/);
    //! HaveKey implementation that also checks the mapHdPubKeys
    bool HaveKey(const CKeyID &address) const;
    //! GetPubKey implementation that also checks the mapHdPubKeys
    bool GetPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const;
    //! GetKey implementation that can derive a HD private key on the fly
    bool GetKey(const CKeyID &address, CKey& keyOut) const;
    //! Adds a HDPubKey into the wallet(database)
    bool AddHDPubKey(const CExtPubKey &extPubKey, bool fInternal);
    //! loads a HDPubKey into the wallets memory
    bool LoadHDPubKey(const CHDPubKey &hdPubKey);
    PairResult getNewAddress(CBitcoinAddress& ret, const std::string addressLabel, const std::string purpose,
                                           const CChainParams::Base58Type addrType = CChainParams::PUBKEY_ADDRESS);
    PairResult getNewAddress(CBitcoinAddress& ret, std::string label);
    PairResult getNewStakingAddress(CBitcoinAddress& ret, std::string label);
    int64_t GetKeyCreationTime(CPubKey pubkey);
    int64_t GetKeyCreationTime(const CBitcoinAddress& address);

    //! Adds a key to the store, and saves it to disk.
    bool AddKeyPubKey(const CKey& key, const CPubKey& pubkey);
    //! Adds a key to the store, without saving it to disk (used by LoadWallet)
    bool LoadKey(const CKey& key, const CPubKey& pubkey) { return CCryptoKeyStore::AddKeyPubKey(key, pubkey); }
    //! Load metadata (used by LoadWallet)
    bool LoadKeyMetadata(const CPubKey& pubkey, const CKeyMetadata& metadata);

    bool LoadMinVersion(int nVersion);

    //! Adds an encrypted key to the store, and saves it to disk.
    bool AddCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret);
    //! Adds an encrypted key to the store, without saving it to disk (used by LoadWallet)
    bool LoadCryptedKey(const CPubKey& vchPubKey, const std::vector<unsigned char>& vchCryptedSecret);
    bool AddCScript(const CScript& redeemScript);
    bool LoadCScript(const CScript& redeemScript);

    //! Adds a destination data tuple to the store, and saves it to disk
    bool AddDestData(const CTxDestination& dest, const std::string& key, const std::string& value);
    //! Erases a destination data tuple in the store and on disk
    bool EraseDestData(const CTxDestination& dest, const std::string& key);
    //! Adds a destination data tuple to the store, without saving it to disk
    bool LoadDestData(const CTxDestination& dest, const std::string& key, const std::string& value);

    //! Adds a watch-only address to the store, and saves it to disk.
    bool AddWatchOnly(const CScript& dest);
    bool RemoveWatchOnly(const CScript& dest);
    //! Adds a watch-only address to the store, without saving it to disk (used by LoadWallet)
    bool LoadWatchOnly(const CScript& dest);

    //! Adds a MultiSig address to the store, and saves it to disk.
    bool AddMultiSig(const CScript& dest);
    bool RemoveMultiSig(const CScript& dest);
    //! Adds a MultiSig address to the store, without saving it to disk (used by LoadWallet)
    bool LoadMultiSig(const CScript& dest);

    //! Lock Wallet
    bool Lock();
    bool Unlock(const SecureString& strWalletPassphrase, bool anonimizeOnly = false);
    bool Unlock(const CKeyingMaterial& vMasterKeyIn);
    bool ChangeWalletPassphrase(const SecureString& strOldWalletPassphrase, const SecureString& strNewWalletPassphrase);
    bool EncryptWallet(const SecureString& strWalletPassphrase);

    void GetKeyBirthTimes(std::map<CKeyID, int64_t>& mapKeyBirth) const;

    /**
     * Increment the next transaction order id
     * @return next transaction order id
     */
    int64_t IncOrderPosNext(CWalletDB* pwalletdb = NULL);

    void MarkDirty();
    bool AddToWallet(const CWalletTx& wtxIn, bool fFromLoadWallet, CWalletDB* pwalletdb);
    void SyncTransaction(const CTransaction& tx, const CBlock* pblock);
    bool AddToWalletIfInvolvingMe(const CTransaction& tx, const CBlock* pblock, bool fUpdate);
    void EraseFromWallet(const uint256& hash);

    /**
     * Upgrade wallet to HD if needed. Does nothing if not.
     */

    int ScanForWalletTransactions(CBlockIndex* pindexStart, bool fUpdate = false, bool fromStartup = false);
    void ReacceptWalletTransactions(bool fFirstLoad = false);
    void ResendWalletTransactions();

    CAmount loopTxsBalance(std::function<void(const uint256&, const CWalletTx&, CAmount&)>method) const;
    CAmount GetBalance() const;
    CAmount GetColdStakingBalance() const;  // delegated coins for which we have the staking key
    CAmount GetImmatureColdStakingBalance() const;
    CAmount GetStakingBalance(const bool fIncludeColdStaking = true) const;
    CAmount GetDelegatedBalance() const;    // delegated coins for which we have the spending key
    CAmount GetImmatureDelegatedBalance() const;
    CAmount GetLockedCoins() const;
    CAmount GetUnlockedCoins() const;
    CAmount GetUnconfirmedBalance() const;
    CAmount GetImmatureBalance() const;
    CAmount GetWatchOnlyBalance() const;
    CAmount GetUnconfirmedWatchOnlyBalance() const;
    CAmount GetImmatureWatchOnlyBalance() const;
    CAmount GetLockedWatchOnlyBalance() const;
    bool CreateTransaction(const std::vector<std::pair<CScript, CAmount> >& vecSend,
        CWalletTx& wtxNew,
        CReserveKey& reservekey,
        CAmount& nFeeRet,
        std::string& strFailReason,
        const CCoinControl* coinControl = NULL,
        AvailableCoinsType coin_type = ALL_COINS,
        bool useIX = false,
        CAmount nFeePay = 0,
        bool fIncludeDelegated = false);
    bool CreateTransaction(CScript scriptPubKey, const CAmount& nValue, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRet, std::string& strFailReason, const CCoinControl* coinControl = NULL, AvailableCoinsType coin_type = ALL_COINS, bool useIX = false, CAmount nFeePay = 0, bool fIncludeDelegated = false);
    bool CommitTransaction(CWalletTx& wtxNew, CReserveKey& reservekey, std::string strCommand = "tx");
    bool AddAccountingEntry(const CAccountingEntry&, CWalletDB & pwalletdb);
    int GenerateAnonsendOutputs(int nTotalValue, std::vector<CTxOut>& vout);
    bool CreateCoinStake(const CKeyStore& keystore, const CBlockIndex* pindexPrev, unsigned int nBits, CMutableTransaction& txNew, int64_t& nTxNewTime);
    bool MultiSend();
    void AutoCombineDust();

    static CFeeRate minTxFee;
    static CAmount GetMinimumFee(unsigned int nTxBytes, unsigned int nConfirmTarget, const CTxMemPool& pool);

    size_t KeypoolCountExternalKeys();
    size_t KeypoolCountInternalKeys();
    bool TopUpKeyPool(unsigned int kpSize = 0);
    void ReserveKeyFromKeyPool(int64_t& nIndex, CKeyPool& keypool, bool fInternal);
    void KeepKey(int64_t nIndex);
    void ReturnKey(int64_t nIndex, bool fInternal);
    bool GetKeyFromPool(CPubKey &key, bool fInternal /*= false*/);
    int64_t GetOldestKeyPoolTime();
    void GetAllReserveKeys(std::set<CKeyID>& setAddress) const;

    std::set<std::set<CTxDestination> > GetAddressGroupings();
    std::map<CTxDestination, CAmount> GetAddressBalances();

    std::set<CTxDestination> GetAccountAddresses(std::string strAccount) const;

    bool GetBudgetSystemCollateralTX(CWalletTx& tx, uint256 hash, bool useIX);
    bool GetBudgetFinalizationCollateralTX(CWalletTx& tx, uint256 hash, bool useIX); // Only used for budget finalization

    bool IsDenominated(const CTxIn& txin) const;

    bool IsDenominatedAmount(CAmount nInputAmount) const;

    bool IsUsed(const CBitcoinAddress address) const;

    isminetype IsMine(const CTxIn& txin) const;
    CAmount GetDebit(const CTxIn& txin, const isminefilter& filter) const;
    isminetype IsMine(const CTxOut& txout) const;
    CAmount GetCredit(const CTxOut& txout, const isminefilter& filter) const;
    bool IsChange(const CTxOut& txout) const;
    CAmount GetChange(const CTxOut& txout) const;
    bool IsMine(const CTransaction& tx) const;
    /** should probably be renamed to IsRelevantToMe */
    bool IsFromMe(const CTransaction& tx) const;
    CAmount GetDebit(const CTransaction& tx, const isminefilter& filter) const;
    CAmount GetCredit(const CTransaction& tx, const isminefilter& filter, const bool fUnspent = false) const;
    CAmount GetChange(const CTransaction& tx) const;
    void SetBestChain(const CBlockLocator& loc);

    DBErrors LoadWallet(bool& fFirstRunRet);
    DBErrors ZapWalletTx(std::vector<CWalletTx>& vWtx);

    static CBitcoinAddress ParseIntoAddress(const CTxDestination& dest, const std::string& purpose);

    bool SetAddressBook(const CTxDestination& address, const std::string& strName, const std::string& purpose);
    bool DelAddressBook(const CTxDestination& address, const CChainParams::Base58Type addrType = CChainParams::PUBKEY_ADDRESS);
    bool HasAddressBook(const CTxDestination& address) const;
    bool HasDelegator(const CTxOut& out) const;

    std::string purposeForAddress(const CTxDestination& address) const;

    bool NewKeyPool();
    bool UpdatedTransaction(const uint256& hashTx);

    void Inventory(const uint256& hash);

    unsigned int GetKeyPoolSize();

    //! signify that a particular wallet feature is now used. this may change nWalletVersion and nWalletMaxVersion if those are lower
    bool SetMinVersion(enum WalletFeature, CWalletDB* pwalletdbIn = NULL, bool fExplicit = false);

    //! change which version we're allowed to upgrade to (note that this does not immediately imply upgrading to that format)
    bool SetMaxVersion(int nVersion);

    //! get the current wallet format (the oldest client version guaranteed to understand this wallet)
    int GetVersion();

    /**
     * HD Wallet Functions
     */

    /* Returns true if HD is enabled */
    bool IsHDEnabled();
    /* Generates a new HD chain */
    void GenerateNewHDChain(const std::string& words, const SecureString& strWalletPassphrase = std::string().c_str());
    void GenerateNewHDChain(const std::vector<std::string>& words, const SecureString& strWalletPassphrase = std::string().c_str());
    /* Set the HD chain model (chain child index counters) */
    bool SetHDChain(const CHDChain& chain, bool memonly);
    bool UpgradeHdChainEncrypted(const SecureString& strWalletPassphrase, const CHDChain& chain);
    bool SetCryptedHDChain(const CHDChain& chain, bool memonly);
    bool GetDecryptedHDChain(CHDChain& hdChainRet);

    //! Get wallet transactions that conflict with given transaction (spend same outputs)
    std::set<uint256> GetConflicts(const uint256& txid) const;

    /* Mark a transaction (and it in-wallet descendants) as abandoned so its inputs may be respent. */
    bool AbandonTransaction(const uint256& hashTx);

    /**
     * Address book entry changed.
     * @note called with lock cs_wallet held.
     */
    boost::signals2::signal<void(CWallet* wallet, const CTxDestination& address, const std::string& label, bool isMine, const std::string& purpose, ChangeType status)> NotifyAddressBookChanged;

    /**
     * Wallet transaction added, removed or updated.
     * @note called with lock cs_wallet held.
     */
    boost::signals2::signal<void(CWallet* wallet, const uint256& hashTx, ChangeType status)> NotifyTransactionChanged;

    /** Show progress e.g. for rescan */
    boost::signals2::signal<void(const std::string& title, int nProgress)> ShowProgress;

    /** Watch-only address added */
    boost::signals2::signal<void(bool fHaveWatchOnly)> NotifyWatchonlyChanged;

    /** MultiSig address added */
    boost::signals2::signal<void(bool fHaveMultiSig)> NotifyMultiSigChanged;

    /** notify wallet file backed up */
    boost::signals2::signal<void (const bool& fSuccess, const std::string& filename)> NotifyWalletBacked;
};


/** A key allocated from the key pool. */
class CReserveKey
{
protected:
    CWallet* pwallet;
    int64_t nIndex;
    bool internal{false};
    CPubKey vchPubKey;
    bool fInternal;

public:
    CReserveKey(CWallet* pwalletIn)
    {
        nIndex = -1;
        pwallet = pwalletIn;
        fInternal = false;
    }

    ~CReserveKey()
    {
        ReturnKey();
    }

    void ReturnKey();
    bool GetReservedKey(CPubKey &pubkey, bool fInternalIn = false);
    void KeepKey();
};


typedef std::map<std::string, std::string> mapValue_t;


static void ReadOrderPos(int64_t& nOrderPos, mapValue_t& mapValue)
{
    if (!mapValue.count("n")) {
        nOrderPos = -1; // TODO: calculate elsewhere
        return;
    }
    nOrderPos = atoi64(mapValue["n"].c_str());
}


static void WriteOrderPos(const int64_t& nOrderPos, mapValue_t& mapValue)
{
    if (nOrderPos == -1)
        return;
    mapValue["n"] = i64tostr(nOrderPos);
}

struct COutputEntry {
    CTxDestination destination;
    CAmount amount;
    int vout;
};

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
private:
    /** Constant used in hashBlock to indicate tx has been abandoned */
    static const uint256 ABANDON_HASH;

public:
    uint256 hashBlock;
    /* An nIndex == -1 means that hashBlock (in nonzero) refers to the earliest
     * block in the chain we know this or any in-wallet dependency conflicts
     * with. Older clients interpret nIndex == -1 as unconfirmed for backward
     * compatibility.
     */
    int nIndex;

    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        hashBlock = UINT256_ZERO;
        nIndex = -1;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        std::vector<uint256> vMerkleBranch; // For compatibility with older versions.
        READWRITE(*(CTransaction*)this);
        nVersion = this->nVersion;
        READWRITE(hashBlock);
        READWRITE(vMerkleBranch);
        READWRITE(nIndex);
    }

    void SetMerkleBranch(const CBlock& block);


    /**
     * Return depth of transaction in blockchain:
     * <0  : conflicts with a transaction this deep in the blockchain
     *  0  : in memory pool, waiting to be included in a block
     * >=1 : this many blocks deep in the main chain
     */
    int GetDepthInMainChain(const CBlockIndex*& pindexRet, bool enableIX = true) const;
    int GetDepthInMainChain(bool enableIX = true) const;
    bool IsInMainChain() const;
    bool IsInMainChainImmature() const;
    int GetBlocksToMaturity() const;
    bool AcceptToMemoryPool(bool fLimitFree = true, bool fRejectInsaneFee = true, bool ignoreFees = false);
    int GetTransactionLockSignatures() const;
    bool IsTransactionLockTimedOut() const;
    bool hashUnset() const { return (hashBlock.IsNull() || hashBlock == ABANDON_HASH); }
    bool isAbandoned() const { return (hashBlock == ABANDON_HASH); }
    void setAbandoned() { hashBlock = ABANDON_HASH; }
};

/**
 * A transaction with a bunch of additional info that only the owner cares about.
 * It includes any unrecorded transactions needed to link it back to the block chain.
 */
class CWalletTx : public CMerkleTx
{
private:
    const CWallet* pwallet;

public:
    mapValue_t mapValue;
    std::vector<std::pair<std::string, std::string> > vOrderForm;
    unsigned int fTimeReceivedIsTxTime;
    unsigned int nTimeReceived; //! time received by this node
    /**
     * Stable timestamp representing the block time, for a transaction included in a block,
     * or else the time when the transaction was received if it isn't yet part of a block.
     */
    unsigned int nTimeSmart;
    char fFromMe;
    std::string strFromAccount;
    int64_t nOrderPos; //! position in ordered transaction list

    // memory only
    mutable bool fDebitCached;
    mutable bool fCreditCached;
    mutable bool fImmatureCreditCached;
    mutable bool fAvailableCreditCached;
    mutable bool fAnonymizableCreditCached;
    mutable bool fAnonymizedCreditCached;
    mutable bool fDenomUnconfCreditCached;
    mutable bool fDenomConfCreditCached;
    mutable bool fWatchDebitCached;
    mutable bool fWatchCreditCached;
    mutable bool fImmatureWatchCreditCached;
    mutable bool fAvailableWatchCreditCached;
    mutable bool fChangeCached;
    mutable bool fColdDebitCached;
    mutable bool fColdCreditCached;
    mutable bool fDelegatedDebitCached;
    mutable bool fDelegatedCreditCached;
    mutable bool fStakeDelegationVoided;
    mutable CAmount nDebitCached;
    mutable CAmount nCreditCached;
    mutable CAmount nImmatureCreditCached;
    mutable CAmount nAvailableCreditCached;
    mutable CAmount nAnonymizableCreditCached;
    mutable CAmount nAnonymizedCreditCached;
    mutable CAmount nDenomUnconfCreditCached;
    mutable CAmount nDenomConfCreditCached;
    mutable CAmount nWatchDebitCached;
    mutable CAmount nWatchCreditCached;
    mutable CAmount nImmatureWatchCreditCached;
    mutable CAmount nAvailableWatchCreditCached;
    mutable CAmount nChangeCached;
    mutable CAmount nColdDebitCached;
    mutable CAmount nColdCreditCached;
    mutable CAmount nDelegatedDebitCached;
    mutable CAmount nDelegatedCreditCached;

    CWalletTx();
    CWalletTx(const CWallet* pwalletIn);
    CWalletTx(const CWallet* pwalletIn, const CMerkleTx& txIn);
    CWalletTx(const CWallet* pwalletIn, const CTransaction& txIn);
    void Init(const CWallet* pwalletIn);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (ser_action.ForRead())
            Init(NULL);
        char fSpent = false;

        if (!ser_action.ForRead()) {
            mapValue["fromaccount"] = strFromAccount;

            WriteOrderPos(nOrderPos, mapValue);

            if (nTimeSmart)
                mapValue["timesmart"] = strprintf("%u", nTimeSmart);
        }

        READWRITE(*(CMerkleTx*)this);
        std::vector<CMerkleTx> vUnused; //! Used to be vtxPrev
        READWRITE(vUnused);
        READWRITE(mapValue);
        READWRITE(vOrderForm);
        READWRITE(fTimeReceivedIsTxTime);
        READWRITE(nTimeReceived);
        READWRITE(fFromMe);
        READWRITE(fSpent);

        if (ser_action.ForRead()) {
            strFromAccount = mapValue["fromaccount"];

            ReadOrderPos(nOrderPos, mapValue);

            nTimeSmart = mapValue.count("timesmart") ? (unsigned int)atoi64(mapValue["timesmart"]) : 0;
        }

        mapValue.erase("fromaccount");
        mapValue.erase("version");
        mapValue.erase("spent");
        mapValue.erase("n");
        mapValue.erase("timesmart");
    }

    //! make sure balances are recalculated
    void MarkDirty();

    void BindWallet(CWallet* pwalletIn);
    //! checks whether a tx has P2CS inputs or not
    bool HasP2CSInputs() const;

    int GetDepthAndMempool(bool& fConflicted, bool enableIX = true) const;

    //! filter decides which addresses will count towards the debit
    CAmount GetDebit(const isminefilter& filter) const;
    CAmount GetCredit(const isminefilter& filter) const;
    CAmount GetUnspentCredit(const isminefilter& filter) const;
    CAmount GetImmatureCredit(bool fUseCache = true, const isminefilter& filter = ISMINE_SPENDABLE_ALL) const;
    CAmount GetAvailableCredit(bool fUseCache = true) const;
    // Return sum of unlocked coins
    CAmount GetUnlockedCredit() const;
    // Return sum of unlocked coins
    CAmount GetLockedCredit() const;
    CAmount GetImmatureWatchOnlyCredit(const bool& fUseCache = true) const;
    CAmount GetAvailableWatchOnlyCredit(const bool& fUseCache = true) const;
    CAmount GetLockedWatchOnlyCredit() const;
    CAmount GetChange() const;

    // Cold staking contracts credit/debit
    CAmount GetColdStakingCredit(bool fUseCache = true) const;
    CAmount GetColdStakingDebit(bool fUseCache = true) const;
    CAmount GetStakeDelegationCredit(bool fUseCache = true) const;
    CAmount GetStakeDelegationDebit(bool fUseCache = true) const;

    // Helper method to update the amount and cacheFlag.
    CAmount UpdateAmount(CAmount& amountToUpdate, bool& cacheFlagToUpdate, bool fUseCache, isminetype mimeType, bool fCredit = true) const;

    void GetAmounts(std::list<COutputEntry>& listReceived,
        std::list<COutputEntry>& listSent,
        CAmount& nFee,
        std::string& strSentAccount,
        const isminefilter& filter) const;

    void GetAccountAmounts(const std::string& strAccount, CAmount& nReceived, CAmount& nSent, CAmount& nFee, const isminefilter& filter) const;

    bool IsFromMe(const isminefilter& filter) const;

    bool InMempool() const;

    // True if only scriptSigs are different
    bool IsEquivalentTo(const CWalletTx& tx) const;

    bool IsTrusted() const;
    bool IsTrusted(int& nDepth, bool& fConflicted) const;

    bool WriteToDisk(CWalletDB *pwalletdb);

    int64_t GetTxTime() const;
    void UpdateTimeSmart();
    int GetRequestCount() const;
    void RelayWalletTransaction(std::string strCommand = "tx");
    std::set<uint256> GetConflicts() const;
};


class COutput
{
public:
    const CWalletTx* tx;
    int i;
    int nDepth;
    bool fSpendable;

    COutput(const CWalletTx* txIn, int iIn, int nDepthIn, bool fSpendableIn)
    {
        tx = txIn;
        i = iIn;
        nDepth = nDepthIn;
        fSpendable = fSpendableIn;
    }

    //Used with Anonsend. Will return largest nondenom, then denominations, then very small inputs
    int Priority() const
    {
        for (CAmount d : anonSendDenominations)
            if (tx->vout[i].nValue == d) return 10000;
        if (tx->vout[i].nValue < 1 * COIN) return 20000;

        //nondenom return largest first
        return -(tx->vout[i].nValue / COIN);
    }

    CAmount Value() const
    {
        return tx->vout[i].nValue;
    }

    std::string ToString() const;
};


/** Private key that includes an expiration date in case it never gets used. */
class CWalletKey
{
public:
    CPrivKey vchPrivKey;
    int64_t nTimeCreated;
    int64_t nTimeExpires;
    std::string strComment;
    //! todo: add something to note what created it (user, getnewaddress, change)
    //!   maybe should have a map<string, string> property map

    CWalletKey(int64_t nExpires = 0);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vchPrivKey);
        READWRITE(nTimeCreated);
        READWRITE(nTimeExpires);
        READWRITE(LIMITED_STRING(strComment, 65536));
    }
};


/**
 * Account information.
 * Stored in wallet with key "acc"+string account name.
 */
class CAccount
{
public:
    CPubKey vchPubKey;

    CAccount()
    {
        SetNull();
    }

    void SetNull()
    {
        vchPubKey = CPubKey();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vchPubKey);
    }
};


/**
 * Internal transfers.
 * Database key is acentry<account><counter>.
 */
class CAccountingEntry
{
public:
    std::string strAccount;
    CAmount nCreditDebit;
    int64_t nTime;
    std::string strOtherAccount;
    std::string strComment;
    mapValue_t mapValue;
    int64_t nOrderPos; //! position in ordered transaction list
    uint64_t nEntryNo;

    CAccountingEntry()
    {
        SetNull();
    }

    void SetNull()
    {
        nCreditDebit = 0;
        nTime = 0;
        strAccount.clear();
        strOtherAccount.clear();
        strComment.clear();
        nOrderPos = -1;
        nEntryNo = 0;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        //! Note: strAccount is serialized as part of the key, not here.
        READWRITE(nCreditDebit);
        READWRITE(nTime);
        READWRITE(LIMITED_STRING(strOtherAccount, 65536));

        if (!ser_action.ForRead()) {
            WriteOrderPos(nOrderPos, mapValue);

            if (!(mapValue.empty() && _ssExtra.empty())) {
                CDataStream ss(nType, nVersion);
                ss.insert(ss.begin(), '\0');
                ss << mapValue;
                ss.insert(ss.end(), _ssExtra.begin(), _ssExtra.end());
                strComment.append(ss.str());
            }
        }

        READWRITE(LIMITED_STRING(strComment, 65536));

        size_t nSepPos = strComment.find("\0", 0, 1);
        if (ser_action.ForRead()) {
            mapValue.clear();
            if (std::string::npos != nSepPos) {
                CDataStream ss(std::vector<char>(strComment.begin() + nSepPos + 1, strComment.end()), nType, nVersion);
                ss >> mapValue;
                _ssExtra = std::vector<char>(ss.begin(), ss.end());
            }
            ReadOrderPos(nOrderPos, mapValue);
        }
        if (std::string::npos != nSepPos)
            strComment.erase(nSepPos);

        mapValue.erase("n");
    }

private:
    std::vector<char> _ssExtra;
};

#endif // BITCOIN_WALLET_H
