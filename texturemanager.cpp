
#include "includes.hpp"
#include "image.h"
#include "libpng\png.h"
#include "ijl.h"

#pragma comment(lib, "ijl.lib")

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

int CTextureManager::RecognizeTextType(const char *fname)
{
	if (strlen(fname) < 4) 
		return TEXTURE_EMPTY;

	char strExt[4];

	int j = 0;
	for (DWORD i = strlen(fname)-3; i < strlen(fname); i++)
		strExt[j++] = fname[i];
	strExt[3] = '\0';

	for (i = 0; i < 4; i++)
		strExt[i] = toupper(strExt[i]);
	
	if (strcmp(strExt, "GIF") == 0)
		return TEXTURE_GIF;

	if (strcmp(strExt, "PNG") == 0)
		return TEXTURE_PNG;

	if (strcmp(strExt, "JPG") == 0)
		return TEXTURE_JPG;

	if (strcmp(strExt, "TGA") == 0)
		return TEXTURE_TGA;

	if (strcmp(strExt, "BMP") == 0)
		return TEXTURE_BMP;

	return TEXTURE_UNKNOWN;
}

////////////////////////////////////////////////////////////////////

bool IsPowerOfTwo(DWORD val)
{
#define COMPARE(v, n) if (v == n) return true;
	COMPARE(val, 1);
	COMPARE(val, 2);
	COMPARE(val, 4);
	COMPARE(val, 8);
	COMPARE(val, 16);
	COMPARE(val, 32);
	COMPARE(val, 64);
	COMPARE(val, 128);
	COMPARE(val, 256);
	COMPARE(val, 512);
	COMPARE(val, 1024);
#undef COMPARE
	return false;
}

////////////////////////////////////////////////////////////////////

/// metoda dobiera rozmiar tekstury:
void CTextureManager::ScaleImage(BYTE **ptr, DWORD &w, DWORD &h)
{
	DWORD	tmp[10];
	DWORD	i;

	DWORD	act_size = (DWORD)sqrt(w*h);
	for (i = 0; i < 10; i++) {
		tmp[i] = (DWORD)abs((int)texture_sizes[i] - (int)act_size);
	}

	/// znajdŸ teraz najlepiej pasuj¹cy nowy rozmiar:
	DWORD	min = 0;
	for (i = 0; i < 10; i++)
		if (tmp[min] > tmp[i]) min = i;
	DWORD	new_size = texture_sizes[min];

	/// przydziel pamiêæ na obraz o nowym rozmiarze:
	BYTE	*new_ptr = new BYTE[new_size*new_size*3];

	/// w³aœciwe skalowanie odbywa siê tutaj:
	for (DWORD y = 0; y < new_size; y++)
		for (DWORD x = 0; x < new_size; x++) {

			DWORD x1 = x*(w-1)/(new_size-1);
			DWORD y1 = y*(h-1)/(new_size-1);
			DWORD pos = y1*w + x1;
			new_ptr[3*(y*new_size + x) + 0] = (*ptr)[pos*3 + 0];
			new_ptr[3*(y*new_size + x) + 1] = (*ptr)[pos*3 + 1];
			new_ptr[3*(y*new_size + x) + 2] = (*ptr)[pos*3 + 2];
		}

	DELETE_ARRAY( *ptr );
	*ptr = new_ptr;
	w = h = new_size;
}

////////////////////////////////////////////////////////////////////

bool CTextureManager::LoadGIF(GLuint &text_id, const char *fname)
{
	CImage		gifImage;
	BYTE		*data = NULL;
	int		ret;

	if ((ret = gifImage.LoadGIF(fname)) != IMAGE_OK) 
		return false;

	DWORD		dwHeight = gifImage.GetHeight();
	DWORD		dwWidth = gifImage.GetWidth();

	data = new BYTE[dwHeight * dwWidth * 3];
	gifImage.CopyImage(data, CImage::FORMAT_RGB);

	if ((dwHeight != dwWidth) || (!IsPowerOfTwo(dwHeight))) {
		/// trzeba przeskalowaæ:
		ScaleImage(&data, dwWidth, dwHeight);

	}

	glGenTextures(1, &text_id);
	glBindTexture(GL_TEXTURE_2D, text_id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		3,
		dwWidth,
		dwHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	DELETE_ARRAY(data);
	gifImage.Release();

	return true;
}

////////////////////////////////////////////////////////////////////

bool CTextureManager::LoadPNG(GLuint &text_id, const char *fname)
{
	FILE		*hFile;


	hFile = fopen(fname, "r+b");
	if (!hFile) 
		return false;

        png_structp read_ptr;
        png_infop info_ptr;
        png_infop end_info;
        png_uint_32 rowbytes;
        int channels, num_pass, pass;

        read_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
        info_ptr = png_create_info_struct( read_ptr );
        end_info = png_create_info_struct( read_ptr );

        if ( setjmp(read_ptr->jmpbuf) )
        {
          png_destroy_read_struct( &read_ptr, &info_ptr, &end_info );
		return false;
        }

        png_init_io( read_ptr, hFile );
        png_read_info( read_ptr, info_ptr );

        if ( (info_ptr->color_type&PNG_COLOR_TYPE_PALETTE) == PNG_COLOR_TYPE_PALETTE )
        {
          channels = 1;
        }
        else
          channels = 3;

        if ( info_ptr->color_type&PNG_COLOR_MASK_ALPHA ) channels++;

        rowbytes = ((info_ptr->width*info_ptr->bit_depth*channels + 7) >> 3);

        if ( info_ptr->interlace_type )
        {
		num_pass = png_set_interlace_handling(read_ptr);
        }
        else
		num_pass = 1;


        BYTE* pRaw = new BYTE[info_ptr->width*info_ptr->height*channels];
        BYTE* pRow;

        for (pass = 0; pass < num_pass; pass++ ) {
          for ( unsigned int i = 0; i != info_ptr->height; i++ ) 
          {
            pRow = &pRaw[rowbytes*i];
            png_read_rows( read_ptr, &pRow, 0, 1 );
          }
        }

	DWORD	dwHeight = info_ptr->height;
	DWORD	dwWidth  = info_ptr->width;

	if ((dwHeight != dwWidth) || (!IsPowerOfTwo(dwHeight))) {
		/// trzeba przeskalowaæ:
		ScaleImage(&pRaw, dwWidth, dwHeight);

	}

	glGenTextures(1, &text_id);
	glBindTexture(GL_TEXTURE_2D, text_id);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		3,
		dwWidth,
		dwHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		pRaw);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	png_read_end( read_ptr, end_info );
        png_destroy_read_struct( &read_ptr, &info_ptr, &end_info );
	DELETE_ARRAY(pRaw);
	fclose(hFile);
	return true;
}

////////////////////////////////////////////////////////////////////

bool CTextureManager::LoadJPG(GLuint &text_id, const char *fname)
{
	int			RetVal;
	JPEG_CORE_PROPERTIES	image;

	ZeroMemory(&image, sizeof(JPEG_CORE_PROPERTIES));

	if (ijlInit(&image) != IJL_OK)
		return false;

	image.JPGFile = strdup(fname);

	if ((RetVal = ijlRead(&image, IJL_JFILE_READPARAMS)) == IJL_OK) {
		DWORD m_Height = image.JPGHeight;
		DWORD m_Width = image.JPGWidth;

		BYTE *img = new BYTE[m_Height * m_Width * 3];
		if (img) {
			image.DIBBytes		= img;
			image.DIBColor		= IJL_RGB;
			image.DIBHeight		= m_Height;
			image.DIBWidth		= m_Width;
			image.DIBChannels 	= 3;

			if ((RetVal = ijlRead(&image,IJL_JFILE_READWHOLEIMAGE)) == IJL_OK) {
			
				glGenTextures(1, &text_id);
				glBindTexture(GL_TEXTURE_2D, text_id);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

				glTexImage2D(GL_TEXTURE_2D,
					0,
					3,
					image.DIBWidth,
					image.DIBHeight,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					img);

				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			}
		}
		delete [] img;
	}

	ijlFree(&image);

	return RetVal?false:true;
}

////////////////////////////////////////////////////////////////////

bool CTextureManager::LoadTexture(DWORD &ref, const char *fname)
{
	if (pTextures == NULL)
		throw CException("CTextureManager: Nie zainicjowano tekstur za pomoc¹ SetSize().");

	if (nCounter == nTextures)
		throw CException("CTextureManager: Skoñczy³o siê miejsce na tekstury.");

	GLuint		texture_id;
	
	switch (RecognizeTextType(fname)) {

	case TEXTURE_GIF:
		if (!LoadGIF(texture_id, fname))
			return false;
		break;

	case TEXTURE_JPG: 
		if (!LoadJPG(texture_id, fname))
			return false;
		break;

	case TEXTURE_EMPTY: 
	case TEXTURE_UNKNOWN: 
	case TEXTURE_BMP: 
	case TEXTURE_TGA: 
		return false;

	case TEXTURE_PNG: 
		if (!LoadPNG(texture_id, fname))
			return false;
		break;

	default: break;
	}

	pTextures[nCounter] = texture_id;
	nCounter++;
	ref = nCounter-1;
	return true;
}