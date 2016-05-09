
#ifndef __keyframer_hpp__
#define __keyframer_hpp__

////////////////////////////////////////////////////////////////////

class CKeyframer
{
public:
	/// ka¿dy keyframer jest zwi¹zany z jakimœ obiektem
	CObject			*m_pOwnerObject;

	/// keyframer posiada œcie¿ki za pomoc¹ których nastêpuje
	/// przekszta³cenie obiektu
	CRotationTrack		*m_pRotationTrack;
	CPositionTrack		*m_pPositionTrack;
	CScaleTrack		*m_pScaleTrack;
	CMorphTrack		*m_pMorphTrack;

	WORD			link, hierarchy;  /// identyfikator rodzica i pozycja w hierarchii
	CVector3		Pivot; 	    /// pivot point


	/// wskaŸniki na brata, dziecko i ojca:
	/// wszystkie keyframery (a co za tym idzie: wszystkie obiekty)
	/// tworz¹ drzewo
	CKeyframer		*m_pkBrother,
				*m_pkChild,
				*m_pkFather;

	CMatrix4x4		mtxTransformation;


	/// konstrukcja/destrukcja:
	CKeyframer();
	virtual ~CKeyframer();

	void Transform(float);

	/// oblicza wszystkie potrzebne wspó³czynniki:
	void PreCompute();

};

////////////////////////////////////////////////////////////////////

#endif