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

extern viddef_t	vid;				// global video state

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
unsigned int rgui_saved_palette[ 128 ];

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

	if( lcd_init( SCR_320x240_8 ) == 0 )
	{
		Sys_Error( "unable to set 320x240 8bpp mode" );
	}
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

	if( lcd_init( lcd_type() ) == 0 )
	{
		Sys_Error( "could not restore native lcd mode"); /* fuck */
	}
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


static inline void VID_CopyTile_8x8_flipped( unsigned char *dst, int32_t i_dst_stride, unsigned char *srci, int32_t i_src_stride )
{
	int i_x, i_y;
	const unsigned char *src;

	src = ( const unsigned char * ) srci;

	for( i_y = 0; i_y < 8; i_y++ )
	{
		for( i_x = 0; i_x < 8; i_x++ )
		{
			dst[ i_y + i_x * i_dst_stride ] = src[ i_x + i_y * i_src_stride ];
		}
	}
}

static inline void VID_CopyTile_flipped( unsigned char *dst, int32_t i_dst_stride, unsigned char *srci, int32_t i_src_stride, int32_t i_width, int32_t i_height )
{
	int i_x, i_y;
	const unsigned char *src;

	src = ( const unsigned char * ) srci;

	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			dst[ i_y + i_x * i_dst_stride ] = src[ i_x + i_y * i_src_stride ];
		}
	}
}

static inline void VID_CopyTile_8x8_unflipped( unsigned char *dst, int32_t i_dst_stride, unsigned char *srci, int32_t i_src_stride )
{
	int i_x, i_y;
	const unsigned char *src;

	src = ( const unsigned char * ) srci;

	for( i_y = 0; i_y < 8; i_y++ )
	{
		for( i_x = 0; i_x < 8; i_x++ )
		{
			dst[ i_x + i_y * i_dst_stride ] = src[ i_x + i_y * i_src_stride ];
		}
	}
}

static inline void VID_CopyTile_unflipped( unsigned char *dst, int32_t i_dst_stride, unsigned char *srci, int32_t i_src_stride, int32_t i_width, int32_t i_height )
{
	int i_x, i_y;
	const unsigned char *src;

	src = ( const unsigned char * ) srci;

	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			dst[ i_x + i_y * i_dst_stride ] = src[ i_x + i_y * i_src_stride ];
		}
	}
}


#if 0
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

#endif

void	VID_Update (vrect_t *rects)
{
#ifdef FORNSPIRE
	int i_x, i_y, i_width, i_line_left;
	unsigned char *ptr = REAL_SCREEN_BASE_ADDRESS;
	unsigned char *pui8_dstline, *pui8_srcline, *pui8_dst, *pui8_src;

	i_width = SCREEN_WIDTH;

	if( lcd_type() == SCR_240x320_565 )
	{
		int32_t i_rem_height, i_rem_width, i_tile_height, i_tile_width;

		i_rem_height = rects->height;
		pui8_srcline = &vid_buffer[ rects->x + rects->y * SCREEN_WIDTH ];
		pui8_dstline = &ptr[ rects->y + rects->x * SCREEN_HEIGHT ];

		while( i_rem_height > 0 )
		{
			i_rem_width = rects->width;
			pui8_src = pui8_srcline;
			pui8_dst = pui8_dstline;
			pui8_srcline += SCREEN_WIDTH * 8;
			pui8_dstline += 8;
			while( i_rem_width > 0 )
			{
				if( i_rem_width >= 8 && i_rem_height >= 8 )
				{
					VID_CopyTile_8x8_flipped( pui8_dst, SCREEN_HEIGHT, pui8_src, SCREEN_WIDTH );
				}
				else
				{
					VID_CopyTile_flipped( pui8_dst, SCREEN_HEIGHT, pui8_src, SCREEN_WIDTH, i_rem_width, i_rem_height );
				}
				pui8_src += 8;
				pui8_dst += 8 * SCREEN_HEIGHT;
				i_rem_width -= 8;
			}
			i_rem_height -= 8;
		}
	}
	else if( lcd_type() == SCR_320x240_565 )
	{
		int32_t i_rem_height, i_rem_width, i_tile_height, i_tile_width;

		i_rem_height = rects->height;
		pui8_srcline = &vid_buffer[ rects->x + rects->y * SCREEN_WIDTH ];
		pui8_dstline = &ptr[ rects->x + rects->y * SCREEN_WIDTH ];

		while( i_rem_height > 0 )
		{
			i_rem_width = rects->width;
			pui8_src = pui8_srcline;
			pui8_dst = pui8_dstline;
			pui8_srcline += SCREEN_WIDTH * 8;
			pui8_dstline += SCREEN_WIDTH * 8;
			while( i_rem_width > 0 )
			{
				if( i_rem_width >= 8 && i_rem_height >= 8 )
				{
					VID_CopyTile_8x8_unflipped( pui8_dst, SCREEN_WIDTH, pui8_src, SCREEN_WIDTH );
				}
				else
				{
					VID_CopyTile_unflipped( pui8_dst, SCREEN_WIDTH, pui8_src, SCREEN_WIDTH, i_rem_width, i_rem_height );
				}
				pui8_src += 8;
				pui8_dst += 8;
				i_rem_width -= 8;
			}
			i_rem_height -= 8;
		}
	}
	else
	{
		lcd_blit( &vid_buffer[ 0 ], SCR_320x240_8 );
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


