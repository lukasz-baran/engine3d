
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

ostream& operator<<(ostream& str, SColor& color)
{
	str<<'['<<color.rgba[0]<<' '<<color.rgba[1]<<' '<<
		color.rgba[2]<<']';
	return str;
}

////////////////////////////////////////////////////////////////////

void CMaterial::SetUp()
{
	if (text1) 
		glBindTexture(GL_TEXTURE_2D, text1->id);

	GLfloat material_specular[4],
		material_ambient[4],
		material_diffuse[4];
	material_ambient[0] = ambient.rgba[0]/255.f;
	material_ambient[1] = ambient.rgba[1]/255.f;
	material_ambient[2] = ambient.rgba[2]/255.f;
	material_ambient[3] = m_fTransparency/255.f;
	material_diffuse[0] = diffuse.rgba[0]/255.f;
	material_diffuse[1] = diffuse.rgba[1]/255.f;
	material_diffuse[2] = diffuse.rgba[2]/255.f;
	material_diffuse[3] = m_fTransparency/255.f;
	material_specular[0] = specular.rgba[0]/255.f;
	material_specular[1] = specular.rgba[1]/255.f;
	material_specular[2] = specular.rgba[2]/255.f;
	material_specular[3] = m_fTransparency/255.f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);

	GLfloat s = (GLfloat)pow(2, 10.0*m_fShininess);
	if (s > 128.0f) s = 128.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, s); 
}

