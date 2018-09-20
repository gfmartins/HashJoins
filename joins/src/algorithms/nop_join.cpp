//
// Created by benjamin on 14.09.18.
//

#include "algorithms/nop_join.h"
#include <utility>
#include <mutex>

namespace algorithms{

    struct nop_join::hash_table{
        /// One of the hash table entries
        struct bucket{

            /// Overflow bucket used for chaining
            struct overflow{
                tuple t;
                std::unique_ptr<overflow> next;

                overflow(tuple t): t(t){}
            };

            uint32_t count;
            tuple t1;
            tuple t2;
            std::unique_ptr<overflow> next;

            /// Default constructor
            bucket(): count(0), next(nullptr) {}

        };

        std::unique_ptr<bucket[]> arr;

        explicit hash_table(uint64_t size){
            arr = std::make_unique<bucket[]>(size);
        }

    };

    nop_join::nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right):
       nop_join(std::move(left), std::move(right), 1.5){}

    nop_join::nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right, double table_size):
            left(left), right(right), table_size(table_size), result() {
        // Internally, the left table should always be the one with which we execute, i.e. the smaller one
        if((*left).size() >= (*right).size()){
            auto temp = right;
            right = left;
            left = right;
        }
    }

    void nop_join::execute() {
        auto new_size = static_cast<uint64_t>(1.5 * (*left).size());
        hash_table table = hash_table(new_size);
        // Build Phase
        for(tuple& curr: *left){
            uint64_t index = curr.value % new_size;
            hash_table::bucket& bucket = table.arr[index];
            switch(bucket.count){
                case 0:
                    bucket.t1 = curr;
                    break;
                case 1:
                    bucket.t2 = curr;
                    break;
                case 2:
                    bucket.next = std::make_unique<hash_table::bucket::overflow>(curr);
                    break;
                default:
                    hash_table::bucket::overflow* ptr = bucket.next.get();
                    // Follow pointer indirection
                    for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 3); i++){
                        ptr = ptr->next.get();
                    }
                    // Create new bucket containing tuple
                    ptr->next = std::make_unique<hash_table::bucket::overflow>(curr);
            }
            ++bucket.count;
        }
        // Probe Phase
        // Initialize result array
        result = std::make_shared<std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>>();
        for(tuple& curr: *right){
            uint64_t index = curr.value % new_size;
            hash_table::bucket& bucket = table.arr[index];
            // Follow overflow buckets
            if(bucket.count > 2){
                hash_table::bucket::overflow* curr_over = bucket.next.get();
                for(uint64_t i = 0; i < static_cast<uint64_t>(bucket.count - 2); ++i){
                    if(curr_over->t.value == curr.value){
                        result->push_back({curr_over->t.value, curr_over->t.rid, curr.rid});
                    }
                    curr_over = curr_over->next.get();
                }
            }
            // Look at second tuple
            if(bucket.count > 1 && bucket.t2.value == curr.value){
                result->push_back({bucket.t2.value, bucket.t2.rid, curr.rid});
            }
            // Look at first tuple
            if(bucket.count > 0 && bucket.t1.value == curr.value){
                result->push_back({bucket.t1.value, bucket.t1.rid, curr.rid});
            }
        }
    }

    std::shared_ptr<std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>> nop_join::get() {
        if(result == nullptr){
            throw std::logic_error("Join must be performed before querying results.");
        }
        return result;
    }

} // namespace algorithms