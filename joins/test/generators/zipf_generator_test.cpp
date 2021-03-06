//
// Benjamin Wagner 2018
//

#include "generators/zipf_generator.h"
#include "gtest/gtest.h"
#include <unordered_set>

using namespace generators; // NOLINT

/*
 * These test cases are not exhaustive. We are not doing statistical
 * tests of the distribution itself but rather just a very basic one
 * ensuring range functionality.
 */

// Assert that the generated values are within range
TEST(ZipfTester, RangeTester) {
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(10);
    zipf_generator gen(1000, 0.25, count);
    gen.build();
    auto res = gen.get_vec_copy();
    ASSERT_EQ(gen.get_count(), count);

    for(uint64_t k = 0; k < count; ++k){
        ASSERT_LE(std::get<0>(res[k]), 1000);
    }
}