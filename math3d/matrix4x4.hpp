
#ifndef __matrix4x4_hpp__
#define __matrix4x4_hpp__

///>====================================----------------------------
///
///	d e f i n i c j a
///
///>====================================----------------------------

class CMatrix4x4
{
public:

	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float _mat_1D[16];
		float _mat_2D[4][4];
	};

	enum {
		INIT_ZEROS,
		INIT_ONES,
		INIT_IDENTITY
	};

	CMatrix4x4(int = CMatrix4x4::INIT_ZEROS);
	CMatrix4x4(float, float, float, float,
			float, float, float, float,
			float, float, float, float,
			float, float, float, float);
	CMatrix4x4(const CMatrix4x4 &);

	~CMatrix4x4() { }


	/// dostêp do wartoœci macierzy:
	float& operator() (const int, const int);
	const float& operator() (const int, const int) const;

	CVector4 GetRow(const int) const;
	CVector4 GetColumn(const int) const;

	void CameraMtx(CVector3 &, CVector3 &, float);

	/// tworz¹ macierze translacji, skalowania, rotacji:
	void TranslationMtx(CVector3 &);
	void ScaleMtx(CVector3 &);
	void RotationMtx(const CQuaternion &);

	/// transpozycja, macierz jednostkowa..:
	void Transpose();
	void Identity();
	void Zeros();
	void Ones();

	/// operatory matematyczne:
	CMatrix4x4 operator -();

	/// inne macierze:
	CMatrix4x4& operator+=(const CMatrix4x4 &);
	CMatrix4x4& operator-=(const CMatrix4x4 &);
	CMatrix4x4& operator*=(const CMatrix4x4 &);
	CMatrix4x4 operator+(const CMatrix4x4 &) const;
	CMatrix4x4 operator-(const CMatrix4x4 &) const;
	CMatrix4x4 operator*(const CMatrix4x4 &) const;

	/// floaty:
	CMatrix4x4& operator+=(const float);
	CMatrix4x4& operator-=(const float);
	CMatrix4x4& operator*=(const float);
	CMatrix4x4& operator/=(const float);
	friend CMatrix4x4 operator+(const CMatrix4x4 &, const float);
	friend CMatrix4x4 operator-(const CMatrix4x4 &, const float);
	friend CMatrix4x4 operator*(const CMatrix4x4 &, const float);
	friend CMatrix4x4 operator/(const CMatrix4x4 &, const float);
	friend CMatrix4x4 operator+(const float, const CMatrix4x4 &);
	friend CMatrix4x4 operator*(const float, const CMatrix4x4 &);

	/// wektory:
	friend CVector4 operator*(const CMatrix4x4 &, const CVector4 &);
	friend CVector4 operator*(const CVector4 &, const CMatrix4x4 &);

	/// porównywanie macierzy:
	friend bool operator!=(const CMatrix4x4 &, const CMatrix4x4 &);
	friend bool operator==(const CMatrix4x4 &, const CMatrix4x4 &);

	CMatrix4x4& operator=(const CMatrix4x4 &);

	friend ostream& operator<<(ostream &, CMatrix4x4 &);

	friend CMatrix4x4 InverseMtx(const CMatrix4x4 &);
	friend bool FastInverseMtx(CMatrix4x4 &m);

};

////////////////////////////////////////////////////////////////////

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

inline CMatrix4x4::CMatrix4x4(int _type)
{
	if (_type == CMatrix4x4::INIT_ONES)
		Ones();
	else {
		Zeros();

		if (_type == CMatrix4x4::INIT_IDENTITY)
			_11 = _22 = _33 = _44 = 1.0f;
	}
}

inline CMatrix4x4::CMatrix4x4(float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p) :
_11(a), _12(b), _13(c), _14(d),
_21(e), _22(f), _23(g), _24(h),
_31(i), _32(j), _33(k), _34(l),
_41(m), _42(n), _43(o), _44(p)
{
}

inline CMatrix4x4::CMatrix4x4(const CMatrix4x4 &_other)
{
	*this = _other;
}

inline void CMatrix4x4::TranslationMtx(CVector3 &v)
{
	Identity();
	_14 = v.x;
	_24 = v.y;
	_34 = v.z;
}

inline void CMatrix4x4::ScaleMtx(CVector3 &v)
{
	Zeros();
	_11 = v.x;
	_22 = v.y;
	_33 = v.z;
	_44 = 1.0f;
}

inline void CMatrix4x4::Transpose()
{
	float tmp;
	tmp = _12; _12 = _21; _21 = tmp;
	tmp = _13; _13 = _31; _31 = tmp;
	tmp = _14; _14 = _41; _41 = tmp;
	tmp = _24; _24 = _42; _42 = tmp;
	tmp = _34; _34 = _43; _43 = tmp;
	tmp = _23; _23 = _32; _32 = tmp;
}

inline void CMatrix4x4::Identity()
{
	Zeros();
	_11 = _22 = _33 = _44 = 1.0f;
}

inline void CMatrix4x4::Zeros()
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] = 0.0f;
}

inline void CMatrix4x4::Ones()
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] = 1.0f;
}

inline float& CMatrix4x4::operator() (const int i, const int j)
{
	return _mat_2D[i][j];
}

inline CVector4 CMatrix4x4::GetRow(const int _row) const
{
	if (_row < 0 || _row > 3)
		throw _row;
	return CVector4(
		_mat_2D[_row][0],
		_mat_2D[_row][1],
		_mat_2D[_row][2],
		_mat_2D[_row][3]);
}

inline CVector4 CMatrix4x4::GetColumn(const int _col) const
{
	if (_col < 0 || _col > 3)
		throw _col;
	return CVector4(
		_mat_2D[0][_col],
		_mat_2D[1][_col],
		_mat_2D[2][_col],
		_mat_2D[3][_col]);
}

inline CMatrix4x4 CMatrix4x4::operator -()
{
	return CMatrix4x4(-_11, -_12, -_13, -_14,
			-_21, -_22, -_23, -_24,
			-_31, -_32, -_33, -_34,
			-_41, -_42, -_43, -_44);
}

inline CMatrix4x4& CMatrix4x4::operator+= (const CMatrix4x4 &_other)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] += _other._mat_1D[i];
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator-= (const CMatrix4x4 &_other)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] -= _other._mat_1D[i];
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator*= (const CMatrix4x4 &_other)
{
	CMatrix4x4 ret;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			float sum = 0.0f;
			for (int k = 0; k < 4; k++)
				sum += (_mat_2D[i][k] * _other._mat_2D[k][j]);
			ret(i, j) = sum;
		}
	memcpy((void *)_mat_1D, (void *)ret._mat_1D, 16*sizeof(float));
	return *this;
}

inline CMatrix4x4 CMatrix4x4::operator +(const CMatrix4x4 &_m) const
{
	CMatrix4x4 ret;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			ret(i, j) = _mat_2D[i][j] + _m._mat_2D[i][j];
	return ret;
}

inline CMatrix4x4 CMatrix4x4::operator -(const CMatrix4x4 &_m) const
{
	CMatrix4x4 ret;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			ret(i, j) = _mat_2D[i][j] - _m._mat_2D[i][j];
	return ret;
}

inline CMatrix4x4 CMatrix4x4::operator *(const CMatrix4x4 &_m) const
{
	CMatrix4x4 ret;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++) {
			float sum = 0.0f;
			for (int k = 0; k < 4; k++)
				sum += (_mat_2D[i][k] * _m._mat_2D[k][j]);
			ret(i, j) = sum;
		}
	return ret;
}

inline CMatrix4x4& CMatrix4x4::operator+= (const float f)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] += f;
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator-= (const float f)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] -= f;
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator*= (const float f)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] *= f;
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator/= (const float f)
{
	for (int i = 0; i < 16; i++)
		_mat_1D[i] /= f;
	return *this;
}

inline CMatrix4x4& CMatrix4x4::operator= (const CMatrix4x4 &_other)
{
	if (this != &_other)
		memcpy((void *)_mat_1D, (void *)_other._mat_1D, 16*sizeof(float));
	return *this;
}

inline CMatrix4x4 operator +(const CMatrix4x4 &m, const float f)
{
	return CMatrix4x4(
		m._11+f, m._12+f, m._13+f, m._14+f,
		m._21+f, m._22+f, m._23+f, m._24+f,
		m._31+f, m._32+f, m._33+f, m._34+f,
		m._41+f, m._42+f, m._43+f, m._44+f);
}

inline CMatrix4x4 operator -(const CMatrix4x4 &m, const float f)
{
	return CMatrix4x4(
		m._11-f, m._12-f, m._13-f, m._14-f,
		m._21-f, m._22-f, m._23-f, m._24-f,
		m._31-f, m._32-f, m._33-f, m._34-f,
		m._41-f, m._42-f, m._43-f, m._44-f);
}

inline CMatrix4x4 operator *(const CMatrix4x4 &m, const float f)
{
	return CMatrix4x4(
		m._11*f, m._12*f, m._13*f, m._14*f,
		m._21*f, m._22*f, m._23*f, m._24*f,
		m._31*f, m._32*f, m._33*f, m._34*f,
		m._41*f, m._42*f, m._43*f, m._44*f);
}

inline CMatrix4x4 operator /(const CMatrix4x4 &m, const float f)
{
	return CMatrix4x4(
		m._11/f, m._12/f, m._13/f, m._14/f,
		m._21/f, m._22/f, m._23/f, m._24/f,
		m._31/f, m._32/f, m._33/f, m._34/f,
		m._41/f, m._42/f, m._43/f, m._44/f);
}

inline CMatrix4x4 operator +(const float f, const CMatrix4x4 &m)
{
	return CMatrix4x4(
		m._11+f, m._12+f, m._13+f, m._14+f,
		m._21+f, m._22+f, m._23+f, m._24+f,
		m._31+f, m._32+f, m._33+f, m._34+f,
		m._41+f, m._42+f, m._43+f, m._44+f);
}

inline CMatrix4x4 operator *(const float f, const CMatrix4x4 &m)
{
	return CMatrix4x4(
		m._11*f, m._12*f, m._13*f, m._14*f,
		m._21*f, m._22*f, m._23*f, m._24*f,
		m._31*f, m._32*f, m._33*f, m._34*f,
		m._41*f, m._42*f, m._43*f, m._44*f);
}

inline CVector4 operator *(const CMatrix4x4 &m, const CVector4 &v)
{
	return CVector4(
		m._11*v.x + m._12*v.y + m._13*v.z + m._14*v.w,
		m._21*v.x + m._22*v.y + m._23*v.z + m._24*v.w,
		m._31*v.x + m._32*v.y + m._33*v.z + m._34*v.w,
		m._41*v.x + m._42*v.y + m._43*v.z + m._44*v.w);
}

inline CVector4 operator *(const CVector4 &v, const CMatrix4x4 &m)
{
	return CVector4(
		m._11*v.x + m._12*v.y + m._13*v.z + m._14*v.w,
		m._21*v.x + m._22*v.y + m._23*v.z + m._24*v.w,
		m._31*v.x + m._32*v.y + m._33*v.z + m._34*v.w,
		m._41*v.x + m._42*v.y + m._43*v.z + m._44*v.w);
}

#endif
