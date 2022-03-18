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
#define NSPIRE_SURFCACHESIZE (700*1024)

byte	vid_buffer[ BASEWIDTH * BASEHEIGHT ] __attribute__ ((aligned (8))) ;
short	zbuffer[ BASEWIDTH * BASEHEIGHT ] __attribute__ ((aligned (8)));
byte	surfcache[ NSPIRE_SURFCACHESIZE ] __attribute__ ((aligned (8)));

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];



byte align256_colormap[ 0x4000 + 255 ];


struct
{
	int i_vid_mode;
	int b_palette_changed;
	int b_refresh_u16palette;
	int b_commit_palette;
	unsigned char rgui8_palette[ 256 ][ 3 ];
	unsigned int rgui_nspire_palette[ 128 ];
	unsigned int rgui_nspire_saved_palette[ 128 ];
	unsigned short rgui16_palette[ 256 ];
} nspire_vid;

void VID_UpdatePalette( )
{
	int i;

	if( nspire_vid.b_refresh_u16palette )
	{
		for( i = 0; i < 256; i++ )
		{
			unsigned short ui16_entry = ( ( nspire_vid.rgui8_palette[ i ][ 0 ] >> 3 ) << 11 ) | ( ( nspire_vid.rgui8_palette[ i ][ 1 ] >> 2 ) << 5 ) | ( ( nspire_vid.rgui8_palette[ i ][ 2 ] >> 3 ) );
			nspire_vid.rgui16_palette[ i ] = ui16_entry;
		}
	}

	if( nspire_vid.b_commit_palette )
	{
		for( i = 0; i < 128; i++ )
		{
			nspire_vid.rgui_nspire_palette[ i ] = ( nspire_vid.rgui8_palette[ i * 2 ][ 2 ] >> 3 ) | ( ( nspire_vid.rgui8_palette[ i * 2 ][ 1 ] >> 3 ) << 5 ) | ( ( nspire_vid.rgui8_palette[ i * 2 ][ 0 ] >> 3 ) << 10 );
			nspire_vid.rgui_nspire_palette[ i ] |= ( ( nspire_vid.rgui8_palette[ i * 2 + 1 ][ 2 ] >> 3 ) | ( ( nspire_vid.rgui8_palette[ i * 2 + 1 ][ 1 ] >> 3 ) << 5 ) | ( ( nspire_vid.rgui8_palette[ i * 2 + 1 ][ 0 ] >> 3 ) << 10 ) ) << 16;
			PALETTEREGS[ i ] = nspire_vid.rgui_nspire_palette[ i ];
		}
	}
}

void	VID_SetPalette (unsigned char *palette)
{
	memcpy( nspire_vid.rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	nspire_vid.b_palette_changed = 1;
}

void	VID_ShiftPalette (unsigned char *palette)
{
	memcpy( nspire_vid.rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );
	nspire_vid.b_palette_changed = 1;
}

void VID_SetPaletteMode()
{
	int i;

	if( !has_colors )
	{
		return; /* this should never happen as we check it in main() */
	}

	for( i = 0; i < 128; i++ )
	{
		nspire_vid.rgui_nspire_saved_palette[ i ] = PALETTEREGS[ i ];
	}

	if( lcd_init( SCR_320x240_8 ) == 0 )
	{
		Sys_Error( "unable to set 320x240 8bpp mode" );
	}

	nspire_vid.b_commit_palette = 1;
	nspire_vid.b_refresh_u16palette = 0;
	nspire_vid.i_vid_mode = SCR_320x240_8;
}


void VID_Set16BitMode( )
{
	if( !has_colors )
	{
		return; /* this should never happen as we check it in main() */
	}

	if( lcd_type( ) == SCR_240x320_565 )
	{
		nspire_vid.i_vid_mode = SCR_240x320_565;
		if( lcd_init( nspire_vid.i_vid_mode ) == 0 )
		{
			Sys_Error( "unable to set 240x320 16bpp mode" );
		}
	}
	else /*if( lcd_type( ) == SCR_320x240_565 ) well ... */
	{
		nspire_vid.i_vid_mode = SCR_320x240_565;
		if( lcd_init( nspire_vid.i_vid_mode ) == 0 )
		{
			Sys_Error( "unable to set 320x240 16bpp mode" );
		}
	}

	nspire_vid.b_commit_palette = 0;
	nspire_vid.b_refresh_u16palette = 1;
}


void VID_RestoreColorMode()
{
	int i;
	if( !has_colors )
	{
		return; /* this should never happen as we check it in main() */
	}
	else
	{
		if( nspire_vid.b_commit_palette )
		{
			for( i = 0; i < 128; i++ )
			{
				PALETTEREGS[ i ] = nspire_vid.rgui_nspire_saved_palette[ i ];
			}
		}
	}

	if( lcd_init( -1 ) == 0 )
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

	memcpy( nspire_vid.rgui8_palette, palette, 256 * 3 * sizeof( unsigned char ) );

	if( is_cx2 )
	{
		VID_Set16BitMode( );
	}
	else
	{
		VID_SetPaletteMode( );
		VID_UpdatePalette( );
	}
}

void	VID_Shutdown (void)
{
	VID_RestoreColorMode();
}



void	VID_Update (vrect_t *rects)
{
	int i_x, i_y, i_from_x, i_to_x, i_from_y, i_to_y;

	i_from_x = rects->x;
	i_from_y = rects->y;
	i_to_x = i_from_x + rects->width;
	i_to_y = i_from_y + rects->height;

	if( nspire_vid.b_palette_changed )
	{
		VID_UpdatePalette( );
		nspire_vid.b_palette_changed = 0;

		if( nspire_vid.b_refresh_u16palette )
		{
			/* if palette changed and we are using 16bpp we need to refresh the whole framebuffer with new the colors */
			i_from_x = 0;
			i_from_y = 0;
			i_to_x = SCREEN_WIDTH;
			i_to_y = SCREEN_HEIGHT;
		}
	}

	if( nspire_vid.i_vid_mode == SCR_320x240_8 )
	{
		lcd_blit( &vid_buffer[ 0 ], SCR_320x240_8 ); /* FIXME: partial ... ? */
	}
	else if( nspire_vid.i_vid_mode == SCR_240x320_565 )
	{
		unsigned short *pui16_fb = ( unsigned short * ) REAL_SCREEN_BASE_ADDRESS;
		for( i_y = i_from_y; i_y < i_to_y; i_y++ )
		{
			unsigned short *pui16_dst = &pui16_fb[ i_y + i_from_x * SCREEN_HEIGHT ];
			const unsigned char *pui8_src = ( const unsigned char * ) &vid_buffer[ i_y * SCREEN_WIDTH + i_from_x ];
			for( i_x = 0; i_x < i_to_x - i_from_x; i_x++ )
			{
				*pui16_dst = nspire_vid.rgui16_palette[ pui8_src[ i_x ] ];
				pui16_dst += 240;
			}
		}
	}
	else /*if( nspire_vid.i_vid_mode == SCR_320x240_565 ) well .. */
	{
		unsigned short *pui16_fb = ( unsigned short * ) REAL_SCREEN_BASE_ADDRESS;
		for( i_y = i_from_y; i_y < i_to_y; i_y++ )
		{
			unsigned short *pui16_dst = &pui16_fb[ i_y * SCREEN_WIDTH + i_from_x ];
			const unsigned char *pui8_src = ( const unsigned char * ) &vid_buffer[ i_y * SCREEN_WIDTH + i_from_x ];
			for( i_x = 0; i_x < i_to_x - i_from_x; i_x++ )
			{
				pui16_dst[ i_x ] = nspire_vid.rgui16_palette[ pui8_src[ i_x ] ];
			}
		}
	}
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


