
#ifndef __texturemanager_hpp__
#define __texturemanager_hpp__


/// TextureManager to taka tablica przechowuj¹ca tekstury.
/// Na razie dla ka¿dej tekstury przechowujemy jedynie 
/// identyfikatory dla OpenGL-a.

class CTextureManager
{
	enum {
		TEXTURE_EMPTY = 0,
		TEXTURE_UNKNOWN,
		TEXTURE_GIF,
		TEXTURE_PNG,
		TEXTURE_JPG,
		TEXTURE_BMP,
		TEXTURE_TGA
	};

	DWORD texture_sizes[10]; 

	void ScaleImage(BYTE **, DWORD &, DWORD &);
	int RecognizeTextType(const char *);

	bool LoadGIF(GLuint&, const char*);
	bool LoadPNG(GLuint&, const char*);
	bool LoadJPG(GLuint&, const char*);


	DWORD		nTextures;
	DWORD		nCounter;	/// wskazuje pierwszy wolny slot

	GLuint		*pTextures;

public:
	CTextureManager() : 
	pTextures(NULL), nTextures(0), nCounter(0)
	{
		DWORD s = 1;
		for (int i = 0; i < 10; i++) {
			texture_sizes[i] = s;
			s *= 2;
		}
	}

	~CTextureManager()
	{
		this->Release();
	}

	void Release()
	{
		if (nCounter > 0)
			glDeleteTextures(nCounter, pTextures);
		DELETE_ARRAY( pTextures );
		nTextures = nCounter = 0;
	}

	/// zwraca 'true' jeœli wczytano teksturê:
	bool LoadTexture(DWORD &ref, const char *fname);

	void SetSize(DWORD n) 
	{ 
		nTextures = n; 
		pTextures = new GLuint[nTextures];
	}

	DWORD GetSize() const { return nTextures; }

	GLuint GetTexture(DWORD nr)
	{
		if (nr >= nCounter)
			throw CException("CTextureManager: Próba odwo³ania siê do tekstury, której nie ma");
		return pTextures[nr];
	}

};

#endif

