#include "include/bignum.hpp"
#include "gtest/gtest.h"

#include <random>
#include <sstream>

TEST(Init, UnsignedIntegersAssignment) {
	UnsignedBigInt num = 5;
	ASSERT_EQ(num.to_string(), "5");
	ASSERT_EQ(num.digits(), 1);

	num = 12312983718ul;
	ASSERT_EQ(num.to_string(), "12312983718");
	ASSERT_EQ(num.digits(), 1);

	num = (static_cast<__uint128_t>(322712313) << 64) + 128903334;
	ASSERT_EQ(num.to_string(), "5952991447345851892321151142");
	ASSERT_EQ(num.digits(), 2);
}

TEST(Init, UnsignedIntegersFuzz) {
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());

	for(size_t i = 0; i < 1000; i++) {
		uint64_t number = dist(rng);

		std::stringstream ss;
		ss << number;

		UnsignedBigInt num = number;

		ASSERT_EQ(num.to_string(), ss.str());
	}
}

TEST(Init, Strings) {
	std::string number = "2811";
	UnsignedBigInt num = number;
	ASSERT_EQ(num.to_string(), number);

	number = "21890934578489537984";
	num = number;
	ASSERT_EQ(num.to_string(), number);

	number = "9176746473850934858902374163786453237498725983798247837262873648394748597398798237";
	num = number;
	ASSERT_EQ(num.to_string(), number);
}

TEST(Init, StringsFuzz) {
	auto string_generator = [](const size_t& len) {
		if(len == 0) {
			return std::string("0");
		}
		std::mt19937 gen(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, 9);

		std::stringstream ss;
		ss << std::to_string(dist(gen) % 9 + 1);
		for(size_t i = 1; i < len; ++i) {
			ss << std::to_string(dist(gen));
		}
		return ss.str();
	};

	for(size_t i = 0; i < 1000; i++) {
		std::string number = string_generator(500);
		UnsignedBigInt num = number;
		ASSERT_EQ(num.to_string(), number);
	}
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}