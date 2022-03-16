
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

// sys_nspires.S -- functions to align and redirect the stack on the TI-Nspire CX [CAS]


	.cpu arm926ej-s
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 4
	.eabi_attribute 18, 4
	.text

	.align	2
	.global	nspire_stack_redirect
	.type	nspire_stack_redirect, %function
nspire_stack_redirect:
	stmfd	sp!, {r1, r2, r3, r4, lr}
	mov	r4, sp
	mov sp, r1
	blx	r0
	mov sp, r4
	ldmfd	sp!, {r1, r2, r3, r4, pc}


	.align	2
	.global	nspire_stack_align
	.type	nspire_stack_align, %function
nspire_stack_align:
	stmfd	sp!, {r0, r1, r2, lr}
	mov	r4, sp
	tst sp, #4
	subne sp, sp, #4
	bl main_s(PLT)
	mov sp, r4
	ldmfd	sp!, {r1, r2, r3, pc}


