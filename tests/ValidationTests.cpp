#include <gtest/gtest.h>
#include "utils/Validation.h"

TEST(ValidationTests, AcceptsValidIpv4) {
    EXPECT_TRUE(api::validation::looksLikeIp("95.44.73.19"));
}

TEST(ValidationTests, AcceptsValidIpv6) {
    EXPECT_TRUE(api::validation::looksLikeIp("2a01:05a9:01a4:095c:0000:0000:0000:0001"));
}

TEST(ValidationTests, RejectsClearlyInvalidText) {
    EXPECT_FALSE(api::validation::looksLikeIp("not-an-ip"));
}

TEST(ValidationTests, RejectsEmptyString) {
    EXPECT_FALSE(api::validation::looksLikeIp(""));
}