#include "include/bignum.hpp"
#include "gtest/gtest.h"

TEST(Modulus, Simple) {
	UnsignedBigInt a = std::string("27");
	UnsignedBigInt b = std::string("5");
	UnsignedBigInt ans = std::string("2");
	a %= b;
	std::cout << "ans" << std::endl;
	std::cout << ans.to_bitstring() << std::endl;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Modulus, Simple2) {
	UnsignedBigInt a = std::string("81");
	UnsignedBigInt b = std::string("9");
	UnsignedBigInt ans = std::string("0");
	a %= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Modulus, Simple3) {
	UnsignedBigInt a = std::string("231");
	UnsignedBigInt b = std::string("33");
	UnsignedBigInt ans = std::string("0");
	a %= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Modulus, Medium) {
	UnsignedBigInt a = std::string("987897893479839813798");
	UnsignedBigInt b = std::string("231767842");
	UnsignedBigInt ans = std::string("216307918");
	a %= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Modulus, Hard) {
	UnsignedBigInt a = std::string(
		"248098402834842452548924375023847520347580283475023457928034570942357802438759");
	UnsignedBigInt b = std::string("38427893");
	UnsignedBigInt ans = std::string("35519763");
	a %= b;
	std::cout << "comparison" << std::endl;
	std::cout << a.to_bitstring() << std::endl;
	std::cout << ans.to_bitstring() << std::endl;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
