#include "include/bignum.hpp"
#include <cstring>
#include <iostream>

bool valid(const char* number) {
	int strlen = std::strlen(number);
	if(strlen == 0) {
		std::cout << "Error: " << '"' << number << '"' << " is not an unsigned integer"
				  << std::endl;
		;
		return false;
	}

	if(number[0] == '-') {
		std::cout << "Error: " << '"' << number << '"' << " is not an unsigned integer"
				  << std::endl;
		return false;
	}

	for(int i = 0; number[i] != '\0'; ++i) {
		if(!std::isdigit(static_cast<unsigned char>(number[i]))) {
			std::cout << "Error: " << '"' << number << '"' << " is not an unsigned integer"
					  << std::endl;
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Error: No operator provided" << std::endl;
		return 1;
	}

	if((std::strcmp(argv[1], "+") == 0 || std::strcmp(argv[1], "-") == 0 ||
		std::strcmp(argv[1], "*") == 0 || std::strcmp(argv[1], "/") == 0 ||
		std::strcmp(argv[1], "%") == 0) &&
	   argc != 4) {
		std::cout << "Error: +-*/% requires two numbers" << std::endl;
		return 1;
	} else if(std::strcmp(argv[1], "^") == 0 && argc != 5) {
		std::cout << "Error: Exponent requires three numbers" << std::endl;
		return 1;
	} else if(std::strcmp(argv[1], "+") != 0 && std::strcmp(argv[1], "-") != 0 &&
			  std::strcmp(argv[1], "*") != 0 && std::strcmp(argv[1], "/") != 0 &&
			  std::strcmp(argv[1], "%") != 0 && std::strcmp(argv[1], "^") != 0) {
		std::cout << "Error: \"" << argv[1] << "\" is not a supported operator" << std::endl;
		return 1;
	}

	if(std::strcmp(argv[1], "^") == 0) {
		char* number1 = argv[2];
		char* number2 = argv[3];
		char* number3 = argv[4];

		bool good = true;
		good &= valid(number1);
		good &= valid(number2);
		good &= valid(number3);

		if(!good)
			return 1;

		std::cout << "bignum ^ " << number1 << " " << number2 << " " << number3 << std::endl;

		UnsignedBigInt n1(number1);
		UnsignedBigInt n2(number2);
		UnsignedBigInt n3(number3);

		try {
			std::cout << n1.modulus_exp_eq(n2, n3).to_string() << std::endl;
		} catch(BigNumDivideByZeroException& _) {
			std::cout << "Error: Divide by zero" << std::endl;
			std::cout << 0 << std::endl;
			return 1;
		}
	} else {
		char* number1 = argv[2];
		char* number2 = argv[3];

		bool good = true;
		good &= valid(number1);
		good &= valid(number2);

		if(!good)
			return 1;

		std::cout << "bignum " << argv[1] << " " << number1 << " " << number2 << std::endl;

		UnsignedBigInt n1(number1);
		UnsignedBigInt n2(number2);

		if(argv[1][0] == '+') {
			std::cout << (n1 += n2).to_string() << std::endl;
			return 0;
		}

		try {
			if(argv[1][0] == '-') {
				std::cout << (n1 -= n2).to_string() << std::endl;
				return 0;
			}
		} catch(BigNumUnderflowException& _) {
			std::cout << "Unsupported: Negative number" << std::endl;
			std::cout << 0 << std::endl;
			return 1;
		}

		if(argv[1][0] == '*') {
			std::cout << (n1 *= n2).to_string() << std::endl;
			return 0;
		}

		try {

			if(argv[1][0] == '/') {
				std::cout << (n1 /= n2).to_string() << std::endl;
				return 0;
			}
			if(argv[1][0] == '%') {
				std::cout << (n1 %= n2).to_string() << std::endl;
				return 0;
			}
		} catch(BigNumDivideByZeroException& _) {
			std::cout << "Error: Divide by zero" << std::endl;
			std::cout << 0 << std::endl;
			return 1;
		}
	}

	return 0;
}