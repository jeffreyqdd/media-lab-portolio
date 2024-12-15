#include "include/bignum.hpp"
#include <chrono>
#include <iostream>

const static UnsignedBigInt rsa_e{ "65537" };
const static UnsignedBigInt rsa_n{
	"9616540267013058477253762977293425063379243458473593816900454019721117570003248"
	"808113992652836857529658675570356835067184715201230519907361653795328462699"
}; // 511 bits long
const static UnsignedBigInt rsa_d{
	"4802033916387221748426181350914821072434641827090144975386182740274856853318276"
	"518446521844642275539818092186650425384826827514552122318308590929813048801"
};

inline constexpr static uint64_t barret_k = 512;
const static uint64_t barret_2k = barret_k * 2;
const static UnsignedBigInt barret_mu = (UnsignedBigInt(1) << barret_2k) / rsa_n;

int main() {
	UnsignedBigInt n1("424324324234");
	auto start = std::chrono::high_resolution_clock::now();
	std::cout << n1.barret_modexp_eq(rsa_d, rsa_n, barret_mu, barret_2k).to_string() << '\n';
	auto end = std::chrono::high_resolution_clock::now();
	auto millis_elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << millis_elapsed << std::endl;

	UnsignedBigInt a{
		"980980980958905340893489058947357893457894537895349783457893459783548762547837658346584736"
	};
	UnsignedBigInt b{
		"3894726357476354763534946808736543748694653345245623567347564985389053480934508989034890"
	};
	start = std::chrono::high_resolution_clock::now();
	std::cout << (a * b).to_string() << '\n';
	end = std::chrono::high_resolution_clock::now();
	millis_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << millis_elapsed << std::endl;

	return 0;
}