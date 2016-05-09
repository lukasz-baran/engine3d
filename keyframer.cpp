
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

CKeyframer::CKeyframer() : m_pOwnerObject(NULL),
	m_pRotationTrack(NULL),
	m_pPositionTrack(NULL),
	m_pScaleTrack(NULL),
	m_pMorphTrack(NULL),
	link(0),
	hierarchy(0)
{
}

////////////////////////////////////////////////////////////////////

CKeyframer::~CKeyframer()
{
	RELEASE(m_pRotationTrack);
	RELEASE(m_pPositionTrack);
	RELEASE(m_pScaleTrack);
	RELEASE(m_pMorphTrack);
}

////////////////////////////////////////////////////////////////////

void CKeyframer::Transform(float time)
{
	CMatrix4x4	mtxPivot;
	

	/// ta macierz zapamiêtuje transformacje obiektu:
	mtxTransformation.Identity();

	if (m_pkFather)
		mtxTransformation *= m_pkFather->mtxTransformation;

	if (m_pPositionTrack) {
		mtxTransformation *= m_pPositionTrack->GetTransformMtx(time);
	}

	if (m_pRotationTrack) {
		mtxTransformation *= m_pRotationTrack->GetTransformMtx(time);
	}

	if (m_pScaleTrack) {
		mtxTransformation *= m_pScaleTrack->GetTransformMtx(time);
	}

	if (m_pOwnerObject) {
		m_pOwnerObject->UpdateFrame(this);
	}

	if (m_pkBrother) 
		m_pkBrother->Transform(time);
	
	if (m_pkChild) 
		m_pkChild->Transform(time);

}

////////////////////////////////////////////////////////////////////

void CKeyframer::PreCompute()
{
	if (m_pPositionTrack != NULL)
		m_pPositionTrack->ComputeCoefficients();

	if (m_pRotationTrack != NULL)
		m_pRotationTrack->ComputeCoefficients();

	if (m_pScaleTrack != NULL)
		m_pScaleTrack->ComputeCoefficients();
}


