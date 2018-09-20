//
// Created by benjamin on 14.09.18.
//

#ifndef HASHJOINS_NOP_JOIN_H
#define HASHJOINS_NOP_JOIN_H

#include <memory>
#include <vector>
#include <tuple>
#include "tuple.h"

namespace algorithms{

    /// Very simple no partitioning join using no threads at all
    class nop_join {

    public:
        /// Basic constructor, join will be running on 4 threads by default
        nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right);
        /// Join constructor with additional parameters
        nop_join(std::shared_ptr<std::vector<tuple>> left, std::shared_ptr<std::vector<tuple>> right,
                 double table_size);

        /// Performs the actual join and writes result
        void execute();

        /// Returns a pointer to the result vector
        std::shared_ptr<std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>> get();


    private:
        /// Left join partner
        std::shared_ptr<std::vector<tuple>> left;
        /// Right join partner
        std::shared_ptr<std::vector<tuple>> right;
        /// table_size*|left| is the size of the hash table being built
        double table_size;
        /// Result vector
        std::shared_ptr<std::vector<std::tuple<uint64_t, uint64_t, uint64_t>>> result;

        struct hash_table;

    };

} // namespace algorithms

#endif //HASHJOINS_NOP_JOIN_H