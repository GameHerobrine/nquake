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

// d_scan_nspire.c -- span drawing functions for the ARM926EJ-S as used in the TI-Nspire CX [CAS]

#include "quakedef.h"

#include "d_local.h"

/*extern int bbextents;
extern int bbextentt;*/


#if FORNSPIRE

static inline void draw_span_nspire_fw_8( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth )
{
	int i_tmp;
	__asm volatile(														\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #1										\n\t"	\
	"smlabt	%[_pbase], %[_i_cachewidth], %[_f16_rpt], %[_pbase]					\n\t"	\
	"ldrb	%[_f16_sstep], [%[_pbase], %[_f16_rps], asr #16]					\n\t"	\
	"strb	%[_f16_sstep], [%[_pdest], #0]										\n\t"	\
	: [_pdest] "+r" (pdest), [_pbase] "+r" (pbase), [_f16_rps] "+r" (f16_rps), [_f16_sstep] "+r" (f16_sstep), [_f16_rpt] "+r" (f16_rpt), [_f16_tstep] "+r" (f16_tstep), [_i_cachewidth] "+r" (i_cachewidth), [_i_tmp] "=r" (i_tmp) : : "cc", "memory" );
}


static inline void draw_span_nspire_bw_8( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth )
{
	int i_tmp;
	__asm volatile (
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_i_tmp], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_tmp], [%[_i_tmp], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_tmp], [%[_pdest]], #-1										\n\t"	\
	"smlabt	%[_pbase], %[_i_cachewidth], %[_f16_rpt], %[_pbase]					\n\t"	\
	"ldrb	%[_f16_sstep], [%[_pbase], %[_f16_rps], asr #16]					\n\t"	\
	"strb	%[_f16_sstep], [%[_pdest], #0]										\n\t"	\
	: [_pdest] "+r" (pdest), [_pbase] "+r" (pbase), [_f16_rps] "+r" (f16_rps), [_f16_sstep] "+r" (f16_sstep), [_f16_rpt] "+r" (f16_rpt), [_f16_tstep] "+r" (f16_tstep), [_i_cachewidth] "+r" (i_cachewidth), [_i_tmp] "=r" (i_tmp) : : "cc", "memory" );
}

static inline void draw_span_nspire_fw_s( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count )
{
	__asm volatile (
	"sub	%[_i_count], %[_i_count], #1										\n\t"	\
	"cmp	%[_i_count], #7														\n\t"	\
	"addls	pc, pc, %[_i_count], asl #2											\n\t"	\
	"b	.draw_span_nspire_fw_s0													\n\t"	\
	"b	.draw_span_nspire_fw_s1													\n\t"	\
	"b	.draw_span_nspire_fw_s2													\n\t"	\
	"b	.draw_span_nspire_fw_s3													\n\t"	\
	"b	.draw_span_nspire_fw_s4													\n\t"	\
	"b	.draw_span_nspire_fw_s5													\n\t"	\
	"b	.draw_span_nspire_fw_s6													\n\t"	\
	"b	.draw_span_nspire_fw_s7													\n\t"	\
	"b	.draw_span_nspire_fw_s8													\n\t"	\
".draw_span_nspire_fw_s8:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s7:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s6:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s5:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s4:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s3:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s2:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #1										\n\t"	\
".draw_span_nspire_fw_s1:														\n\t"	\
	"smlabt	%[_pbase], %[_i_cachewidth], %[_f16_rpt], %[_pbase]					\n\t"	\
	"ldrb	%[_f16_sstep], [%[_pbase], %[_f16_rps], asr #16]					\n\t"	\
	"strb	%[_f16_sstep], [%[_pdest], #0]										\n\t"	\
".draw_span_nspire_fw_s0:														\n\t"	\
	: [_pdest] "+r" (pdest), [_pbase] "+r" (pbase), [_f16_rps] "+r" (f16_rps), [_f16_sstep] "+r" (f16_sstep), [_f16_rpt] "+r" (f16_rpt), [_f16_tstep] "+r" (f16_tstep), [_i_cachewidth] "+r" (i_cachewidth), [_i_count] "+r" (i_count) : : "cc", "memory" );

}


static inline void draw_span_nspire_bw_s( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count )
{
	__asm volatile (
	"sub	%[_i_count], %[_i_count], #1										\n\t"	\
	"cmp	%[_i_count], #7														\n\t"	\
	"addls	pc, pc, %[_i_count], asl #2											\n\t"	\
	"b	.draw_span_nspire_bw_s0													\n\t"	\
	"b	.draw_span_nspire_bw_s1													\n\t"	\
	"b	.draw_span_nspire_bw_s2													\n\t"	\
	"b	.draw_span_nspire_bw_s3													\n\t"	\
	"b	.draw_span_nspire_bw_s4													\n\t"	\
	"b	.draw_span_nspire_bw_s5													\n\t"	\
	"b	.draw_span_nspire_bw_s6													\n\t"	\
	"b	.draw_span_nspire_bw_s7													\n\t"	\
	"b	.draw_span_nspire_bw_s8													\n\t"	\
".draw_span_nspire_bw_s8:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s7:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s6:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s5:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s4:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s3:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s2:														\n\t"	\
	"smlabt	%[_i_count], %[_i_cachewidth], %[_f16_rpt], %[_pbase]				\n\t"	\
	"add	%[_f16_rpt], %[_f16_rpt], %[_f16_tstep]								\n\t"	\
	"ldrb	%[_i_count], [%[_i_count], %[_f16_rps], asr #16]					\n\t"	\
	"add	%[_f16_rps], %[_f16_rps], %[_f16_sstep]								\n\t"	\
	"strb	%[_i_count], [%[_pdest]], #-1										\n\t"	\
".draw_span_nspire_bw_s1:														\n\t"	\
	"smlabt	%[_pbase], %[_i_cachewidth], %[_f16_rpt], %[_pbase]					\n\t"	\
	"ldrb	%[_f16_sstep], [%[_pbase], %[_f16_rps], asr #16]					\n\t"	\
	"strb	%[_f16_sstep], [%[_pdest], #0]										\n\t"	\
".draw_span_nspire_bw_s0:														\n\t"	\
	: [_pdest] "+r" (pdest), [_pbase] "+r" (pbase), [_f16_rps] "+r" (f16_rps), [_f16_sstep] "+r" (f16_sstep), [_f16_rpt] "+r" (f16_rpt), [_f16_tstep] "+r" (f16_tstep), [_i_cachewidth] "+r" (i_cachewidth), [_i_count] "+r" (i_count) : : "cc", "memory" );
}

/*#define llmull_s16( m0, m1 ) ( ( ( long long )( m0 ) * ( m1 ) ) >> 16 );*/

static const unsigned char nr_table[] =
{
	0xff, 0xfc, 0xf8, 0xf4, 0xf0, 0xed, 0xea, 0xe6,
	0xe3, 0xe0, 0xdd, 0xda, 0xd7, 0xd4, 0xd2, 0xcf,
	0xcc, 0xca, 0xc7, 0xc5, 0xc3, 0xc0, 0xbe, 0xbc,
	0xba, 0xb8, 0xb6, 0xb4, 0xb2, 0xb0, 0xae, 0xac,
	0xaa, 0xa8, 0xa7, 0xa5, 0xa3, 0xa2, 0xa0, 0x9f,
	0x9d, 0x9c, 0x9a, 0x99, 0x97, 0x96, 0x94, 0x93,
	0x92, 0x90, 0x8f, 0x8e, 0x8d, 0x8c, 0x8a, 0x89,
	0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81
};

static inline int udiv_fast_32_32( int den, int num )
{
	int tmp1, tmp3;
	const unsigned char *pui8_nr_table = &nr_table[ 0 ];
	__asm volatile (
	"clz %[_tmp1], %[_den]								\n\t"	\
	"movs %[_tmp3], %[_den], lsl %[_tmp1]				\n\t"	\
	"sub %[_tmp2], %[_tmp2], #64						\n\t"	\
	"ldrneb %[_tmp3], [%[_tmp2], %[_tmp3], lsr #25 ]	\n\t"	\
	"subs %[_tmp1], %[_tmp1], #7						\n\t"	\
	"rsb %[_tmp2], %[_den], #0							\n\t"	\
	"movpl %[_den], %[_tmp3], lsl %[_tmp1]				\n\t"	\
	"mulpl %[_tmp3], %[_den], %[_tmp2]					\n\t"	\
	"bmi 1f												\n\t"	\
	"smlawt %[_den], %[_den], %[_tmp3], %[_den]			\n\t"	\
	"teq %[_tmp2], %[_tmp2], asr #1						\n\t"	\
	"mulne %[_tmp3], %[_den], %[_tmp2]					\n\t"	\
	"movne %[_tmp1], #0									\n\t"	\
	"smlalne %[_tmp1], %[_den], %[_tmp3], %[_den]		\n\t"	\
	"beq 2f												\n\t"	\
	"umull %[_tmp1], %[_den], %[_num], %[_den]			\n\t"	\
	"add %[_num], %[_num], %[_tmp2]						\n\t"	\
	"mla %[_num], %[_den], %[_tmp2], %[_num]			\n\t"	\
	"cmn %[_num], %[_tmp2]								\n\t"	\
	"addcc %[_den], %[_den], #1							\n\t"	\
	"addpl %[_den], %[_den], #2							\n\t"	\
	"bl 3f												\n\t"	\
"1:														\n\t"	\
	"sub %[_tmp3], %[_tmp3], #4							\n\t"	\
	"rsb %[_tmp1], %[_tmp1], #0							\n\t"	\
	"mov %[_den], %[_tmp3], lsr %[_tmp1]				\n\t"	\
	"umull %[_tmp1], %[_den], %[_num], %[_den]			\n\t"	\
	"mla %[_num], %[_den], %[_tmp2], %[_num]			\n\t"	\
	"cmn %[_tmp2], %[_num], lsr #1						\n\t"	\
	"addcs %[_num], %[_num], %[_tmp2], lsl #1			\n\t"	\
	"addcs %[_den], %[_den], #2							\n\t"	\
	"cmn %[_tmp2], %[_num]								\n\t"	\
	"addcs %[_den], %[_den], #1							\n\t"	\
	"bl 3f												\n\t"	\
"2:														\n\t"	\
	"movcs %[_den], %[_num]								\n\t"	\
	"movcc %[_den], #-1									\n\t"	\
"3:														\n\t"	\
	: [_den] "+r" (den), [_num] "+r" (num), [_tmp1] "=&r" (tmp1), [_tmp2] "+r" (pui8_nr_table), [_tmp3] "=&r" (tmp3) : : "cc" );

	return den;
}

#else

static unsigned int udiv_fast_32_32( unsigned int den, unsigned int num )
{
	return num / den;
}

void draw_span_nspire_fw_8( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth )
{
	byte ui8_pel;

	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest++ = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
}


void draw_span_nspire_bw_8( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth )
{
	byte ui8_pel;

	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
	ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
	*pdest-- = ui8_pel;
	f16_rps += f16_sstep;
	f16_rpt += f16_tstep;
}

static void draw_span_nspire_fw_s( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count )
{
	byte ui8_pel;

	switch( i_count )
	{
	case 8:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 7:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 6:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 5:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 4:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 3:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 2:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 1:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest++ = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	}
}

static void draw_span_nspire_bw_s( byte *pdest, byte *pbase, fixed16_t f16_rps, fixed16_t f16_sstep, fixed16_t f16_rpt, fixed16_t f16_tstep, int i_cachewidth, int i_count )
{
	byte ui8_pel;

	switch( i_count )
	{
	case 8:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 7:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 6:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 5:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 4:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 3:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 2:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	case 1:
		ui8_pel = *(pbase + (f16_rps >> 16) + (f16_rpt >> 16) * i_cachewidth);
		*pdest-- = ui8_pel;
		f16_rps += f16_sstep;
		f16_rpt += f16_tstep;
	}
}


#endif


#define NSPIRE_NOCLIP_FTW 1

void draw_span_nspire_fw_c( draw_span8_nspire_t *ps_dset )
{
	int spancount;
	fixed16_t f16_psnext;
	fixed16_t f16_ptnext;
	register int f16_rps, f16_rpt, f16_sstep = 0, f16_tstep = 0;

	ps_dset->f16_s = 0;
	ps_dset->f16_t = 0;
	do
	{
	// calculate s and t at the far end of the span
		if (ps_dset->count >= 8)
			spancount = 8;
		else
			spancount = ps_dset->count;

		ps_dset->count -= spancount;

		if ( ps_dset->count)
		{
			fixed16_t i_ipersp;
			ps_dset->f16_p = ( ps_dset->f16_p + ( ps_dset->f16_stepp ) );
			i_ipersp = udiv_fast_32_32( ps_dset->f16_p, 0xffffffffU );

			ps_dset->f16_s = ( ps_dset->f16_s + ( ps_dset->f16_steps << 3 ) );
			f16_psnext = ps_dset->f16_sstart + llmull_s16( ps_dset->f16_s, i_ipersp );
#if !NSPIRE_NOCLIP_FTW
			if( f16_psnext > ps_dset->i_bbextents )
			{
				f16_psnext = ps_dset->i_bbextents;
			}
			if( f16_psnext < 8 )
			{
				f16_psnext = 8;
			}
#endif
			f16_sstep = ( f16_psnext - ps_dset->f16_ps ) >> 3;
			f16_rps = ps_dset->f16_ps;
			ps_dset->f16_ps = f16_psnext;


			ps_dset->f16_t = ( ps_dset->f16_t + ( ps_dset->f16_stept << 3 ) );
			f16_ptnext = ps_dset->f16_tstart + llmull_s16( ps_dset->f16_t, i_ipersp );
#if !NSPIRE_NOCLIP_FTW
			if( f16_ptnext > ps_dset->i_bbextentt )
			{
				f16_ptnext = ps_dset->i_bbextentt;
			}
			if( f16_ptnext < 8 )
			{
				f16_ptnext = 8;
			}
#endif
			f16_tstep = ( f16_ptnext - ps_dset->f16_pt ) >> 3;
			f16_rpt = ps_dset->f16_pt;
			ps_dset->f16_pt = f16_ptnext;

			draw_span_nspire_fw_8( ps_dset->pdest, ps_dset->pbase, f16_rps, f16_sstep, f16_rpt, f16_tstep, ps_dset->i_cachewidth );
			ps_dset->pdest += 8;
		}
		else
		{
			static const int rgi_mtab[ 8 ] = { 0, 65536, 32768, 21845, 16384, 13107, 10922, 9362 };

			if (spancount > 1 )
			{
				f16_psnext = ps_dset->f16_psend;
				f16_sstep = llmull_s16( f16_psnext - ps_dset->f16_ps, rgi_mtab[ spancount - 1 ] );

				f16_ptnext = ps_dset->f16_ptend;
				f16_tstep = llmull_s16( f16_ptnext - ps_dset->f16_pt, rgi_mtab[ spancount - 1 ] );
			}

			draw_span_nspire_fw_s( ps_dset->pdest, ps_dset->pbase, ps_dset->f16_ps, f16_sstep, ps_dset->f16_pt, f16_tstep, ps_dset->i_cachewidth, spancount );
		}
	} while (ps_dset->count > 0);
}


void draw_span_nspire_bw_c( draw_span8_nspire_t *ps_dset )
{
	int spancount;
	fixed16_t f16_psnext;
	fixed16_t f16_ptnext;
	register int f16_rps, f16_rpt, f16_sstep = 0, f16_tstep = 0;

	ps_dset->f16_s = 0;
	ps_dset->f16_t = 0;
	do
	{
	// calculate s and t at the far end of the span
		if (ps_dset->count >= 8)
			spancount = 8;
		else
			spancount = ps_dset->count;

		ps_dset->count -= spancount;

		if ( ps_dset->count)
		{
			fixed16_t i_ipersp;
			ps_dset->f16_p = ( ps_dset->f16_p + ( ps_dset->f16_stepp ) );
			i_ipersp = udiv_fast_32_32( ps_dset->f16_p, 0xffffffffU );

			ps_dset->f16_s = ( ps_dset->f16_s + ( ps_dset->f16_steps << 3 ) );
			f16_psnext = ps_dset->f16_sstart + llmull_s16( ps_dset->f16_s, i_ipersp );
#if !NSPIRE_NOCLIP_FTW
			if( f16_psnext > ps_dset->i_bbextents )
			{
				f16_psnext = ps_dset->i_bbextents;
			}
			if( f16_psnext < 8 )
			{
				f16_psnext = 8;
			}
#endif
			f16_sstep = ( f16_psnext - ps_dset->f16_ps ) >> 3;
			f16_rps = ps_dset->f16_ps;
			ps_dset->f16_ps = f16_psnext;


			ps_dset->f16_t = ( ps_dset->f16_t + ( ps_dset->f16_stept << 3 ) );
			f16_ptnext = ps_dset->f16_tstart + llmull_s16( ps_dset->f16_t, i_ipersp );
#if !NSPIRE_NOCLIP_FTW
			if( f16_ptnext > ps_dset->i_bbextentt )
			{
				f16_ptnext = ps_dset->i_bbextentt;
			}
			if( f16_ptnext < 8 )
			{
				f16_ptnext = 8;
			}
#endif
			f16_tstep = ( f16_ptnext - ps_dset->f16_pt ) >> 3;
			f16_rpt = ps_dset->f16_pt;
			ps_dset->f16_pt = f16_ptnext;

			draw_span_nspire_bw_8( ps_dset->pdest, ps_dset->pbase, f16_rps, f16_sstep, f16_rpt, f16_tstep, ps_dset->i_cachewidth );
			ps_dset->pdest -= 8;
		}
		else
		{
			static const int rgi_mtab[ 8 ] = { 0, 65536, 32768, 21845, 16384, 13107, 10922, 9362 };

			if (spancount > 1 )
			{
				f16_psnext = ps_dset->f16_psend;
				f16_sstep = llmull_s16( f16_psnext - ps_dset->f16_ps, rgi_mtab[ spancount - 1 ] );

				f16_ptnext = ps_dset->f16_ptend;
				f16_tstep = llmull_s16( f16_ptnext - ps_dset->f16_pt, rgi_mtab[ spancount - 1 ] );
			}

			draw_span_nspire_bw_s( ps_dset->pdest, ps_dset->pbase, ps_dset->f16_ps, f16_sstep, ps_dset->f16_pt, f16_tstep, ps_dset->i_cachewidth, spancount );
		}
	} while (ps_dset->count > 0);
}
