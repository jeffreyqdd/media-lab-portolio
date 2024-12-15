#include "include/bignum.hpp"
#include "gtest/gtest.h"

#include <limits>
TEST(Init, MultiplicationSimple) {
	UnsignedBigInt a = std::string("99");
	UnsignedBigInt b = std::string("99");
	UnsignedBigInt c = a * b;
	a *= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "9801");
}

TEST(Init, MultiplicationCarry) {
	UnsignedBigInt a = std::numeric_limits<uint64_t>::max();
	UnsignedBigInt b = std::numeric_limits<uint64_t>::max();
	UnsignedBigInt c = a * b;
	a *= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "340282366920938463426481119284349108225");
}

TEST(Init, AdditionLarge) {
	UnsignedBigInt a = std::string("897093897589049889899898345896737543125136989078342768914");
	UnsignedBigInt b = std::string("156346783789238972345123467823478976234512334513256790765");
	UnsignedBigInt c = a * b;
	a *= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(),
			  "140257745645000872142616468868498922094060947798926726180220809091903011394329338806"
			  "698623469389850013443644279210");
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}