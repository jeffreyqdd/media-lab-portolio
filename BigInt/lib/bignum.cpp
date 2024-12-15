#include "include/bignum.hpp"

#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <immintrin.h>
#include <iostream>
#include <sstream>
#include <string>

// ============================================================================
// Section: Constructors
// ============================================================================
UnsignedBigInt::UnsignedBigInt(const std::string& number)
	: m_container{ 0 }
	, m_digits(1) {
	static constexpr size_t LEN = 18;
	static constexpr uint64_t BASE = 1000000000000000000ULL;

	std::string_view slice{ number };

	while(slice.length() >= LEN) {
		std::string sliced_number{ slice.substr(0, LEN) };
		slice.remove_prefix(LEN);
		uint64_t num = std::strtoull(sliced_number.c_str(), nullptr, 10);
		*this *= BASE;
		*this += num;
	}

	if(slice.length()) {
		uint64_t number = std::stoull(slice.data());
		*this *= std::pow(10ull, slice.length());
		*this += number;
	}
}
// ============================================================================
// Section: Assignment Operators
// ============================================================================
UnsignedBigInt& UnsignedBigInt::operator=(const int& value) {
	*this = UnsignedBigInt(value);
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator=(const uint64_t& value) {
	*this = UnsignedBigInt(value);
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator=(const __uint128_t& value) {
	*this = UnsignedBigInt(value);
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator=(const std::string& number) {
	*this = UnsignedBigInt(number);
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator=(const UnsignedBigInt& bignum) {
	if(this != &bignum) {
		std::memcpy(m_container, bignum.m_container, sizeof(m_container));
		m_digits = bignum.m_digits;
	}
	return *this;
}

// ============================================================================
// Section: algebraic operations
// ============================================================================
template <typename T>
inline UnsignedBigInt apply_binary_op(const UnsignedBigInt& lhs,
									  const T& rhs,
									  UnsignedBigInt& (UnsignedBigInt::*op)(const T&)) {
	UnsignedBigInt ret = lhs;
	(ret.*op)(rhs);
	return ret;
}

UnsignedBigInt UnsignedBigInt::operator+(const uint64_t& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator+=);
}
UnsignedBigInt UnsignedBigInt::operator-(const uint64_t& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator-=);
}
UnsignedBigInt UnsignedBigInt::operator*(const uint64_t& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator*=);
}
// UnsignedBigInt UnsignedBigInt::operator/(const uint64_t& other) const {
// 	return apply_binary_op(*this, other, &UnsignedBigInt::operator/=);
// }
UnsignedBigInt UnsignedBigInt::operator<<(const uint64_t& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator<<=);
}
UnsignedBigInt UnsignedBigInt::operator>>(const uint64_t& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator>>=);
}

UnsignedBigInt UnsignedBigInt::operator+(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator+=);
}
UnsignedBigInt UnsignedBigInt::operator-(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator-=);
}
UnsignedBigInt UnsignedBigInt::operator*(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator*=);
}
UnsignedBigInt UnsignedBigInt::operator/(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator/=);
}
// UnsignedBigInt UnsignedBigInt::operator<<(const UnsignedBigInt& other) const {
// 	return apply_binary_op(*this, other, &UnsignedBigInt::operator<<=);
// }
// UnsignedBigInt UnsignedBigInt::operator>>(const UnsignedBigInt& other) const {
// 	return apply_binary_op(*this, other, &UnsignedBigInt::operator>>=);
// }
UnsignedBigInt UnsignedBigInt::operator^(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator^=);
}
UnsignedBigInt UnsignedBigInt::operator%(const UnsignedBigInt& other) const {
	return apply_binary_op(*this, other, &UnsignedBigInt::operator%=);
}
UnsignedBigInt UnsignedBigInt::modulus_exp(const UnsignedBigInt& exp,
										   const UnsignedBigInt& mod) const {
	UnsignedBigInt ret = *this;
	ret.modulus_exp_eq(exp, mod);
	return ret;
}

UnsignedBigInt UnsignedBigInt::barret_mod(const UnsignedBigInt& mod,
										  const UnsignedBigInt& barret_mu,
										  const uint64_t& barret_2k) const {
	UnsignedBigInt ret = *this;
	ret.barret_mod_eq(mod, barret_mu, barret_2k);
	return ret;
}

UnsignedBigInt UnsignedBigInt::barret_modexp(const UnsignedBigInt& exp,
											 const UnsignedBigInt& mod,
											 const UnsignedBigInt& barret_mu,
											 const uint64_t& barret_2k) const {
	UnsignedBigInt ret = *this;
	ret.barret_modexp_eq(exp, mod, barret_mu, barret_2k);
	return ret;
}

// ============================================================================
// Section: assignment algebraic operations for primitives
// ============================================================================

UnsignedBigInt& UnsignedBigInt::operator+=(const uint64_t& other) {
	__uint128_t carry = static_cast<__uint128_t>(other);

	for(size_t i = 0; i < m_digits; i++) {
		__uint128_t sum = m_container[i] + carry;
		m_container[i] = static_cast<uint64_t>(sum);
		carry = sum >> 64;
	}

	if(carry) {
		m_container[m_digits] = carry;
		m_digits++;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator-=(const uint64_t& other) {
	if(m_digits == 1 && m_container[0] < other) {
		throw BigNumUnderflowException(*this, other);
	}

	size_t i = 0;
	uint64_t borrow = other;
	for(; i < m_digits; i++) {
		if(borrow == 0) {
			break;
		}

		if(m_container[i] >= borrow) {
			// fully absorb the borrow
			m_container[i] -= borrow;
			borrow = 0;
		} else {
			uint64_t new_value = (UnsignedBigInt::MAX_U64_VALUE - borrow) + m_container[i];
			m_container[i] = new_value;
			borrow = 1;
		}
	}

	if(i == m_digits && m_digits != 1 && m_container[i - 1] == 0) {
		// if we borrowed from the highest digit,
		// we need to check if we decreased the number of digits
		// in the number
		m_digits--;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator*=(const uint64_t& other) {
	__uint128_t carry = 0;
	for(size_t i = 0; i < m_digits; i++) {
		__uint128_t product = static_cast<__uint128_t>(m_container[i]) * other + carry;
		m_container[i] = product; // use truncation
		carry = product >> 64;
	}

	if(carry) {
		m_container[m_digits] = carry;
		m_digits++;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator<<=(const uint64_t& other) {
	const size_t base_shift = other / 64;
	const size_t bit_shift = other % 64;
	const size_t required_size = m_digits + base_shift + (bit_shift != 0);

	for(int i = m_digits - 1; i >= 0; i--) {
		// start on rightmost digit
		const __uint128_t after_shift = static_cast<__uint128_t>(m_container[i]) << bit_shift;

		// shrd was taking too long, so I just use pointer arithmetic to emulate ">> 64"
		const uint64_t upper_half =
			*(static_cast<const uint64_t*>(static_cast<const void*>(&after_shift)) + 1);
		const uint64_t lower_half = static_cast<uint64_t>(after_shift); // use truncation

		// because we're going from right to left, assume upper half is already set
		// so need to or
		if(i + base_shift + 1 < BUFFER_SIZE) {
			m_container[i + base_shift + 1] |= upper_half;
		}

		if(i + base_shift < BUFFER_SIZE) {
			m_container[i + base_shift] = lower_half;
		}
	}

	// fill in with zeros
	for(size_t i = 0; i < base_shift && i < BUFFER_SIZE; i++) {
		m_container[i] = 0;
	}

	// if leading zeros not used
	m_digits = required_size;
	if(m_digits > 1 && m_container[m_digits - 1] == 0) {
		m_digits--;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator>>=(const uint64_t& other) {
	const size_t base_shift = other / 64;
	const size_t bit_shift = other % 64;
	const uint64_t shift_amount = 64 - bit_shift;

	if(base_shift >= m_digits) {
		*this = 0;
		return *this;
	}

	for(size_t i = base_shift; i < m_digits + base_shift; i++) {
		// shifting 128 bit integers is expensive, use clever shifting with 64 bit integers
		const uint64_t& value = (i >= BUFFER_SIZE) ? 0 : m_container[i];
		const uint64_t& upper_half = (value >> bit_shift);
		const uint64_t& lower_half = shift_amount == 64 ? 0 : (value << shift_amount);
		if(i > base_shift) {
			m_container[i - base_shift - 1] |= lower_half;
		}
		m_container[i - base_shift] = upper_half;
	}

	while(m_digits > 1 && m_container[m_digits - 1] == 0) {
		m_digits--;
	}

	return *this;
}

// ============================================================================
// Section: assignment algebraic operations for big ints
// ============================================================================
UnsignedBigInt& UnsignedBigInt::operator+=(const UnsignedBigInt& other) {
	size_t max_digit = std::max(m_digits, other.m_digits);
	__uint128_t carry = 0;
	for(size_t i = 0; i < max_digit; i++) {
		const __uint128_t lhs = m_container[i];
		const __uint128_t rhs = other.m_container[i];
		const __uint128_t sum = lhs + rhs + carry;

		m_container[i] = sum & UnsignedBigInt::LOWER_MASK_128;
		carry = sum >> 64;
	}

	m_digits = max_digit;
	if(carry) {
		m_container[m_digits] = carry;
		m_digits++;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator-=(const UnsignedBigInt& other) {
	if(*this < other) {
		throw BigNumUnderflowException(*this, other);
	}

	uint64_t borrow = 0;
	size_t i = 0;

	// when the most expensive part of this for loop is the increment
	// so we unroll, and so we don't need to compute add x0, x0, 1
#pragma GCC unroll 8
	// add x0, x0, 1
	for(; i < other.m_digits; i++) {
		const __uint128_t lhs = m_container[i];
		const __uint128_t rhs = (i < other.m_digits) ? other.m_container[i] : 0;

		if(lhs >= rhs + borrow) {
			// can absorb the subtraction
			m_container[i] = lhs - borrow - rhs;
			borrow = 0;
		} else {
			// do the subtraction first to avoid overflow
			m_container[i] += ((UnsignedBigInt::MAX_U64_VALUE - rhs) - borrow) + 1;
			borrow = 1;
		}
	}

#pragma GCC unroll 8
	for(; borrow != 0; i++) {
		const __uint128_t lhs = m_container[i];
		const __uint128_t rhs = (i < other.m_digits) ? other.m_container[i] : 0;

		if(lhs >= rhs + borrow) {
			// can absorb the subtraction
			m_container[i] = lhs - borrow - rhs;
			borrow = 0;
		} else {
			// do the subtraction first to avoid overflow
			m_container[i] += ((UnsignedBigInt::MAX_U64_VALUE - rhs) - borrow) + 1;
			borrow = 1;
		}
	}

	while(m_digits != 1 && m_container[m_digits - 1] == 0) {
		// if we borrowed from the highest digit,
		// we need to check if we decreased the number of digits
		// in the number
		m_digits--;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator*=(const UnsignedBigInt& other) {
	if(*this == 0 || other == 0) {
		*this = 0;
		return *this;
	}

	alignas(64) base_t buffer[BUFFER_SIZE] = {};
	__uint128_t carry = 0;
	for(size_t i = 0; i < m_digits; i++) {
		for(size_t j = 0; j < other.m_digits; j++) {
			const __uint128_t& lhs = m_container[i];
			const __uint128_t& rhs = other.m_container[j];
			const __uint128_t product = (lhs * rhs) + carry + buffer[i + j];
			carry = product >> 64;
			buffer[i + j] = product & UnsignedBigInt::LOWER_MASK_128;
		}

		if(carry) {
			buffer[i + other.m_digits] += carry;
			carry = 0;
		}
	}

	const size_t sum_size = m_digits + other.m_digits;
	m_digits = sum_size - ((buffer[sum_size - 1] == 0) ? 1 : 0);
	std::memcpy(m_container, buffer, sizeof(m_container));
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator/=(const UnsignedBigInt& other) {
	if(other == UnsignedBigInt(0)) {
		throw BigNumDivideByZeroException();
	}

	if(*this < other) {
		*this = 0;
		return *this;
	}

	if(*this == other) {
		*this = 1;
		return *this;
	}

	UnsignedBigInt dividend = *this;
	UnsignedBigInt divisor = other;
	UnsignedBigInt quotient;
	quotient.m_digits = dividend.m_digits;

	int shift = dividend.most_significant_bit_idx() - divisor.most_significant_bit_idx();
	divisor <<= shift;

	for(int i = shift; i >= 0; i--) {
		if(dividend >= divisor) {
			dividend -= divisor;
			quotient.set_bit(i, 1);
		}
		divisor >>= 1;
	}

	*this = quotient;

	// normalize
	while(m_digits > 1 && m_container[m_digits - 1] == 0) {
		m_digits--;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator^=(const UnsignedBigInt& other) {
	UnsignedBigInt res = 1;
	UnsignedBigInt n = other;

	while(n > UnsignedBigInt(0)) {
		int last_bit = n.m_container[0] & 1;
		if(last_bit) {
			res *= (*this);
		}
		*this *= *this;
		n >>= 1;
	}

	*this = res;
	return *this;
}

UnsignedBigInt& UnsignedBigInt::operator%=(const UnsignedBigInt& other) {
	if(other == UnsignedBigInt(0)) {
		throw BigNumDivideByZeroException();
	}

	if(*this == other) {
		*this = 0;
		return *this;
	}

	if(*this < other) {
		return *this;
	}

	UnsignedBigInt divisor = other;
	int shift = most_significant_bit_idx() - divisor.most_significant_bit_idx();
	divisor <<= shift;

	for(int i = shift; i >= 0; --i) {
		if(*this >= divisor) {
			*this -= divisor;
		}
		divisor >>= 1;
	}

	// normalize
	while(m_digits > 1 && m_container[m_digits - 1] == 0) {
		m_digits--;
	}

	return *this;
}

UnsignedBigInt& UnsignedBigInt::modulus_exp_eq(const UnsignedBigInt& exp,
											   const UnsignedBigInt& mod) {

	*this %= mod;
	if(*this == 0) {
		return *this;
	}

	UnsignedBigInt ret = 1;
	for(UnsignedBigInt i = exp; i > 0;) {
		if(i.least_significant_bit()) {
			(ret *= *this) %= mod;
		}

		// even now
		i >>= 1;
		if(i > 0) {
			(*this *= *this) %= mod;
		}
	}
	*this = std::move(ret);
	return *this;
}
UnsignedBigInt& UnsignedBigInt::barret_mod_eq(const UnsignedBigInt& mod,
											  const UnsignedBigInt& barret_mu,
											  const uint64_t& barret_2k) {
	UnsignedBigInt q = ((*this * barret_mu) >> barret_2k);
	UnsignedBigInt imm = q * mod;
	*this -= imm;

	if(*this < mod) {
		return *this;
	} else {
		*this -= mod;
		return *this;
	}
}

UnsignedBigInt& UnsignedBigInt::barret_modexp_eq(const UnsignedBigInt& exp,
												 const UnsignedBigInt& mod,
												 const UnsignedBigInt& barret_mu,
												 const uint64_t& barret_2k) {
	*this = barret_mod_eq(mod, barret_mu, barret_2k);

	if(*this == 0) {
		return *this;
	}

	UnsignedBigInt ret = 1;
	for(UnsignedBigInt i = exp; i > 0;) {
		if(i.least_significant_bit()) {
			ret *= *this;
			ret.barret_mod_eq(mod, barret_mu, barret_2k);
		}

		// even now
		i >>= 1;
		if(i > 0) {
			*this *= *this;
			barret_mod_eq(mod, barret_mu, barret_2k);
		}
	}
	*this = std::move(ret);
	return *this;
}

// ============================================================================
// Section: operators
// ============================================================================
bool UnsignedBigInt::operator<(const UnsignedBigInt& other) const {
	if(m_digits != other.m_digits) {
		return m_digits < other.m_digits;
	}
	for(size_t i = m_digits; i > 0; i--) {
		if(m_container[i - 1] != other.m_container[i - 1]) {
			return m_container[i - 1] < other.m_container[i - 1];
		}
	}
	return false;
}

bool UnsignedBigInt::operator>(const UnsignedBigInt& other) const {
	if(m_digits != other.m_digits) {
		return m_digits > other.m_digits;
	}
	for(size_t i = m_digits; i > 0; i--) {
		if(m_container[i - 1] != other.m_container[i - 1]) {
			return m_container[i - 1] > other.m_container[i - 1];
		}
	}
	return false;
}

bool UnsignedBigInt::operator<=(const UnsignedBigInt& other) const {
	return !(*this > other);
}
bool UnsignedBigInt::operator>=(const UnsignedBigInt& other) const {
	return !(*this < other);
}

bool UnsignedBigInt::operator==(const UnsignedBigInt& other) const {
	if(m_digits != other.m_digits) {
		return false;
	}
	return std::memcmp(m_container, other.m_container, sizeof(m_container)) == 0;
}

bool UnsignedBigInt::operator!=(const UnsignedBigInt& other) const {
	return !(*this == other);
}

// ============================================================================
// Section: helpers
// ============================================================================

std::string UnsignedBigInt::to_string() const {
	const uint64_t BASE = 1000000000000000000ULL;
	std::vector<uint64_t> result;

	for(int i = m_digits - 1; i >= 0; i--) {
		uint64_t carry = m_container[i];

		for(size_t j = 0; j < result.size(); j++) {
			// Perform the multiplication and addition, ensuring the result fits in 128 bits
			__uint128_t temp = (static_cast<__uint128_t>(result[j]) << 64) + carry;
			result[j] = static_cast<uint64_t>(temp % BASE);
			carry = static_cast<uint64_t>(temp / BASE);
		}

		// Push any remaining carry as a new digit
		while(carry > 0) {
			result.push_back(carry % BASE);
			carry /= BASE;
		}
	}
	// Convert the result to a string
	std::string base10;
	if(result.empty()) {
		return "0";
	}

	base10 = std::to_string(result.back());
	for(int i = result.size() - 2; i >= 0; --i) {
		std::string part = std::to_string(result[i]);
		base10 += std::string(18 - part.length(), '0') + part;
	}

	return base10;
}

std::string UnsignedBigInt::to_bitstring() const {
	std::stringstream ss;

	for(int i = m_digits - 1; i >= 0; i--) {
		ss << std::bitset<sizeof(uint64_t) * 8>(m_container[i]) << ' ';
	}

	return ss.str();
}

size_t UnsignedBigInt::digits() const noexcept {
	return m_digits;
}

size_t UnsignedBigInt::most_significant_bit_idx() const noexcept {
	size_t ret = (m_digits - 1) * 64;
	ret += 64 - __builtin_clzll(m_container[m_digits - 1]);
	return ret;
}

uint64_t UnsignedBigInt::least_significant_bit() const noexcept {
	return m_container[0] & 1;
}

void UnsignedBigInt::set_bit(size_t idx, bool on) noexcept {
	size_t block = idx / 64;
	size_t shift = idx % 64;

	if(on) {
		m_container[block] |= 1ULL << shift;
	} else {
		m_container[block] &= ~(1ULL << shift);
	}
}