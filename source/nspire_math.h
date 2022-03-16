/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// nspire_math.h -- math functions for the ARM926EJ-S as used in the TI-Nspire CX [CAS]

#if FORNSPIRE
extern unsigned int udiv_fast_32_32_correct(unsigned int d, unsigned int n);
extern unsigned int udiv_64_32( unsigned long long n, unsigned int d );
extern unsigned int udiv_s31_32(unsigned int d, unsigned int n);

static inline long long mul_64_32_r64( long long m0, int m1 )
{
	int __tmp;
	long long __ires;
	__asm volatile(															\
		"smull %Q[result], %R[result], %Q[_m0], %[_m1]	\n\t"				\
		"tst %Q[_m0], #0x80000000						\n\t"				\
		"addne %R[result], %R[result], %[_m1]			\n\t"				\
		"smlal %R[result], %[_tmp], %R[_m0], %[_m1]		\n\t"				\
		: [result] "=&r" (__ires), [_tmp] "=&r" (__tmp ) : [_m0] "r" (m0), [_m1] "r" (m1) : );
	return __ires;
}

static inline long long llmull_s0( int m0, int m1 )
{
	long long __ires;
	__asm volatile(														\
		"smull %Q[result], %R[result], %[_m0], %[_m1] \n"					\
		: [result] "=&r" (__ires) : [_m0] "r" (m0), [_m1] "r" (m1) : );
	return __ires;
}

static inline int llmull_s16( int m0, int m1 )
{
	int __ires;
	__asm volatile(														\
		"smull %[result], %[_m1], %[_m0], %[_m1] \n"					\
		"mov %[result], %[result], lsr #16 \n"							\
		"orr %[result], %[_m1], asl #16 \n"								\
		: [result] "=&r" (__ires), [_m1] "+r" (m1) : [_m0] "r" (m0) : );
	return __ires;
}

static inline int llmull_s21( int m0, int m1 )
{
	int __ires;
	__asm volatile(														\
		"smull %[result], %[_m1], %[_m0], %[_m1] \n"					\
		"mov %[result], %[result], lsr #21 \n"							\
		"orr %[result], %[_m1], asl #11 \n"								\
		: [result] "=&r" (__ires), [_m1] "+r" (m1) : [_m0] "r" (m0) : );
	return __ires;
}

static inline int llmull_s24( int m0, int m1 )
{
	int __ires;
	__asm volatile(														\
		"smull %[result], %[_m1], %[_m0], %[_m1] \n"					\
		"mov %[result], %[result], lsr #24 \n"							\
		"orr %[result], %[_m1], asl #8 \n"								\
		: [result] "=&r" (__ires), [_m1] "+r" (m1) : [_m0] "r" (m0) : );
	return __ires;
}

static inline int llmull_s27( int m0, int m1 )
{
	int __ires;
	__asm volatile(														\
		"smull %[result], %[_m1], %[_m0], %[_m1] \n"					\
		"mov %[result], %[result], lsr #27 \n"							\
		"orr %[result], %[_m1], asl #5 \n"								\
		: [result] "=&r" (__ires), [_m1] "+r" (m1) : [_m0] "r" (m0) : );
	return __ires;
}

static inline int llmull_s28( int m0, int m1 )
{
	int __ires;
	__asm volatile(														\
		"smull %[result], %[_m1], %[_m0], %[_m1] \n"					\
		"mov %[result], %[result], lsr #28 \n"							\
		"orr %[result], %[_m1], asl #4 \n"								\
		: [result] "=&r" (__ires), [_m1] "+r" (m1) : [_m0] "r" (m0) : );
	return __ires;
}

#else

#define udiv_fast_32_32_correct(denom, num) ( ( ( num ) / ( denom ) ) )
#define udiv_64_32( n, d ) ( (unsigned int)((((unsigned long long)(n)))/(d)) )
#define udiv_s31_32( d, n ) ( (unsigned int)((((unsigned long long)(n)) << 31)/(d)) )
#define llmull_s0( m0, m1 ) ( ( ( long long )( m0 ) * ( m1 ) ) )
#define llmull_s16( m0, m1 ) ( int )( ( ( long long )( m0 ) * ( m1 ) ) >> 16 )
#define llmull_s21( m0, m1 ) ( int )( ( ( long long )( m0 ) * ( m1 ) ) >> 21 )
#define llmull_s24( m0, m1 ) ( int )( ( ( long long )( m0 ) * ( m1 ) ) >> 24 )
#define llmull_s27( m0, m1 ) ( int )( ( ( long long )( m0 ) * ( m1 ) ) >> 27 )
#define llmull_s28( m0, m1 ) ( int )( ( ( long long )( m0 ) * ( m1 ) ) >> 28 )

static long long mul_64_32_r64( long long m0, int m1 )
{
	return m0 * m1;
}

#endif