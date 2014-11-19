/* 
 *    decode.c
 *
 *	Copyright (C) Aaron Holtzman - May 1999
 *
 *  This file is part of ac3dec, a free Dolby AC-3 stream decoder.
 *	
 *  ac3dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  ac3dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 *  AC3DEC_WAVE v0.04
 *  -----------------
 *
 *  Several modifications were made to ac3dec's base source in order to
 *  compile under Visual C++ 6.x.
 *
 *  "min(), max()" functions in rematrix.c, imdct.c were renamed to
 *    to min16(), min32(), max16(), max32(), to avoid clashing with
 *    functions of the same name in the standard Windows library.
 *
 *  fopen( filename, "r" ) -> fopen( filename, "rb" )
 *    decode.c -
 *    Under Win32, the bitstream must be opened in *BINARY* mode ["rb"]
 *    Otherwise, 0x00 in the stream causes an EOF (end-of-file.)
 *
 *  Revision history
 *  ----------------
 *  v0.04 - attempted to increase resistance to bitstream errors
 *    exponent.c, mantissa.c, decode_sanity_check() now return # detect errs
 *    based on the this value, decode.c ( main() ) decides whether or not to
 *    resync the AC3-frame, and reset the AC3-decoder's state
 *    Note, improves ac3dec's resistance to corrupted frames, but the
 *    decoder is still not immune.
 *
 *  v0.03 - added output_wavefile.c, writes RIFF WAV files
 *    renamed output_winmci.c -> output_winwave.c, to avoid confusion
 *    with the retired Windows MCI API.  AC3DEC does not use any MCI
 *    function calls.
 *    fixed decoder input-filename parsing bug.  Extensions MPG, M2V are
 *    now demultiplexed with BBDMUX.
 *
 *  v0.02 - integration with Brent Beyeler's BBDMUX
 *  And of course, additional modifications were made to integrate BBDMUX.
 *   bitstream.c - fread( xxx, 4, 1, file) changed to ( xxx, 1, 4, file )
 *       reading "1 unit of 4-bytes" works better than "4 units of 1 byte"
 *       reduces chance of ac3dec crashing at the end of a "chopped" AC3/VOB
 *       file (i.e. incomplete end packet.)
 *
 *  The following files were added/changed to implement wave playback
 *  -----------------------------------------------------------------
 *
 *   ring_buffer.c - added functions for output_winmci.c
 *                   moved BUFFER_SIZE and BUFFER_NUM out of this file.
 *
 *   ring_buffer.h - moved BUFFER_SIZE and BUFFER_NUM into this file.
 *                   changed BUFFER_SIZE and BUFFER_NUM
 *
 *  new_files
 *  ---------
 *   eventbox.h (new) - struct definition for user-defined windows event
 * 
 *   output_winwave.c - core package for playing through Windows waveOut
 *
 *   demuxbuff.cpp & .h - demux-queue, data-buffer that sits
 *                   between BBDMUX and ac3dec's BITSREAM.C bitstream_load()
 *   downmix.c - wave downmixing routines, output is *always* 2-channel
 *           currently not thoroughly tested.  The downmixing coefficients
 *           may lead to saturation
 *
 *   (BBDMUX is the work of Brent Beyeler.)
 *   bbdmux.cpp  - demux_ac3(), extracts an AC3 subprogram from VOB stream
 *   bbdbits.cpp - bitstream management functions for bbdmux.cpp
 *
 *
 *    08/16/99 - first release 0.01
 *    08/17/99 - 0.02 integrated Brent Beyeler's BBDMUX, to support direct
 *               playing of VOB files.
 *    08/27/99 - 0.04 modified main() to force AC3-frame resync, if
 *               number of detected errors exceeds THRESHOLD (see decode.h)
 *    09/02/99 - 0.05 more modifications of th resyncing mechanism.
 *               audblk_old removed (contradicts AC3 specification regarding
 *               AC3 frame independence)
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/timeb.h>
#include <stdarg.h>

#include "ac3.h"
#include "decode.h"
#include "bitstream.h"
#include "imdct.h"
#include "exponent.h"
#include "mantissa.h"
#include "bit_allocate.h"
#include "parse.h"
#include "output.h"
#include "crc.h"
#include "rematrix.h"
#include "time.h"
#include "debug.h"

//static void decode_find_sync(bitstream_t *bs);
static void decode_get_sync(bitstream_t *bs);
static int decode_resync(bitstream_t *bs);
static void decode_print_banner(void);

static stream_coeffs_t stream_coeffs;
static stream_samples_t stream_samples;
static audblk_t audblk;
static bsi_t bsi;
static syncinfo_t syncinfo;
static uint_32 frame_count = 0;

/* global variables */
FILE *g_pInfile; /* filepointer to input bitstream */
int g_fUseDemuxer; /* flag, UseDemuxer (bbdmux) ? */

#define _WINWAVE		0
#define _RIFFWAVFILE	1
#define _AACFILE		2
#define _MP3FILE		3
#define _TARGETNONE		-1
#define KEY_QUIT 'Q'

#ifdef _CONSOLE2
int climode = 1;
#else
extern int climode;
#endif

int fUserQuit = 0; /* flag, user wants to quit? */

void StopNow( void )
{
	fUserQuit = 1;
}

int OutPrintf( char *txt, ... )
{
	long count, len, ret;
	va_list		args;
	char lineout[4000];

	if ( txt ){
		va_start( args, txt );
		vsprintf( lineout, txt, args );
		va_end( args );
		if ( climode )
			printf( lineout );
		else
			ShowStatus( lineout ); 
	}
	return 1;
}

int OutError( char *txt, ... )
{
	long count, len, ret;
	char lineout[4000];

	if ( txt ){
		va_list		args;
		va_start( args, txt );
		vsprintf( lineout, txt, args );
		if ( climode )
			fprintf( stderr, lineout );
		else
			ErrorBox( lineout );
		va_end( args );
	}
	return 1;
}


/* read 12:44:01 */
int StringToDaysTime( char * aStr )
{
	int hh,mm,ss, ctime = 0;
	
	if ( aStr ) {
		if ( strchr( aStr, ':' ) ){
			hh = atoi( aStr );
			mm = atoi( aStr+3 );
			ss = atoi( aStr+6 );
			ctime = (hh*3600) + (mm*60) + ss;
		} else {
			ctime = atoi( aStr );
		}
	}
	return ctime;
}

void TimetoString( long ctime, char *outStr )
{
	long	dd,hh,mm,ss;
	char 	*p;
	
	ss = ctime % 60;
	mm = (ctime/60) % 60;
	hh = (ctime/3600) % 24;
	
	p = outStr;	
	if ( hh ){
		*p++ = (hh/10) + 0x30;
		*p++ = (hh%10) + 0x30;
		*p++ = ':';
	}
	if ( mm || hh ){
		*p++ = (mm/10) + 0x30;
		*p++ = (mm%10) + 0x30;
		*p++ = ':';
	}
	*p++ = (ss/10) + 0x30;
	*p++ = (ss%10) + 0x30;
	*p++ = 0;
}

double getms( void )
{
	double  usec1;
	struct timeb t;
	
	ftime( &t );

	usec1 = (t.time) + (t.millitm/1000.0);
	return usec1;
}



static char *outbuffers = NULL;
static long outindex = 0;
static long m_outblock_count = 100;

// do bufferedoutput to speedup writing.
long SaveData( FILE *fp, char *data, long len )
{
	long i; unsigned long x=0, skip=0;
	char *d;

	if ( !fp ){
		if( outbuffers )free( outbuffers );
		return 0;
	}
	if ( !outbuffers ){
		outbuffers = malloc( m_outblock_count*1024 );
	}
	if ( d = outbuffers ){
		i = outindex;
		if ( i >= ((m_outblock_count-1)*1024) || !data ){
			fwrite( d, i, 1, fp );
			i = outindex = 0;
		}
		if ( i < ((m_outblock_count-1)*1024) && data ){
			memcpy( d+i, data, len );
			i+=len;
			outindex = i;
		}
	}
	return len;
}

void SeekToAC3Header( FILE *fp )
{
	short	buffer[2048];
	int		found = 0, header, i, lba=0;

	while( !found && fp ){
		i = fread( &buffer[0], 1, 2048, fp );

		for(i=0;i<1024;i++){
			if ( buffer[i] == 0x770b )
				found = 1;
		}
		OutPrintf( "Seeking to AC3 Header (LBA = %d)\r", lba++ );
	}
}



int m_guion = 0;
int m_ac3seek = 0;
int m_output_pcm = 0;
int	m_force44100 = 0;
int	m_lba_seek = 0;
int	m_allvobs = 1;
int	m_showsummary = 0;
int m_overwrite = 0;
int m_substream = 0;
int m_gainlevel = 0;
int m_gain2level = 0;
int m_gaincenter = 0;
int m_gainrear = 0;
int m_gainlfe = 0;
int m_outputaac = 0;
int m_insertms = 0;
int m_savecodec = 0;
int m_loadcodec = 0;
char m_codecfile[256];

long m_maxlength = 0;
char m_inputfile[256];

char *m_waveoutname = NULL ; /* output filename, if present */
int m_fDecodeTarget = _WINWAVE; /* default, decode to soundcard */



void clear_settings( void )
{
	m_guion = 0;
	m_ac3seek = 0;
	m_output_pcm = 0;
	m_force44100 = 0;
	m_lba_seek = 0;
	m_allvobs = 0;
	m_showsummary = 0;
	m_overwrite = 0;
	m_substream = 0;
	m_gainlevel = 0;
	m_gain2level = 0;
	m_gaincenter = 0;
	m_gainrear = 0;
	m_gainlfe = 0;
	m_outputaac = 0;
	m_insertms = 0;
	m_savecodec = 0;
	m_loadcodec = 0;
}



void sprintKBytes( long counter, char *out )
{
	if ( out ){
		if ( counter < 100 )
			sprintf(out, "[%ldbytes]", counter*1024 );
		else
		if ( counter < 1024 )
			sprintf(out, "[%ldKB]", counter );
		else
		if ( counter < 1024*1024 )
			sprintf(out, "[%.2fMB]", counter/1024.0 );
		else
			sprintf(out, "[%1.3fGB]", counter/(double)(1024*1024) );
	}
}


int handle_args(int argc,char *argv[])
{
	long i;
	char tmpStr[ 255 ]; /* temporary string */


	g_fUseDemuxer = 0; /* initially set to false */

	/* If we get an argument then use it as a filename... otherwise use
	 * stdin */
	if (argc > 1)
	{
		/* in_file = fopen(argv[1],"r");	*/
		/* under Win32, the file must be opened in BINARY mode ! */
		i = 1;
		if ( !strcmp( argv[i], "-help" ) || !strcmp( argv[i], "-h" ) || !strcmp( argv[i], "-?" ) || !strcmp( argv[i], "?" )  )
		{
			fprintf( stdout, "\n -pcmwav FILE  - output data to a PCM WAV file without the Chooser");
			fprintf( stdout, "\n -wav FILE     - output data to a WAV file");
			fprintf( stdout, "\n -savecodec X  - save codec definition to file X from Chooser");
			fprintf( stdout, "\n -loadcodec X  - load codec definition from file X");
			fprintf( stdout, "\n -pcm          - automaticly select PCM output with no Chooser Dialog");
			fprintf( stdout, "\n -ac3seek #    - seek to timecode of ac3 (00:00:00 format)");
			fprintf( stdout, "\n -seek #       - seek to VOB LBA position (ie which sector)");
			fprintf( stdout, "\n -seekmb #     - seek to file Megabyte position");
			fprintf( stdout, "\n -insertms #   - insert X ms amount of silence at the start");
			fprintf( stdout, "\n -allvobs      - span over multiple VOB files automaticly");
			fprintf( stdout, "\n -substream 0x## - only decode X subtream (hexformat, 0x81)");
			fprintf( stdout, "\n -overwrite    - overwrite output wav file without asking.");
			fprintf( stdout, "\n -blocksize    - file writing buffer size.");
			fprintf( stdout, "\n -gain #       - adjust global output gain (1..100) percent");
			fprintf( stdout, "\n -gain2 #      - adjust global output inverse-squared gain (1..500) percent");
			fprintf( stdout, "\n -gaincenter # - adjust centre channels gain level (1..500)");
			fprintf( stdout, "\n -gainrear #   - adjust rear channels gain level");
			fprintf( stdout, "\n -gainlfe #    - adjust lfe channel gain level");
			fprintf( stdout, "\n -zerocenter   - silence centre channel");
			fprintf( stdout, "\n -zerorear     - silence rear channels");
			fprintf( stdout, "\n -zerolfe      - silence lfe channel");
			fprintf( stdout, "\n -length       - limit the length of the decode to X seconds");
			fprintf( stdout, "\n -info         - show VOB summary information");
			fprintf( stdout, "\n -44100        - convert the 48000 stream to 44100 allowing othe codecs/formats");
			fprintf( stdout, "\n\n   FUTURE TODOs/Features that are NOT implemented in this release!!");
			fprintf( stdout, "\n -aac FILE     - output data to an AAC format file");
			fprintf( stdout, "\n -out5.1       - output stereo + stereo_rear + LFE + Center files");
			fprintf( stdout, "\n   ------------ realtime playback keyboard commands -----------");
			fprintf( stdout, "\n   0 - 9    :  switches substreams in real time (0x80 to 0x89)");
			fprintf( stdout, "\n   A,Z      :  + or - Gain Level");
			fprintf( stdout, "\n   S,X      :  + or - Gain Rear Level");
			fprintf( stdout, "\n   D,C      :  + or - Gain Center Level");
			fprintf( stdout, "\n   F,V      :  + or - Gain LFE Level");
			fprintf( stdout, "\n   W        :  Zero Rear Gain Level");
			fprintf( stdout, "\n   E        :  Zero Center Gain Level");
			fprintf( stdout, "\n   R        :  Zero LFE Gain Level");
		} else
		if ( !strcmp( argv[1], "--" ) )
		{
			g_pInfile = stdin;
			strcpy(tmpStr, ".vob" );
		} else 
		if ( *argv[1] != '-' )
		{
			g_pInfile = fopen(argv[1],"rb");	
			strcpy(tmpStr, argv[1] );
			strcpy(m_inputfile, argv[1] );
			if (!g_pInfile){
				OutPrintf( "%s - Couldn't open file %s\n",strerror(errno),argv[1]);
				exit(1);
			}
			i++;
		}	

		while( argc > i )
		{
			printf( "argc=%d\n", argc );
			if ( !strcmp( argv[i], "-ac3seek" ) || !strcmp( argv[i], "-seekac3" ) ){
				m_ac3seek = StringToDaysTime( argv[i+1] );
				OutPrintf( "seeking to timecode %s ...\n", argv[i+1] );
			} else
			if ( !strcmp( argv[i], "-44100" ) ){
				m_force44100 = 1;
			} else
			if ( !strcmp( argv[i], "-pcm" ) ){
				m_output_pcm = 1;
				i++;
			}
			else
			if ( !strcmp( argv[i], "-savecodec" ) ){
				m_savecodec = 1;
				strcpy( m_codecfile,  argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-loadcodec" ) ){
				strcpy( m_codecfile,  argv[i+1] );
				m_loadcodec = 1;
				i++;
			}
			else
			if ( !strcmp( argv[i], "-listen" ) ){
				m_output_pcm = 0;
				m_waveoutname = NULL;
				m_fDecodeTarget = _WINWAVE;
				i++;
			}
			else
			if ( !strcmp( argv[i], "-pcmwav" ) ){
				m_output_pcm = 1;
				m_waveoutname = argv[i+1];
				m_fDecodeTarget = _RIFFWAVFILE; /* output mode to FILE */
				i++;
			}
			else
			if ( !strcmp( argv[i], "-out" ) || !strcmp( argv[i], "-wav" ) ){
				m_waveoutname = argv[i+1];
				m_fDecodeTarget = _RIFFWAVFILE; /* output mode to FILE */
				i++;
			}
			else
			if ( !strcmp( argv[i], "-outaac" ) || !strcmp( argv[i], "-aac" ) ){
				m_waveoutname = argv[i+1];
				m_fDecodeTarget = _AACFILE; /* output mode to FILE */
				OutPrintf( "AAC output format has not yet been inplemented\n" );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-outmp3" ) || !strcmp( argv[i], "-mp3" ) ){
				m_waveoutname = argv[i+1];
				m_fDecodeTarget = _MP3FILE; /* output mode to FILE */
				OutPrintf( "Mp3 output format has not yet been inplemented\n" );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-wma" ) ){
				m_waveoutname = argv[i+1];
				//m_fDecodeTarget = _MP3FILE; /* output mode to FILE */
				OutPrintf( "WMA(asf) output format has not yet been inplemented\n" );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-out5.1" ) || !strcmp( argv[i], "-5.1" ) ){
				OutPrintf( "5.1 output format has not yet been inplemented\n" );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-insertms" ) ){
				m_insertms = atoi( argv[i+1] );
				OutPrintf( "Inserting X blank %d ms...\n", m_insertms );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-lba" ) || !strcmp( argv[i], "-@" ) || !strcmp( argv[i], "-seek" ) ){
				m_lba_seek = atoi( argv[i+1] );
				fseek( g_pInfile, m_lba_seek*2048, SEEK_SET );
				OutPrintf( "Seeking %d bytes...\n", m_lba_seek*2048 );
				SeekToAC3Header( g_pInfile );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-mb" ) || !strcmp( argv[i], "-seekmb" ) ){
				m_lba_seek = atoi( argv[i+1] ) * 512;
				fseek( g_pInfile, m_lba_seek*2048, SEEK_SET );
				OutPrintf( "Seeking %d bytes...\n", m_lba_seek*2048 );
				SeekToAC3Header( g_pInfile );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-blocksize" ) ){
				m_outblock_count = atoi( argv[i+1] );
				OutPrintf( "Setting write IO block write to %d blocks (%dKb)\n", m_outblock_count, m_outblock_count/2 );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-gain" ) ){
				m_gainlevel = atoi( argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-gain2" ) ){
				m_gain2level = atoi( argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-gaincenter" ) || !strcmp( argv[i], "-centergain" ) ){
				m_gaincenter = atoi( argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-gainrear" ) || !strcmp( argv[i], "-reargain" ) ){
				m_gainrear = atoi( argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-gainlfe" ) || !strcmp( argv[i], "-rearlfe" ) ){
				m_gainlfe = atoi( argv[i+1] );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-zerocenter" ) )
				m_gaincenter = 1000;
			else
			if ( !strcmp( argv[i], "-zerorear" ) )
				m_gainrear = 1000;
			else
			if ( !strcmp( argv[i], "-zerolfe" ) )
				m_gainlfe = 1000;
			else
			if ( !strcmp( argv[i], "-length" ) ){
				m_maxlength = StringToDaysTime( argv[i+1] );
				OutPrintf( "Limiting length to %s (%d secs)\n", argv[i+1], m_maxlength );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-substream" ) ){
				sscanf( argv[i+1], "0x%x", &m_substream );
				if (m_substream < 0 || m_substream > 0xFF){
			        OutPrintf("Substream ID must be in the range 1..0xFF\n");
			        exit (1);
				} else
					OutPrintf( "Trying to only decode substream 0x%02x ...\n", m_substream );
				i++;
			}
			else
			if ( !strcmp( argv[i], "-allvob" ) || !strcmp( argv[i], "-allvobs" ) )
				m_allvobs = 1;
			else
			if ( !strcmp( argv[i], "-overwrite" ) )
				m_overwrite = 1;
			else
			if ( !strcmp( argv[i], "-info" ) ){
				m_showsummary = 1;
				m_fDecodeTarget = _TARGETNONE;
				OutPrintf( "Showing File summary\n" );
			} else {
				m_waveoutname = argv[i]; /* name of output wave file */
				if ( strstr( m_waveoutname, ".wav" ) )
					m_fDecodeTarget = _RIFFWAVFILE; /* output mode to FILE */
			}
			i++;
		}

		strlwr(tmpStr); /* convert string to lower case */
		/* check input-filename's extension for *.VOB, *.M2V, *.MPG */
		/* if so, we want to use BBDMUX and the demux_buffer system */
		if ( strstr(tmpStr, ".vob") || strstr(tmpStr, ".mpg") || strstr(tmpStr, ".m2v") )
			g_fUseDemuxer = 1;
		  /* g_fUseDemuxer is used by only one other file, my_fread.cpp */
	}
	else
	if ( climode )
	/*	in_file = stdin;*/
	{
		OutPrintf( "A(3 V0.8.21 (C) 2001 (www.ac3dec.com, built %s %s)\n", __DATE__, __TIME__  );
		OutPrintf( "\nUsage : ac3dec.exe <input file> args\n" );
		OutPrintf( "\n   <input filename> - extensions VOB, M2V, MPG are first demultiplexed");
		OutPrintf( "\n   by BBDMUX.  All other filenames are processed directly by ac3dec.");
		OutPrintf( "\n   optional : [input filename] can be a -- which is stdin");
		OutPrintf( "\n   optional : -out [output filename] - ac3dec writes a WAVE file of this name.");
		OutPrintf( "\n\t\t otherwise, ac3dec plays to the <default> waveOut device.");
		OutPrintf( "\n   -help    : show all command args and info");
		return 1;
	}
	return 0;
}




int convert_now( FILE *inputFile, char *sourceFilename, char *outputFilename )
{
	int i, ch;
	int n; /* return value from decode_resync() */
	bitstream_t *bs;	/*FILE *in_file; turned to global variable */
	int done_banner = 0;
	long cbIteration = 0; /* while loop counter  */
	long SampleRate = 0; /* bitstream sample-rate */
	uint_32 cbErrors = 0; /* bitstream error count returned by 							decode_sanity_check() */
	uint_32 cbSkippedFrames = 0; /* running total of skipped frames */
	uint_32 cbDecodedFrames = 0; /* running total, #decoded frames */
	uint_32 cbMantErrors, cbExpErrors;		/* error counters for mantissa & exponent unpacking errors */
	double	time_start, time_end, time_other=0, timesofar, x;
	audblk_t audblk_blank = {0};
	bsi_t bsi_blank = {0};



	if ( !inputFile ){
		printf( "no file to process\n" );
		return 1;
	}

	if ( sourceFilename ){
		char tmpStr[256];

		strcpy( tmpStr, sourceFilename );
		strlwr(tmpStr); /* convert string to lower case */

		/* check input-filename's extension for *.VOB, *.M2V, *.MPG */
		/* if so, we want to use BBDMUX and the demux_buffer system */
		if ( strstr(tmpStr, ".vob") || strstr(tmpStr, ".mpg") || strstr(tmpStr, ".m2v") )
			g_fUseDemuxer = 1;
	}

	g_pInfile = inputFile;
	bs = bitstream_open( inputFile );

	/* initialize decoder and counter variables */
	imdct_init();
	(bsi) = bsi_blank;
	(audblk) = audblk_blank;

	decode_sanity_check_init();

	bsi_blank = bsi;
	audblk_blank = audblk;

	cbErrors = 0; 
	cbIteration = 0;
	cbSkippedFrames = 0;
	cbDecodedFrames = 0;

	fUserQuit = 0; 

/* ========== main while loop(), does all the decoding =========== */
	while( !fUserQuit )
	{
		audblk = audblk_blank; /* clear out audioblock */
		if ( _kbhit() ) 
		{
			ch = _getch();
			ch = toupper( ch );
			if ( ch == toupper( KEY_QUIT ) )
			{
			/* we must exit nicely, or the waveOut device is "stuck" open!
			   Then other apps can't use it until the user reboots! yuck */
				OutPrintf("\nUSER ABORTED DECODING OPERATION.");
				fUserQuit = 1; /* set flag */
			}
			if ( ch == toupper( '>' ) ){
				fseek( inputFile, 1000*2048, SEEK_CUR );
				SeekToAC3Header( inputFile );
				OutPrintf ( "\nForwarding 1000 LBA" );
			} else
			if ( ch == toupper( '<' ) ){
				fseek( inputFile, -1000*2048, SEEK_CUR );
				SeekToAC3Header( inputFile );
				OutPrintf ( "\nRewinding 1000 LBA" );
			} else

			if ( ch >= '0' && ch <= '9' ){
				m_substream = 0x80 + (ch - 0x30);
				OutPrintf( "\nSwitching to substream 0x%02x\n", m_substream );
			} else
			if ( ch == toupper( 'A' ) ){
				m_gain2level += 10;
				OutPrintf ( "\nGain level increased to %d\n", m_gain2level );
			} else
			if ( ch == toupper( 'Z' ) ){
				m_gain2level -= 10;
				OutPrintf ( "\nGain level decreased to %d\n", m_gain2level );
			} else

			if ( ch == toupper( 'S' ) ){
				m_gainrear += 10;
				OutPrintf ( "\nGain Rear level increased to %d\n", m_gainrear );
			} else
			if ( ch == toupper( 'X' ) ){
				m_gainrear -= 10;
				OutPrintf ( "\nGain Rear level decreased to %d\n", m_gainrear );
			} else
			if ( ch == toupper( 'W' ) ){
				if ( m_gainrear < 1000 ){
					m_gainrear = 1000;
					OutPrintf ( "\nGain Rear level Zeroed\n" );
				} else {
					m_gainrear = 0;
					OutPrintf ( "\nGain Rear level Reset\n" );
				}
			} else

			if ( ch == toupper( 'D' ) ){
				m_gaincenter += 10;
				OutPrintf ( "\nGain Center level increased to %d\n", m_gaincenter );
			} else
			if ( ch == toupper( 'C' ) ){
				m_gaincenter -= 10;
				OutPrintf ( "\nGain Center level decreased to %d\n", m_gaincenter );
			} else
			if ( ch == toupper( 'E' ) ){
				if ( m_gaincenter < 1000 ){
					m_gaincenter = 1000;
					OutPrintf ( "\nGain Center level Zeroed\n" );
				} else {
					m_gaincenter = 0;
					OutPrintf ( "\nGain Center level Reset\n" );
				}
			} else

			if ( ch == toupper( 'F' ) ){
				m_gainlfe += 10;
				OutPrintf ( "\nGain LFE level increased to %d\n", m_gainlfe );
			} else
			if ( ch == toupper( 'V' ) ){
				m_gainlfe += 10;
				OutPrintf ( "\nGain LFE level decreased to %d\n", m_gainlfe );
			} else
			if ( ch == toupper( 'R' ) ){
				if ( m_gainlfe < 1000 ){
					m_gainlfe = 1000;
					OutPrintf ( "\nGain LFE level Zeroed\n" );
				} else {
					m_gainlfe = 0;
					OutPrintf ( "\nGain LFE level Reset\n" );
				}
			}
		}

		n = decode_resync(bs);
		if ( n < 0 )	/* stop if we have serious file errors */
			fUserQuit = 2; /* set flag */
		if ( !bs->file )
			break;

		/* if the error count (calculated by previous loop iteration)
		   exceeds our defined threshold, force a resync. */
		if ( cbErrors > ERR_RESYNC_THRESHOLD || 
			cbMantErrors > ERR_MANT_THRESHOLD ||
			cbExpErrors > ERR_EXP_THRESHOLD )
		{	
			OutPrintf(" Error threshold exceeded (%lu), aborted current frame.\n",	cbErrors );
			++cbSkippedFrames;
		}

		bsi = bsi_blank; /* v0.04 wipe bsi clear, not really necessary */
		parse_syncinfo(&syncinfo,bs);
		parse_bsi(&bsi,bs);

		if(!done_banner)
		{
			decode_print_banner();
			done_banner = 1;
		}

		if ( cbIteration == 0 ) /* first time this loop is run? */
		{
			if ( m_ac3seek && !cbIteration ){
				long seekpos;

				seekpos = (syncinfo.bit_rate * m_ac3seek) * (1024/8);
				OutPrintf( "Seeking %d Kbytes...\n", seekpos/1024 );
				fseek( inputFile, seekpos, SEEK_SET );
				SeekToAC3Header( inputFile );
			}


			switch (syncinfo.fscod)
			{
				case 2:
					SampleRate = 32000;
					break;
				case 1:
					SampleRate = 44100;
					break;
				case 0:
					SampleRate = 48000;
					if ( m_force44100 && m_fDecodeTarget != _WINWAVE )
						SampleRate = 44100;
					break;
				default:
					OutPrintf( "\nmain() : syncinfo.fscod, ERROR Invalid sampling rate ");
					exit( 1);
			}

			switch( m_fDecodeTarget )
			{
				case _WINWAVE :
					if ( output_open(16,SampleRate,2) )
						return 1;
					break;
				case _RIFFWAVFILE :
					if ( output_wavefile_open(16,SampleRate,2, outputFilename ) )
						return 1;
					break;
				case _AACFILE :
					output_aacfile_open( 16,44100,5, outputFilename ); 
					break;
				case _TARGETNONE: break;
				default :
					OutPrintf( "\nmain() : ERROR unknown m_fDecodeTarget=%d",	m_fDecodeTarget );
			}
			time_start = getms();
		} /* endif ( cbIteration == 0 ) */

		++cbIteration; /* increment counter */

		/* reset bitstream error counters */
		cbErrors =
		cbExpErrors =
		cbMantErrors = 0;

		for(i=0; (i < 6) && (cbErrors < ERR_RESYNC_THRESHOLD ); i++)
		{

			/* Extract most of the audblk info from the bitstream (minus the mantissas */
			parse_audblk(&bsi,&audblk,bs,i);					// CPU time 10%

			/* Take the differential exponent data and turn it into absolute exponents */
			cbExpErrors += exponent_unpack(&bsi,&audblk);

			/* Figure out how many bits per mantissa */
			bit_allocate(syncinfo.fscod,&bsi,&audblk);			// CPU TIME 1.2%

			if ( bsi.nfchans > 6 ) { bsi.nfchans = 0; break; }

			/* Extract the mantissas from the data stream */
			x = getms();
			cbMantErrors += mantissa_unpack(&bsi,&audblk,bs);	// CPU TIME 62.0%
			time_other += (getms() - x );
			/* Uncouple the coupling channel if it exists and
			 * convert the mantissa and exponents to IEEE floating point format */
			uncouple(&bsi,&audblk,&stream_coeffs);				// CPU TIME 1.7%

			if(bsi.acmod == 0x2)
				rematrix(&audblk,&stream_coeffs);				// CPU TIME 0.1%
#if 0	
			/* Perform dynamic range compensation */
			dynamic_range(&bsi,&audblk,&stream_coeffs); 
#endif

			/* Convert the frequency data into time samples */
			imdct(&bsi,&audblk,&stream_coeffs,&stream_samples);	// CPU TIME 11.2%

			/* Send the samples to the output device */
			n = 0;
			switch( m_fDecodeTarget ){
			  case _WINWAVE :
				n = output_play(&stream_samples, &bsi ); /* v0.03 downmixing */
				timesofar = 0;
				break;
			  case _RIFFWAVFILE :
				n = output_wavefile_write(&stream_samples, &bsi );	// CPU TIME 8.0 %
				/* v0.04 WAVwrite*/
				break;
			  case _AACFILE :
				n = output_aacfile_write(&stream_samples, &bsi );	// CPU TIME 8.0 %
				/* v0.04 WAVwrite*/
				break;
				case _TARGETNONE: break;
			  default :
				OutPrintf( "\nmain() : ERROR unknown m_fDecodeTarget=%d",	m_fDecodeTarget );
			}

			time_end = getms();

			if ( n == -1 )
				fUserQuit = 1;
			else
			if ( n ) {	/* ########### SHOW TIMECODE AS IT OUTPUTS ########### */
				char timeStr[16];
				char bytesStr[32];
				double timelength;
				long seclength;
				long samplesout = (double)cbDecodedFrames * 1536.0;

				timelength = samplesout / (double)SampleRate;
				TimetoString( (long)timelength, timeStr );
				seclength = (long)((double)cbDecodedFrames * 1536.0) % SampleRate;
				timesofar = getms() - time_start;

				sprintKBytes( n, bytesStr );

				OutPrintf( "\r%s DUR=%s.%03d (%0.3fsecs) (%d/sec, 1:%0.1f)    ", bytesStr,
					timeStr, seclength*1000/SampleRate, timelength,
					(long)(samplesout /timesofar), samplesout /(double)(timesofar*SampleRate) );
				
			}
		} /* endfor ( i = 0 ... ) */

		/* v0.04, if cbErrors exceeds defined threshold, force frame-resync */
		cbErrors = decode_sanity_check();
		/* v0.04, if cbErrors exceeds defined threshold, force frame-resync */
		if ( cbErrors >= ERR_RESYNC_THRESHOLD ||
			cbExpErrors >= ERR_EXP_THRESHOLD ||
			cbMantErrors >= ERR_MANT_THRESHOLD )
		{
			//OutPrintf( "Resyncing...\n");
			continue; /* return to TOP of while-loop */
		}

		parse_auxdata(&syncinfo,bs);			// CPU TIME 2.0%

		if(!crc_validate())
			dprintf("(crc) CRC check failed\n");
		else
			dprintf("(crc) CRC check passed\n");


		++cbDecodedFrames; /* update # successfully decoded frames */

		if ( m_maxlength && cbIteration ) {
			long samplesout = (double)cbDecodedFrames * 1536.0;
			long secondsout = samplesout / SampleRate;
			if ( secondsout >= m_maxlength )
				fUserQuit = 2; /* set flag */
		}
		if ( m_showsummary && cbIteration )
			fUserQuit = 2; /* set flag */

	} /* endwhile (1) */
/* ################ end of main decoding loop ################## */

	time_end = getms();

	if ( fUserQuit == 1 )
		OutPrintf("\nUser abort.");
	else
		OutPrintf("\nDecoding complete.");

	bitstream_close(bs);

	switch( m_fDecodeTarget )
	{
	  case _WINWAVE :
		output_close();
		timesofar = 0;
		break;
	  case _RIFFWAVFILE :
		timesofar = output_wavefile_close( outputFilename );
		break;
	  case _AACFILE :
		timesofar = output_aacfile_close( outputFilename );
		break;
	  case _TARGETNONE: break;
	  default :
		OutPrintf( "\nmain() : ERROR unknown m_fDecodeTarget=%d",	m_fDecodeTarget );
	}
	timesofar = time_end - time_start;

	if ( !m_showsummary )
	{
		double samplesout = (double)cbDecodedFrames * 1536.0;
		long timelength;
		char timeStr[16];

		if ( !SampleRate ) SampleRate = 1;
		TimetoString( (samplesout / SampleRate), timeStr );
		timelength = (unsigned long)samplesout % SampleRate;
		OutPrintf("\n Skipped AC-3 frames          = %lu times", cbSkippedFrames );
		OutPrintf("\n Successfully decoded         = %lu frames", cbDecodedFrames );
		OutPrintf("\n Duration                     = %s.%03d secs (%0.3fsecs)", timeStr, timelength*1000/SampleRate, samplesout / (double)SampleRate );
		OutPrintf("\n stereo-audio samples written = %0.1fKb samples", samplesout/1024.0 );
		if ( timesofar )
		{
			TimetoString( timesofar, timeStr );
			//printf("\n mantisa()                    = %.3f  (%.1f%%)", time_other, time_other*100/timesofar );
			OutPrintf("\n Time taken                   = %s.%03d  , mantisa=(%0.1f, %.1f%%)", timeStr,
				(long)(1000*(timesofar-((long)timesofar))),
				time_other, (time_other*100)/(double)timesofar );
			OutPrintf("\n Speed rate                   = %d Samples/sec , Realtime ratio 1:%0.1f",
				(long)(samplesout /timesofar),
				samplesout /(double)(timesofar*SampleRate) );
		}
	}
	fUserQuit = 0;
	return 0;
}













int ac3main(int argc,char *argv[])
{
	if( handle_args( argc, argv ) )
		return 0;
	else
		return convert_now( g_pInfile, 0, m_waveoutname );
}











static 
void decode_get_sync(bitstream_t *bs)
{
	uint_16 sync_word;
	uint_32 i = 0;

	sync_word = bitstream_get(bs,16);

	/* Make sure we sync'ed */
	if ( sync_word != 0x0b77 )
		OutPrintf( "\n<%X SYNC NOMATCH>\n", sync_word );

	dprintf("(sync) %ld bits skipped to synchronize\n",i);
	dprintf("(sync) begin frame %ld\n",frame_count);
	frame_count++;

	bs->total_bits_read = 16;
	crc_init();
}


static  int decode_resync(bitstream_t *bs) {
	uint_16 sync_word;
	int i = 0;

	if ( !bs->file ) return -1;

	sync_word = bitstream_get(bs,16);

	/* Make sure we sync'ed */
	while(1)
	{
		if(sync_word == 0x0b77 )
			break;
		sync_word = ( sync_word << 1 );
		sync_word |= bitstream_get(bs,1);
		i++;
		if ( i>32 ){		// fix added to handle EOF or closed files 
			//printf( "EOF %d\n", ftell( bs->file ) );
			if ( ftell( bs->file )==-1 || feof(bs->file) ){
				i = -1;
				break;
			}
		}
	}
	if ( i > 0 )
		OutPrintf(" decode_resync() : %lu bits skipped\n",i);

	dprintf("(sync) begin frame %ld\n",frame_count);
	frame_count++;

	bs->total_bits_read = 16;
	crc_init();
	return i; /* return # bits skipped */
}


void 
decode_sanity_check_init(void)
{
	syncinfo.magic = DECODE_MAGIC_NUMBER;
	bsi.magic = DECODE_MAGIC_NUMBER;
	audblk.magic1 = DECODE_MAGIC_NUMBER;
	audblk.magic2 = DECODE_MAGIC_NUMBER;
	audblk.magic3 = DECODE_MAGIC_NUMBER;
}


/* decode_sanity_check() now returns # errors detected */
long 
decode_sanity_check(void)
{
	int i;
	long cbError = 0; /* error count */

	if(syncinfo.magic != DECODE_MAGIC_NUMBER)
	{
		fprintf(stderr,"** Sanity check failed -- syncinfo magic number **");
		++cbError;
	}
	
	if(bsi.magic != DECODE_MAGIC_NUMBER)
	{
		fprintf(stderr,"** Sanity check failed -- bsi magic number **");
		++cbError;
	}

	if(audblk.magic1 != DECODE_MAGIC_NUMBER)
	{
		fprintf(stderr,"** Sanity check failed -- audblk magic number 1 **"); 
		++cbError;
	}

	if(audblk.magic2 != DECODE_MAGIC_NUMBER)
	{
		fprintf(stderr,"** Sanity check failed -- audblk magic number 2 **"); 
		++cbError;
	}

	if(audblk.magic3 != DECODE_MAGIC_NUMBER)
	{
		fprintf(stderr,"** Sanity check failed -- audblk magic number 3 **"); 
		++cbError;
	}

	for(i = 0;i < 5 ; i++)
	{
		if (audblk.fbw_exp[i][255] !=0 || audblk.fbw_exp[i][254] !=0 || 
				audblk.fbw_exp[i][253] !=0)
		{
			audblk.fbw_exp[i][255] = 0;
			audblk.fbw_exp[i][254] = 0;
			audblk.fbw_exp[i][253] = 0;
			fprintf(stderr,"** Sanity check failed -- fbw_exp out of bounds **"); 
			++cbError;
		}

		if (audblk.fbw_bap[i][255] !=0 || audblk.fbw_bap[i][254] !=0 || 
				audblk.fbw_bap[i][253] !=0)
		{	
			audblk.fbw_bap[i][255] = 0;
			audblk.fbw_bap[i][254] = 0;
			audblk.fbw_bap[i][253] = 0;
			fprintf(stderr,"** Sanity check failed -- fbw_bap out of bounds **"); 
			++cbError;
		}

		if (audblk.chmant[i][255] !=0 || audblk.chmant[i][254] !=0 || 
				audblk.chmant[i][253] !=0)
		{
			audblk.chmant[i][255] = 0;
			audblk.chmant[i][254] = 0;
			audblk.chmant[i][253] = 0;
			fprintf(stderr,"** Sanity check failed -- chmant out of bounds **"); 
			++cbError;
		}
	} /* endfor ( i=0 ... ) */

	if (audblk.cpl_exp[255] !=0 || audblk.cpl_exp[254] !=0 || 
			audblk.cpl_exp[253] !=0)
	{
		audblk.cpl_exp[255] = 0;
		audblk.cpl_exp[254] = 0;
		audblk.cpl_exp[253] = 0;
		fprintf(stderr,"** Sanity check failed -- cpl_exp out of bounds **"); 
		++cbError;
	}

	if (audblk.cpl_bap[255] !=0 || audblk.cpl_bap[254] !=0 || 
			audblk.cpl_bap[253] !=0)
	{
		audblk.cpl_bap[255] = 0;
		audblk.cpl_bap[254] = 0;
		audblk.cpl_bap[253] = 0;
		fprintf(stderr,"** Sanity check failed -- cpl_bap out of bounds **"); 
		++cbError;
	}

	if (audblk.cplmant[255] !=0 || audblk.cplmant[254] !=0 || 
			audblk.cplmant[253] !=0)
	{
		audblk.cplmant[255] = 0;
		audblk.cplmant[254] = 0;
		audblk.cplmant[253] = 0;
		fprintf(stderr,"** Sanity check failed -- cpl_mant out of bounds **"); 
		++cbError;
	}

	if ((audblk.cplinu == 1) && (audblk.cplbegf > (audblk.cplendf+2)))
	{
		fprintf(stderr,"** Sanity check failed -- cpl params inconsistent **"); 
		++cbError;
	}

	for(i=0; i < bsi.nfchans; i++)
	{
		if((audblk.chincpl[i] == 0) && (audblk.chbwcod[i] > 60))
		{
			fprintf(stderr,"** Sanity check failed -- chbwcod too big **"); 
			audblk.chbwcod[i] = 60;
			++cbError;
		}
	} /* endfor ( i = 0 ... ) */

	if ( cbError > 0 )
		OutPrintf(" decode_sanity_check() : %ld errors caught!\n", cbError );

	return cbError;
}	


void decode_print_banner(void)
{
	/*printf(PACKAGE"-"VERSION" (C) 1999 Aaron Holtzman (aholtzma@engr.uvic.ca)\n");*/
	//printf("ac3dec-0.5.4 (C) 1999 Aaron Holtzman (aholtzma@engr.uvic.ca)\n");

	OutPrintf("%d.%d Mode ",bsi.nfchans,bsi.lfeon);
	switch (syncinfo.fscod)
	{
		case 2:
			OutPrintf("32 KHz   ");
			break;
		case 1:
			OutPrintf("44.1 KHz ");
			break;
		case 0:
			OutPrintf("48 KHz   ");
			break;
		default:
			OutPrintf("Invalid sampling rate ");
			break;
	}
	OutPrintf("%4d kbps ",syncinfo.bit_rate);
	switch(bsi.bsmod)
	{
		case 0:
			OutPrintf("Complete Main Audio Service");
			break;
		case 1:
			OutPrintf("Music and Effects Audio Service");
		case 2:
			OutPrintf("Visually Impaired Audio Service");
			break;
		case 3:
			OutPrintf("Hearing Impaired Audio Service");
			break;
		case 4:
			OutPrintf("Dialogue Audio Service");
			break;
		case 5:
			OutPrintf("Commentary Audio Service");
			break;
		case 6:
			OutPrintf("Emergency Audio Service");
			break;
		case 7:
			OutPrintf("Voice Over Audio Service");
			break;
	}
	OutPrintf("\n");
}
