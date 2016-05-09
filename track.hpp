
///			CTrack
///		       /      \
///                  /          \
///                /              \
///              /                  \
///         CVectorTrack        CRotationTrack    CMorphTrack
///         /        \
///       /            \
///  CScaleTrack   CPositionTrack
///
///
///
///
///

#ifndef __track_hpp__
#define __track_hpp__

////////////////////////////////////////////////////////////////////

float ComputeEase(float, float, float);

////////////////////////////////////////////////////////////////////

/// klasa abstrakcyjna
class CTrack
{
public:
	/// typy œcie¿ek
	enum {
		SINGLE		= 0,
		UNKNOWN		= 1,
		REPEAT		= 2,
		LOOP		= 3
	};


	DWORD		nKeys;
	int		Type; /// SINGLE, REPEAT, LOOP

	/// zwraca macierz przekszta³cenia w okreœlonym punkcie czasu:
	virtual CMatrix4x4 GetTransformMtx(float) = 0;
};

////////////////////////////////////////////////////////////////////

class CVectorTrack : public CTrack
{
public:
	struct SVectorKey
	{
		CVector3	vec;

		/// nr klatki, w ktorej lezy ten klucz:
		float 		frame;

		/// wspó³czynniki
		float		easefrom;
		float		easeto;
		float		T, B, C;

		CVector3	ds; /// styczna wejœciowa
		CVector3	dd; /// styczna wyjœciowa
	};

	SVectorKey	*pKeys;

public:
	CVectorTrack();
	virtual ~CVectorTrack();

	/// trzeba wywo³aæ t¹ metodê zanim odtworzy siê animacjê:
	void ComputeCoefficients();
	CVector3 GetTransformedVector(float);
};

////////////////////////////////////////////////////////////////////

class CScaleTrack : public CVectorTrack
{
public:

	virtual CMatrix4x4 GetTransformMtx(float);
};

////////////////////////////////////////////////////////////////////

class CPositionTrack : public CVectorTrack
{
public:
	virtual CMatrix4x4 GetTransformMtx(float);
};

////////////////////////////////////////////////////////////////////

class CRotationTrack : public CTrack
{
public:

	struct SQuatKey
	{
		CQuaternion	quat;

		/// dane z pliku .3DS:
		CVector3	axis;
		float		angle;

		/// nr klatki, w ktorej lezy ten klucz:
		float 		frame;

		/// wspó³czynniki
		float		easefrom;
		float		easeto;
		float		T, B, C;

		long		spin;
		CQuaternion 
			a,  // incomming tangent
			b;  // outgoing tangent

	};

	SQuatKey	*pKeys;
	SQuatKey	*last_key;
	float		first_frame, last_frame;

	void Interpolate01(float, CQuaternion &);  // single key
	void Interpolate02(float, CQuaternion &);  // standard
	void Interpolate03(float, CQuaternion &);  // extra spins
	void (CRotationTrack ::* InterpolateOffset)(float, CQuaternion &);

public:
	CRotationTrack();
	virtual ~CRotationTrack();
	virtual CMatrix4x4 GetTransformMtx(float);
	void CompAB(SQuatKey *prev, SQuatKey *cur, SQuatKey *next);

	void ComputeCoefficients();
};

////////////////////////////////////////////////////////////////////

/// TODO: Jeœli kiedykolwiek napiszê morphing to bêdzie _cud_!
class CMorphTrack : public CTrack
{
public:
	struct SObjectKey
	{
		char		*name;

		/// nr klatki, w ktorej lezy ten klucz:
		float 		frame;

		/// wspó³czynniki
		float		easefrom;
		float		easeto;
		float		T, B, C;
	};

	SObjectKey	*pKeys;

public:	
	CMorphTrack();
	virtual ~CMorphTrack();
	virtual CMatrix4x4 GetTransformMtx(float);
};


////////////////////////////////////////////////////////////////////

#endif

