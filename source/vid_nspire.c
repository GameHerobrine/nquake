/*
Copyright (C) 1996-1997 Id Software, Inc.

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
// vid_nspire.c -- video driver for the TI-Nspire CX [CAS]

#ifndef WIN32
#include "os.h"
#endif

#include "quakedef.h"
#include "d_local.h"

viddef_t	vid;				// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	240
#define PALETTEREGS ( ( volatile unsigned int * ) 0xC0000200 )

byte	vid_buffer[BASEWIDTH*BASEHEIGHT] __attribute__ ((aligned (8))) ;
short	zbuffer[BASEWIDTH*BASEHEIGHT] __attribute__ ((aligned (8)));
byte	surfcache[SURFCACHE_SIZE_AT_320X200] __attribute__ ((aligned (8)));

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];


unsigned char rgui8_palette[ 256 ][ 3 ];
unsigned int rgui_palette[ 128 ];
unsigned short rgui_saved_palette[ 128 ];

byte align256_colormap[ 0x4000 + 255 ];

void VID_UpdatePalette( )
{
	int i;
	for( i = 0; i < 128; i++ )
	{
		rgui_palette[ i ] = ( rgui8_palette[ i * 2 ][ 2 ] >> 3 ) | ( ( rgui8_palette[ i * 2 ][ 1 ] >> 3 ) << 5 ) | ( ( rgui8_palette[ i * 2 ][ 0 ] >> 3 ) << 10 );
		rgui_palette[ i ] |= ( ( rgui8_palette[ i * 2 + 1 ][ 2 ] >> 3 ) | ( ( rgui8_palette[ i * 2 + 1 ][ 1 ] >> 3 ) << 5 ) | ( ( rgui8_palette[ i * 2 + 1 ][ 0 ] >> 3 ) << 10 ) ) << 16;
		PALETTEREGS[ i ] = rgui_palette[ i ];
	}
}

void	VID_SetPalette (unsigned char *palette)
{
	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void	VID_ShiftPalette (unsigned char *palette)
{
	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void VID_SetPaletteMode()
{
	int i;
	unsigned int i_ctrl;
	volatile unsigned int *pi_lcd_control = IO_LCD_CONTROL;

	if( !has_colors )
	{
		return; /* this should never happen as we check it in main() */
	}

	for( i = 0; i < 128; i++ )
	{
		rgui_saved_palette[ i ] = PALETTEREGS[ i ];
	}

	i_ctrl = ( *pi_lcd_control ) & ~0b1110; /* clear mode */

	 /* PL111 - 8bpp mode */
	*pi_lcd_control = i_ctrl | 0b0110;
}

void VID_RestoreColorMode()
{
	int i;
	if( !has_colors )
	{
		return; /* this should never happen as we check it in main() */
	}

	for( i = 0; i < 128; i++ )
	{
		PALETTEREGS[ i ] = rgui_saved_palette[ i ];
	}


	lcd_incolor();
}

void	VID_Init (unsigned char *palette)
{
	byte *aligned_colormap;
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;

	/* man the colormap has just been loaded and then we copy it over and let it sit there unused while quake runs.. wasted 16k, sad.. */
	aligned_colormap = ( byte * )( ( ( ( size_t )&align256_colormap[ 0 ] ) + 255 ) & ~0xff );
	memcpy( aligned_colormap, host_colormap, 0x4000 );
	vid.colormap = aligned_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;
	
	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));

	VID_SetPaletteMode();

	memcpy( rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	VID_UpdatePalette();
}

void	VID_Shutdown (void)
{
	VID_RestoreColorMode();
}

static inline void VID_CopyLine( unsigned char *dst, unsigned char *src, int line_width )
{
	/* FIXME: why is the memcpy call faster ? */
#if 0
	int i_idx, i_end;
	long long pels0, pels1, *pllsrc, *plldst;

	if( (size_t)dst & 0x7 || (size_t)src & 0x7 )
	{
#endif
		memcpy( dst, src, line_width );
#if 0
		return;
	}

	pllsrc = ( long long *)src;
	plldst = ( long long *)dst;

	__builtin_prefetch( pllsrc );
	__builtin_prefetch( pllsrc + 32 );

	i_end = line_width & ~0x3f;
	i_idx = 0;
	while( i_idx < i_end )
	{
		__builtin_prefetch( pllsrc + 64 );
		__builtin_prefetch( pllsrc + 64+32 );
		pels0 = *( pllsrc++ );
		pels1 = *( pllsrc++ );
		*( plldst++ ) = pels0;
		*( plldst++ ) = pels1;
		pels0 = *( pllsrc++ );
		pels1 = *( pllsrc++ );
		*( plldst++ ) = pels0;
		*( plldst++ ) = pels1;
		pels0 = *( pllsrc++ );
		pels1 = *( pllsrc++ );
		*( plldst++ ) = pels0;
		*( plldst++ ) = pels1;
		pels0 = *( pllsrc++ );
		pels1 = *( pllsrc++ );
		*( plldst++ ) = pels0;
		*( plldst++ ) = pels1;
		i_idx += 64;
	}
	if( i_end < line_width )
	{
		int i_delt = line_width - i_end;

		if( i_delt & 0x20 )
		{
			pels0 = *( pllsrc++ );
			pels1 = *( pllsrc++ );
			*( plldst++ ) = pels0;
			*( plldst++ ) = pels1;
			pels0 = *( pllsrc++ );
			pels1 = *( pllsrc++ );
			*( plldst++ ) = pels0;
			*( plldst++ ) = pels1;
		}
		if( i_delt & 0x10 )
		{
			pels0 = *( pllsrc++ );
			pels1 = *( pllsrc++ );
			*( plldst++ ) = pels0;
			*( plldst++ ) = pels1;
		}
		if( i_delt & 0x8 )
		{
			pels0 = *( pllsrc++ );
			*( plldst++ ) = pels0;
		}
		i_delt = i_delt & 0x7;
		src = ( unsigned char * ) pllsrc;
		dst = ( unsigned char * ) plldst;
		while( i_delt-- )
		{
			*( dst++ ) = *( src++ );
		}
	}
#endif
}

void	VID_Update (vrect_t *rects)
{
#ifdef FORNSPIRE
	int i_x, i_y, i_width, i_line_left;
	unsigned char *ptr = SCREEN_BASE_ADDRESS;
	unsigned char *pui8_dstline, *pui8_srcline;

	i_width = SCREEN_WIDTH;

	if( rects->x == 0 && rects->width == BASEWIDTH )
	{
		pui8_srcline = &vid_buffer[ rects->y * BASEWIDTH ];
		pui8_dstline = &ptr[ rects->y * i_width ];
		VID_CopyLine( pui8_dstline, pui8_srcline, rects->height * BASEWIDTH );
	}
	else
	{
		i_x = rects->x;
		for( i_y = rects->y; i_y < rects->y+rects->height; i_y++ )
		{
			pui8_srcline = &vid_buffer[ i_y * BASEWIDTH + i_x ];
			pui8_dstline = &ptr[ i_y * i_width + i_x ];
			VID_CopyLine( pui8_dstline, pui8_srcline, rects->width );
		}
	}
#else
	volatile int i_pel;
	i_pel = 0;
#endif
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}


/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
}


