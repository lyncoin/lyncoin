// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/pureheader.h>

#include <hash.h>
#include <util/strencodings.h>

uint256 CPureBlockHeader::GetHash() const
{
    if(nVersion & 0x8000) {
        return (Hash3Writer{} << *this).GetHash();
    } else {
        return (HashWriter{} << *this).GetHash();
    }
}

uint256 CPureBlockHeader::GetHash(int32_t nBlockVersion) const
{
    if(nBlockVersion & 0x8000) {
        return (Hash3Writer{} << *this).GetHash();
    } else {
        return (HashWriter{} << *this).GetHash();
    }
}

uint256 CPureBlockHeader::GetHash2() const
{
    return (Hash4Writer{} << *this).GetHash();
}

uint256 CPureBlockHeader::GetPoWHash() const
{
    uint256 hash;
    uint256 hash2;
    std::string result;
    hash = GetHash();
    if(nVersion & 0x8000) {
        SQLiteCache cache("cache.db");
        result = cache.get(hash.ToString());
        if(result == "") {
            hash2 = GetHash2();
            cache.set(hash.ToString(), hash2.ToString());
        } else {
            hash2 = uint256S(result);
        }
        return hash2;
    }
    return hash;
}

uint256 CPureBlockHeader::GetPoWHash(int32_t nBlockVersion) const
{
    uint256 hash;
    uint256 hash2;
    std::string result;
    hash = GetHash(nBlockVersion);
    if(nBlockVersion & 0x8000) {
        SQLiteCache cache("cache.db");
        result = cache.get(hash.ToString());
        if(result == "") {
            hash2 = GetHash2();
            cache.set(hash.ToString(), hash2.ToString());
        } else {
            hash2 = uint256S(result);
        }
        return hash2;
    }
    return hash;
}

void CPureBlockHeader::SetBaseVersion(int32_t nBaseVersion, int32_t nChainId)
{
    assert(nBaseVersion >= 1 && nBaseVersion < VERSION_AUXPOW);
    assert(!IsAuxpow());
    nVersion = nBaseVersion | (nChainId * VERSION_CHAIN_START);
}
