/* 
 *  bitstream.h
 *
 *	Aaron Holtzman - May 1999
 *  Mod, cheekyboy
 *
 */

#ifndef _BITSTREAM_H
#define _BITSTREAM_H 1

typedef struct bitstream_s
{
	FILE *file;
	uint_32 current_word;
	uint_32 bits_left;
	uint_32 total_bits_read;
	uint_32 done;

} bitstream_t;

uint_32 bitstream_get(bitstream_t *bs,uint_32 num_bits);
bitstream_t* bitstream_open(FILE *file);
void bitstream_close(bitstream_t *bs);
int bitstream_done(bitstream_t *bs);
long bitstream_spanover(bitstream_t *bs);
int bitstream_setfile(bitstream_t *bs, FILE *file );
#endif