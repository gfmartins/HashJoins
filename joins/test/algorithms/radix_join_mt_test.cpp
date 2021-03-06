//
// Benjamin Wagner 2018
//


#include "generators/uniform_generator.h"
#include "algorithms/mt_radix/radix_join_mt.h"
#include "gtest/gtest.h"
#include <iostream>
#include <chrono>

// Number of threads the program should be run on
#define thread_count 4
#define part_runs 3
#define part_bits 5

using namespace generators;  // NOLINT
using namespace algorithms; // NOLINT

/*
 * Helper function. Takes the vector of output vectors and calculates the total length
 * of the output.
 */
uint64_t get_size_radix(std::vector<std::vector<radix_join_mt::triple>>& output){
    uint64_t size = 0;
    for(auto& vec: output){
        size += vec.size();
    }
    return size;
}

/// The following tests perform single threaded, single-run computations
// Ensure proper creation of object and no return before execution
TEST(RadixTestMT, CreationTesterSTSP) {
    uniform_generator uni(0, 10000, 1000);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), 1000, 1000, 1.5, 1, 8, 1);
    ASSERT_ANY_THROW(join.get());
}

// No result should be returned
TEST(RadixTestMT, NoResTesterSTSP) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, 1, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), 0);
}

// This is a simple cross product
TEST(RadixTestMT, CrossTester1STSP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, 1, 1.5, 1, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count);
}


// This is a more complicated cross product
TEST(RadixTestMT, CrossTester2STSP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, 1, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count*count);
}

// Statistical test, usually should not fail
TEST(RadixTestMT, StatisticalTesterSTSP){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, 1, 8, 1);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "MT Radix Join Time Single Threaded Single Pass: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, get_size_radix(join.get()));
    ASSERT_GE(1.05 * expected, get_size_radix(join.get()));
}


/// Now running tests on multiple threads but only with one run
// No result should be returned
TEST(RadixTestMT, NoResTesterMTSP) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), 0);
}

// This is a simple cross product
TEST(RadixTestMT, CrossTester1MTSP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, 1, 1.5, thread_count, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count);
}


// This is a more complicated cross product
TEST(RadixTestMT, CrossTester2MTSP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, 8, 1);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count*count);
}

// Statistical test, usually should not fail
TEST(RadixTestMT, StatisticalTesterMTSP){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, 8, 1);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "MT Radix Join Time Multi Threaded Single Pass: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, get_size_radix(join.get()));
    ASSERT_GE(1.05 * expected, get_size_radix(join.get()));
}


/// Finally we are running multi threaded tests with multiple partitioning runs
// No result should be returned
TEST(RadixTestMT, NoResTesterMTMP) {
    uint64_t min = 0;
    uint64_t max = 10000;
    uint64_t count = 1000;
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    min = 20000;
    max = 30000;
    gen = uniform_generator(min, max, count);
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, part_bits, part_runs);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), 0);
}

// This is a simple cross product
TEST(RadixTestMT, CrossTester1MTMP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni = uniform_generator(1,1,1);
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, 1, 1.5, thread_count, part_bits, part_runs);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count);
}


// This is a more complicated cross product
TEST(RadixTestMT, CrossTester2MTMP) {
    uint64_t count = 1000;
    uniform_generator uni(1, 1, count);
    uni.build();
    auto left = uni.get_vec_copy();
    uni.build();
    auto right = uni.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, part_bits, part_runs);
    join.execute();
    ASSERT_EQ(get_size_radix(join.get()), count*count);
}

// Statistical test, usually should not fail
TEST(RadixTestMT, StatisticalTesterMTMP){
    uint64_t count = static_cast<uint64_t>(1) << static_cast<uint64_t>(17);
    uint64_t min = 1;
    uint64_t max = static_cast<uint64_t>(1) << static_cast<uint64_t>(12);
    uniform_generator gen(min, max, count);
    gen.build();
    auto left = gen.get_vec_copy();
    gen.build();
    auto right = gen.get_vec_copy();
    radix_join_mt join(left.data(), right.data(), count, count, 1.5, thread_count, part_bits, part_runs);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    join.execute();
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << "MT Radix Join Time Multi Threaded Multi Pass: " << duration << " milliseconds.\n";
    // Expected overall amount of join partners
    auto expected = static_cast<uint64_t>(max * (static_cast<double>(count)/max) * static_cast<double>((count))/max);
    ASSERT_LE(0.95 * expected, get_size_radix(join.get()));
    ASSERT_GE(1.05 * expected, get_size_radix(join.get()));
}
