#include "include/bignum.hpp"
#include "gtest/gtest.h"

TEST(Division, Simple) {
	UnsignedBigInt a = std::string("27");
	UnsignedBigInt b = std::string("5");
	UnsignedBigInt ans = std::string("5");
	a /= b;
	std::cout << "ans" << std::endl;
	std::cout << ans.to_bitstring() << std::endl;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Division, Simple2) {
	UnsignedBigInt a = std::string("81");
	UnsignedBigInt b = std::string("9");
	UnsignedBigInt ans = std::string("9");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Division, Simple3) {
	UnsignedBigInt a = std::string("231");
	UnsignedBigInt b = std::string("33");
	UnsignedBigInt ans = std::string("7");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Division, Simple4) {
	UnsignedBigInt a = std::string("231");
	UnsignedBigInt b = std::string("33");
	UnsignedBigInt ans = std::string("7");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Division, Medium) {
	UnsignedBigInt a = std::string("987897893479839813798");
	UnsignedBigInt b = std::string("231767842");
	UnsignedBigInt ans = std::string("4262445924140");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

TEST(Division, Hard) {
	UnsignedBigInt a = std::string("987897893479839813798428979830275082975098375082374508764725431"
								   "6254365578348990890895839378452746726352452456245623");
	UnsignedBigInt b = std::string("123878013787163287649879469873678563874568375693587893267520502"
								   "75029475809237859025244180748917491764916498716");
	UnsignedBigInt ans = std::string("797476");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}
TEST(Division, Hard2) {
	UnsignedBigInt a =
		std::string("179769313486231590772930519078902473361797697894230657273430081157732675805500"
					"963132708477322407536021120113879871393357658789768814416622492847430639474124"
					"377767893424865485276302219601246094119453082952085005768838150682342462881473"
					"913110540827237163350510684586298239947245938479716304835356329624224137216");

	UnsignedBigInt b{
		"9616540267013058477253762977293425063379243458473593816900454019721117570003248"
		"808113992652836857529658675570356835067184715201230519907361653795328462699"
	};

	UnsignedBigInt ans = std::string("18693761841031500638871306762234754805148737719492588113423584839077330414863185179658238916245313320244187109883656605324766832050990535102515369898064321");
	a /= b;
	ASSERT_EQ(a.to_string(), ans.to_string());
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
