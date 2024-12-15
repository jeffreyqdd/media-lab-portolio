#include "include/bignum.hpp"
#include "gtest/gtest.h"

#include <limits>
TEST(Init, AdditionSimple) {
	UnsignedBigInt a = std::string("99");
	UnsignedBigInt b = std::string("99");
	UnsignedBigInt c = a + b;
	a += b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "198");
}

TEST(Init, AdditionCarry) {
	UnsignedBigInt a = std::numeric_limits<uint64_t>::max();
	UnsignedBigInt b = std::numeric_limits<uint64_t>::max();
	UnsignedBigInt c = a + b;
	a += b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "36893488147419103230");
}

TEST(Init, AdditionLarge) {
	UnsignedBigInt a =
		std::string("8998732198739876985776345675347384765839240938402834876537864523489087");
	UnsignedBigInt b =
		std::string("9823467487629384779287463927834823794886723784678948902390478923465123423");
	UnsignedBigInt c = a + b;
	a += b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(),
			  "9832466219828124656273240273510171179652563025617351737267016787988612510");
}
TEST(Init, AdditionRepeated) {
	UnsignedBigInt a =
		std::string("8998732198739876985776345675347384765839240938402834876537864523489087");

	for(size_t i = 0; i < 100; i++) {
		a += a;
	}

	ASSERT_EQ(a.to_string(),
			  "114072482730256995688930902678732555671674914279197733701016489473212512601188672822"
			  "97951356255731712");
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}