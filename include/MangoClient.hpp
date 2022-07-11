#pragma once

#include "MangoAccount.hpp"
#include "MangoGroup.hpp"
#include "mango_v3.hpp"

namespace mango_v3 {
///  Automatically fetch MangoAccounts for this PerpMarket
///  Pick enough MangoAccounts that have opposite sign and send them in to get settled
/// @return
std::string settlePnl(MangoGroup& mangoGroup,
                      MangoCache& mangoCache,
                      PerpMarket& perpMarket,
                      RootBankCache quoteRootBank,
                      i80f48 price, // should be the MangoCache price
                      solana::Keypair owner,
                      std::vector<MangoAccount> mangoAccounts){
  /// fetch all MangoAccounts filtered for having this perp market in basket
  // const marketIndex = mangoGroup.getPerpMarkeyIndex()

  return "";
}
}