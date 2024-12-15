#include "include/bignum.hpp"
#include "gtest/gtest.h"
TEST(Right, SimpleShift) {
	UnsignedBigInt a = std::string("9923904823092348290");
	std::cout << a.to_bitstring() << std::endl;
	a >>= 123;
	ASSERT_EQ(a.to_string(), "0");
}

TEST(Right, SimpleShift2) {
	UnsignedBigInt a = std::string("3422");
	std::cout << a.to_bitstring() << std::endl;
	a >>= 1;
	ASSERT_EQ(a.to_string(), "1711");
}

TEST(Right, MediumShift) {
	UnsignedBigInt a = std::string("9923904823092332894790823448290");
	a >>= 31;
	ASSERT_EQ(a.to_string(), "4621178295040658160481");
}

TEST(Init, HardShift) {
	UnsignedBigInt a =
		std::string("992390482309233289479082234908239057823405972349057234905783924857348957894578"
					"912456723456737865774653249480565098750678506978905673448290");
	UnsignedBigInt ans =
		std::string("75386488440116526277765946928110317199105972284416019210126339004478387207");
	std::cout << a.to_bitstring() << std::endl;
	std::cout << a.digits() << std::endl;
	a >>= 213;
	std::cout << a.to_bitstring() << std::endl;
	std::cout << ans.to_bitstring() << std::endl;
	std::cout << ans.digits() << std::endl;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}