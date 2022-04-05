#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <bitset>

#include "fixedp.h"
#include "int128.hpp"
#include "solana.hpp"

namespace mango_v3 {
using json = nlohmann::json;

const int MAX_TOKENS = 16;
const int MAX_PAIRS = 15;
const int MAX_PERP_OPEN_ORDERS = 64;
const int INFO_LEN = 32;
const int QUOTE_INDEX = 15;
const int EVENT_SIZE = 200;
const int EVENT_QUEUE_SIZE = 256;

struct Config {
  std::string endpoint;
  std::string group;
  std::string program;
  std::vector<uint8_t> decimals;
  std::vector<std::string> symbols;
};

const Config MAINNET = {
    "https://mango.rpcpool.com/946ef7337da3f5b8d3e4a34e7f88",
    "98pjRuQjK3qA6gXts96PqZT4Ze5QmnCmt3QYjhbUSPue",
    "mv3ekLzLbnVPNxjSKvqBpU3ZeZXPQdEC3bp5MDEBG68",
    {6, 6, 6, 9, 6, 6, 6, 6, 6, 9, 8, 8, 6, 0, 0, 6},
    {"MNGO", "BTC", "ETH", "SOL", "USDT", "SRM", "RAY", "COPE", "FTT", "MSOL",
     "BNB", "AVAX", "LUNA", "", "", "USDC"}};

const Config DEVNET = {
    "https://mango.devnet.rpcpool.com",
    "Ec2enZyoC4nGpEfu2sUNAa2nUGJHWxoUWYSEJ2hNTWTA",
    "4skJ85cdxQAFVKbcGgfun8iZPL7BadVYXG3kGEGkufqA",
    {6, 6, 6, 9, 6, 6, 6, 6, 6, 9, 8, 8, 8, 0, 0, 6},
    {"MNGO", "BTC", "ETH", "SOL", "SRM", "RAY", "USDT", "ADA", "FTT", "AVAX",
     "LUNA", "BNB", "MATIC", "", "", "USDC"}};

const std::unordered_map<std::string, size_t> SERUM_PROGRAM_LAYOUT_VERSIONS = {
    {"4ckmDgGdxQoPDLUkDT3vHgSAkzA3QRdNq5ywwY4sUSJn", 1},
    {"BJ3jrUzddfuSrZHXSCxMUUQsjKEyLmuuyZebkcaFp2fg", 1},
    {"EUqojwWA2rd19FZrzeBncJsm38Jm1hEhE3zsmX3bRc2o", 2},
    {"9xQeWvG816bUx9EPjHmaT23yvVM2ZWbrrpZb9PusVFin", 3}
};
size_t  getLayoutVersion(std::string programId){
  auto it = SERUM_PROGRAM_LAYOUT_VERSIONS.find(programId);
  return (it != SERUM_PROGRAM_LAYOUT_VERSIONS.end())?it->second:3;
}
// all rust structs assume padding to 8
#pragma pack(push, 8)

struct MetaData {
  uint8_t dataType;
  uint8_t version;
  uint8_t isInitialized;
  uint8_t padding[5];
};

struct TokenInfo {
  solana::PublicKey mint;
  solana::PublicKey rootBank;
  uint8_t decimals;
  uint8_t padding[7];
};

struct SpotMarketInfo {
  solana::PublicKey spotMarket;
  i80f48 maintAssetWeight;
  i80f48 initAssetWeight;
  i80f48 maintLiabWeight;
  i80f48 initLiabWeight;
  i80f48 liquidationFee;
};

struct PerpMarketInfo {
  solana::PublicKey perpMarket;
  i80f48 maintAssetWeight;
  i80f48 initAssetWeight;
  i80f48 maintLiabWeight;
  i80f48 initLiabWeight;
  i80f48 liquidationFee;
  i80f48 makerFee;
  i80f48 takerFee;
  int64_t baseLotSize;
  int64_t quoteLotSize;
};

struct MangoGroup {
  MetaData metaData;
  uint64_t numOracles;
  TokenInfo tokens[MAX_TOKENS];
  SpotMarketInfo spotMarkets[MAX_PAIRS];
  PerpMarketInfo perpMarkets[MAX_PAIRS];
  solana::PublicKey oracles[MAX_PAIRS];
  uint64_t signerNonce;
  solana::PublicKey signerKey;
  solana::PublicKey admin;
  solana::PublicKey dexProgramId;
  solana::PublicKey mangoCache;
  uint64_t validInterval;
  solana::PublicKey insuranceVault;
  solana::PublicKey srmVault;
  solana::PublicKey msrmVault;
  solana::PublicKey feesVault;
  uint32_t maxMangoAccounts;
  uint32_t numMangoAccounts;
  uint8_t padding[24];
};

enum Side : uint8_t { Buy, Sell };

struct PerpAccountInfo {
  int64_t basePosition;
  i80f48 quotePosition;
  i80f48 longSettledFunding;
  i80f48 shortSettledFunding;
  int64_t bidsQuantity;
  int64_t asksQuantity;
  int64_t takerBase;
  int64_t takerQuote;
  uint64_t mngoAccrued;
};

struct MangoAccountInfo {
  MetaData metaData;
  solana::PublicKey mangoGroup;
  solana::PublicKey owner;
  bool inMarginBasket[MAX_PAIRS];
  uint8_t numInMarginBasket;
  i80f48 deposits[MAX_TOKENS];
  i80f48 borrows[MAX_TOKENS];
  solana::PublicKey spotOpenOrders[MAX_PAIRS];
  PerpAccountInfo perpAccounts[MAX_PAIRS];
  uint8_t orderMarket[MAX_PERP_OPEN_ORDERS];
  Side orderSide[MAX_PERP_OPEN_ORDERS];
  __int128_t orders[MAX_PERP_OPEN_ORDERS];
  uint64_t clientOrderIds[MAX_PERP_OPEN_ORDERS];
  uint64_t msrmAmount;
  bool beingLiquidated;
  bool isBankrupt;
  uint8_t info[INFO_LEN];
  solana::PublicKey advancedOrdersKey;
  bool notUpgradable;
  solana::PublicKey delegate;
  uint8_t padding[5];
};

// https://github.com/project-serum/serum-dex/blob/master/dex/src/state.rs#L587
#pragma pack(1)
struct OpenOrders {
  uint8_t padding0[5];
  // Account Flags:
  //  accountFlags[0] = "initialized"
  //  accountFlags[1] = "market"
  //  accountFlags[2] = "openOrders"
  //  accountFlags[3] = "requestQueue"
  //  accountFlags[4] = "eventQueue"
  //  accountFlags[5] = "bids"
  //  accountFlags[6] = "asks"
  std::bitset<64> accountFlags;
  solana::PublicKey market;
  solana::PublicKey owner;
  uint64_t baseTokenFree;
  uint64_t baseTokenTotal;
  uint64_t quoteTokenFree;
  uint64_t quoteTokenTotal;
  __uint128_t freeSlotBits;
  __uint128_t isBidBits;
  __uint128_t orders[128];
  uint64_t clientIds[128];
  uint64_t referrerRebatesAccrued;
  uint8_t padding1[7];
};

struct MangoAccount {
  MangoAccountInfo accountInfo;
  std::map<std::string, OpenOrders> spotOpenOrdersAccounts; // Address, AccountInfo
  explicit MangoAccount(const MangoAccountInfo &accountInfo_) noexcept {
    accountInfo = accountInfo_;
  }
  // Fetch `accountInfo` from `endpoint` and decode it
  explicit MangoAccount(const std::string &pubKey,
                        const std::string &endpoint = MAINNET.endpoint) {
    auto connection = solana::rpc::Connection(endpoint);
    const auto &accountInfo_ =
        connection.getAccountInfo<MangoAccountInfo>(pubKey);
    accountInfo = accountInfo_;
  }
  std::map<std::string, OpenOrders> loadOpenOrders(solana::rpc::Connection& connection);
};
std::map<std::string, OpenOrders> MangoAccount::loadOpenOrders(solana::rpc::Connection &connection) {
  // Filter only non-empty open orders
  std::vector<std::string> filteredOpenOrders;
  for(auto item: accountInfo.spotOpenOrders){
    if(item == solana::PublicKey::empty())
      continue;
    filteredOpenOrders.emplace_back(std::move(item.toBase58()));
  }
  // Fetch account info, OpenOrdersV2
  const auto accountsInfo = connection.getMultipleAccounts<OpenOrders>(filteredOpenOrders);
  spotOpenOrdersAccounts.clear();
  std::copy_if(accountsInfo.begin(), accountsInfo.end(),
               std::inserter(spotOpenOrdersAccounts, spotOpenOrdersAccounts.end()), [](auto& accountInfo){
    // Check initialized and OpenOrders account flags
    return !(!accountInfo.second.accountFlags[0]
           || !accountInfo.second.accountFlags[2]);
  });
  return spotOpenOrdersAccounts;
}

struct LiquidityMiningInfo {
  i80f48 rate;
  i80f48 maxDepthBps;
  uint64_t periodStart;
  uint64_t targetPeriodLength;
  uint64_t mngoLeft;
  uint64_t mngoPerPeriod;
};

struct PerpMarket {
  MetaData metaData;
  solana::PublicKey mangoGroup;
  solana::PublicKey bids;
  solana::PublicKey asks;
  solana::PublicKey eventQueue;
  int64_t quoteLotSize;
  int64_t baseLotSize;
  i80f48 longFunding;
  i80f48 shortFunding;
  int64_t openInterest;
  uint64_t lastUpdated;
  uint64_t seqNum;
  i80f48 feesAccrued;
  LiquidityMiningInfo liquidityMiningInfo;
  solana::PublicKey mngoVault;
};

struct EventQueueHeader {
  MetaData metaData;
  uint64_t head;
  uint64_t count;
  uint64_t seqNum;
};

enum EventType : uint8_t { Fill, Out, Liquidate };

struct AnyEvent {
  EventType eventType;
  uint8_t padding[EVENT_SIZE - 1];
};

struct FillEvent {
  EventType eventType;
  Side takerSide;
  uint8_t makerSlot;
  uint8_t makerOut;
  uint8_t version;
  uint8_t padding[3];
  uint64_t timestamp;
  uint64_t seqNum;
  solana::PublicKey maker;
  __int128_t makerOrderId;
  uint64_t makerClientOrderId;
  i80f48 makerFee;
  int64_t bestInitial;
  uint64_t makerTimestamp;
  solana::PublicKey taker;
  __int128_t takerOrderId;
  uint64_t takerClientOrderId;
  i80f48 takerFee;
  int64_t price;
  int64_t quantity;
};

struct LiquidateEvent {
  EventType eventType;
  uint8_t padding0[7];
  uint64_t timestamp;
  uint64_t seqNum;
  solana::PublicKey liqee;
  solana::PublicKey liqor;
  i80f48 price;
  int64_t quantity;
  i80f48 liquidationFee;
  uint8_t padding1[EVENT_SIZE - 128];
};

struct OutEvent {
  EventType eventType;
  Side side;
  uint8_t slot;
  uint8_t padding0[5];
  uint64_t timestamp;
  uint64_t seqNum;
  solana::PublicKey owner;
  int64_t quantity;
  uint8_t padding1[EVENT_SIZE - 64];
};

struct EventQueue {
  EventQueueHeader header;
  AnyEvent items[EVENT_QUEUE_SIZE];
};

#pragma pack(pop)

// instructions are even tighter packed, every byte counts
#pragma pack(push, 1)

namespace ix {
template <typename T>
std::vector<uint8_t> toBytes(const T &ref) {
  const auto bytePtr = (uint8_t *)&ref;
  return std::vector<uint8_t>(bytePtr, bytePtr + sizeof(T));
}

std::pair<int64_t, int64_t> uiToNativePriceQuantity(double price,
                                                    double quantity,
                                                    const Config &config,
                                                    const int marketIndex,
                                                    const PerpMarket &market) {
  const int64_t baseUnit = pow(10LL, config.decimals[marketIndex]);
  const int64_t quoteUnit = pow(10LL, config.decimals[QUOTE_INDEX]);
  const auto nativePrice = ((int64_t)(price * quoteUnit)) * market.baseLotSize /
                           (market.quoteLotSize * baseUnit);
  const auto nativeQuantity =
      ((int64_t)(quantity * baseUnit)) / market.baseLotSize;
  return std::pair<int64_t, int64_t>(nativePrice, nativeQuantity);
};

enum OrderType : uint8_t {
  Limit = 0,
  IOC = 1,
  PostOnly = 2,
  Market = 3,
  PostOnlySlide = 4
};

struct PlacePerpOrder {
  static const uint32_t CODE = 12;
  uint32_t ixs = CODE;
  int64_t price;
  int64_t quantity;
  uint64_t clientOrderId;
  Side side;
  OrderType orderType;
  uint8_t reduceOnly;
};

solana::Instruction placePerpOrderInstruction(
    const PlacePerpOrder &ixData, const solana::PublicKey &ownerPk,
    const solana::PublicKey &accountPk, const solana::PublicKey &marketPk,
    const PerpMarket &market, const solana::PublicKey &groupPk,
    const MangoGroup &group, const solana::PublicKey &programPk) {
  std::vector<solana::AccountMeta> accs = {
      {groupPk, false, false},    {accountPk, false, true},
      {ownerPk, true, false},     {group.mangoCache, false, false},
      {marketPk, false, true},    {market.bids, false, true},
      {market.asks, false, true}, {market.eventQueue, false, true},
  };
  for (int i = 0; i < MAX_PAIRS; ++i) {
    accs.push_back({{}, false, false});
  }
  return {programPk, accs, toBytes(ixData)};
};

struct CancelAllPerpOrders {
  static const uint32_t CODE = 39;
  uint32_t ixs = CODE;
  // limit the number of cancelled orders to stay within compute limits
  uint8_t limit;
};

solana::Instruction cancelAllPerpOrdersInstruction(
    const CancelAllPerpOrders &ixData, const solana::PublicKey &ownerPk,
    const solana::PublicKey &accountPk, const solana::PublicKey &marketPk,
    const PerpMarket &market, const solana::PublicKey &groupPk,
    const solana::PublicKey &programPk) {
  const std::vector<solana::AccountMeta> accs = {
      {groupPk, false, false},    {accountPk, false, true},
      {ownerPk, true, false},     {marketPk, false, true},
      {market.bids, false, true}, {market.asks, false, true},
  };
  return {programPk, accs, toBytes(ixData)};
};
}  // namespace ix

#pragma pack(pop)

}  // namespace mango_v3
