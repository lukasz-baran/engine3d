

#ifndef __quaternion_hpp__
#define __quaternion_hpp__

///>====================================----------------------------
///
///	d e f i n i c j a
///
///>====================================----------------------------

///>====================================----------------------------
///	Kwaternion:
///		q = w + x*i + y*j + z*k
///			i, j, k - jednoœci urojone
///
///	Mno¿enie dwóch kwaternionów nie jest przemienne.
///>====================================----------------------------

class CQuaternion
{
public:
	float w, x, y, z;

	CQuaternion();
	CQuaternion(const CQuaternion &);
	CQuaternion(float, float, float, float);
	~CQuaternion() { }

        /// dodawanie i odejmowanie dwóch kwaterionów:
        CQuaternion operator+ (CQuaternion &) const;
        CQuaternion operator- (CQuaternion &) const;
        /// mno¿enie dwóch kwaternionów (dzia³anie nie przemienne):
        CQuaternion operator* (CQuaternion &) const;
	/// mno¿enie kwaterniona przez skalar:
	friend CQuaternion operator *(const float, const CQuaternion &);
	friend CQuaternion operator *(const CQuaternion &, const float);

	/// kwaternion przeciwny:
        CQuaternion operator -() const;

        /// norma kwaterniona:
        float Norm() const;

	/// modu³ kwaterniona:
	float Mod() const;

	/// tworzy kwaternion jednostkowy:
	CQuaternion Unit() const;

        /// utworzenie kwaterniona z notacji (oœ, k¹t)
        void FromAxisAngle(const CVector3 &, const float);


	/// dot-product (iloczyn skalarny :) dwóch kwaternionów:
	friend float Dot(const CQuaternion &, const CQuaternion &);

	friend CQuaternion Exp(const CQuaternion &);
	
	/// spherical linear interpolation:
	friend void SLERP(CQuaternion &, CQuaternion &, float t, CQuaternion &);

	friend ostream& operator<<(ostream &_out, CQuaternion &_vec);

};

////////////////////////////////////////////////////////////////////

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

inline CQuaternion::CQuaternion() :
w(0.0f), x(0.0f), y(0.0f), z(0.0f)
{
}

inline CQuaternion::CQuaternion(const CQuaternion &other) :
w(other.w), x(other.x), y(other.y), z(other.z)
{
}

inline CQuaternion::CQuaternion(float _w, float _x, float _y, float _z) :
w(_w), x(_x), y(_y), z(_z)
{
}

inline CQuaternion CQuaternion::operator+ (CQuaternion &q) const
{
	return CQuaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

inline CQuaternion CQuaternion::operator- (CQuaternion &q) const
{
	return CQuaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

inline CQuaternion CQuaternion::operator* (CQuaternion &q) const
{
	return CQuaternion(w*q.w - x*q.x - y*q.y - z*q.z,
		w*q.x + x*q.w + y*q.z - z*q.y,
		w*q.y - x*q.z + y*q.w + z*q.x,
		w*q.z + x*q.y - y*q.x + z*q.w);
}

inline CQuaternion operator *(const float f, const CQuaternion &q)
{
	return CQuaternion(q.w*f, q.x*f, q.y*f, q.z*f);
}

inline CQuaternion operator *(const CQuaternion &q, const float f)
{
	return CQuaternion(q.w*f, q.x*f, q.y*f, q.z*f);
}

inline CQuaternion CQuaternion::operator -() const
{
	return CQuaternion(w, -x, -y, -z);
}

inline float CQuaternion::Norm() const
{
	return w*w + x*x + y*y + z*z;
}

inline float CQuaternion::Mod() const
{
	float d;
	d = sqrtf(Norm());
	if (d == 0.0f) 
		d = 1.0f;
	return d;
}

inline CQuaternion CQuaternion::Unit() const
{
	float s;
	s = 1.0f/Mod();
	return s * (*this);
}

inline float Dot(CQuaternion &q1, CQuaternion &q2)
{
	return (q1.w*q2.w) + (q1.x*q2.x) + (q1.y*q2.y) + (q1.z*q2.z);
}

#endif