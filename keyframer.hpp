
#ifndef __keyframer_hpp__
#define __keyframer_hpp__

////////////////////////////////////////////////////////////////////

class CKeyframer
{
public:
	/// ka�dy keyframer jest zwi�zany z jakim� obiektem
	CObject			*m_pOwnerObject;

	/// keyframer posiada �cie�ki za pomoc� kt�rych nast�puje
	/// przekszta�cenie obiektu
	CRotationTrack		*m_pRotationTrack;
	CPositionTrack		*m_pPositionTrack;
	CScaleTrack		*m_pScaleTrack;
	CMorphTrack		*m_pMorphTrack;

	WORD			link, hierarchy;  /// identyfikator rodzica i pozycja w hierarchii
	CVector3		Pivot; 	    /// pivot point


	/// wska�niki na brata, dziecko i ojca:
	/// wszystkie keyframery (a co za tym idzie: wszystkie obiekty)
	/// tworz� drzewo
	CKeyframer		*m_pkBrother,
				*m_pkChild,
				*m_pkFather;

	CMatrix4x4		mtxTransformation;


	/// konstrukcja/destrukcja:
	CKeyframer();
	virtual ~CKeyframer();

	void Transform(float);

	/// oblicza wszystkie potrzebne wsp�czynniki:
	void PreCompute();

};

////////////////////////////////////////////////////////////////////

#endif