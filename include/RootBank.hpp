#pragma once

#include "mango_v3.hpp"
#include "solana.hpp"

namespace mango_v3 {
struct RootBank {
  RootBankInfo rootBankInfo;
  explicit RootBank(RootBankInfo accountInfo) { rootBankInfo = accountInfo; }
  explicit RootBank(std::string& pubKey,
                    const solana::rpc::Connection& connection) {
    auto accountInfo = connection.getAccountInfo<RootBankInfo>(pubKey);
    rootBankInfo = accountInfo;
  }
};
}  // namespace mango_v3
