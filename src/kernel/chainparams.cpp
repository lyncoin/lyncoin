// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const char* extranonce)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << OP_0 << ParseHex(extranonce) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const char* pk, const char* extranonce)
{
    const char* pszTimestamp = "The Wall Street Journal 30/Dec/2022 Tumbling Bitcoin Prices Were Just One Element in a Terrible Year for Crypto";
    const CScript genesisOutputScript = CScript() << ParseHex(pk) << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward, extranonce);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 4320;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000001147a922b6ebe0ce9ac8fd");
        consensus.defaultAssumeValid = uint256S("0x8c60b9b2fa51cde874fbb6f774fc000d07a0ed7a7ffa95d6013414946397a6b8"); // height 157400

        consensus.nAuxpowChainId = 0x0b0d;
        consensus.nAuxpowStartHeight = 0;
        consensus.fStrictChainId = true;
        consensus.nLegacyBlocksBefore = 0;

        consensus.nSubsidyHalvingInterval2 = 43200;
        consensus.n2023Height = 48950;
        consensus.n2023Window = 10;
        consensus.n2023Timespan = 10 * 10 * 60;
        consensus.n2023Bits = 0x1908cf19;
        consensus.n2023Height2 = 71700;
        consensus.n2023Bits2 = 0x185c7bae;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x52;
        pchMessageStart[1] = 0x03;
        pchMessageStart[2] = 0x0e;
        pchMessageStart[3] = 0x2f;
        nDefaultPort = 5054;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1672403913, 3502459508, 0x1d00ffff, 1, 0 * COIN, "049dd1a54f4261cb766c34e4d72a3ca87fb9329d056191f409f2740b553a00a6c0c1b5901406d175584a5ebecfc470a1d96c76b813f1c4190fff6bbd0f9e95c7ed", "7defe4d1f46338cf0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000002b8761c63862f5047afb9ac5fdd1c67e87cd376c387628bc772bb39d"));
        assert(genesis.hashMerkleRoot == uint256S("0x90868a42e67370c4f543a97a896337db1f99c238b043115e9cdf8b0a09e6b1bc"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed.lyncoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,234);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,55);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,126);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x01, 0x9c, 0x35, 0x4f}; // Lpub
        base58Prefixes[EXT_SECRET_KEY] = {0x01, 0x9c, 0x31, 0x15}; // Lprv

        bech32_hrp = "lc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {      0, uint256S("0x000000002b8761c63862f5047afb9ac5fdd1c67e87cd376c387628bc772bb39d")},
                {   1000, uint256S("0xfad7864364b1bd1fa259b72ce7fbe29830615412f77675781f4e90f0996f607d")},
                {  10000, uint256S("0x000000000002ce00fce2f53c833902878c457c7ad04c411c7cb7c484e91327b5")},
                {  20000, uint256S("0x2ed35f5d1e12679cb9ee0761e055c84d6d9f016489732ef275a0bf8f0f6b1a30")},
                {  30000, uint256S("0x5427dbfe4804300d7a8f5a73ec1bda71ca1cdc0bbbba5e7518a6fa42358534e9")},
                {  40000, uint256S("0xff3c7d78cb9054a3602867f23958f441aacc87d29ec0968df4904ae9dc85c184")},
                {  50000, uint256S("0x7aa3a580745059f06694d2b3e91037d7b764a48c49d39a6463e8ebb94460ef74")},
                {  60000, uint256S("0x9eb558eb1779eeda5ad1cac490acc82f866754756da22e0db0806e540538a13c")},
                {  70000, uint256S("0x84f0df6fbcfeb6d864af1ba85ac62ab6bda03adc6ad54008b26a0bba702a7d35")},
                {  80000, uint256S("0x3934eef0bda0567058223c6b89bc20f5564a0e771762587b1f87a494f050f86c")},
                {  90000, uint256S("0x7f8fd1e35f306a4bb994bf43b9c1fb89df9b1d356064237e7c4aaeb9161b3e7b")},
                { 100000, uint256S("0x96833a9b1298412904c0b49f1004a733883e84907b8b8479391ccb3de058ce86")},
                { 110000, uint256S("0x39d2e22963a3b89fe59189181cc5f6e543073a693eb7ef2cb23c2e44f98b23d9")},
                { 120000, uint256S("0x7128ff745463066a4e471ed1bd7c12ea982193ce546478e02f42e690598c3861")},
                { 130000, uint256S("0xbfb9511bc2ab99c8940b84a8418d5e683790fa27e76d50dc4c5ae700ec01104d")},
                { 140000, uint256S("0x05e5824c026e4a5238e0eef0155af385ac5a89dfff60e1080f431422556cb333")},
                { 150000, uint256S("0x5831d97677cd1e8d7144d2168b2b0156e066a4d1ef538d07b33238e564b97d9a")},
                { 157400, uint256S("0x8c60b9b2fa51cde874fbb6f774fc000d07a0ed7a7ffa95d6013414946397a6b8")},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 d0daae07f4ed48f0d8bd5c3b2a7611eef32745786e3802f87a0caeaecb7cf1b2
            .nTime    = 1711096517,
            .nTxCount = 4332,
            .dTxRate  = 0.01040518241495349,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 4320;
        consensus.BIP16Height = 0;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 0;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000100010001");
        consensus.defaultAssumeValid = uint256{};

        consensus.nAuxpowChainId = 0x0229;
        consensus.nAuxpowStartHeight = 0;
        consensus.fStrictChainId = false;
        consensus.nLegacyBlocksBefore = 0;

        consensus.nSubsidyHalvingInterval = 43200;
        consensus.n2023Height = 0; // No activation delay
        consensus.n2023Window = 10;
        consensus.n2023Timespan = 10 * 10 * 60;
        consensus.n2023Bits = 0x1d00ffff;
        consensus.n2023Height2 = 0; // No activation delay
        consensus.n2023Bits2 = 0x1d00ffff;

        pchMessageStart[0] = 0x17;
        pchMessageStart[1] = 0x3a;
        pchMessageStart[2] = 0x5e;
        pchMessageStart[3] = 0x13;
        nDefaultPort = 5363;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1672403913, 3502459508, 0x1d00ffff, 1, 0 * COIN, "049dd1a54f4261cb766c34e4d72a3ca87fb9329d056191f409f2740b553a00a6c0c1b5901406d175584a5ebecfc470a1d96c76b813f1c4190fff6bbd0f9e95c7ed", "7defe4d1f46338cf0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000000002b8761c63862f5047afb9ac5fdd1c67e87cd376c387628bc772bb39d"));
        assert(genesis.hashMerkleRoot == uint256S("0x90868a42e67370c4f543a97a896337db1f99c238b043115e9cdf8b0a09e6b1bc"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("seed-testnet.lyncoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,168);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,15);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,221);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x02, 0x2d, 0xbf, 0x5b}; // Tpub
        base58Prefixes[EXT_SECRET_KEY] = {0x02, 0x2d, 0xbb, 0x21}; // Tprv

        bech32_hrp = "tlc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                { 0, uint256S("0x000000002b8761c63862f5047afb9ac5fdd1c67e87cd376c387628bc772bb39d")},
            }
        };

        m_assumeutxo_data = {

        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096
            .nTime    = 0,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae");
            vSeeds.emplace_back("seed.signet.bitcoin.sprovoost.nl.");

            // Hardcoded nodes can be removed once there are more DNS seeds
            vSeeds.emplace_back("seed-signet.lyncoin.com");

            consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000100010001");
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096
                .nTime    = 0,
                .nTxCount = 0,
                .dTxRate  = 0,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 4320;
        consensus.BIP16Height = 1;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1815; // 90% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000377ae000000000000000000000000000000000000000000000000000000");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nAuxpowChainId = 0x0610;
        consensus.nAuxpowStartHeight = 0;
        consensus.fStrictChainId = true;
        consensus.nLegacyBlocksBefore = 0;

        consensus.nSubsidyHalvingInterval = 43200;
        consensus.n2023Height = 0; // No activation delay
        consensus.n2023Window = 10;
        consensus.n2023Timespan = 10 * 10 * 60;
        consensus.n2023Bits = 0x1e0377ae;
        consensus.n2023Height2 = 0; // No activation delay
        consensus.n2023Bits2 = 0x1e0377ae;

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 5236;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1672404052, 3220913935, 0x1e0377ae, 1, 0 * COIN, "049dd1a54f4261cb766c34e4d72a3ca87fb9329d056191f409f2740b553a00a6c0c1b5901406d175584a5ebecfc470a1d96c76b813f1c4190fff6bbd0f9e95c7ed", "465bb7695ddccec60000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000000078784df3709f12a1b23492661623230ce15317b0b02ca98a054da72e"));
        assert(genesis.hashMerkleRoot == uint256S("0x6bfc3c6d6e2a0da18e6ba7a763dc66f108a74735a84ac9711e645aec1a0ebf75"));

        vFixedSeeds.clear();

        m_assumeutxo_data = {
            {
                .height = 160'000,
                .hash_serialized = AssumeutxoHash{uint256S("0xfe0a44309b74d6b5883d246cb419c6221bcccf0b308c9b59b7d70783dbdf928a")},
                .nChainTx = 2289496,
                .blockhash = uint256S("0x0000003ca3c99aff040f2563c2ad8f8ec88bd0fd6b8f0895cfaf1ef90353a62c")
            }
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,168);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,15);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,221);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x02, 0x2d, 0xbf, 0x5b}; // Tpub
        base58Prefixes[EXT_SECRET_KEY] = {0x02, 0x2d, 0xbb, 0x21}; // Tprv

        bech32_hrp = "tlc";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP16Height = 1; // Always active unless overridden
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        consensus.nAuxpowChainId = 0x07ef;
        consensus.nAuxpowStartHeight = 0;
        consensus.fStrictChainId = true;
        consensus.nLegacyBlocksBefore = 0;

        consensus.nSubsidyHalvingInterval = 1500;
        consensus.n2023Height = 0; // No activation delay
        consensus.n2023Window = 10;
        consensus.n2023Timespan = 10 * 10 * 60;
        consensus.n2023Bits = 0x207fffff;
        consensus.n2023Height2 = 0; // No activation delay
        consensus.n2023Bits2 = 0x207fffff;

        pchMessageStart[0] = 0xa8;
        pchMessageStart[1] = 0xc4;
        pchMessageStart[2] = 0x9e;
        pchMessageStart[3] = 0x69;
        nDefaultPort = 5260;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1672404137, 3553817527, 0x207fffff, 1, 0 * COIN, "049dd1a54f4261cb766c34e4d72a3ca87fb9329d056191f409f2740b553a00a6c0c1b5901406d175584a5ebecfc470a1d96c76b813f1c4190fff6bbd0f9e95c7ed", "fd83be1d6af7fd7e0100000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000000924f066756057e3b97d8d981b17fe7665e3664c65549fe9b47ba0699"));
        assert(genesis.hashMerkleRoot == uint256S("0x5bf3bab10ba0f6d3c89f65cfa6a834a4e2ecee1b694a7147cbf2e2f63a00b6ea"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("0x00000000924f066756057e3b97d8d981b17fe7665e3664c65549fe9b47ba0699")},
            }
        };

        m_assumeutxo_data = {

        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,168);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,15);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,221);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x02, 0x2d, 0xbf, 0x5b}; // Tpub
        base58Prefixes[EXT_SECRET_KEY] = {0x02, 0x2d, 0xbb, 0x21}; // Tprv

        bech32_hrp = "lcrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}
