
#ifndef __vector4_hpp__
#define __vector4_hpp__

///>====================================----------------------------
///
///	d e f i n i c j a
///
///>====================================----------------------------

class CVector4
{
public:
	struct { float x, y, z, w; };

	CVector4();
	CVector4(const CVector3 &); /// uwaga dodaje 1 pod 'w'
	CVector4(const CVector4 &);
	CVector4(float, float, float, float);
	CVector4(float);

	~CVector4() { };

	void Neg();
	void Normalize();
	void Zeros();
	bool IsZero() const;
	void Ones();
	float Length();

	float operator [](int) const;

	/// operatory konwersji:
	operator CVector3(void);

	/// operacje matematyczne:
	friend CVector4 operator +(const float, const CVector4 &);
	friend CVector4 operator *(const float, const CVector4 &);
	friend CVector4 operator +(const CVector4 &, const float);
	friend CVector4 operator -(const CVector4 &, const float);
	friend CVector4 operator *(const CVector4 &, const float);
	friend CVector4 operator /(const CVector4 &, const float);

        CVector4 operator +(CVector4 &) const;
        CVector4 operator -(CVector4 &) const;
	CVector4 operator *(CVector4 &) const;

	CVector4& operator += (CVector4 &);
	CVector4& operator -= (CVector4 &);
	CVector4& operator *= (CVector4 &);

	CVector4& operator +=(float);
	CVector4& operator -=(float);
	CVector4& operator *=(float);
	CVector4& operator /=(float);

	CVector4 operator -();
	float Dot(CVector4 &) const;

	/// porównywanie wektorów
	friend bool operator ==(const CVector4 &, const CVector4 &);
	friend bool operator !=(const CVector4 &, const CVector4 &);

	CVector4& operator=(const CVector4 &);

	friend ostream& operator<<(ostream &, CVector4 &);
};

////////////////////////////////////////////////////////////////////

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

inline CVector4::CVector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

inline CVector4::CVector4(const CVector3 &v3) :
x(v3.x), y(v3.y), z(v3.z), w(1.0f)
{
}

inline CVector4::CVector4(const CVector4 &_other) :
x(_other.x), y(_other.y), z(_other.z), w(_other.w)
{
}

inline CVector4::CVector4(float _x, float _y, float z, float _w) :
x(_x), y(_y), z(z), w(_w)
{
}

inline CVector4::CVector4(float _all) :
x(_all), y(_all), z(_all), w(_all)
{
}

inline void CVector4::Zeros()
{
	x = y = z = w = 0.0f;
}

inline bool CVector4::IsZero() const
{
	return (x==0.0f && y==0.0f && z==0.0f && w==0.0f);
}

inline void CVector4::Ones()
{
	x = y = z = w = 1.0f;
}

inline float CVector4::Length()
{
	return (float)sqrt(x*x + y*y + z*z + w*w);
}

inline void CVector4::Neg()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
}

inline float CVector4::operator [](int i) const
{
	switch (i) {
	case 0: return x; break;
	case 1: return y; break;
	case 2: return z; break;
	case 3: return w; break;
	default: throw i;
	}
}

///	operatory konwersji:

inline CVector4::operator CVector3(void)
{
	return CVector3(x/w, y/w, z/w);
}

///	operacje matematyczne:

inline CVector4& CVector4::operator +=(float _a)
{
	x += _a;
	y += _a;
	z += _a;
	w += _a;
	return *this;
}

inline CVector4& CVector4::operator -=(float _a)
{
	x -= _a;
	y -= _a;
	z -= _a;
	w -= _a;
	return *this;
}

inline CVector4& CVector4::operator *=(float _a)
{
	x *= _a;
	y *= _a;
	z *= _a;
	w *= _a;
	return *this;
}

inline CVector4& CVector4::operator /=(float _a)
{
	x /= _a;
	y /= _a;
	z /= _a;
	w /= _a;
	return *this;
}

inline CVector4& CVector4::operator=(const CVector4 &other)
{
	if (this != &other)
		(*this).CVector4::CVector4(other);
	return *this;
}

inline CVector4 CVector4::operator +(CVector4 &_other) const
{
	return CVector4(x+_other.x, y+_other.y, z+_other.z,
		w+_other.w);
}

inline CVector4 CVector4::operator -(CVector4 &_other) const
{
	return CVector4(x-_other.x, y-_other.y, z-_other.z,
		w+_other.w);
}

inline CVector4 CVector4::operator *(CVector4 &_other) const
{
	return CVector4(x*_other.x, y*_other.y, z*_other.z,
		w+_other.w);
}

inline CVector4& CVector4::operator += (CVector4 &_v)
{
	x += _v.x;
	y += _v.y;
	z += _v.z;
	w += _v.w;
	return *this;
}

inline CVector4& CVector4::operator -= (CVector4 &_v)
{
	x -= _v.x;
	y -= _v.y;
	z -= _v.z;
	w -= _v.w;
	return *this;
}

inline CVector4& CVector4::operator *= (CVector4 &_v)
{
	x *= _v.x;
	y *= _v.y;
	z *= _v.z;
	w *= _v.w;
	return *this;
}

/// TODO: sprawdziæ czy tu nie mo¿e byæ referencja zwracana:
inline CVector4 CVector4::operator -()
{
	return CVector4(-x, -y, -z, -w);
}

inline float CVector4::Dot(CVector4 &_other) const
{
	return x*_other.x + y*_other.y + z*_other.z + w*_other.w;
}

inline bool operator==(const CVector4 &a, const CVector4 &b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z &&
		a.w == b.w);
}

inline bool operator!=(const CVector4 &a, const CVector4 &b)
{
	return (a.x != b.x || a.y != b.y || a.z != b.z ||
		a.w != b.w);
}

inline CVector4 operator +(const float f, const CVector4 &v)
{
	return CVector4(f+v.x, f+v.y, f+v.z, f+v.w);
}

inline CVector4 operator *(const float r, const CVector4 &v)
{
	return CVector4(r*v.x, r*v.y, r*v.z, r*v.w);
}

inline CVector4 operator *(const CVector4 &v, const float r)
{
	return CVector4(r*v.x, r*v.y, r*v.z, r*v.w);
}

inline CVector4 operator /(const CVector4 &v, const float r)
{
	return CVector4(v.x/r, v.y/r, v.z/r, v.w/r);
}

inline CVector4 operator +(const CVector4 &v, const float f)
{
	return CVector4(v.x+f, v.y+f, v.z+f, v.w+f);
}

inline CVector4 operator -(const CVector4 &v, const float f)
{
	return CVector4(v.x-f, v.y-f, v.z-f, v.w-f);
}

#endif