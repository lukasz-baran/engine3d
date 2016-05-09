
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

void CCamera::UpdateFrame(CKeyframer *pKeyf)
{
	Position = (CVector3)pKeyf->mtxTransformation.GetColumn(3);
}

////////////////////////////////////////////////////////////////////

void CCameraTarget::UpdateFrame(CKeyframer *pKeyf)
{
	Target = (CVector3)pKeyf->mtxTransformation.GetColumn(3);
}

////////////////////////////////////////////////////////////////////

void CLight::UpdateFrame(CKeyframer *pKeyf)
{
	Position = (CVector3)pKeyf->mtxTransformation.GetColumn(3);
}

////////////////////////////////////////////////////////////////////

void CLightTarget::UpdateFrame(CKeyframer *pKeyf)
{
	Target = (CVector3)pKeyf->mtxTransformation.GetColumn(3);
}

////////////////////////////////////////////////////////////////////

CMesh::CMesh() : m_pVertices(NULL),
	m_pIndices(NULL),
	m_nVertices(0),
	m_nIndices(0)
{
}

////////////////////////////////////////////////////////////////////

CMesh::~CMesh()
{
	glDeleteLists(ListIndex, 1);
	DELETE_ARRAY(m_pIndices);
	DELETE_ARRAY(m_pVertices);
}

////////////////////////////////////////////////////////////////////

void CMesh::SetUpStandardMaterial()
{
	GLfloat material_specular[4] = {0.5f, 0.5f, 0.5f, 1.0f},
		material_ambient[4] = {0.6f, 0.6f, 0.6f, 1.0f},
		material_diffuse[4] = {0.7f, 0.7f, 0.7f, 1.0f};

	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 0.5f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
}

////////////////////////////////////////////////////////////////////

void CMesh::Draw()
{
	DWORD		i;
	CVector3	point;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_nIndices; i++) {
		glNormal3f(m_pVertices[ m_pIndices[i].a ].normal[0],
			m_pVertices[ m_pIndices[i].a ].normal[1],
			m_pVertices[ m_pIndices[i].a ].normal[2]);
		point = m_pVertices[ m_pIndices[i].a ].p;
		glVertex3f(point.x, point.y, point.z);
		
		glNormal3f(m_pVertices[ m_pIndices[i].b ].normal[0],
			m_pVertices[ m_pIndices[i].b ].normal[1],
			m_pVertices[ m_pIndices[i].b ].normal[2]);
		point = m_pVertices[ m_pIndices[i].b ].p;
		glVertex3f(point.x, point.y, point.z);
		
		glNormal3f(m_pVertices[ m_pIndices[i].c ].normal[0],
			m_pVertices[ m_pIndices[i].c ].normal[1],
			m_pVertices[ m_pIndices[i].c ].normal[2]);
		point = m_pVertices[ m_pIndices[i].c ].p;
		glVertex3f(point.x, point.y, point.z);
	} /// for
	glEnd();
}

////////////////////////////////////////////////////////////////////

void CMesh::DrawTextured()
{
	DWORD		i;
	CVector3	point;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < m_nIndices; i++) {
		glNormal3f(m_pVertices[ m_pIndices[i].a ].normal[0],
			m_pVertices[ m_pIndices[i].a ].normal[1],
			m_pVertices[ m_pIndices[i].a ].normal[2]);
		point = m_pVertices[ m_pIndices[i].a ].p;
		glTexCoord2f(m_pVertices[ m_pIndices[i].a ].u,
			m_pVertices[ m_pIndices[i].a ].v);
		glVertex3f(point.x, point.y, point.z);
		
		glNormal3f(m_pVertices[ m_pIndices[i].b ].normal[0],
			m_pVertices[ m_pIndices[i].b ].normal[1],
			m_pVertices[ m_pIndices[i].b ].normal[2]);
		point = m_pVertices[ m_pIndices[i].b ].p;
		glTexCoord2f(m_pVertices[ m_pIndices[i].b ].u,
			m_pVertices[ m_pIndices[i].b ].v);
		glVertex3f(point.x, point.y, point.z);
		
		glNormal3f(m_pVertices[ m_pIndices[i].c ].normal[0],
			m_pVertices[ m_pIndices[i].c ].normal[1],
			m_pVertices[ m_pIndices[i].c ].normal[2]);
		point = m_pVertices[ m_pIndices[i].c ].p;
		glTexCoord2f(m_pVertices[ m_pIndices[i].c ].u,
			m_pVertices[ m_pIndices[i].c ].v);
		glVertex3f(point.x, point.y, point.z);
	} /// for
	glEnd();
}

////////////////////////////////////////////////////////////////////

void CMesh::CreateDisplayList()
{
	DWORD		i;
	bool		bBlend = false;
	bool		bOneMat = true;

	/// sprawdzamy czy mesh jest jednorodny pod wzglêdem materia³u jaki jest
	/// mu przypisany, tzn. sprawdzamy czy ka¿dy trójk¹t na meshu ma taki
	/// sam materia³:
	CMaterial	*pMat = m_pIndices[0].pmat;
	for (i = 0; i < m_nIndices; i++)
		if (m_pIndices[i].pmat != pMat) 
			bOneMat = false;
		
	if (bOneMat) {
		ListIndex = glGenLists(1);
		glNewList(ListIndex, GL_COMPILE);
		if (pMat) {
			pMat->SetUp();
			if (pMat->text1) 
				DrawTextured();
			else
				Draw();
		} else {
			SetUpStandardMaterial();
			Draw();
		}
		glEndList();
	} else {

		CVector3	point;

		ListIndex = glGenLists(1);
		glNewList(ListIndex, GL_COMPILE);

		glBegin(GL_TRIANGLES);
		for (i = 0; i < m_nIndices; i++) {
			if (m_pIndices[i].pmat->text1 != NULL) 
				glBindTexture(GL_TEXTURE_2D, m_pIndices[i].pmat->text1->id);

			glNormal3f(m_pVertices[ m_pIndices[i].a ].normal[0],
				m_pVertices[ m_pIndices[i].a ].normal[1],
				m_pVertices[ m_pIndices[i].a ].normal[2]);
			point = m_pVertices[ m_pIndices[i].a ].p;
			glTexCoord2f(m_pVertices[ m_pIndices[i].a ].u,
				m_pVertices[ m_pIndices[i].a ].v);
			glVertex3f(point.x, point.y, point.z);

			glNormal3f(m_pVertices[ m_pIndices[i].b ].normal[0],
				m_pVertices[ m_pIndices[i].b ].normal[1],
				m_pVertices[ m_pIndices[i].b ].normal[2]);
			point = m_pVertices[ m_pIndices[i].b ].p;
			glTexCoord2f(m_pVertices[ m_pIndices[i].b ].u,
				m_pVertices[ m_pIndices[i].b ].v);
			glVertex3f(point.x, point.y, point.z);
	
			glNormal3f(m_pVertices[ m_pIndices[i].c ].normal[0],
				m_pVertices[ m_pIndices[i].c ].normal[1],
				m_pVertices[ m_pIndices[i].c ].normal[2]);
			point = m_pVertices[ m_pIndices[i].c ].p;
			glTexCoord2f(m_pVertices[ m_pIndices[i].c ].u,
				m_pVertices[ m_pIndices[i].c ].v);
			glVertex3f(point.x, point.y, point.z);
		}
		glEnd();
		glEndList();
	}
}

////////////////////////////////////////////////////////////////////

void CMesh::UpdateFrame(CKeyframer *pKeyf)
{
	mtxTransformation = pKeyf->mtxTransformation;

	/// to nale¿y odkomentarzowaæ jeœli nie jest wywo³ana
	/// metoda CScene::PrepareAnimation:
///	CMatrix4x4	mtxPivot;
///	mtxPivot.TranslationMtx(-pKeyf->Pivot);
///	mtxTransformation *= mtxPivot;
	
	mtxTransformation.Transpose();
}

////////////////////////////////////////////////////////////////////

void CMesh::Render()
{
	glPushMatrix();
	glMultMatrixf(mtxTransformation._mat_1D);
	glCallList(ListIndex);
	glPopMatrix();
}

////////////////////////////////////////////////////////////////////

void CMesh::CalcNormals()
{
	DWORD	i, j;
	/// oblicz wektory normalne dla œcian:
	for (i = 0; i < m_nIndices; i++) {
		m_pIndices[i].normal.CalcNormal(
			m_pVertices[ m_pIndices[i].a ].p,
			m_pVertices[ m_pIndices[i].b ].p,
			m_pVertices[ m_pIndices[i].c ].p);
		m_pIndices[i].normal.Neg();
	}

	/// oblicz wektory normalne dla wierzcho³ków:
	for (i = 0; i < m_nVertices; i++) {

		int		counts = 0;
		CVector3	n(0.0f);

		for (j = 0; j < m_nIndices; j++)
			if (i == m_pIndices[j].a || i == m_pIndices[j].b ||
			i == m_pIndices[j].c) {
				n += m_pIndices[j].normal;
				counts++;
			}

		m_pVertices[i].normal = n / ((float)counts);
		m_pVertices[i].normal.Normalize();
	}

}

////////////////////////////////////////////////////////////////////

void CLight::Render()
{
	/// czy nie jest za du¿o œwiate³?
	if (number > 7)
		return;

	float pfAmbient[4], pfDiffuse[4], pfSpecular[4],
		pfPosition[4];

	pfAmbient[0] = 0.5f;
	pfAmbient[1] = 0.5f;
	pfAmbient[2] = 0.5f;
	pfAmbient[3] = 1.0f;

	pfDiffuse[0] =  Color.rgba[0];
	pfDiffuse[1] =  Color.rgba[1];
	pfDiffuse[2] =  Color.rgba[2];
	pfDiffuse[3] =  1.0f;

	pfSpecular[0] = Color.rgba[0];
	pfSpecular[1] = Color.rgba[1];
	pfSpecular[2] = Color.rgba[2];
	pfSpecular[3] = 1.0f;

	pfPosition[0] =  Position.x;
	pfPosition[1] =  Position.y;
	pfPosition[2] =  Position.z;
	pfPosition[3] =  1.0f;

	GLenum enumlight;
	switch (number) {
	case 0: enumlight = GL_LIGHT0; break;
	case 1: enumlight = GL_LIGHT1; break;
	case 2: enumlight = GL_LIGHT2; break;
	case 3: enumlight = GL_LIGHT3; break;
	case 4: enumlight = GL_LIGHT4; break;
	case 5: enumlight = GL_LIGHT5; break;
	case 6: enumlight = GL_LIGHT6; break;
	case 7: enumlight = GL_LIGHT7; break;
	default: break;
	}

	glLightfv(enumlight, GL_AMBIENT, pfAmbient);
	glLightfv(enumlight, GL_DIFFUSE, pfDiffuse);
	glLightfv(enumlight, GL_SPECULAR, pfSpecular);
	glLightfv(enumlight, GL_POSITION, pfPosition);
	glEnable(enumlight);
}

////////////////////////////////////////////////////////////////////
