// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "consensus/merkle.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"
#include "net.h"
#include "base58.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>
#include <limits>

#include "chainparamsseeds.h"

std::string CDNSSeedData::getHost(uint64_t requiredServiceBits) const {
    //use default host for non-filter-capable seeds or if we use the default service bits (NODE_NETWORK)
    if (!supportsServiceBitsFiltering || requiredServiceBits == NODE_NETWORK)
        return host;

    return strprintf("x%x.%s", requiredServiceBits, host);
}

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(42) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].SetEmpty();
    txNew.nTime = 1498159985;

    CBlock genesis;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.nVersion = nVersion;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    //printf("block genesis: %s\n", genesis.ToString().c_str());
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "E quando eu pensar em desistir, lembro-me dos motivos que te fizeram aguentar ate agora!";
    const CScript genesisOutputScript = CScript() << ParseHex("042406e703f2d473fbbdfb4b66970061c3555f9938ea221e08d9ca64260f10b102fac36cf6b5684f25bbf37025892f892d894fef863ca01814e51623cacd2b41db") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (      0, uint256("617fc383b07fbab3505213b41fe34f9705b92d854a9f72593cb616e4726e155c"))
    (   1500, uint256("0xb5a149ba0d464837b3a7125242260b4c84b0ec9918a9f84ba3d1efb7142b912a"))
    (   9918, uint256("0xbeb9f4fe6264d21912923923d35a490b529e6221b474808309cf48245a33d14e"))
    (  16912, uint256("0x2a2816aa9e0f5a6d5a2ef4f375e1c83481187935d6a7823fd01f49d409d6ed8f"))
    (  23912, uint256("0x2e1854683abfa6458691c907dbab8cfd77a6e0ebe6117b774a8f27424a1760d0"))
    (  35457, uint256("0xf88790ce6476cd4b694d17512c5f1f34d50b747462635efdf9e3dc8f3d6b8362"))
    (  45479, uint256("0xf848d60c6483997770dac2e88f48b8128762c47329393df7e57954ad6157cde8"))
    (  55895, uint256("0x757f1d1289d115151c30539ecac339c125d866c7267c754fd337b168ca7156d6"))
    (  68899, uint256("0x91a5c5df389e5983013be36f9d0870f08bb5f78bdaec32582cf5aa0793691772"))
    (  74619, uint256("0xeeb023a6a7ca4ff9f3007abb19d3900107eea11f72363c97618924f4c4702374"))
    (  75095, uint256("0x8b2eb0051856ce9f8c31afc495b3cfb948a97244663409c7418924a459d24369"))
    (  88805, uint256("0x407da90696cd4f3db6a3fa65dd5f14818cd559fad084e7421d4cd9138a32ed0d"))
    ( 107996, uint256("0x399401999d00f054497b0a5189957d568237d417b78af09bbf2f0071c84e9840"))
    ( 137993, uint256("0x230ac694157c523e3b0894587f5a4f003e180b913b049489f6517ab473585b73"))
    ( 167996, uint256("0x3fe6760f6daf5e9371c3d24c915174cf0bba3ec541e1bb9517f6f19dbcb7d018"))
    ( 207992, uint256("0x05bd2ed01f6a7815bddf30b56bef0f4a8c00da9156e43f6a9e8e3e25487c93b7"))
    ( 312645, uint256("0x00484a83353616fbde8d86a93c48e89c4facbf993126acfc6c163559ccf08735"))
    ( 407452, uint256("0xef122d0c6e0f1d71b9f803e58ce52e952d186a7a386703001df8ff1dc3fb7f57"))
    ( 523412, uint256("0x9b9a7952624668f7a8e06fc592b371b98d5355c575c9285243a2ba41069d3a11"))
    ( 523930, uint256("0xb7e4b36ec249c665e64f1e93e370f2a385a4f732610c2b17d22e18a51309bf62"))
    ( 750000, uint256("0x41d0a1255a06ec652bd525ecb3fe280efa7b1020139642cf4412b34b491ede68"))
    ( 888900, uint256("0x8475b4ab4bd41b150466c32c8188cf57ba22336cc6576f68dc9fc56aea5510cd"))
    ( 967800, uint256("0xbd953dea29cce275befc317e5d772e3e5dce6e811a6257ffdeb3a00d6fc11541"))
    (1680167, uint256("0xe4ff3619d84659b7f7c3ff4ab92124d116d3c35414145f31fc42743fd92e8047"))
    (1775837, uint256("0x514166616dd8c1dd50e0fdf224fbe1a6f4f6a911cc0aa7bf9d90d3ec5f3d37a2"))
    (1836997, uint256("0xaca8b0a56715316704bbca364e0d5914a72e26f80d258d11f5ce9666bf901632"))
    (1920869, uint256("0x00299e02c3418ef5afadf1c1d6b0e529f4d169428d9cf6cbbbb8baaa388963b4"));
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1587396559, // * UNIX timestamp of last checkpoint block
    3013596,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    100         // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (      0, uint256S("0xbe10a5eb2ff7c7f2c958826bc773b05748cf3c8c851744425e1af311ed36e502"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1558746545,
    473679,
    100};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256S("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1454124731,
    0,
    100};

std::string CChainParams::GetEcoFundAddressAtHeight(int nHeight) const {
    return vEcoFundAddress;
}

CScript CChainParams::GetEcoFundScriptAtHeight(int nHeight) const {
    CBitcoinAddress address(GetEcoFundAddressAtHeight(nHeight).c_str());
    assert(address.IsValid());

    CScript script = GetScriptForDestination(address.Get());
    return script;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";

        genesis = CreateGenesisBlock(1498159985, 857701, 0x1e3fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256("617fc383b07fbab3505213b41fe34f9705b92d854a9f72593cb616e4726e155c"));
        assert(genesis.hashMerkleRoot == uint256("04035aa1d2a55488b3a8e4f84beb66e30a428140e88071ac83b7b333b0425e48"));

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.powLimit   = ~UINT256_ZERO >> 20;   // MARTEX starting difficulty is 1 / 2^12
        consensus.posLimitV1 = ~UINT256_ZERO >> 24;
        consensus.posLimitV2 = ~UINT256_ZERO >> 24;
	consensus.nBudgetPaymentsStartBlock = 1593000; // actual historical value
        consensus.nBudgetCycleBlocks = 21800;       // approx. 1 every 30 days
        consensus.nBudgetFeeConfirmations = 6;      // Number of confirmations for the finalization fee
        consensus.nCoinbaseMaturity = 3;
        consensus.nCoinbaseMaturity_NEW = 150;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMasternodeCountDrift = 20;       // num of MN we allow the see-saw payments to be off by
        consensus.nMaxMoneyOut = 11600000 * COIN;
        consensus.nPoolMaxTransactions = 3;
        consensus.nProposalEstablishmentTime = 60 * 60 * 24;    // must be at least a day old to make it into a budget
        consensus.nStakeMinAge = 2 * 60 * 60; //2 hours
        consensus.nStakeMinDepth = 60;
        consensus.nTargetTimespan = 10 * 60;
        consensus.nTargetTimespanV2 = 10 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;
        consensus.strAnonsendPoolDummyAddress = "MARTEXC5boqkfW1JJWtKLjSKJEfPMgrcJA";
	consensus.nMasternodeCollateral = 5000;
	vEcoFundAddress = "MARTEXC5boqkfW1JJWtKLjSKJEfPMgrcJA";

        // spork keys
        consensus.strSporkPubKey = "047E86214867FF3AD73D1C2BC52A1643B104245BF8ED54C62C6A65F8F37393A0F17911A301F36A07E672E901F1489B10728582A7FCA273A200C7643C6E29B1CCE5";

        // height-based activations
        consensus.height_last_PoW = 1950000;
        consensus.height_RHF = 2500000;
        consensus.height_last_ZC_AccumCheckpoint = consensus.height_RHF - 100;
        consensus.height_start_BIP65 = consensus.height_RHF;             // 82629b7a9978f5c7ea3f70a12db92633a7d2e436711500db28b97efd48b1e527
        consensus.height_start_StakeModifierNewSelection = 1;
        consensus.height_start_StakeModifierV2 = consensus.height_RHF;
        consensus.height_start_TimeProtoV2 = consensus.height_RHF;       // TimeProtocolV2, Blocks V7
        consensus.height_start_ZC = consensus.height_RHF;
        consensus.height_start_ZC_PublicSpends = consensus.height_RHF;
        consensus.height_start_ZC_SerialRangeCheck = consensus.height_RHF;
        consensus.height_start_ZC_SerialsV2 = consensus.height_RHF;

        // Zerocoin-related params
        consensus.ZC_Modulus = "d59f1d99dae2770f40fb82066b6f69bb0b3783113505ecf4d958a6021d7204a8612d7c824741ac69cbf426ba4056a0598f2683c54a72c9162821864da23add323b9af365c63d1c60af802a15c3961c4a23a0a4b8f8d0cd681faf9ff5f308a9d8348993a7f5e2560bdc4274aaa670878562ad8774c7fa15ec449385a7e3f2621b152e1f9978890cf02058d3f00d7ed1fc2fba76fe2b8358205dec3f0bd0b648b995f84b74e34ae77a2c134033075cf966b4339f028e039ce8200e279bd0169cf5994a4b135699280fa7be8f0328cfcaa1f7dc7cabe18ba0ec6f42e00792b3f128ec64fee8eb9306b871f6514946649d3fa2247c62ecd5050914570bb35b035fa80ef0995006790eb5ef2e383e7919b7e1aea89f59917c1a7adfdb1a73239c09e191cdde217c53ba0bf96ac9c265054aef811da8b51b1b3ea31d96f5d1ab9acf87363be80f42acf7353b3c4a5297eb3f5676f04b987a3144c5b04d1f6f3fdec243bab3fa2f463a1c50be50b49c156c421befad74c9b6f4367149163d3796355331";
        consensus.ZC_MaxPublicSpendsPerTx = 637;    // Assume about 220 bytes each input
        consensus.ZC_MaxSpendsPerTx = 7;            // Assume about 20kb each input
        consensus.ZC_MinMintConfirmations = 20;
        consensus.ZC_MinMintFee = 1 * CENT;
        consensus.ZC_MinStakeDepth = 200;
        consensus.ZC_TimeStart = 4070908800ULL;     // never

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x2d;
        pchMessageStart[1] = 0x3f;
        pchMessageStart[2] = 0xa2;
        pchMessageStart[3] = 0xf5;
	nDefaultPort = 51315;

        // Note that of those with the service bits flag, most only support a subset of possible options
        vSeeds.push_back(CDNSSeedData("seed0", "seed.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed1", "seed1.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed2", "seed2.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed3", "seed3.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed4", "seed4.martexcoin.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,50);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
	base58Prefixes[STAKING_ADDRESS] = std::vector<unsigned char>(1, 110);     // starting with 'm'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1, 178);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
	// BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        nExtCoinType = 180;

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));
	fSkipProofOfWorkCheck = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }

};
static CMainParams mainParams;

/**
 * Testnet (v4)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";

        genesis = CreateGenesisBlock(1498159985, 857701, 0x1f00ffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xbe10a5eb2ff7c7f2c958826bc773b05748cf3c8c851744425e1af311ed36e502"));

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.powLimit   = ~UINT256_ZERO >> 20;   // MARTEX starting difficulty is 1 / 2^12
        consensus.posLimitV1 = ~UINT256_ZERO >> 20;
        consensus.posLimitV2 = ~UINT256_ZERO >> 20;
        consensus.nBudgetCycleBlocks = 144;         // approx 10 cycles per day
        consensus.nBudgetFeeConfirmations = 3;      // (only 8-blocks window for finalization on testnet)
        consensus.nCoinbaseMaturity = 1;
	consensus.nCoinbaseMaturity_NEW = 3;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMasternodeCountDrift = 4;        // num of MN we allow the see-saw payments to be off by
        consensus.nMaxMoneyOut = 12000000 * COIN;
        consensus.nPoolMaxTransactions = 2;
        consensus.nProposalEstablishmentTime = 60 * 5;  // at least 5 min old to make it into a budget
        consensus.nStakeMinAge = 60 * 60; //1 hour
        consensus.nStakeMinDepth = 30;
        consensus.nTargetTimespan = 10 * 60;
        consensus.nTargetTimespanV2 = 10 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;
        consensus.strAnonsendPoolDummyAddress = "mvcNScAif4qiBLKyaVZxXnN2C84dxowq6d";
	consensus.nMasternodeCollateral = 5000;
	vEcoFundAddress = "mvcNScAif4qiBLKyaVZxXnN2C84dxowq6d";

        // spork keys
        consensus.strSporkPubKey = "0457b7cb37995c7db0e17e50e83fd1e09c2d37329a66c4296466f2d043650a4aef5d13d344e8b6fb3ae6231712a130f94cbf59d81d97d386f9e6d6177f3ac8f234";

        // height-based activations
        consensus.height_last_PoW = 250;
        consensus.height_RHF = 999999999;
        consensus.height_last_ZC_AccumCheckpoint = consensus.height_RHF - 100;
        consensus.height_start_BIP65 = consensus.height_RHF;             // 82629b7a9978f5c7ea3f70a12db92633a7d2e436711500db28b97efd48b1e527
        consensus.height_start_StakeModifierNewSelection = 1;
        consensus.height_start_StakeModifierV2 = consensus.height_RHF;
        consensus.height_start_TimeProtoV2 = consensus.height_RHF;       // TimeProtocolV2, Blocks V7
        consensus.height_start_ZC = consensus.height_RHF;
        consensus.height_start_ZC_PublicSpends = consensus.height_RHF;
        consensus.height_start_ZC_SerialRangeCheck = consensus.height_RHF;
        consensus.height_start_ZC_SerialsV2 = consensus.height_RHF;

        // Zerocoin-related params
        consensus.ZC_Modulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
                "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
                "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
                "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
                "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
                "31438167899885040445364023527381951378636564391212010397122822120720357";
        consensus.ZC_MaxPublicSpendsPerTx = 637;    // Assume about 220 bytes each input
        consensus.ZC_MaxSpendsPerTx = 7;            // Assume about 20kb each input
        consensus.ZC_MinMintConfirmations = 20;
        consensus.ZC_MinMintFee = 1 * CENT;
        consensus.ZC_MinStakeDepth = 200;
	consensus.ZC_TimeStart = 4070908800ULL;     // never

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x70;
        pchMessageStart[1] = 0x35;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0x05;
        nDefaultPort = 41315;

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("seed0", "testnet.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed1", "testnet1.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed2", "testnet2.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed3", "testnet3.martexcoin.org"));
        vSeeds.push_back(CDNSSeedData("seed4", "testnet4.martexcoin.org"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 111); // Testnet MARTEX addresses start with 'm'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 196); // Testnet MARTEX script addresses start with '8' or '9'
        base58Prefixes[STAKING_ADDRESS] = std::vector<unsigned char>(1, 50); // starting with 'M'
        base58Prefixes[SECRET_KEY] = boost::assign::list_of(324).convert_to_container<std::vector<unsigned char> >();     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet MARTEX BIP32 pubkeys start with 'DRKV'
	base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        // Testnet MARTEX BIP32 prvkeys start with 'DRKP'
	base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        // Testnet MARTEX BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));
	fSkipProofOfWorkCheck = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";

        genesis = CreateGenesisBlock(1454124731, 2402015, 0x1e0ffff0, 1, 250 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // assert(consensus.hashGenesisBlock == uint256("0x0000041e482b9b9691d98eefb48473405c0b8ec31b76df3797c74a78680ef818"));
        // assert(genesis.hashMerkleRoot == uint256("0x1b2ef6e2f28be914103a277377ae7729dcd125dfeb8bf97bd5964ba72b6dc39b"));

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.powLimit   = ~UINT256_ZERO >> 20;   // MARTEX starting difficulty is 1 / 2^12
        consensus.posLimitV1 = ~UINT256_ZERO >> 24;
        consensus.posLimitV2 = ~UINT256_ZERO >> 20;
        consensus.nBudgetCycleBlocks = 144;         // approx 10 cycles per day
        consensus.nBudgetFeeConfirmations = 3;      // (only 8-blocks window for finalization on regtest)
        consensus.nCoinbaseMaturity = 100;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMasternodeCountDrift = 4;        // num of MN we allow the see-saw payments to be off by
        consensus.nMaxMoneyOut = 43199500 * COIN;
        consensus.nPoolMaxTransactions = 2;
        consensus.nProposalEstablishmentTime = 60 * 5;  // at least 5 min old to make it into a budget
        consensus.nStakeMinAge = 0;
        consensus.nStakeMinDepth = 2;
        consensus.nTargetTimespan = 40 * 60;
        consensus.nTargetTimespanV2 = 30 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;
        consensus.strAnonsendPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";

        /* Spork Key for RegTest:
        WIF private key: 932HEevBSujW2ud7RfB1YF91AFygbBRQj3de3LyaCRqNzKKgWXi
        private key hex: bd4960dcbd9e7f2223f24e7164ecb6f1fe96fc3a416f5d3a830ba5720c84b8ca
        Address: yCvUVd72w7xpimf981m114FSFbmAmne7j9
        */
        consensus.strSporkPubKey = "043969b1b0e6f327de37f297a015d37e2235eaaeeb3933deecd8162c075cee0207b13537618bde640879606001a8136091c62ec272dd0133424a178704e6e75bb7";

        // height based activations
        consensus.height_last_PoW = 250;
        consensus.height_last_ZC_AccumCheckpoint = 310;     // no checkpoints on regtest
        consensus.height_start_BIP65 = 851019;              // Not defined for regtest. Inherit TestNet value.
        consensus.height_start_StakeModifierNewSelection = 0;
        consensus.height_start_StakeModifierV2 = 251;       // start with modifier V2 on regtest
        consensus.height_start_TimeProtoV2 = 999999999;
        consensus.height_start_ZC = 300;
        consensus.height_start_ZC_PublicSpends = 400;
        consensus.height_start_ZC_SerialRangeCheck = 300;
        consensus.height_start_ZC_SerialsV2 = 300;

        // Zerocoin-related params
        consensus.ZC_Modulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
                "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
                "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
                "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
                "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
                "31438167899885040445364023527381951378636564391212010397122822120720357";
        consensus.ZC_MaxPublicSpendsPerTx = 637;    // Assume about 220 bytes each input
        consensus.ZC_MaxSpendsPerTx = 7;            // Assume about 20kb each input
        consensus.ZC_MinMintConfirmations = 10;
        consensus.ZC_MinMintFee = 1 * CENT;
        consensus.ZC_MinStakeDepth = 10;
        consensus.ZC_TimeStart = 0;                 // not implemented on regtest


        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */

        pchMessageStart[0] = 0xa1;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xac;
        nDefaultPort = 51476;

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

static CChainParams* pCurrentParams = 0;

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
