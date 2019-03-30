#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "queues.hpp"

using namespace ::testing;

struct queues_test : ::testing::Test
{
};

// export GTEST_ALSO_RUN_DISABLED_TESTS=1 to enable disabled tests
TEST_F(queues_test, DISABLED_test_fail)
{
    FAIL();
}

TEST_F(queues_test, test_)
{
    EXPECT_THAT(0, Eq(false));
}
