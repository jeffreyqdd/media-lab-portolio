#pragma once

#include "aligned_alloc.tpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

/// @brief immutable container representing unsigned numbers in little endian format
class UnsignedBigInt {
public:
	// ========================================================================
	// Section: Constructors, Destructor, and Assignment Operators
	// ========================================================================

	constexpr UnsignedBigInt() noexcept
		: m_container{ 0 }
		, m_digits(1) { }

	constexpr UnsignedBigInt(int number) noexcept
		: m_container{ 0 }
		, m_digits(1) {
		assert(number >= 0);
		m_container[0] = number;
	}

	constexpr UnsignedBigInt(uint64_t number) noexcept
		: m_container{ 0 }
		, m_digits(1) {
		// static_assert(sizeof(number) <= HEAP_THRESHOLD * sizeof(base_t));
		m_container[0] = number;
	}

	constexpr UnsignedBigInt(__uint128_t number) noexcept
		: m_container{ 0 }
		, m_digits(number & UPPER_MASK_128 ? 2 : 1) {
		// treat 0th index as pointer to __uint128, setting both at once
		// without masks or shifts
		__uint128_t* location = static_cast<__uint128_t*>(static_cast<void*>(&m_container[0]));
		*location = number;
	}

	~UnsignedBigInt() = default;
	UnsignedBigInt(const std::string& number);

	UnsignedBigInt& operator=(const int& number);
	UnsignedBigInt& operator=(const uint64_t& number);
	UnsignedBigInt& operator=(const __uint128_t& number);
	UnsignedBigInt& operator=(const std::string& number);
	UnsignedBigInt& operator=(const UnsignedBigInt& number);

	// ========================================================================
	// Section: Arithmetic overloads with uint64_t
	// ========================================================================
	UnsignedBigInt operator+(const uint64_t& other) const;
	UnsignedBigInt operator-(const uint64_t& other) const;
	UnsignedBigInt operator*(const uint64_t& other) const;
	UnsignedBigInt operator<<(const uint64_t& other) const;
	UnsignedBigInt operator>>(const uint64_t& other) const;

	UnsignedBigInt& operator+=(const uint64_t& other);
	UnsignedBigInt& operator-=(const uint64_t& other);
	UnsignedBigInt& operator*=(const uint64_t& other);
	UnsignedBigInt& operator<<=(const uint64_t& other);
	UnsignedBigInt& operator>>=(const uint64_t& other);

	// ========================================================================
	// Section: Arithmetic overloads for UnsignedBigInt
	// ========================================================================
	UnsignedBigInt operator+(const UnsignedBigInt& other) const;
	UnsignedBigInt operator-(const UnsignedBigInt& other) const;
	UnsignedBigInt operator*(const UnsignedBigInt& other) const;
	UnsignedBigInt operator/(const UnsignedBigInt& other) const;
	UnsignedBigInt operator%(const UnsignedBigInt& other) const;
	UnsignedBigInt operator^(const UnsignedBigInt& other) const;
	UnsignedBigInt modulus_exp(const UnsignedBigInt& exp, const UnsignedBigInt& mod) const;
	UnsignedBigInt barret_mod(const UnsignedBigInt& mod,
							  const UnsignedBigInt& barret_mu,
							  const uint64_t& barret_2k) const;
	UnsignedBigInt barret_modexp(const UnsignedBigInt& exp,
								 const UnsignedBigInt& mod,
								 const UnsignedBigInt& barret_mu,
								 const uint64_t& barret_2k) const;

	UnsignedBigInt& operator+=(const UnsignedBigInt& other);
	UnsignedBigInt& operator-=(const UnsignedBigInt& other);
	UnsignedBigInt& operator*=(const UnsignedBigInt& other);
	UnsignedBigInt& operator/=(const UnsignedBigInt& other);
	UnsignedBigInt& operator%=(const UnsignedBigInt& other);
	UnsignedBigInt& operator^=(const UnsignedBigInt& other);
	UnsignedBigInt& modulus_exp_eq(const UnsignedBigInt& exp, const UnsignedBigInt& mod);
	UnsignedBigInt& barret_mod_eq(const UnsignedBigInt& mod,
								  const UnsignedBigInt& barret_mu,
								  const uint64_t& barret_2k);
	UnsignedBigInt& barret_modexp_eq(const UnsignedBigInt& exp,
									 const UnsignedBigInt& mod,
									 const UnsignedBigInt& barret_mu,
									 const uint64_t& barret_2k);

	// ========================================================================
	// Section: Comparators
	// ========================================================================
	bool operator<(const UnsignedBigInt& other) const;
	bool operator>(const UnsignedBigInt& other) const;
	bool operator<=(const UnsignedBigInt& other) const;
	bool operator>=(const UnsignedBigInt& other) const;
	bool operator==(const UnsignedBigInt& other) const;
	bool operator!=(const UnsignedBigInt& other) const;

 	// ========================================================================
	// Section: Utility methods
 	// ========================================================================
	size_t digits() const noexcept;
	size_t most_significant_bit_idx() const noexcept;
	uint64_t least_significant_bit() const noexcept;
	void set_bit(size_t idx, bool on) noexcept;

	std::string to_string() const;
	std::string to_bitstring() const;


private:
	typedef uint64_t base_t;
	typedef __uint128_t carry_t;

	inline static constexpr size_t BUFFER_SIZE = 32;

	alignas(64) base_t m_container[BUFFER_SIZE] = {};
	size_t m_digits;
};

class BigNumDivideByZeroException : public std::exception {
	const char* what() const noexcept override {
		return "cannot divide by zero";
	}
};

class BigNumUnderflowException : public std::exception {
private:
	std::string _error_msg;

public:
	BigNumUnderflowException(const UnsignedBigInt& lhs, const uint64_t& rhs) {
		_error_msg = "cannot subtract " + std::to_string(rhs) + " from " + lhs.to_string();
	}
	BigNumUnderflowException(const UnsignedBigInt& lhs, const UnsignedBigInt& rhs) {
		_error_msg = "cannot subtract " + rhs.to_string() + " from " + lhs.to_string();
	}

	const char* what() const noexcept override {
		return _error_msg.c_str();
	}
};
