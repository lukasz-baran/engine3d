
#ifndef __object_hpp__
#define __object_hpp__

////////////////////////////////////////////////////////////////////

enum {
	OBJECT_MESH,
	OBJECT_LIGHT,
	OBJECT_LIGHT_TARGET,
	OBJECT_CAMERA,
	OBJECT_CAMERA_TARGET,
	OBJECT_3DS_DUMMY
};


class CKeyframer;

/// klasa abstrakcyjna 'CObject':
class CObject
{
public:
	/// macierz zawieraj¹ca przekszta³cenie obiektu jakie
	/// jest konieczne przy animowaniu:
	virtual ~CObject() { }

	virtual void Render() = 0;
	virtual int Type() const = 0;

	virtual void UpdateFrame(CKeyframer *) = 0;
};

////////////////////////////////////////////////////////////////////

class CMesh : public CObject
{

public:

	struct SVertex
	{
		CVector3	p;
		CVector3	normal;
		float		u, v;	/// texture coordinates
	};

	struct STriangle
	{
		CVector3	normal;

		DWORD		a, b, c;
		WORD		flag;

		CMaterial	*pmat;
	};

	SVertex		*m_pVertices;
	STriangle	*m_pIndices;

	DWORD		m_nVertices;
	DWORD		m_nIndices;

	CVector3	X1, X2, X3, Origin;

	CMatrix4x4	mtxTransformation;

	CMesh();
	virtual ~CMesh();

	GLuint	ListIndex;

	/// ta metoda jest u¿ywana przez CreateDisplayList
	/// w przypadku gdy nie zosta³ zdefiniowany materia³
	void SetUpStandardMaterial();

	void Draw();
	void DrawTextured();
	void CreateDisplayList();


	virtual void Render();

	virtual int Type() const { return OBJECT_MESH; }

	void CalcNormals();

	virtual void UpdateFrame(CKeyframer *);
};

////////////////////////////////////////////////////////////////////

class CCamera : public CObject
{
public:
	CVector3	Position;	/// wektor polozenia

	float		FOV;		/// FOV - field of view - rozpietosc ostroslupa widoku
	float		roll;		/// kat "przechylu"

	CCamera() { }
	virtual ~CCamera() { }

	virtual void Render() { }
	virtual int Type() const { return OBJECT_CAMERA; }
	virtual void UpdateFrame(CKeyframer *);
};

////////////////////////////////////////////////////////////////////

class CCameraTarget : public CObject
{
public:
	CVector3	Target;

	char*		name;

	CCameraTarget() { }
	virtual ~CCameraTarget() { }

	virtual void Render()
	{

	}
	virtual int Type() const { return OBJECT_CAMERA_TARGET; }
	virtual void UpdateFrame(CKeyframer *);
};

////////////////////////////////////////////////////////////////////

class CLight : public CObject
{
public:

	CVector3	Position;
	int		number;

	SColor		Color;

	CLight() { }
	virtual ~CLight() { }

	virtual void Render();
	virtual int Type() const { return OBJECT_LIGHT; }
	virtual void UpdateFrame(CKeyframer *);
};

////////////////////////////////////////////////////////////////////

class CLightTarget : public CObject
{
public:
	CVector3	Target;

	char*		name;

	CLightTarget() { }
	virtual ~CLightTarget() { }

	virtual void Render()
	{
	}
	virtual int Type() const { return OBJECT_LIGHT_TARGET; }
	virtual void UpdateFrame(CKeyframer *);
};

////////////////////////////////////////////////////////////////////

class C3DSDummy : public CObject
{
public:
	C3DSDummy() { }
	virtual ~C3DSDummy() { }

	virtual void Render() { }
	virtual int Type() const { return OBJECT_3DS_DUMMY; } 

	virtual void UpdateFrame(CKeyframer *) { } 

};

#endif