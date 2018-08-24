// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "crypto/scrypt/scrypt.h"
#include "crypto/hashargon2d.h"
#include "crypto/yescrypt/yescrypt.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "crypto/common.h"

#define BEGIN(a)            ((char*)&(a))
#define END(a)              ((char*)&((&(a))[1]))

uint256 CBlockHeader::GetHash() const
{
    //WIP
    return Hash(BEGIN(nVersion), END(nNonce));
    //return HashX11(BEGIN(nVersion), END(nNonce));
}

uint256 CBlockHeader::GetPoWHash(int algo, const Consensus::Params& consensusParams) const
{
    LogPrintf("DEBUG: GetPoWHash %d \n",algo);
    switch (algo)
    {
        case ALGO_SLOT1:
            return GetHash();
        case ALGO_SLOT2:
            uint256 thash;
            // Caution: scrypt_1024_1_1_256 assumes fixed length of 80 bytes
            scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
            return thash;
        case ALGO_SLOT3:
            return GetHash(); // TODO: till need to implement these libraries, may change algo
        case ALGO_SLOT4:
            return HashArgon2d(BEGIN(nVersion), END(nNonce));
        case ALGO_SLOT5:
                uint256 thash;
                yescrypt_hash(BEGIN(nVersion), BEGIN(thash));
                return thash;           
    }
    // catch-all if above doesn't match anything to algo
    return GetHash();
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }
    return s.str();
}

int GetAlgo(int nVersion)
{
    switch (nVersion & BLOCK_VERSION_ALGO)
    {
        case 0:
            return ALGO_SLOT1;
        case BLOCK_VERSION_SLOT2:
            return ALGO_SLOT2;
        case BLOCK_VERSION_SLOT3:
            return ALGO_SLOT3;
        case BLOCK_VERSION_SLOT4:
            return ALGO_SLOT4;
        case BLOCK_VERSION_SLOT5:
            return ALGO_SLOT5;            
    }
    return ALGO_SLOT1;
}
