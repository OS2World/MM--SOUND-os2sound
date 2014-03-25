/*
**	file			sound.c
**	description	Sound sample player for OS/2
**	author		Paul van Keep
**	copyright	(NC)Not Copyrighted by the Frobozz Magic Software Company
**	remarks		Plays .snd & mac sound files
**					compile with MSC 6.0
**
** use and distribute freely
** 
*/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

#define INCL_DOS
#include <os2.h>

extern void pascal play(char far *buf, int size, char freq);

#define NRBUFS		20
static char *block[NRBUFS];
static unsigned short bsize[NRBUFS];

#define BS	(unsigned short)(1024 * 60)

static short data[4];
static short freq[] = { 22000, 11000, 7333, 5500 , 4400, 3666, 3143, 2750, 2444, 2200, 2000 };

unsigned char hdr[128];

int main(int argc, char *argv[])
{
	register int i, j, k;
	unsigned short s;
	long len, base;
	FILE *f;
	if (argc == 1) {
		printf("Usage: sound <file> ... \n");
		return 1;
	}
	DosPortAccess(0, 0, 0x21, 0x21);				// IRQ mask
	DosPortAccess(0, 0, 0x42, 0x43);				// Timer 2 access
	DosPortAccess(0, 0, 0x61, 0x61);				// speaker output
	for (k = 1; k < argc; k++) {
		f = fopen(argv[k], "rb");
		if (f == NULL) {
			printf("Cannot open file %s\n", argv[k]);
			continue;
		}
		printf("Playing %s\r", argv[k]);
		fread(hdr, sizeof(hdr), 1, f);
		if (hdr[1] == 0 || (hdr[1] == 0xFF)) {				// it is an SND file
			memcpy(data, hdr, 4 * sizeof(short));
			base = 8L;
		} else {
			data[0] = 0;
			data[1] = 22000;
			data[2] = 10;
			data[3] = 4;
			base = 0L;
		}
		if (memcmp(hdr + 65, "FSS", 3) == 0) {		// has header
			len = 256L * 256L * 256L * hdr[83] +
						 	256L * 256L * hdr[84] +
							     	256L * hdr[85] +
										   	hdr[86];
			base = 128L;
		} else {
			len = filelength(fileno(f)) - base;
		}
		fseek(f, base, 0);
		for (i = 0; i < sizeof(freq)/sizeof(short); i++) {
			if (data[1] >= freq[i]) {
				data[1] = i + 1;
				break;
			}
		}
		i = 0;
		while (len > 0L) {
			bsize[i] = (len > (unsigned long)BS) ? BS : (unsigned short)len;
			len -= (long)bsize[i];
			block[i] = malloc(bsize[i]);
			fread(block[i], 1, bsize[i], f);
			i++;
		}
		fclose(f);
		for (j = 0; j < i; j++) {
			play(block[j], bsize[j], (char)data[1]);
		}
		for (j = 0; j < i; j++) {
			free(block[j]);
		}
		DosPortAccess(0, 1, 0x61, 0x61);				// speaker output
		DosPortAccess(0, 1, 0x42, 0x43);				// Timer 2 access
		DosPortAccess(0, 1, 0x21, 0x21);				// IRQ mask
	}
	return 0;
}
