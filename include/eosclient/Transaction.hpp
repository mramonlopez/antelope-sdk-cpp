//
//  Transaction.hpp
//  waxtestapp
//
//  Created by Manuel Ramon Lopez Torres on 30/12/22.
//

#ifndef Transaction_h
#define Transaction_h

#include <eosclient/Action.hpp>

namespace onikami {
namespace eosclient {

class TransactionExtension {
    uint16_t type;
    Buffer data;
};

class Transaction {
public:
    std::vector<Action> context_free_actions;
    std::vector<Action> actions;
    std::vector<TransactionExtension> transaction_extensions;
    
    uint32_t expiration;
    /** *Specifies a block num in the last 2^16 blocks. */
    uint16_t ref_block_num;
    /** Specifies the lower 32 bits of the block id. */
    uint32_t ref_block_prefix;
    /** Upper limit on total network bandwidth (in 8 byte words) billed for this transaction. */
    uint8_t max_net_usage_words;
    /** Upper limit on the total CPU time billed for this transaction. */
    uint8_t max_cpu_usage_ms;
    /** Number of seconds to delay this transaction for during which it may be canceled. */
    uint32_t delay_sec;
};

}}

#endif /* Transaction_h */
