
#include <math.h>
#include <windows.h>
#include "image.h"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

#define DELARR(tt) if (tt != NULL) { delete[] tt; tt = NULL; }

CImage::CImage() : pPalette(NULL), image(NULL), data(NULL)
{
        for (int i = 0; i < 4096; i++) {
                strarray[i].str = NULL;
                strarray[i].num = 0;
        }
}

int CImage::LoadGIF(const char *fname)
{
	int i, version;
	
	if (!buffer.Init(fname))
		return IMAGE_MEMORY_ALLOCATION;
	
	pos = 0; // pozycja w buforze
	
	if ((GIFSignature[0] != buffer[pos+0]) ||
		(GIFSignature[1] != buffer[pos+1]) ||
		(GIFSignature[2] != buffer[pos+2]))
		return IMAGE_NO_GIF_FILE;
	pos+=3;
	
	if ((GIF89aId[0] != buffer[pos+0]) ||
		(GIF89aId[1] != buffer[pos+1]) ||
		(GIF89aId[2] != buffer[pos+2])) {

		if ((GIF87aId[0] != buffer[pos+0]) ||
			(GIF87aId[1] != buffer[pos+1]) ||
			(GIF87aId[2] != buffer[pos+2])) 
			return IMAGE_UNKNOWN_VERSION;
		else
			version = 87;

	}
		else
			version = 89;
	pos+=3;
	
	GIFScrWidth = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
	pos+=2;
	
	GIFScrHeight = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
	pos+=2;
	
	BYTE packedf = buffer[pos++];
	GIFGlobalColorTable = packedf >> 7;
	GIFColorResolution = (packedf&112)>>4 + 1;
	GIFSort = (packedf&8)>>3;
	GIFSizeOfGCT = (int)pow(2,(packedf&7)+1);
	
	GIFBackgroundColor = buffer[pos++];
	GIFPixelAspectRatio = buffer[pos++];
	
	//jesli globalna tablica kolorow jest obecna:
	if (GIFGlobalColorTable) {
		pPalette = new BYTE[3*GIFSizeOfGCT];
		
		//trzeba tu uwzglednic ile bajtow opisuje jeden kolor:
		for (int i = 0; i < 3*GIFSizeOfGCT; i++)
			pPalette[i] = buffer[pos++];
	}
	
	//omin rozszerzenie:
	if (buffer[pos] == 0x21) {
		pos++;
		pos++;
		
		while (buffer[pos]>0) {
			for (int i = buffer[pos]; i > 0; i--)
				pos++;
			pos++;
		}
		pos++;
	}
	
	//image descriptor:
	if (buffer[pos] == 0x2C) {
		pos++;
		
		GIFLeftPosition = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
		pos += 2;
		
		GIFTopPosition = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
		pos += 2;
		
		GIFWidth = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
		pos += 2;
		
		GIFHeight = (int)buffer[pos+0] + (int)buffer[pos+1]*256;
		pos += 2;
		
		//dekodowanie flag:
		packedf = buffer[pos++];
		GIFLocalColorTable = packedf >> 7;
		GIFInterlace = (packedf >> 6)&1;
		GIFIDSort = (packedf >> 5)&1;
		if (GIFLocalColorTable)
			GIFSizeOfLCT = (int)pow(2,(packedf&7)+1);
		else
			GIFSizeOfLCT = 0;
		
	}
	
	GIFCodeSize = buffer[pos++];
	
	//bez pieprzenia sie z buforem danych
	int		size = 0;
	DWORD	temppos = pos;
	
	while (buffer[temppos] != 0) {
		size += buffer[temppos];
		temppos += (buffer[temppos]+1);
	}
	
	data = new BYTE[size];
	
	temppos = pos;
	DWORD datapos = 0;
	while (buffer[temppos] != 0) {
		for (i = 1; i < (int)buffer[temppos]+1; i++) 
			data[datapos++] = buffer[temppos + i];
		temppos += (buffer[temppos]+1);
	}
	
	//--------------------------------------------------------
#define         BLOCK_TERMINATOR        0
	
	DWORD	oldcode,
		ClearCode = (int)pow(2, GIFCodeSize),
		EOICode   = ClearCode + 1,
		strapos,
		imagepos, //pozycja w image[]
		subsize;
	int		codelen;

	image = new BYTE[GIFWidth * GIFHeight];
	imagepos = 0;

	subsize = size;
	subpos = 0;
	bitsubpos = 0;

	//na wszelki wypadek przygotuj tablice strarray
	//niektore GIF-y nie maja na poczatku ClearCode!
	for (i = 0; i < 4096; i++) {
		DELARR(strarray[i].str);
		strarray[i].num = 0;
	}
	size = 1<<GIFCodeSize;
	for (i = 0; i < size; i++) {
		strarray[i].str = new char[1];
		strarray[i].str[0] = (char)i;
		strarray[i].num = 1;
	}
	strapos = EOICode + 1;
	codelen = GIFCodeSize + 1;


	while ((code = ReadCode(codelen)) != EOICode) {

		if (code == ClearCode) {
			
			//czysc tablice:
			for (i = 0; i < 4096; i++) {
				DELARR(strarray[i].str);
				strarray[i].num = 0;
			}
			
			int size = 1<<GIFCodeSize;
			for (i = 0; i < size; i++) {
				strarray[i].str = new char[1];
				strarray[i].str[0] = (char)i;
				strarray[i].num = 1;
			}
			strapos = EOICode + 1;
			codelen = GIFCodeSize + 1;
			
			code = ReadCode(codelen);
			if (code == EOICode) 
				break;
			for (i = 0; i < strarray[code].num; i++)
				image[imagepos++] = strarray[code].str[i];
			
			oldcode = code;
			
		} else {
			
			if (IsInTable(code) == TRUE) { 
				for (i = 0; i < strarray[code].num; i++)
					image[imagepos++] = strarray[code].str[i];
				
				//przetlumacz oldcode na prefix
				strarray[strapos].str = new char[ strarray[oldcode].num + 1 ];
				strarray[strapos].num = strarray[oldcode].num + 1;
				for (i = 0; i < strarray[oldcode].num; i++)
					strarray[ strapos ].str[i] = strarray[oldcode].str[i];
				
				//dopisz pierwszy znak z lancucha code
				strarray[strapos].str[ strarray[oldcode].num ] = strarray[code].str[0];
				strapos++;
				
				oldcode = code;
				
			} else { 
				strarray[strapos].str = new char[ strarray[oldcode].num + 1 ];
				strarray[strapos].num = strarray[oldcode].num + 1;
				for (i = 0; i < strarray[oldcode].num; i++)
					strarray[strapos].str[i] = strarray[oldcode].str[i];
				strarray[strapos].str[strarray[oldcode].num] = strarray[code].str[0];
				
				//wypisz to do bufora wyjsciowego:
				for (i = 0; i < strarray[code].num; i++)
					image[imagepos++] = strarray[code].str[i];
				
				strapos++;
				
				oldcode = code;
			}
			
			if (((strapos >> codelen) > 0) && (codelen < 12))
				codelen++;
		}
		
	} //while

#undef          BLOCK_TERMINATOR

	DELARR(data);

	return IMAGE_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////

DWORD CImage::ReadCode(int codelen)
{
	DWORD  retcode; //kod ktory zwrocimy
	
	//odczytaj kod
	retcode = data[subpos];

	retcode += (((DWORD)data[subpos+1])<<8);
	if (codelen > 9)
		retcode += (((DWORD)data[subpos+2])<<16);

	retcode >>= bitsubpos;
	retcode <<= (32-codelen);
	retcode >>= (32-codelen);

	//sprawdz czy newsubpos nie przekroczylo zakresu
	int    newbitsubpos  = bitsubpos;
	DWORD  newsubpos     = subpos;
	newbitsubpos += codelen;
	while (newbitsubpos > 7) {
          newbitsubpos -= 8;
          newsubpos++;
	}
	
	subpos = newsubpos;
	bitsubpos = newbitsubpos;
	
	return retcode;
}

/////////////////////////////////////////////////////////////////////////////////////////

CImage::~CImage()
{
	this->Release();
}

void CImage::Release()
{
	DELARR(pPalette);
	DELARR(data);
	DELARR(image);
}

/////////////////////////////////////////////////////////////////////////////////////////


void CImage::CopyImage(unsigned char *where, int type)
{
	int	i, x, y, format;

	if (type == FORMAT_RGB)
		format = 3;

	if (!GIFInterlace) {
		for (i = 0; i < GIFWidth*GIFHeight; i++) {

			where[format*i + 0] = pPalette[3*image[i] + 0];
			where[format*i + 1] = pPalette[3*image[i] + 1];
			where[format*i + 2] = pPalette[3*image[i] + 2];

		}
	} else {
		int ykon;
		i = 0;

		ykon = GIFHeight/8 + 1;
		for (y = 0; y < ykon; y++) {
			for (x = 0; x < GIFWidth; x++) {
/*				where[y*GIFWidth*8 + x] = RGB(pPalette[3*image[i] + 2],
										pPalette[3*image[i] + 1],
										pPalette[3*image[i] + 0]);*/

				where[(y*GIFWidth*8 + x)*format + 0] = pPalette[3*image[i] + 0];
				where[(y*GIFWidth*8 + x)*format + 1] = pPalette[3*image[i] + 1];
				where[(y*GIFWidth*8 + x)*format + 2] = pPalette[3*image[i] + 2];

				i++;
			}
		}

		ykon = GIFHeight/8;
		if ((8*ykon + 4) < GIFHeight)
			ykon++;
		for (y = 0; y < ykon; y++) {
			for (x = 0; x < GIFWidth; x++) {
/*				where[4*GIFWidth + y*GIFWidth*8 + x] = RGB(pPalette[3*image[i] + 2],
											pPalette[3*image[i] + 1],
											pPalette[3*image[i] + 0]);*/
				where[(4*GIFWidth + y*GIFWidth*8 + x)*format + 0] = pPalette[3*image[i] + 0];
				where[(4*GIFWidth + y*GIFWidth*8 + x)*format + 1] = pPalette[3*image[i] + 1];
				where[(4*GIFWidth + y*GIFWidth*8 + x)*format + 2] = pPalette[3*image[i] + 2];


				i++;
			}
		}

		ykon = GIFHeight/4;
		if ((4*ykon + 2) < GIFHeight)
			ykon++;
		for (y = 0; y < ykon; y++) {
			for (x = 0; x < GIFWidth; x++) {
/*				where[2*GIFWidth + y*GIFWidth*4 + x] = RGB(pPalette[3*image[i] + 2],
											pPalette[3*image[i] + 1],
											pPalette[3*image[i] + 0]);*/

				where[(2*GIFWidth + y*GIFWidth*4 + x)*format + 0] = pPalette[3*image[i] + 0];
				where[(2*GIFWidth + y*GIFWidth*4 + x)*format + 1] = pPalette[3*image[i] + 1];
				where[(2*GIFWidth + y*GIFWidth*4 + x)*format + 2] = pPalette[3*image[i] + 2];

				i++;
			}
		}

		ykon = GIFHeight/2;
		for (y = 0; y < ykon; y++) {
			for (x = 0; x < GIFWidth; x++) {
/*				where[GIFWidth + y*GIFWidth*2 + x] = RGB(pPalette[3*image[i] + 2],
											pPalette[3*image[i] + 1],
											pPalette[3*image[i] + 0]);*/

				where[(GIFWidth + y*GIFWidth*2 + x)*format + 0] = pPalette[3*image[i] + 0];
				where[(GIFWidth + y*GIFWidth*2 + x)*format + 1] = pPalette[3*image[i] + 1];
				where[(GIFWidth + y*GIFWidth*2 + x)*format + 2] = pPalette[3*image[i] + 2];

				i++;
			}
		}

	}
}
