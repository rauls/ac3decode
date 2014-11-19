#include <stdio.h>
#include <stdlib.h>

static unsigned char	_reverse[0x100];
static unsigned char	_table[0x100] = {
	0x33, 0x73, 0x3B, 0x26, 0x63, 0x23, 0x6B, 0x76, 0x3E, 0x7E, 0x36, 0x2B, 0x6E, 0x2E, 0x66, 0x7B,
	0xD3, 0x93, 0xDB, 0x06, 0x43, 0x03, 0x4B, 0x96, 0xDE, 0x9E, 0xD6, 0x0B, 0x4E, 0x0E, 0x46, 0x9B,
	0x57, 0x17, 0x5F, 0x82, 0xC7, 0x87, 0xCF, 0x12, 0x5A, 0x1A, 0x52, 0x8F, 0xCA, 0x8A, 0xC2, 0x1F,
	0xD9, 0x99, 0xD1, 0x00, 0x49, 0x09, 0x41, 0x90, 0xD8, 0x98, 0xD0, 0x01, 0x48, 0x08, 0x40, 0x91,
	0x3D, 0x7D, 0x35, 0x24, 0x6D, 0x2D, 0x65, 0x74, 0x3C, 0x7C, 0x34, 0x25, 0x6C, 0x2C, 0x64, 0x75,
	0xDD, 0x9D, 0xD5, 0x04, 0x4D, 0x0D, 0x45, 0x94, 0xDC, 0x9C, 0xD4, 0x05, 0x4C, 0x0C, 0x44, 0x95,
	0x59, 0x19, 0x51, 0x80, 0xC9, 0x89, 0xC1, 0x10, 0x58, 0x18, 0x50, 0x81, 0xC8, 0x88, 0xC0, 0x11,
	0xD7, 0x97, 0xDF, 0x02, 0x47, 0x07, 0x4F, 0x92, 0xDA, 0x9A, 0xD2, 0x0F, 0x4A, 0x0A, 0x42, 0x9F,
	0x53, 0x13, 0x5B, 0x86, 0xC3, 0x83, 0xCB, 0x16, 0x5E, 0x1E, 0x56, 0x8B, 0xCE, 0x8E, 0xC6, 0x1B,
	0xB3, 0xF3, 0xBB, 0xA6, 0xE3, 0xA3, 0xEB, 0xF6, 0xBE, 0xFE, 0xB6, 0xAB, 0xEE, 0xAE, 0xE6, 0xFB,
	0x37, 0x77, 0x3F, 0x22, 0x67, 0x27, 0x6F, 0x72, 0x3A, 0x7A, 0x32, 0x2F, 0x6A, 0x2A, 0x62, 0x7F,
	0xB9, 0xF9, 0xB1, 0xA0, 0xE9, 0xA9, 0xE1, 0xF0, 0xB8, 0xF8, 0xB0, 0xA1, 0xE8, 0xA8, 0xE0, 0xF1,
	0x5D, 0x1D, 0x55, 0x84, 0xCD, 0x8D, 0xC5, 0x14, 0x5C, 0x1C, 0x54, 0x85, 0xCC, 0x8C, 0xC4, 0x15,
	0xBD, 0xFD, 0xB5, 0xA4, 0xED, 0xAD, 0xE5, 0xF4, 0xBC, 0xFC, 0xB4, 0xA5, 0xEC, 0xAC, 0xE4, 0xF5,
	0x39, 0x79, 0x31, 0x20, 0x69, 0x29, 0x61, 0x70, 0x38, 0x78, 0x30, 0x21, 0x68, 0x28, 0x60, 0x71,
	0xB7, 0xF7, 0xBF, 0xA2, 0xE7, 0xA7, 0xEF, 0xF2, 0xBA, 0xFA, 0xB2, 0xAF, 0xEA, 0xAA, 0xE2, 0xFF,
};

//----------------------------------------------------------------------------------------------------------

inline void _ClockLfsr0Forward(int &lfsr0)
{
	int	temp;

	temp = (lfsr0 << 3) | (lfsr0 >> 14);
	lfsr0 = (lfsr0 >> 8) | ((((((temp << 3) ^ temp) << 3) ^ temp ^ lfsr0) & 0xFF) << 9);
}

inline void _ClockLfsr1Forward(int &lfsr1)
{
	lfsr1 = (lfsr1 >> 8) | ((((((((lfsr1 >> 8) ^ lfsr1) >> 1) ^ lfsr1) >> 3) ^ lfsr1) & 0xFF) << 17);
}

inline void _ClockBackward(int &lfsr0, int &lfsr1)
{
	int	temp0, temp1;

	lfsr0 = ((lfsr0 << 8) ^ ((((lfsr0 >> 3) ^ lfsr0) >> 6) & 0xFF)) & ((1 << 17) - 1);
	temp0 = ((lfsr1 >> 17) ^ (lfsr1 >> 4)) & 0xFF;
	temp1 = (lfsr1 << 5) | (temp0 >> 3);
	temp1 = ((temp1 >> 1) ^ temp1) & 0xFF;
	lfsr1 = ((lfsr1 << 8) | ((((((temp1 >> 2) ^ temp1) >> 1) ^ temp1) >> 3) ^ temp1 ^ temp0)) & ((1 << 25) - 1);
}

static void _Salt(int &lfsr0, int &lfsr1, const unsigned char salt[5])
{
	lfsr0 ^= (_reverse[salt[0]] << 9) | _reverse[salt[1]];
	lfsr1 ^= ((_reverse[salt[2]] & 0xE0) << 17) | ((_reverse[salt[2]] & 0x1F) << 16) | (_reverse[salt[3]] << 8) | _reverse[salt[4]];
}

static void _PrintKey(int lfsr0, int lfsr1)
{
	printf("key:");
	printf(" %02X", _reverse[lfsr0 >> 9]);
	printf(" %02X", _reverse[lfsr0 & 0xFF]);
	printf(" %02X", _reverse[((lfsr1 >> 16) & 0x1F) | ((lfsr1 >> 17) & 0xE0)]);
	printf(" %02X", _reverse[(lfsr1 >> 8) & 0xFF]);
	printf(" %02X", _reverse[lfsr1 & 0xFF]);
	printf("\n");
}

static int _FindLfsr(const unsigned char *crypt, int offset, const unsigned char *plain, int &result0, int &result1)
{
	int		loop0, loop1, lfsr0, lfsr1, carry, count;

	for(loop0 = count = 0; loop0 != (1 << 18); loop0++) {
		lfsr0 = loop0 >> 1;
		carry = loop0 & 0x01;
		for(loop1 = lfsr1 = 0; loop1 != 4; loop1++) {
			_ClockLfsr0Forward(lfsr0);
			carry = (_table[crypt[offset + loop1]] ^ plain[loop1]) - ((lfsr0 >> 9) ^ 0xFF) - carry;
			lfsr1 = (lfsr1 >> 8) | ((carry & 0xFF) << 17);
			carry = (carry >> 8) & 0x01;
		}
		for(; loop1 != 7; loop1++) {
			_ClockLfsr0Forward(lfsr0);
			_ClockLfsr1Forward(lfsr1);
			carry += ((lfsr0 >> 9) ^ 0xFF) + (lfsr1 >> 17);
			if((carry & 0xFF) != (_table[crypt[offset + loop1]] ^ plain[loop1])) {
				break;
			}
			carry >>= 8;
		}
		if(loop1 == 7) {
			for(loop1 = 0; loop1 != 6; loop1++) {
				_ClockBackward(lfsr0, lfsr1);
			}
			carry = ((lfsr0 >> 9) ^ 0xFF) + (lfsr1 >> 17) + (loop0 & 0x01);
			if((carry & 0xFF) == (_table[crypt[offset]] ^ plain[0])) {
				for(loop1 = 0; loop1 != offset + 1; loop1++) {
					_ClockBackward(lfsr0, lfsr1);
				}
				if(lfsr0 & 0x100 && lfsr1 & 0x200000) {
					result0 = lfsr0;
					result1 = lfsr1;
					count++;
				}
			}
		}
	}
	return count;
}

static int _FindKey(const char *filename, int &lfsr0, int &lfsr1)
{
	unsigned char	buffer[0x800], plain[7] = { 0x00, 0x00, 0x01, 0xBE, 0x00, 0x00, 0xFF };
	int				result, offset, left, flag, block, count;
	FILE			*file;

	result = 1;
	if((file = fopen(filename, "rb")) == 0) {
		printf("error: can't open input file '%s'\n", filename);
	} else {
		for(flag = block = 0; fread(buffer, 1, sizeof buffer, file) == sizeof buffer; block++) {
			printf("\rscanning, block %d", block);
			fflush(stdout);
			if(buffer[0x14] & 0x10) {
				flag |= 0x01;
				if(buffer[0x00] == 0x00 && buffer[0x01] == 0x00 && buffer[0x02] == 0x01 && buffer[0x03] == 0xBA
				   && buffer[0x0E] == 0x00 && buffer[0x0F] == 0x00 && buffer[0x10] == 0x01) {
					offset = 0x14 + (buffer[0x12] << 8) + buffer[0x13];
					if(0x80 <= offset && offset <= 0x7F9) {
						flag |= 0x02;
						left = 0x800 - offset - 6;
						plain[4] = (char) (left >> 8);
						plain[5] = (char) left;
						if((count = _FindLfsr(buffer + 0x80, offset - 0x80, plain, lfsr0, lfsr1)) == 1) {
							_Salt(lfsr0, lfsr1, buffer + 0x54);
							result = 0;
							break;
						} else if(count) {
							printf("\rblock %d reported %d possible keys, skipping\n", block, count);
						}
					}
				}
			}
		}
		fclose(file);
		if(result && feof(file) == 0) {
			printf("error: can't read from input file '%s'\n", filename);
		} else {
			printf("\rscanning, done          \n");
			if(result) {
				printf("error: unable to deduce key from input file (%s)\n", flag & 0x02 ? "the algorithm failed" : flag & 0x01 ? "there were no vulnerable blocks" : "there were no encrypted blocks");
			}
		}
		fclose(file);
	}
	return result;
}

static void _Decrypt(unsigned char *buffer, int lfsr0, int lfsr1)
{
	int	loop0, carry;

	_Salt(lfsr0, lfsr1, buffer + 0x54);
	for(loop0 = carry = 0, buffer += 0x80; loop0 != 0x800 - 0x80; loop0++, buffer++) {
		_ClockLfsr0Forward(lfsr0);
		_ClockLfsr1Forward(lfsr1);
		carry += ((lfsr0 >> 9) ^ 0xFF) + (lfsr1 >> 17);
		*buffer = (unsigned char) (_table[*buffer] ^ carry);
		carry >>= 8;
	}
}

static int _DecryptFile(const char *filename0, const char *filename1, int lfsr0, int lfsr1)
{
	unsigned char	buffer[0x800];
	int				result, block;
	FILE			*file0, *file1;

	result = 1;
	if((file0 = fopen(filename0, "rb")) == 0) {
		printf("error: can't open input file '%s'\n", filename0);
	} else {
		if((file1 = fopen(filename1, "wb")) == 0) {
			printf("error: can't create/truncate output file '%s'\n", filename1);
		} else {
			for(block = 0;; block++) {
				if(block % 10 == 0) {
					printf("\rdecrypting, block %d", block);
					fflush(stdout);
				}
				if(fread(buffer, 1, sizeof buffer, file0) != sizeof buffer) {
					result = 0;
					break;
				}
				if(buffer[0x14] & 0x10) {
					_Decrypt(buffer, lfsr0, lfsr1);
					buffer[0x14] &= ~0x10;
				}
				if(fwrite(buffer, 1, sizeof buffer, file1) != sizeof buffer) {
					printf("\nerror: can't write to output file\n");
					break;
				}
			}
			fclose(file1);
			printf("\rdecrypting, done          \n");
		}
		fclose(file0);
	}
	return result;
}

int main(int argc, char **argv)
{
	unsigned char	key[5], dummy;
	int				loop0, loop1, value, lfsr0, lfsr1;

	printf("VobDec 0.3 - VOB Decryption Utility\n\n");

	for(loop0 = 0; loop0 != 0x100; loop0++) {
		for(loop1 = value = 0; loop1 != 8; loop1++) {
			value |= ((loop0 >> loop1) & 0x01) << (7 - loop1);
		}
		_reverse[loop0] = (unsigned char) value;
	}
	switch(argc) {
		case 8:
			for(loop0 = 0; loop0 != 5; loop0++) {
				if(sscanf(argv[loop0 + 3], "%X%c", &value, &dummy) != 1) {
					printf("error: can't parse '%s' as a hex key byte\n", argv[loop0 + 3]);
					return 1;
				}
				key[loop0] = (unsigned char) value;
			}
			_Salt(lfsr0 = 0x100, lfsr1 = 0x200000, key);

		case 3:
		case 2:
			break;

		default:
			printf("usage: vobdec input.vob [output.vob [K0 K1 K2 K3 K4]]\n");
			return 1;
	}
	if(argc != 8 && _FindKey(argv[1], lfsr0, lfsr1)) {
		return 1;
	}
	_PrintKey(lfsr0, lfsr1);
	if(argc != 2 && _DecryptFile(argv[1], argv[2], lfsr0, lfsr1)) {
		return 1;
	}
	return 0;
}
