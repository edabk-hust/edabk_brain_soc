#ifndef __Forward_test_1__
#define __Forward_test_1__

#include <queue>
#include <cmath>
#include <stdint.h>
#include <ap_int.h>


#define MAX_DIM 28
#define NUM_CLASSES 10


// 1-bit unsigned integer. You can use a bool or uint8_t for this purpose, as there's no direct 1-bit integer type in standard C++.
typedef uint8_t bit_t;

// Stream for 1-bit unsigned integer.
typedef std::queue<bit_t> bit_stream_t;
ap_uint<4> forward_pass(volatile int16_t *image);
#endif

#ifndef __Functions__
#define __Functions__
// Function to calculate maximum of two floating point numbers
int16_t max_custom_float(int16_t a, int16_t b) {
    // Extracting sign, exponent, and mantissa for 'a'
    int16_t sign_a = (a >> 15) & 1;
    int16_t exponent_a = (a >> 10) & (int16_t)0x1F;
    int16_t mantissa_a = a & (int16_t)0x3FF;

    // Extracting sign, exponent, and mantissa for 'b'
    int16_t sign_b = (b >> 15) & 1;
    int16_t exponent_b = (b >> 10) & (int16_t)0x1F;
    int16_t mantissa_b = b & (int16_t)0x3FF;



    // If both numbers have the same sign
    if (sign_a == sign_b) {
        if (exponent_a == exponent_b) {
            return (mantissa_a > mantissa_b) ? a : b;
        } else {
            return (exponent_a > exponent_b) ? a : b;
        }
    } else {
        // If 'a' is positive and 'b' is negative, return 'a'; else return 'b'
        return sign_a ? b : a;
    }
}

// ReLU Activation function
int16_t relu_custom_float(int16_t x) {
    // Extracting the sign bit
    int16_t sign_bit = (x >> 15) & 1;

    // If the sign bit is 1 (i.e., x is negative), then return 0
    if (sign_bit) {
        return 0;
    }

    // Otherwise, return x as it is (since x is already non-negative)
    return x;
}

int16_t subtract_custom_float(int16_t a, int16_t b) {
    // Extracting sign, exponent, and mantissa for 'a'
	if(a==b) return 0;
    int16_t sign_a = (a >> 15) & 1;
    int16_t exponent_a = (a >> 10) & (int16_t)0x1F;
    int32_t mantissa_a = (a & (int16_t)0x3FF) | (int16_t)0x400; // Add implicit leading bit

    // Extracting sign, exponent, and mantissa for 'b'
    int16_t sign_b = (b >> 15) & 1;
    int16_t exponent_b = (b >> 10) & (int16_t)0x1F;
    int32_t mantissa_b = (b & (int16_t)0x3FF) | (int16_t)0x400; // Add implicit leading bit

    // Align mantissas by shifting the one with the smaller exponent
    int16_t i_loop;
    i_loop = 0;
    while (exponent_a > exponent_b) {
        mantissa_b >>= 1;
        exponent_b++;
        i_loop++;
        if(i_loop>25)
        {
        	break;
        }
    }

    while (exponent_b > exponent_a) {
        mantissa_a >>= 1;
        exponent_a++;
        i_loop++;
        if(i_loop>25)
        {
        	break;
        }
    }

    // Perform subtraction, taking the sign into account
    int32_t mantissa_result;
    if (sign_a == sign_b) {
        mantissa_result = mantissa_a - mantissa_b;
    } else {
        mantissa_result = mantissa_a + mantissa_b;
    }

    // Determine result sign
    int16_t sign_result = sign_a;
    if (mantissa_result < 0) {
        sign_result ^= 1;
        mantissa_result = -mantissa_result;
    }

    // Normalize result
    while (mantissa_result < (int16_t)0x400 && mantissa_result != 0) {
        mantissa_result <<= 1;
        exponent_a--;
        i_loop++;
        if(i_loop>25)
        {
        	break;
        }
    }

    // Remove implicit leading bit
    mantissa_result &= (int16_t)0x3FF;

    // Combining sign, exponent, and mantissa into a 16-bit integer
    int16_t result = (sign_result << 15) | ((exponent_a & (int16_t)0x1F) << 10) | mantissa_result;

    return result;
}
/*
int16_t add(uint16_t a, int16_t b) {
	if (a == 0) return b;
	if (b == 0) return a;

  int16_t signA = a >> 15;
  int16_t signB = b >> 15;
  int16_t expA = (a >> 10) & (int16_t)0x1F;
  int16_t expB = (b >> 10) & (int16_t)0x1F;
  int16_t mantissaA = a & (int16_t)0x3FF;
  int16_t mantissaB = b & (int16_t)0x3FF;

  // Denormalize the mantissas
  mantissaA |= (int16_t)0x400;
  mantissaB |= (int16_t)0x400;

  // If the exponents are not equal, shift the mantissa of the smaller value
  while (expA < expB) {
    mantissaA >>= 1;
    expA++;
  }
  while (expB < expA) {
    mantissaB >>= 1;
    expB++;
  }

  // Perform addition or subtraction based on the signs
  int32_t resultMantissa;
  if (signA == signB) {
    resultMantissa = mantissaA + mantissaB;
  } else if (signA) {
    resultMantissa = mantissaB - mantissaA;
  } else {
    resultMantissa = mantissaA - mantissaB;
  }

  // Normalize the result
  while (resultMantissa >= (int16_t)0x800) {
    resultMantissa >>= 1;
    expA++;
  }

  // Create the result
  int16_t result = (resultMantissa & (int16_t)0x3FF) | (expA << 10);
  if ((signA == signB && signA) || (signA != signB && resultMantissa < 0)) {
    result |= (int16_t)0x8000;
  }

  return result;
}
*/

int16_t add(int16_t a, int16_t b) {
  if (a == 0) return b;
  if (b == 0) return a;
  int16_t signA = a >> 15;
  int16_t signB = b >> 15;
  int16_t expA = (a >> 10) & 0x1F;
  int16_t expB = (b >> 10) & 0x1F;
  int16_t mantissaA = a & 0x3FF;
  int16_t mantissaB = b & 0x3FF;
  int16_t i_loop;

  // Denormalize the mantissas
  mantissaA |= 0x400;
  mantissaB |= 0x400;

  // Align mantissas by shifting the one with the smaller exponent
  i_loop = 0;
  while (expA < expB) {
    mantissaA >>= 1;
    expA++;
    i_loop++;
    if(i_loop>25)
    {
    	break;
    }
  }

  i_loop = 0;
  while (expB < expA) {
    mantissaB >>= 1;
    expB++;
    i_loop++;
    if(i_loop>25)
    {
    	break;
    }
  }

  // Perform addition or subtraction based on the signs
  int32_t resultMantissa;
  if (signA == signB) {
    resultMantissa = mantissaA + mantissaB;
  } else if (signA) {
    resultMantissa = mantissaB - mantissaA;
  } else {
    resultMantissa = mantissaA - mantissaB;
  }

  int16_t resultSign = (resultMantissa < 0) ? 1 : 0;

  if (resultMantissa < 0) {
  resultSign = 1;
  resultMantissa = -resultMantissa;
  } else {
    resultSign = 0;
  }

  // Normalize the result
  i_loop = 0;
  while (resultMantissa >= 0x800) {
    resultMantissa >>= 1;
    expA++;
    i_loop++;
    if(i_loop>25)
    {
    	break;
    }
  }

  i_loop = 0;
  while (resultMantissa < 0x400) {
    resultMantissa <<= 1;
    expA--;
    i_loop++;
    if(i_loop>25)
    {
    	break;
    }
  }

  // Create the result
  int16_t result = (resultMantissa & 0x3FF) | (expA << 10);
  if ((signA && signB) || resultSign) {
    result |= 0x8000;
  }

  return result;
}
/*
int16_t multiply_custom_float(int16_t a, int16_t b) {
	if (a == 0 || b == 0) return 0;
    // Extracting sign, exponent, and mantissa for 'a'
    int16_t sign_a = (a >> 15) & 1;
    int16_t exponent_a = (a >> 10) & (int16_t)0x1F;
    int16_t mantissa_a = a & (int16_t)0x3FF;

    // Extracting sign, exponent, and mantissa for 'b'
    int16_t sign_b = (b >> 15) & 1;
    int16_t exponent_b = (b >> 10) & (int16_t)0x1F;
    int16_t mantissa_b = b & (int16_t)0x3FF;

    // Calculating the result's sign, exponent, and mantissa
    int16_t sign_result = sign_a ^ sign_b;
    int16_t exponent_result = exponent_a + exponent_b - 15;
    int32_t mantissa_result = (1024 + mantissa_a) * (1024 + mantissa_b);

    // Normalizing the mantissa
    if (mantissa_result >= (1 << 21)) {
        mantissa_result >>= 1;
        exponent_result += 1;
    }
    mantissa_result = (mantissa_result >> 10) & (int16_t)0x3FF;

    // Combining sign, exponent, and mantissa into a 16-bit integer
    int16_t result = (sign_result << 15) | ((exponent_result & (int16_t)0x1F) << 10) | mantissa_result;

    return result;
}
*/
int16_t multiply_custom_float(int16_t a, int16_t b) {
    if (a == 0 || b == 0) return 0;

    // Extracting sign, exponent, and mantissa for 'a'
    int16_t sign_a = (a >> 15) & 1;
    int16_t exponent_a = (a >> 10) & (int16_t)0x1F;
    int16_t mantissa_a = a & (int16_t)0x3FF;

    // Extracting sign, exponent, and mantissa for 'b'
    int16_t sign_b = (b >> 15) & 1;
    int16_t exponent_b = (b >> 10) & (int16_t)0x1F;
    int16_t mantissa_b = b & (int16_t)0x3FF;

    // Calculating the result's sign, exponent, and mantissa
    int16_t sign_result = sign_a ^ sign_b;
    int16_t exponent_result = (exponent_a - 15) + (exponent_b - 15) + 15; // Remove bias, add, then add bias back
    int32_t mantissa_result = (1024 + mantissa_a) * (1024 + mantissa_b);

    // Normalizing the mantissa
    if (mantissa_result >= (1 << 21)) {
        mantissa_result >>= 1;
        exponent_result += 1;
    }
    mantissa_result = (mantissa_result >> 10) - 1024; // Remove the implicit leading one

    // Check for underflow or overflow
    if (exponent_result < 0) return 0; // Underflow
    if (exponent_result >= 0x1F) return sign_result ? (int16_t)0x8000 : (int16_t)0x7FFF; // Overflow

    // Combining sign, exponent, and mantissa into a 16-bit integer
    int16_t result = (sign_result << 15) | ((exponent_result & (int16_t)0x1F) << 10) | (mantissa_result & (int16_t)0x3FF);

    return result;
}

int16_t divide_custom_float(int16_t a, int16_t b) {

    int16_t sign_a = (a >> 15) & 1;
    int16_t exponent_a = (a >> 10) & (int16_t)0x1F;
    int16_t mantissa_a = (a & (int16_t)0x3FF) | (int16_t)0x400;

    int16_t sign_b = (b >> 15) & 1;
    int16_t exponent_b = (b >> 10) & (int16_t)0x1F;
    int16_t mantissa_b = (b & (int16_t)0x3FF) | (int16_t)0x400;

    if (mantissa_b == 0) return 0;

    int16_t sign_result = sign_a ^ sign_b;
    int16_t exponent_result = exponent_a - exponent_b + 15;

    int32_t remainder = mantissa_a << 10;
    int32_t divisor = mantissa_b << 10;
    int32_t quotient = 0;
    for (int i = 0; i < 10; i++) {
        remainder <<= 1;
        if (remainder >= divisor) {
            remainder -= divisor;
            quotient = (quotient << 1) | 1;
        } else {
            quotient <<= 1;
        }
    }

    if (quotient < (int16_t)0x400) {
        quotient <<= 1;
        exponent_result -= 1;
    }
    int16_t mantissa_result = quotient & (int16_t)0x3FF;

    int16_t result = (sign_result << 15) | ((exponent_result & (int16_t)0x1F) << 10) | mantissa_result;

    return result;
}


int16_t exp_custom_float(int16_t x) {
	if (x == 0) return 0x3C00;
    int16_t number_1 = (int16_t)0x3C00; // Representation of 1.0
    int16_t number_2_fact = (int16_t)0x4000; // Representation of 2! (which is 2)
    int16_t number_3_fact = (int16_t)0x4600; // Representation of 3! (which is 6)

    // Representation of 4! (which is 24) with the given structure
    int16_t number_4_fact = (int16_t)0x4E00;
    // Representation of 5! (which is 120) with the given structure
    int16_t number_5_fact = (int16_t)0x5780;

        // The result now represents 1 + x + x^2/2! + x^3/3! + x^4/4! + x^5/5!, which are the first six terms of the Taylor series for e^x.
        // Check for negative value
    int16_t is_negative = x >> 15;

    if (is_negative) {
        x = x & (int16_t)0x7FFF; // Convert to positive by two's complement
    }
    int16_t result = add(number_1, x);


    int16_t x_squared = multiply_custom_float(x, x);
    int16_t x_squared_div_2_fact = divide_custom_float(x_squared, number_2_fact);
    result = add(result, x_squared_div_2_fact);



    int16_t x_cubed = multiply_custom_float(x_squared, x);
    int16_t x_cubed_div_3_fact = divide_custom_float(x_cubed, number_3_fact);
    result = add(result, x_cubed_div_3_fact);


    int16_t x_quartic = multiply_custom_float(x_cubed, x);

    int16_t x_quartic_div_4_fact = divide_custom_float(x_quartic, number_4_fact);
    result = add(result, x_quartic_div_4_fact);

    int16_t x_quintic = multiply_custom_float(x_quartic, x);
    int16_t x_quintic_div_5_fact = divide_custom_float(x_quintic, number_5_fact);
    result = add(result, x_quintic_div_5_fact);


        // If the original number was negative, take the reciprocal
    if (is_negative) {
        result = divide_custom_float(number_1, result);
    }
    return result;
}

void softmax(int16_t x[10]) {
    int16_t max = x[0];
    int16_t sum = 0;
    int16_t aux_sum;

    for(int i = 0; i < 10; i++) {
        max = max_custom_float(max, x[i]);
    }

    for(int i = 0; i < 10; i++) {
        // Cast to float, compute exponential, cast back to int16_t
        int16_t temp = subtract_custom_float(x[i],max);
        //x[i] = hls::exp(temp);
        x[i] = exp_custom_float(temp);
        sum = add(sum,x[i]);
    }

    for(int i = 0; i < 10; i++) {
        x[i] = divide_custom_float(x[i],sum);
    }
}

int greater_than_custom_float(int16_t a, int16_t b) {
    int16_t signA = a >> 15;
    int16_t signB = b >> 15;

    if (signA != signB) {
        return signB;
    }

    int16_t expA = (a >> 10) & (int16_t)0x1F;
    int16_t expB = (b >> 10) & (int16_t)0x1F;

    if (expA != expB) {
        return signA ? (expA < expB) : (expA > expB);
    }

    int16_t mantissaA = a & (int16_t)0x3FF;
    int16_t mantissaB = b & (int16_t)0x3FF;

    return signA ? (mantissaA < mantissaB) : (mantissaA > mantissaB);
}


//int16_t dense[10];

#endif
