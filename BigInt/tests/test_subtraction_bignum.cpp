#include "include/bignum.hpp"
#include "gtest/gtest.h"

TEST(Init, SubtractionSimple) {
	UnsignedBigInt a = std::string("99");
	UnsignedBigInt b = std::string("99");
	UnsignedBigInt c = a - b;
	a -= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "0");
}

TEST(Init, SubtractionUnderflow) {
	UnsignedBigInt a = std::string("99");
	UnsignedBigInt b = std::string("999");
	ASSERT_THROW([&]() { return a - b; }(), BigNumUnderflowException);
}

TEST(Init, SubtractionCarry) {
	UnsignedBigInt a = std::string("890534790435890345890345898427502473590237590283");
	UnsignedBigInt b = std::string("3927846782798569068459087896423764523");
	UnsignedBigInt c = a - b;
	a -= b;

	ASSERT_EQ(a.to_string(), c.to_string());
	ASSERT_EQ(c.to_string(), "890534790431962499107547329359043385693813825760");
}

TEST(Init, SubtractionRepeated) {
	UnsignedBigInt a = std::string("219878975893724397861789468912346916389745667427338905234786587"
								   "38907897863567890789768456678678235890567");
	UnsignedBigInt b = std::string("98790342802340927849023849089");
	UnsignedBigInt answer("219878975893724397861789468912346916389745667427338905234786587389066739"
						  "26893972372265558005540291582741");

	for(size_t i = 0; i < 12389234; i++) {
		a -= b;
	}

	ASSERT_EQ(a.to_string(), answer.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}