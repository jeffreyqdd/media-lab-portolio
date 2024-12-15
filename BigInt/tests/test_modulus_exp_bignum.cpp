#include "include/bignum.hpp"
#include "gtest/gtest.h"

TEST(ModPower, Simple) {
	UnsignedBigInt a = std::string("9");
	UnsignedBigInt b = std::string("9");
	UnsignedBigInt mod = std::string("4");

	UnsignedBigInt ans = std::string("1");

	UnsignedBigInt c = a.modulus_exp(b, mod);
	a.modulus_exp_eq(b, mod);

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

TEST(ModPower, Edge) {
	UnsignedBigInt a = std::string("9");
	UnsignedBigInt b = std::string("0");
	UnsignedBigInt mod = std::string("1");

	UnsignedBigInt ans = std::string("0");

	UnsignedBigInt c = a.modulus_exp(b, mod);
	a.modulus_exp_eq(b, mod);

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

TEST(ModPower, Hard) {
	UnsignedBigInt a = std::string("89893");
	UnsignedBigInt b = std::string("3423");
	UnsignedBigInt mod = std::string("761237");

	UnsignedBigInt ans = std::string("574736");

	UnsignedBigInt c = a.modulus_exp(b, mod);
	a.modulus_exp_eq(b, mod);

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}