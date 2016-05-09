
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

float ComputeEase(float t, float a, float b)
{
	float		k;
	float		s = a+b;

	if (s == 0.0f)
		return t;

	if (s > 1.0f) {
		a = a/s;
		b = b/s;
	}

	k = 1.0f/(2.0f-a-b);

	if (t < a)
		return ((k/a)*t*t);
	else {
		if (t < 1.0f-b)	{
			return (k*(2*t - a));
		} else {
			t = 1.0f-t;
			return (1.0f-(k/b)*t*t);
		}
	}
}

////////////////////////////////////////////////////////////////////

CVectorTrack::CVectorTrack() : pKeys(NULL) 
{ 
}

////////////////////////////////////////////////////////////////////

CVectorTrack::~CVectorTrack() 
{
	DELETE_ARRAY(pKeys)
}

////////////////////////////////////////////////////////////////////

///>====================================----------------------------
/// method:	   CVectorTrack::ComputeCoefficients
/// description:   Oblicza wspó³czynniki 'ds' (styczna wejœciowa) i
///                'dd' (styczna wyjœciowa) na podstawie parametrów
///                'tension', 'bias', 'continuity' a tak¿e punktów
///                kontrolnych
///>====================================----------------------------
void CVectorTrack::ComputeCoefficients()
{
	if (nKeys <= 1)	return;

	/// teraz wiemy, ¿e w tablicy 'pKeys' s¹ co najmniej
	/// dwa klucze...

	DWORD		i;
	float		ds_coeff1, ds_coeff2;
	float		dd_coeff1, dd_coeff2;

	float		delta1, delta2;


	delta1 = .5f;
	delta2 = .5f;

	/// obliczenie 'ds' i 'dd' dla indeksu pKeys[0]:
	ds_coeff1 = (1.0f - pKeys[0].T)*(1.0f - pKeys[0].C)*(1.0f + pKeys[0].B)*delta1;
	ds_coeff2 = (1.0f - pKeys[0].T)*(1.0f + pKeys[0].C)*(1.0f - pKeys[0].B)*delta1;
	pKeys[0].ds = ds_coeff1*(pKeys[1].vec - pKeys[0].vec) +
		ds_coeff2*(pKeys[1].vec - pKeys[0].vec);

	dd_coeff1 = (1.0f - pKeys[0].T)*(1.0f + pKeys[0].C)*(1.0f + pKeys[0].B)*delta2;
	dd_coeff2 = (1.0f - pKeys[0].T)*(1.0f - pKeys[0].C)*(1.0f - pKeys[0].B)*delta2;
	pKeys[0].dd = dd_coeff1*(pKeys[1].vec - pKeys[0].vec) +
		dd_coeff2*(pKeys[1].vec - pKeys[0].vec);

	/// obliczenie 'ds' i 'dd' dla indeksu pKeys[n-1]:
	ds_coeff1 = (1.0f - pKeys[nKeys-1].T)*(1.0f - pKeys[nKeys-1].C)*(1.0f + pKeys[nKeys-1].B)*delta1;
	ds_coeff2 = (1.0f - pKeys[nKeys-1].T)*(1.0f + pKeys[nKeys-1].C)*(1.0f - pKeys[nKeys-1].B)*delta1;
	pKeys[nKeys-1].ds = ds_coeff1*(pKeys[nKeys-1].vec - pKeys[nKeys-2].vec) +
		ds_coeff2*(pKeys[nKeys-1].vec - pKeys[nKeys-2].vec);

	dd_coeff1 = (1.0f - pKeys[nKeys-1].T)*(1.0f + pKeys[nKeys-1].C)*(1.0f + pKeys[nKeys-1].B)*delta2;
	dd_coeff2 = (1.0f - pKeys[nKeys-1].T)*(1.0f - pKeys[nKeys-1].C)*(1.0f - pKeys[nKeys-1].B)*delta2;
	pKeys[nKeys-1].dd = dd_coeff1*(pKeys[nKeys-1].vec - pKeys[nKeys-2].vec) +
		dd_coeff2*(pKeys[nKeys-1].vec - pKeys[nKeys-2].vec);


	/// obliczenia dla pozosta³ych kluczy:
	for (i = 1; i < nKeys-1; i++) {

		delta1 = (pKeys[i+1].frame - pKeys[i].frame)/
			(pKeys[i+1].frame - pKeys[i-1].frame);
		delta2 = (pKeys[i].frame - pKeys[i-1].frame)/
			(pKeys[i+1].frame - pKeys[i-1].frame);

		ds_coeff1 = (1.0f - pKeys[i].T)*(1.0f - pKeys[i].C)*(1.0f + pKeys[i].B)*delta1;
		ds_coeff2 = (1.0f - pKeys[i].T)*(1.0f + pKeys[i].C)*(1.0f - pKeys[i].B)*delta1;
		pKeys[i].ds = ds_coeff1*(pKeys[i].vec - pKeys[i-1].vec) +
			ds_coeff2*(pKeys[i+1].vec - pKeys[i].vec);

		dd_coeff1 = (1.0f - pKeys[i].T)*(1.0f + pKeys[i].C)*(1.0f + pKeys[i].B)*delta2;
		dd_coeff2 = (1.0f - pKeys[i].T)*(1.0f - pKeys[i].C)*(1.0f - pKeys[i].B)*delta2;
		pKeys[i].dd = dd_coeff1*(pKeys[i].vec - pKeys[i-1].vec) +
			dd_coeff2*(pKeys[i+1].vec - pKeys[i].vec);

	}
}

////////////////////////////////////////////////////////////////////

CVector3 CVectorTrack::GetTransformedVector(float time)
{
	CMatrix4x4	mtx(CMatrix4x4::INIT_IDENTITY);
	CVector3	v;
	CVector4	h;
	DWORD		i;

	if ((nKeys <= 1) || (time < pKeys[0].frame)) 
		return pKeys[0].vec;

	if (time > pKeys[nKeys - 1].frame) 
		return pKeys[nKeys-1].vec;

	for (i = 0; i < nKeys-1  && pKeys[i+1].frame < time; i++);

	time = (time - pKeys[i].frame)/(pKeys[i+1].frame - pKeys[i].frame);
	time = ComputeEase(time, pKeys[i].easefrom, pKeys[i].easeto);

	float time2 = time*time,
		time3 = time*time2;

	h.x = 2*time3 - 3*time2 + 1;
	h.y = -2*time3 + 3*time2;
	h.z = time3 - 2*time2 + time;
	h.w = time3 - time2;

	v = h[0]*pKeys[i].vec + h[1]*pKeys[i+1].vec +
		h[2]*pKeys[i].ds + h[3]*pKeys[i+1].dd;

	return v;
}

////////////////////////////////////////////////////////////////////

CMatrix4x4 CPositionTrack::GetTransformMtx(float time)
{
	CMatrix4x4 mtx;
	mtx.TranslationMtx(CVectorTrack::GetTransformedVector(time));
	return mtx;
}


////////////////////////////////////////////////////////////////////

CMatrix4x4 CScaleTrack::GetTransformMtx(float time)
{
	CMatrix4x4 mtx;
	mtx.ScaleMtx(CVectorTrack::GetTransformedVector(time));
	return mtx;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

CRotationTrack::CRotationTrack() : pKeys(NULL) 
{ 
}

////////////////////////////////////////////////////////////////////

CRotationTrack::~CRotationTrack()
{
	DELETE_ARRAY(pKeys)
}


////////////////////////////////////////////////////////////////////

void CRotationTrack::CompAB(SQuatKey *prev, SQuatKey *cur, SQuatKey *next)
{
	CQuaternion	v_in,v_out;
	float		log_angle;

	if (prev != NULL) {
		if (cur->spin == 0) {
			log_angle = cur->angle*0.5f;
			if (log_angle >= MATH3D_PI_2) 
				log_angle -= MATH3D_PI;
		} else 
			log_angle = 0.5f * MATH3D_PI;
		v_in.w = 0.0f;
		v_in.x = cur->axis.x * log_angle;
		v_in.y = cur->axis.y * log_angle;
		v_in.z = cur->axis.z * log_angle;
	};

	if (next != NULL) {
		if (next->spin == 0) {
			log_angle = next->angle*0.5f;
			if (log_angle >= MATH3D_PI_2) 
				log_angle -= MATH3D_PI;
		} else 
			log_angle = 0.5f * MATH3D_PI;
		v_out.w = 0.0f;
		v_out.x = next->axis.x * log_angle;
		v_out.y = next->axis.y * log_angle;
		v_out.z = next->axis.z * log_angle;
	};

	if (prev == NULL) 
		v_in = v_out;
	if (next == NULL) 
		v_out = v_in;

	float		fp, fn, cm = 1.0f - cur->C;

	if ((prev != NULL) && (next != NULL)) {
		float	dt=2.0f/(next->frame - prev->frame);
		fp = (cur->frame-prev->frame)*dt;
		fn = (next->frame-cur->frame)*dt;

		float	c=fabsf(cur->C);
		fp = fp+c-c*fp;
		fn = fn+c-c*fn;
	} else 
		fp = fn = 1.0f;

	float tm = 0.5f*(1.0f - cur->T);
	float cp = 2.0f - cm;
	float bm = 1.0f - cur->B;
	float bp = 2.0f - bm;
	float tmcm = tm * cm;
	float tmcp = tm * cp;
	float ksm  = 1.0f - tmcm * bp * fp;
	float ksp  = -tmcp * bm * fp;
	float kdm  = tmcp * bp * fn;
	float kdp  = tmcm * bm * fn - 1.0f;

	CQuaternion	arg0, arg1;
	arg0 = v_in * kdm;
	arg1 = v_out * kdp;
	v_in = v_in * ksm;
	v_out = v_out * ksp;

	arg0 = arg0 + arg1;
	arg1 = v_out + v_in;
	arg0 = 0.5f * arg0;
	arg1 = 0.5f * arg0;

	arg0 = Exp(arg0);
	arg1 = Exp(arg1);

	cur->b = cur->quat * arg0;
	cur->a = cur->quat * arg1;

/*	QuatMul(kdm,&v_in,&arg0);
	QuatMul(kdp,&v_out,&arg1);
	QuatMul(ksm,&v_in,&v_in);
	QuatMul(ksp,&v_out,&v_out);*/
/*	QuatAdd(&arg0,&arg1,&arg0);
	QuatAdd(&v_in,&v_out,&arg1);
	QuatMul(0.5f,&arg0,&arg0);
	QuatMul(0.5f,&arg1,&arg1);*/
/*	QuatExp(&arg0,&arg0);
	QuatExp(&arg1,&arg1);
	QuatMul(&cur->q,&arg0,&cur->b);
	QuatMul(&cur->q,&arg1,&cur->a);*/
};

////////////////////////////////////////////////////////////////////

void CRotationTrack::ComputeCoefficients()
{
	DWORD		i;

	for (i = 0; i < nKeys; i++) {
		if (i == 0)
			first_frame = pKeys[0].frame;
		pKeys[i].frame -= first_frame;
	}
	last_frame = pKeys[nKeys-1].frame;
	last_key = pKeys + nKeys - 1;

	
	/// wszystkie k¹ty musz¹ byæ dodatnie:
	for (i = 0; i < nKeys; i++)
		if (pKeys[i].angle < 0.0f) {
			pKeys[i].angle = -pKeys[i].angle;
			pKeys[i].axis = -pKeys[i].axis;
		}

	/// utwórz pierwszy kwaternion:
	pKeys[0].quat.FromAxisAngle(pKeys[0].axis, pKeys[0].angle);
	
	for (i = 1; i < nKeys; i++) {

		// calculate spin value and modify the angle so it feets
		// <0,2*PI) boundaries
		pKeys[i].spin = (long)floorf(pKeys[i].angle / MATH3D_2PI);
		pKeys[i].angle -= pKeys[i].spin * MATH3D_2PI;

		// convert it to quaternion...
		pKeys[i].quat.FromAxisAngle(pKeys[i].axis, pKeys[i].angle);

		// ...and now to absolute rotation
		pKeys[i].quat = pKeys[i-1].quat * pKeys[i].quat;
	};

	if (nKeys == 1) 
		InterpolateOffset = Interpolate01;
	else {
		// middle-keys tangents calculation
		for (i = 1; i < (nKeys-1); i++)
			CompAB(&pKeys[i-1], &pKeys[i], &pKeys[i+1]);

/*		if ((flags&3)==3)
		{
		// loop track
			S3DSRotKey mkey;
			memcpy(&mkey,&track[1],sizeof(S3DSRotKey));
			mkey.frame=track[keys-1].frame+(track[1].frame-track->frame);
			CompAB(&track[keys-2],&track[keys-1],&mkey);
			QuatMul(-1.0f,&track[keys-1].b,&track->b);
		} else
		{*/
		// single and repeat track
		CompAB(NULL, pKeys, &pKeys[1]);
		CompAB(&pKeys[nKeys - 2], &pKeys[nKeys-1], NULL);
/*		};*/

		// check, if simple interpolation is enough
		bool	simple = true;
		for (i = 1; i < nKeys; i++)
			if ((pKeys[i].spin) || (pKeys[i].angle>6.265732)) 
				simple = false; 

		if (simple) 
			InterpolateOffset = Interpolate02;
		else 
			InterpolateOffset = Interpolate03;

	}

}

////////////////////////////////////////////////////////////////////

CMatrix4x4 CRotationTrack::GetTransformMtx(float time)
{
	CMatrix4x4	mtx;
	CQuaternion	q;

	(this->*InterpolateOffset)(time, q);
	mtx.RotationMtx(q);
	return mtx;
}

////////////////////////////////////////////////////////////////////

void CRotationTrack::Interpolate01(float, CQuaternion &q)
{
	// single-key track doesn't need interpolation...
	q = pKeys[0].quat;
}

////////////////////////////////////////////////////////////////////

void CRotationTrack::Interpolate02(float frame, CQuaternion &q)
{
	// the simple-case interpolation
	CRotationTrack::SQuatKey	*cur,*next;
	float				t;
	CQuaternion p1,q1;
	frame -= first_frame;
/*	if(flags&3) 
		frame=fmodf(frame,last_frame); 
	else
	{*/
	if (frame < 0.0f) {
		q = pKeys[0].quat;
		return;
	} else
		if (frame > last_frame) {
			q = pKeys[nKeys-1].quat;
			return;
		}
/*	};*/

	if (frame < last_key->frame)
		last_key = pKeys;

	while (last_key[1].frame < frame) 
		last_key++;

	cur = last_key;
	next = &cur[1];
	t = (frame-cur->frame)/(next->frame-cur->frame);

	t = ComputeEase(t, cur->easefrom, next->easeto);

	SLERP(cur->quat, next->quat, t,		p1);
	SLERP(cur->quat, next->quat, t,		p1);
	SLERP(cur->b,    next->a,    t,		q1);
	SLERP(p1,        q1,         2.0f*t*(1.0f-t), q);
}

////////////////////////////////////////////////////////////////////

void CRotationTrack::Interpolate03(float frame, CQuaternion &q)
{
	SQuatKey		*cur,*next;
	float			t;
	CQuaternion		p1, q1;

	frame -= first_frame;

	if (frame < 0.0f) {
		q = pKeys[0].quat;
		return;
	} else
		if(frame > last_frame) {
			q = pKeys[nKeys - 1].quat;
			return;
		}

	if (frame < last_key->frame) 
		last_key = pKeys;

	while (last_key[1].frame < frame) 
		last_key++;

	cur = last_key;
	next = &cur[1];
	t = (frame - cur->frame)/(next->frame - cur->frame);

	t = ComputeEase(t, cur->easefrom, next->easeto);

	float angle = next->angle + ((float)next->spin)*MATH3D_2PI;

	if (!next->spin) {
		p1.FromAxisAngle(next->axis, t*angle);
		p1 = p1 * cur->quat;
		SLERP(cur->b, next->a, t, q1);
	} else {
		float	_p0, _p1;

		_p0 = MATH3D_PI/angle;
		_p1 = 1.0f - _p0;


//		if (t < _p0) {
			CQuaternion s0;
			s0.FromAxisAngle(next->axis, _p0*angle);
			s0 = s0 * cur->quat;

			t *= 1.0f/_p0;
			SLERP(cur->quat, s0, t, p1);
			SLERP(cur->b, s0, t, q1);
/*		} else
			if (t<=_p1) {
				t -= _p0;
				t /= _p1 - _p0;

				p1.FromAxisAngle(next->axis, fmodf(MATH3D_PI + t*(angle-MATH3D_2PI), MATH3D_2PI));
				q = cur->quat * p1;
				return;
			} else {
				CQuaternion	s1;
				s1.FromAxisAngle(next->axis, _p1*angle);
				s1 = cur->quat * s1;

				if (next->spin & 1) 
					s1 = -1.0f * s1;

				t -= _p1;
				t *= 1.0f/_p0;
				SLERP(s1, next->quat, t, p1);
				SLERP(s1, next->a   , t, q1);
			}*/
	}
	SLERP(p1, q1, 2.0f*t*(1.0f-t), q);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

CMorphTrack::CMorphTrack() : pKeys(NULL) 
{ 
}

////////////////////////////////////////////////////////////////////

CMorphTrack::~CMorphTrack()
{
	DELETE_ARRAY(pKeys)
}

////////////////////////////////////////////////////////////////////

CMatrix4x4 CMorphTrack::GetTransformMtx(float time)
{
	return CMatrix4x4(CMatrix4x4::INIT_IDENTITY);
}

////////////////////////////////////////////////////////////////////


