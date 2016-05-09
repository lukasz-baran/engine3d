
#ifndef __material_hpp__
#define __material_hpp__

struct SColor
{
	float	rgba[3];

	friend ostream& operator<< (ostream&, SColor&);
};



class CMaterial
{
public:

	CMaterial() : text1(NULL), text2(NULL) { } 


	struct SMap
	{
		char		*name;
		float		Uscale, Vscale;
		float		Uoffset, Voffset;
		float		rot_angle;

		GLuint		id;
	};

	SColor		ambient, diffuse, specular;

	float		m_fShininess;
	float		m_fTransparency;	/// przezroczystoœæ

	SMap		*text1;
	SMap		*text2;

	void SetUp();
};


#endif