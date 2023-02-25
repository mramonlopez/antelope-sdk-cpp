//
//  Transaction.hpp
//  waxtestapp
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#ifndef Transaction_h
#define Transaction_h

#include <eosclient/Action.hpp>
#include <eosclient/types.hpp>

namespace onikami {
namespace eosclient {

class TransactionExtension {
public:
    uint16_t type;
    Buffer data;
};

class Transaction {
private:
    static const std::string transactionAbi_str;
    
public:
    std::vector<Action> context_free_actions;
    std::vector<Action> actions;
    std::vector<TransactionExtension> transaction_extensions;
    
    time_t expiration = 0;
    
    /** *Specifies a block num in the last 2^16 blocks. */
    uint16_t ref_block_num  = 0;
    
    /** Specifies the lower 32 bits of the block id. */
    uint32_t ref_block_prefix = 0;
    
    /** Upper limit on total network bandwidth (in 8 byte words) billed for this transaction. */
    uint8_t max_net_usage_words = 0;
    
    /** Upper limit on the total CPU time billed for this transaction. */
    uint8_t max_cpu_usage_ms = 0;
    
    /** Number of seconds to delay this transaction for during which it may be canceled. */
    uint32_t delay_sec = 0;
    
    Buffer context_free_data;
    
    Buffer getPackedData();
};

void to_json(nlohmann::json& j, const Transaction& t);
void from_json(const nlohmann::json& j, Transaction& t);

void to_json(nlohmann::json& j, const TransactionExtension& e);
void from_json(const nlohmann::json& j, TransactionExtension& e);

}}

#endif /* Transaction_h */
