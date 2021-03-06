// Copyright (c) 2018 The LBRY developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CLAIMSCRIPTOP_H
#define BITCOIN_CLAIMSCRIPTOP_H

#include <amount.h>
#include <claimtrie/forks.h>
#include <claimtrie/uints.h>
#include <primitives/transaction.h>
#include <script/script.h>

#include <string>
#include <vector>

/**
 * Claim script operation base class
 */
class CClaimScriptOp
{
public:
    virtual ~CClaimScriptOp() = default;
    /**
     * Pure virtual, OP_CLAIM_NAME handler
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     */
    virtual bool claimName(CClaimTrieCache& trieCache, const std::string& name) = 0;
    /**
     * Pure virtual, OP_UPDATE_CLAIM handler
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool updateClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) = 0;
    /**
     * Pure virtual, OP_SUPPORT_CLAIM handler
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool supportClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) = 0;
};

/**
 * Class to add claim in trie
 */
class CClaimScriptAddOp : public CClaimScriptOp
{
public:
    /**
     * Constructor
     * @param[in] point     pair of transaction hash and its index
     * @param[in] nValue    value of the claim
     * @param[in] nHeight   entry height of the claim
     */
    CClaimScriptAddOp(const COutPoint& point, CAmount nValue, int nHeight);
    /**
     * Implementation of OP_CLAIM_NAME handler
     * @see CClaimScriptOp::claimName
     */
    bool claimName(CClaimTrieCache& trieCache, const std::string& name) override;
    /**
     * Implementation of OP_UPDATE_CLAIM handler
     * @see CClaimScriptOp::updateClaim
     */
    bool updateClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;
    /**
     * Implementation of OP_SUPPORT_CLAIM handler
     * @see CClaimScriptOp::supportClaim
     */
    bool supportClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;

protected:
    /**
     * Reimplement to handle OP_CLAIM_NAME and OP_UPDATE_CLAIM at once
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool addClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId,
            int takeoverHeight, int originalHeight);
    const COutPoint point;
    const CAmount nValue;
    const int nHeight;
};

/**
 * Class to undo added claim in trie
 */
class CClaimScriptUndoAddOp : public CClaimScriptOp
{
public:
    /**
     * Constructor
     * @param[in] point     pair of transaction hash and its index
     * @param[in] nHeight   entry height of the claim
     */
    CClaimScriptUndoAddOp(const COutPoint& point, int nHeight);
    /**
     * Implementation of OP_CLAIM_NAME handler
     * @see CClaimScriptOp::claimName
     */
    bool claimName(CClaimTrieCache& trieCache, const std::string& name) override;
    /**
     * Implementation of OP_UPDATE_CLAIM handler
     * @see CClaimScriptOp::updateClaim
     */
    bool updateClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;
    /**
     * Implementation of OP_SUPPORT_CLAIM handler
     * @see CClaimScriptOp::supportClaim
     */
    bool supportClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;

protected:
    /**
     * Reimplement to handle OP_CLAIM_NAME and OP_UPDATE_CLAIM at once
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool undoAddClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId);
    const COutPoint point;
    const int nHeight;
};

/**
 * Class to spend claim from trie
 */
class CClaimScriptSpendOp : public CClaimScriptOp
{
public:
    /**
     * Constructor
     * @param[in]  point        pair of transaction hash and its index
     * @param[in]  nHeight      entry height of the claim
     * @param[out] nValidHeight valid height of the claim
     */
    CClaimScriptSpendOp(const COutPoint& point, int nHeight, int& nValidHeight);
    /**
     * Implementation of OP_CLAIM_NAME handler
     * @see CClaimScriptOp::claimName
     */
    bool claimName(CClaimTrieCache& trieCache, const std::string& name) override;
    /**
     * Implementation of OP_UPDATE_CLAIM handler
     * @see CClaimScriptOp::updateClaim
     */
    bool updateClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;
    /**
     * Implementation of OP_SUPPORT_CLAIM handler
     * @see CClaimScriptOp::supportClaim
     */
    bool supportClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;

protected:
    /**
     * Reimplement to handle OP_CLAIM_NAME and OP_UPDATE_CLAIM at once
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool spendClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId);
    const COutPoint point;
    const int nHeight;
    int& nValidHeight;
    int nOriginalHeight;
};

/**
 * Class to undo spent claim from trie
 */
class CClaimScriptUndoSpendOp : public CClaimScriptOp
{
public:
    /**
     * Constructor
     * @param[in] point         pair of transaction hash and its index
     * @param[in] nValue        value of the claim
     * @param[in] nHeight       entry height of the claim
     * @param[in] nValidHeight  valid height of the claim
     */
    CClaimScriptUndoSpendOp(const COutPoint& point, CAmount nValue, int nHeight, int nValidHeight, int nOriginalHeight);
    /**
     * Implementation of OP_CLAIM_NAME handler
     * @see CClaimScriptOp::claimName
     */
    bool claimName(CClaimTrieCache& trieCache, const std::string& name) override;
    /**
     * Implementation of OP_UPDATE_CLAIM handler
     * @see CClaimScriptOp::updateClaim
     */
    bool updateClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;
    /**
     * Implementation of OP_SUPPORT_CLAIM handler
     * @see CClaimScriptOp::supportClaim
     */
    bool supportClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId) override;

protected:
    /**
     * Reimplement to handle OP_CLAIM_NAME and OP_UPDATE_CLAIM at once
     * @param[in] trieCache     trie to operate on
     * @param[in] name          name of the claim
     * @param[in] claimId       id of the claim
     */
    virtual bool undoSpendClaim(CClaimTrieCache& trieCache, const std::string& name, const uint160& claimId);
    const COutPoint point;
    const CAmount nValue;
    const int nHeight;
    const int nValidHeight;
    const int nOriginalHeight;
};

/**
 * Function to process operation on claim
 * @param[in] claimOp           operation to be performed
 * @param[in] trieCache         trie to operate on
 * @param[in] scriptPubKey      claim script to be decoded
 */
bool ProcessClaim(CClaimScriptOp& claimOp, CClaimTrieCache& trieCache, const CScript& scriptPubKey);

struct spentClaimType { std::string name; uint160 id; int originalHeight; };
typedef std::vector<spentClaimType> spentClaimsType;

struct CUpdateCacheCallbacks
{
    std::function<CScript(const COutPoint& point)> findScriptKey;
    std::function<void(uint32_t, uint32_t, uint32_t)> claimUndoHeights;
};

/**
 * Function to spend claim from tie, keeping the successful list on
 * @param[in]  tx               transaction inputs/outputs
 * @param[in]  trieCache        trie to operate on
 * @param[in]  view             coins cache
 * @param[in]  point            pair of transaction hash and its index
 * @param[in]  nHeight          entry height of the claim
 * @param[out] fallback         optional callbacks
 */
void UpdateCache(const CTransaction& tx, CClaimTrieCache& trieCache, const CCoinsViewCache& view, int nHeight, const CUpdateCacheCallbacks& callbacks = {});

#endif // BITCOIN_CLAIMSCRIPTOP_H
