
#ifndef __giflibrary_hpp__
#define __giflibrary_hpp__

#include "buffer.h"

const char GIFSignature[] = {'G', 'I', 'F'};
const char GIF89aId[] = {'8', '9', 'a'};
const char GIF87aId[] = {'8', '7', 'a'};

enum 
{
	IMAGE_OK,
	IMAGE_NO_GIF_FILE,
	IMAGE_UNKNOWN_VERSION,
	IMAGE_MEMORY_ALLOCATION
};

// ta struktura ma sluzyc do uzyskiwania danych
// o aktualnym GIF'ie
typedef struct  
{
	char		*FileName;
	char		*Version;
	DWORD	Width, Height;
	DWORD	BPP;			// 8, 4, 2, 1
	BOOL		Interlace;
} GIFInfo;

class CImage
{
	int		GIFScrWidth;	
	int		GIFScrHeight;
	int		GIFBackgroundColor;
	int		GIFPixelAspectRatio;

	BOOL		GIFGlobalColorTable;
	int		GIFColorResolution;
	BOOL		GIFSort;
	int		GIFSizeOfGCT; //GCT = Global Color Table

	//from image descriptor:
	int		GIFTopPosition, GIFLeftPosition;
	int		GIFWidth, GIFHeight;
	BOOL		GIFLocalColorTable;
	BOOL		GIFInterlace;
	BOOL		GIFIDSort;
	int		GIFSizeOfLCT;

	int		GIFCodeSize;
	BYTE		*pPalette;

	CBuffer<BYTE>	buffer;
	DWORD		pos; //pozycja w tym buforze
	struct {
		char   *str;
		DWORD  num;
	} strarray[4096];
	DWORD	subpos, code;
	int		bitsubpos;

	DWORD	ReadCode(int codelen);
	BOOL		IsInTable(DWORD code)  
	{
		if (strarray[code].str != NULL)
			return TRUE;
		else
			return FALSE;
	}

	BYTE		*image; //obrazek w 256 kolorach
	BYTE		*data;

public:

	enum {
		FORMAT_RGB	= 0
	};

	CImage();
	virtual ~CImage();

	int	LoadGIF(const char *);

	DWORD	GetWidth() const { return GIFWidth; }
	DWORD	GetHeight() const { return GIFHeight; }
	
	void	CopyImage(unsigned char *, int);

	void	Release();
};


#endif