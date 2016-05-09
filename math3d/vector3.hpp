
#ifndef __vector3_hpp__
#define __vector3_hpp__

////////////////////////////////////////////////////////////////////
///  wektor kolumnowy!
///

///>====================================----------------------------
///
///	d e f i n i c j a
///
///>====================================----------------------------

class CVector4;

class CVector3
{
public:
	struct { float x, y, z; };

	CVector3();
	CVector3(const CVector3 &);
	CVector3(float, float, float);
	CVector3(float);

	~CVector3() { };

	/// oblicza wektor normalny od p³aszczyzny:
	void CalcNormal(const CVector3 &, const CVector3 &, const CVector3 &);

	/// normalizuje wektor
	void Neg();
	void Normalize();
	void Zeros();
	bool IsZero() const;
	void Ones();
	float Length();

	float operator [](int) const;

	/// operacje matematyczne:
	friend CVector3 operator +(const float, const CVector3 &);
	friend CVector3 operator *(const float, const CVector3 &);
	friend CVector3 operator +(const CVector3 &, const float);
	friend CVector3 operator -(const CVector3 &, const float);
	friend CVector3 operator *(const CVector3 &, const float);
	friend CVector3 operator /(const CVector3 &, const float);

        CVector3 operator +(CVector3 &) const;
        CVector3 operator -(CVector3 &) const;
	CVector3 operator *(CVector3 &) const;

	CVector3& operator += (CVector3 &);
	CVector3& operator -= (CVector3 &);
	CVector3& operator *= (CVector3 &);

	CVector3& operator +=(float);
	CVector3& operator -=(float);
	CVector3& operator *=(float);
	CVector3& operator /=(float);

	CVector3 operator -();
	float Dot(CVector3 &) const;
	void Cross(CVector3 &, CVector3 &);

	/// porównywanie wektorów
	friend bool operator== (const CVector3 &, const CVector3 &);
	friend bool operator!= (const CVector3 &, const CVector3 &);

	CVector3& operator= (const CVector3 &);

	friend ostream& operator<< (ostream &, CVector3 &);
};

////////////////////////////////////////////////////////////////////

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

inline CVector3::CVector3() : x(0.0f), y(0.0f), z(0.0f)
{
}

inline CVector3::CVector3(const CVector3 &_other) :
x(_other.x), y(_other.y), z(_other.z)
{
}

inline CVector3::CVector3(float _x, float _y, float _z) :
x(_x), y(_y), z(_z)
{
}

inline CVector3::CVector3(float _all) :
x(_all), y(_all), z(_all)
{
}

inline void CVector3::Zeros()
{
	x = y = z = 0.0f;
}

inline bool CVector3::IsZero() const
{
	return (x==0.0f && y==0.0f && z==0.0f);
}

inline void CVector3::Ones()
{
	x = y = z = 1.0f;
}

inline float CVector3::Length()
{
	return (float)sqrt(x*x + y*y + z*z);
}

inline void CVector3::Neg()
{
	x = -x;
	y = -y;
	z = -z;
}

inline float CVector3::operator [](int i) const
{
	switch (i) {
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	default: return 0.0f; break;
	}
}

inline CVector3& CVector3::operator +=(float _a)
{
	x += _a;
	y += _a;
	z += _a;
	return *this;
}

inline CVector3& CVector3::operator -=(float _a)
{
	x -= _a;
	y -= _a;
	z -= _a;
	return *this;
}

inline CVector3& CVector3::operator *=(float _a)
{
	x *= _a;
	y *= _a;
	z *= _a;
	return *this;
}

inline CVector3& CVector3::operator /=(float _a)
{
	x /= _a;
	y /= _a;
	z /= _a;
	return *this;
}

inline CVector3& CVector3::operator=(const CVector3 &other)
{
	if (this != &other)
		(*this).CVector3::CVector3(other);
	return *this;
}

inline CVector3 CVector3::operator +(CVector3 &_other) const
{
	return CVector3(x+_other.x, y+_other.y, z+_other.z);
}

inline CVector3 CVector3::operator -(CVector3 &_other) const
{
	return CVector3(x-_other.x, y-_other.y, z-_other.z);
}

inline CVector3 CVector3::operator *(CVector3 &_other) const
{
	return CVector3(x*_other.x, y*_other.y, z*_other.z);
}

inline void CVector3::Cross(CVector3 &_v1, CVector3 &_v2)
{
	z = _v1.y * _v2.z + _v1.z * _v2.y;
	y = _v1.z * _v2.x + _v1.x * _v2.z;
	z = _v1.x * _v2.y + _v1.y * _v2.x;
}

inline CVector3& CVector3::operator += (CVector3 &_v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	return *this;
}

inline CVector3& CVector3::operator -= (CVector3 &_v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	return *this;
}

inline CVector3& CVector3::operator *= (CVector3 &_v)
{
	x *= _v.x;
	y *= _v.y;
	z *= _v.z;
	return *this;
}

/// TODO: sprawdziæ czy tu nie mo¿e byæ referencja zwracana:
inline CVector3 CVector3::operator -()
{
	return CVector3(-x, -y, -z);
}

inline float CVector3::Dot(CVector3 &_other) const
{
	return x*_other.x + y*_other.y + z*_other.z;
}

inline bool operator==(const CVector3 &a, const CVector3 &b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

inline bool operator!=(const CVector3 &a, const CVector3 &b)
{
	return (a.x != b.x || a.y != b.y || a.z != b.z);
}

inline CVector3 operator +(const float f, const CVector3 &v)
{
	return CVector3(f+v.x, f+v.y, f+v.z);
}

inline CVector3 operator *(const float r, const CVector3 &v)
{
	return CVector3(r*v.x, r*v.y, r*v.z);
}

inline CVector3 operator *(const CVector3 &v, const float r)
{
	return CVector3(r*v.x, r*v.y, r*v.z);
}

inline CVector3 operator /(const CVector3 &v, const float r)
{
	return CVector3(v.x/r, v.y/r, v.z/r);
}

inline CVector3 operator +(const CVector3 &v, const float f)
{
	return CVector3(v.x+f, v.y+f, v.z+f);
}

inline CVector3 operator -(const CVector3 &v, const float f)
{
	return CVector3(v.x-f, v.y-f, v.z-f);
}

#endif