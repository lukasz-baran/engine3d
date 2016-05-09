
#ifndef __texture_hpp__
#define __texture_hpp__

class CTexture
{
	GLuint		m_TextId;
	int		m_Width, m_Height;


public:
	CTexture();
	~CTexture();

	bool LoadJPG(const char *);

	bool LoadGIF(const char *);

	GLuint Id() const { return m_TextId; }

	void Set() { glBindTexture(GL_TEXTURE_2D, m_TextId-1); }
};



#endif
