# Big Num Writeup

## Question 1: 4 most expensive functions

The most expensive functions between encryption and decryption are different. This is because the exponent for
decryption is larger than the exponent for encryption, which leads to more time being spent in mod exponentiation
(>>=, -=, and %= for modulus; <<= for exponentiation);

**Encryption**

1. UnsignedBigInt::operator>>=(unsigned long const&)
2. UnsignedBigInt::operator<(UnsignedBigInt const&) const
3. UnsignedBigInt::operator>=(UnsignedBigInt const&) const
4. UnsignedBigInt::operator-=(UnsignedBigInt const&)

**Decryption**

1.  UnsignedBigInt::operator>>=(unsigned long const&)
2.  UnsignedBigInt::operator-=(UnsignedBigInt const&)
3.  UnsignedBigInt::operator%=(UnsignedBigInt const&)
4.  UnsignedBigInt::operator<<=(unsigned long const&)

## Question 2: What algorithm is the root cause

```
##### Encrypt Profile:
 time   seconds   seconds    calls  ms/call  ms/call  name
100.00      0.02     0.02    27834     0.00     0.00  UnsignedBigInt::operator>>=(unsigned long const&)
  0.00      0.02     0.00    42871     0.00     0.00  UnsignedBigInt::operator<(UnsignedBigInt const&) const
  0.00      0.02     0.00    25690     0.00     0.00  UnsignedBigInt::operator>=(UnsignedBigInt const&) const
  0.00      0.02     0.00    14120     0.00     0.00  UnsignedBigInt::operator-=(UnsignedBigInt const&)

#### Decrypt Profile
 time   seconds   seconds    calls  ms/call  ms/call  name
 47.46      0.28     0.28  1433851     0.00     0.00  UnsignedBigInt::operator>>=(unsigned long const&)
 47.46      0.56     0.28   757705     0.00     0.00  UnsignedBigInt::operator-=(UnsignedBigInt const&)
  3.39      0.58     0.02     2430     0.01     0.24  UnsignedBigInt::operator%=(UnsignedBigInt const&)
  1.69      0.59     0.01     3304     0.00     0.00  UnsignedBigInt::operator<<=(unsigned long const&)
```

We're going to focus on decryption because that's where the program spends the most time. According to gprof, `%=`
was called 2430 times and `-=` was called `757705` times. The only time `-=` is called in the code is through `%=`.
So, modulus calls `-=` an average of 311 times per call. Given the fact that the number is at most 512 bits large,
this is really good because I use a fast division algorithm that shifts the divisor to be left aligned with the
dividend. So, the upper bound of number of subtractions is 512 times per modulus.

To speed up the algorithm I used perf-tools to dig into the assembly. I compiled a DebugRelease
binary with the -O2 and -g flags. Digging into the hottest part of the assembly, it shows the most expensive operation was `shrd`,
(shift-right double word), and the or instruction in the `>>=` operation.

```
  1.19 │20:   mov  (%rdi,%rax,8),%r8
       │      mov  %r8,%r9
  5.35 │      xor  %r8d,%r8d
 29.09 │      shrd $0x1,%r9,%r8
```

The code that generated the assembly above was

```cpp
const __uint128_t after_shift& = (static_cast<__uint128_t>(m_container[i]) << 64) >> bit_shift;
const uint64_t upper_half& = static_cast<uint64_t>(after_shift >> 64);
const uint64_t lower_half& = static_cast<uint64_t>(after_shift);
```

I realized that the above code is functionally equivalent to:

```cpp
const uint64_t shift_amount = 64 - bit_shift;
...
const uint64_t& value = (i >= BUFFER_SIZE) ? 0 : m_container[i];
const uint64_t& upper_half = (value >> bit_shift);
const uint64_t& lower_half = shift_amount == 64 ? 0 : (value << shift_amount);
```

There was no noticeable speedup from this optimization. This makes sense as the speed gained
was probably so miniscule, and it was probably "lost back" as a result of the conditional
statements. This shows that the algorithm must change instead of the implementation of the
algorithm (See question 3 part 6 for an algorithm update).

## Question 3:

I benchmarked on a longer file (650 lines, 50k characters).

1. **Multithreading**
   I created a thread pool in `lib/thread_pool.cpp` and `lib/thread_pool.hpp`. Each line of the input gets
   turned into a "task" that is put on a queue, and a corresponding future is created for that task. To
   get the output, I just wait on each future then print the result.

   This approach speed up my code because I can do parallel processing of the lines. I opted for k=4
   because I only get 4 threads on Gradescope. RSA is compute bound, so more threads will just
   increase context switching costs.

   - Single threaded Encryption: 0.47 seconds
   - Single threaded Decryption: 20.95 seconds
   - Multithreaded Encryption: 0.11 seconds
   - Multithreaded Decryption: 4.50 seconds

   The result makes sense, if processing 1 line at a time takes 20.95 seconds, processing 4 lines
   concurrently should speed up the program by 4x.

2. **Modulus 2 Computation**
   It does pay off. Modulus by 2 is a constant time operation (evaluates to `NUM & 1`), and it's called so
   many times in my modular exponentiation. Instead of calling `NUM % 2`, which will take thousands of CPU
   cycles to perform the division and find the remainder, I just check the value of one bit:

   gprof also supported my claim, the amount of time returning the least significant big was nonexistent, compared
   to an expensive modulus call.

```
   time    seconds   seconds    calls  ms/call  ms/call  name
     0.00      0.68     0.00     1594     0.00     0.00  UnsignedBigInt::least_significant_bit() const
```

- Special Case Modulus Encryption: 0.11 seconds
- Special Case Modulus Decryption: 4.50 seconds
- Naive Case Modulus Encryption: 0.11 seconds
- Naive Case Modulus Decryption: 4.97 seconds

3. **Efficient Modulus Computation** I already implemented this optimization

4. **Const References** I already implemented this optimization.

5. **Changing Base** I didn't modify BigNum to run with base 10,000. My implementation uses base `2^64`. I have a byte array
   defined as `uint64_t m_container[MAX_DIGITS]`. This is a more efficient implementation because it makes use of every bit.

6. **Caching Values** This question is really interesting. Since we're doing 512 bit rsa, encryption / decryption, the largest number
   obtained by multiplying a 512 bit by 512 bit number is 1024 bits long. If we take the largest number that can be represented by
   1024 bits, divided by `n` in our case, we get a number that is 155 digits long, so approx. 10^154. Even if each big number took up 1 byte of memory, there would not be enough memory on any computer in the world to cache the division results.

   We use `n` as the example because most of the bignum time is spent in modulus (per the gprof analysis), so division by `n` is
   going to be the most common, thus, it is where we can theoretically obtain the most speedup by implementing this idea.

   However, we can use "Barret Reduction" to speed up the modulus step because `n` is kept constant in both encryption and decryption. The premise of Barret Reduction is that we calculate `x mod N = r` by calculating a `q` such that `r = x - q * N`. Without going into the details, this method leverages the fact that dividing and multiplying
   by powers of `2` is fast (using right/left shift), and that multiplication is way faster than division.

   **Naive modulus**

   ```
      %   cumulative   self              self     total
   time   seconds   seconds    calls   s/call   s/call  name
   56.23     51.51    51.51 120553918     0.00     0.00  UnsignedBigInt::operator>>=(unsigned long const&)
   36.47     84.92    33.41 54418982     0.00     0.00  UnsignedBigInt::operator-=(UnsignedBigInt const&)
   3.99     88.58     3.65 168779186     0.00     0.00  UnsignedBigInt::operator<(UnsignedBigInt const&) const
   1.78     90.20     1.63   207974     0.00     0.00  UnsignedBigInt::operator%=(UnsignedBigInt const&)
   0.70     90.84     0.64   339064     0.00     0.00  UnsignedBigInt::operator*=(UnsignedBigInt const&)
   0.56     91.36     0.51 102936473     0.00     0.00  UnsignedBigInt::operator>=(UnsignedBigInt const&) const
   ```

   **Barret Reduction Modulus**

   ```
   %   cumulative   self              self     total
   time   seconds   seconds    calls  ms/call  ms/call  name
   89.54      4.88     4.88  3471788     0.00     0.00  UnsignedBigInt::operator*=(UnsignedBigInt const&)
   5.32      5.17     0.29  2013039     0.00     0.00  UnsignedBigInt::operator>>=(unsigned long const&)
   2.94      5.33     0.16   533521     0.00     0.00  UnsignedBigInt::operator-=(UnsignedBigInt const&)
   0.92      5.38     0.05  1691976     0.00     0.00  UnsignedBigInt::operator<(UnsignedBigInt const&) const
   0.18      5.39     0.01   569012     0.00     0.00  UnsignedBigInt::operator^=(UnsignedBigInt const&)
   ```

   We can see the number of `-=` and `>>=` calls be reduced by over 30x at the cost of the number of
   multiplications increasing by 10x. However, this is a good tradeoff because the total number of operations
   is still reduced. Plus, multiplication is fast at 1.4 **microseconds** per call.

   If we do a time test:

   - Naive ModExp Encryption: 0.11 seconds
   - Naive ModExp Decryption: 4.97 seconds

   - Barret Reduction Encryption: 0.00 seconds
   - Barret Reduction Decryption: 0.11 seconds
