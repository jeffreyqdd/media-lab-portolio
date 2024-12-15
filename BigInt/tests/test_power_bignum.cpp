#include "include/bignum.hpp"
#include "gtest/gtest.h"

TEST(Power, Simple) {
	UnsignedBigInt a = std::string("2");
	UnsignedBigInt b = std::string("2");
	UnsignedBigInt ans = std::string("4");

	UnsignedBigInt c = a ^ b;
	a ^= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

TEST(Power, Simple2) {
	UnsignedBigInt a = std::string("3");
	UnsignedBigInt b = std::string("4");
	UnsignedBigInt ans = std::string("81");

	UnsignedBigInt c = a ^ b;
	a ^= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

TEST(Power, Zero) {
	UnsignedBigInt a = std::string("2");
	UnsignedBigInt b = std::string("0");
	UnsignedBigInt ans = std::string("1");

	UnsignedBigInt c = a ^ b;
	a ^= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

TEST(Power, Hard) {
	UnsignedBigInt a = std::string("4");
	UnsignedBigInt b = std::string("512");
	UnsignedBigInt ans =
		std::string("179769313486231590772930519078902473361797697894230657273430081157732675805500"
					"963132708477322407536021120113879871393357658789768814416622492847430639474124"
					"377767893424865485276302219601246094119453082952085005768838150682342462881473"
					"913110540827237163350510684586298239947245938479716304835356329624224137216");
	UnsignedBigInt c = a ^ b;
	a ^= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), ans.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}