#include "include/bignum.hpp"
#include "include/thread_pool.hpp"
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>

inline constexpr size_t CHARS_PER_ENCODING = 51;
inline constexpr size_t CHARS_PER_LETTER = 3;
inline constexpr size_t NUM_THREADS = 32;

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

std::pair<std::string, std::string> encrypt(const std::string_view& line, const size_t line_no) {
	// assume that each line is <= 96 characters long
	// step 1: Populate the buffer
	char buffer[CHARS_PER_ENCODING * 2 + 1];
	const int space_no = (CHARS_PER_ENCODING * 2) - (2 * CHARS_PER_LETTER) - line.length();
	std::sprintf(buffer, "%3zu%s%*s%3zu", line_no, line.data(), space_no, "", line_no);

	// step 2: Construct left and right strings
	std::string left;
	std::string right;
	left.reserve(CHARS_PER_ENCODING * CHARS_PER_LETTER);
	right.reserve(CHARS_PER_ENCODING * CHARS_PER_LETTER);

	for(size_t i = 0; i < CHARS_PER_ENCODING; i++) {
		std::string ascii = std::to_string(static_cast<int>(buffer[i]));
		left.append(3 - ascii.length(), '0');
		left.append(ascii);
	}
	for(size_t i = CHARS_PER_ENCODING; i < CHARS_PER_ENCODING * 2; i++) {
		std::string ascii = std::to_string(static_cast<int>(buffer[i]));
		right.append(3 - ascii.length(), '0');
		right.append(ascii);
	}

	// step 3: encrypt and return string representation
	UnsignedBigInt number_left{ left };
	UnsignedBigInt number_right{ right };
	// number_left.modulus_exp_eq(rsa_e, rsa_n);
	// number_right.modulus_exp_eq(rsa_e, rsa_n);
	number_left.barret_modexp_eq(rsa_e, rsa_n, barret_mu, barret_2k);
	number_right.barret_modexp_eq(rsa_e, rsa_n, barret_mu, barret_2k);

	return std::make_pair(number_left.to_string(), number_right.to_string());
}

std::string decrypt(const std::string& left, const std::string& right) {
	// step 1: decode
	UnsignedBigInt left_number{ left };
	UnsignedBigInt right_number{ right };

	// std::string left_decoded = left_number.modulus_exp_eq(rsa_d, rsa_n).to_string();
	// std::string right_decoded = right_number.modulus_exp_eq(rsa_d, rsa_n).to_string();
	std::string left_decoded =
		left_number.barret_modexp_eq(rsa_d, rsa_n, barret_mu, barret_2k).to_string();
	std::string right_decoded =
		right_number.barret_modexp_eq(rsa_d, rsa_n, barret_mu, barret_2k).to_string();

	// step 2: pad to correct length
	const size_t expected_length = CHARS_PER_ENCODING * CHARS_PER_LETTER;

	if(left_decoded.size() != expected_length) {
		const size_t num_to_insert = expected_length - left_decoded.size();
		left_decoded = std::string("0", num_to_insert) + left_decoded;
	}

	if(right_decoded.size() != expected_length) {
		const size_t num_to_insert = expected_length - right_decoded.size();
		right_decoded = std::string("0", num_to_insert) + right_decoded;
	}

	// step 3: reconstruct
	std::string text;
	text.reserve(CHARS_PER_ENCODING * CHARS_PER_LETTER * 2);

	for(size_t i = 0; i < left_decoded.size(); i += CHARS_PER_LETTER) {
		int ascii_code = (left_decoded[i] - '0') * 100 + (left_decoded[i + 1] - '0') * 10 +
						 (left_decoded[i + 2] - '0');
		text.push_back(static_cast<char>(ascii_code));
	}
	for(size_t i = 0; i < right_decoded.size(); i += CHARS_PER_LETTER) {
		int ascii_code = (right_decoded[i] - '0') * 100 + (right_decoded[i + 1] - '0') * 10 +
						 (right_decoded[i + 2] - '0');
		text.push_back(static_cast<char>(ascii_code));
	}

	// step 4: sanitize
	std::string_view stv{ text };
	stv.remove_prefix(3); // remove line count at beginning
	stv.remove_suffix(3); // remove line count at end

	// Find the last non-whitespace character
	int end = stv.size();
	while(end > 0 && std::isspace(static_cast<unsigned char>(stv[end - 1]))) {
		--end;
	}

	return std::string(stv.substr(0, end));
}

void encrypt_input(const std::vector<std::string>& lines) {

	// create futures
	std::vector<std::future<std::pair<std::string, std::string>>> results;
	results.reserve(lines.size() * 2);

	// create workers
	ThreadPool t(NUM_THREADS);
	for(size_t i = 0; i < lines.size(); i++) {
		results.emplace_back(
			t.enqueue([&line = lines[i], line_no = i + 1]() { return encrypt(line, line_no); }));
	}

	for(auto& result : results) {
		auto [left, right] = result.get();
		std::cout << left << '\n' << right << std::endl;
	}
}

void decrypt_input(const std::vector<std::string>& lines) {
	std::vector<std::future<std::string>> results;
	results.reserve(lines.size() / 2);

	ThreadPool t(NUM_THREADS);
	for(size_t i = 0; i < lines.size() / 2; i++) {
		results.emplace_back(t.enqueue(
			[&left = lines[i * 2], &right = lines[i * 2 + 1]] { return decrypt(left, right); }));
	}

	for(auto&& result : results) {
		std::cout << result.get() << '\n';
	}

	std::cout.flush();
}

int main(int argc, char* argv[]) {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);

	if(argc != 2) {
		std::cout << "expected only 'e' or 'd' mode" << std::endl;
		return 0;
	}

	std::vector<std::string> lines;
	lines.reserve(64);

	std::string line;
	while(std::getline(std::cin, line)) {
		lines.emplace_back(std::move(line));
	}

	if(argv[1][0] == 'e') {
		encrypt_input(lines);
	} else if(argv[1][0] == 'd') {
		decrypt_input(lines);
	}

	return 0;
}