#include "include/bignum.hpp"
#include "gtest/gtest.h"
TEST(Left, SimpleShift) {
	UnsignedBigInt a = std::string("9923904823092348290");
	a <<= 123;
	ASSERT_EQ(a.to_string(), "105529056946874417873521680713872697491000854321635000320");
}

TEST(Left, HardShift) {
	UnsignedBigInt a = std::string("992390482309232349872349857907549023485748290");
	a <<= 252;
	ASSERT_EQ(a.to_string(),
			  "718193545536336821339299513334696827379972958467503611129601419193584544383044327783"
			  "4951784538039382632712985854701731840");
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}