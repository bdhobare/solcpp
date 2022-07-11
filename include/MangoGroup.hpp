#pragma once

#include "mango_v3.hpp"
#include "solana.hpp"

namespace mango_v3{
struct MangoGroup {
  MangoGroupInfo mangoGroupInfo;
  /// {Address,RootBankCache}
  std::unordered_map<std::string, RootBankCache> rootBankAccounts;
  explicit MangoGroup(const MangoGroupInfo& accountGroupInfo) noexcept {
    mangoGroupInfo = accountGroupInfo;
  }
  explicit MangoGroup(const std::string& pubKey,
                      const solana::rpc::Connection& connection){
    auto accountInfo = connection.getAccountInfo<MangoGroupInfo>(pubKey);
    mangoGroupInfo = accountInfo;
  }
  /// Sets this MangoGroup's `rootBankAccounts`
  auto loadRootBanks(solana::rpc::Connection& connection){
    this->rootBankAccounts = {};
    return std::unordered_map<std::string, RootBankCache>{};
  }
  auto getSpotMarketIndex(const solana::PublicKey& spotMarketPk){
    for(int i = 0; i < mangoGroupInfo.numOracles; i++){
      if (mangoGroupInfo.spotMarkets[i].spotMarket == spotMarketPk){
        return i;
      }
    }
    throw std::runtime_error("This Market does not belong to this MangoGroup: " + spotMarketPk.toBase58());
  }
  auto getPerpMarketIndex(const solana::PublicKey& perpMarketPk){
    for (int i = 0; i < mangoGroupInfo.numOracles; i++) {
      if (mangoGroupInfo.perpMarkets[i].perpMarket == perpMarketPk) {
        return i;
      }
    }
    throw std::runtime_error("This PerpMarket does not belong to this MangoGroup: " +  perpMarketPk.toBase58());
  }
  auto getTokenIndex(const solana::PublicKey& token){
    for (int i = 0; i < MAX_TOKENS; i++) {
      if (mangoGroupInfo.tokens[i].mint == token) {
        return i;
      }
    }
    throw std::runtime_error("This token does not belong in this MangoGroup: " + token.toBase58());
  }
  auto getRootBankIndex(const solana::PublicKey& rootBank){
    for (int i = 0; i < MAX_TOKENS; i++) {
      if (mangoGroupInfo.tokens[i].rootBank == rootBank) {
        return i;
      }
    }
    throw std::runtime_error("This root bank does not belong in this MangoGroup: " + rootBank.toBase58());
  }

};
}
