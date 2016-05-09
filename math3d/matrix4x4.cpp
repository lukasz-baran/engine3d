
#include "math3d.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

/// UWAGA! Dzia³anie tej funkcji nie zosta³o sprawdzone:
CMatrix4x4 InverseMtx(const CMatrix4x4& mtx)
{
	CMatrix4x4	out;

	float fDetInv = 1.0f / ( mtx._11*( mtx._22*mtx._33 - mtx._23*mtx._32 ) -
                             mtx._12*( mtx._21*mtx._33 - mtx._23*mtx._31 ) +
                             mtx._13*( mtx._21*mtx._32 - mtx._22*mtx._31 ) );

	out._11 =  fDetInv*( mtx._22*mtx._33 - mtx._23*mtx._32 );
	out._12 = -fDetInv*( mtx._12*mtx._33 - mtx._13*mtx._32 );
	out._13 =  fDetInv*( mtx._12*mtx._23 - mtx._13*mtx._22 );
	out._14 = 0.0f;

	out._21 = -fDetInv*( mtx._21*mtx._33 - mtx._23*mtx._31 );
	out._22 =  fDetInv*( mtx._11*mtx._33 - mtx._13*mtx._31 );
	out._23 = -fDetInv*( mtx._11*mtx._23 - mtx._13*mtx._21 );
	out._24 = 0.0f;

	out._31 =  fDetInv*( mtx._21*mtx._32 - mtx._22*mtx._31 );
	out._32 = -fDetInv*( mtx._11*mtx._32 - mtx._12*mtx._31 );
	out._33 =  fDetInv*( mtx._11*mtx._22 - mtx._12*mtx._21 );
	out._34 = 0.0f;

	out._41 = -( mtx._41*out._11 + mtx._42*out._21 + mtx._43*out._31 );
	out._42 = -( mtx._41*out._12 + mtx._42*out._22 + mtx._43*out._32 );
	out._43 = -( mtx._41*out._13 + mtx._42*out._23 + mtx._43*out._33 );
	out._44 = 1.0f;

	return out;
}

////////////////////////////////////////////////////////////////////

/// GGemsII, K.Wu, Fast Matrix Inversion 
bool FastInverseMtx(CMatrix4x4 &m)
{                          
	int	i,j,k;               
	int	pvt_i[4], pvt_j[4];            /* Locations of pivot elements */
	float	pvt_val;               /* Value of current pivot element */
	float	hold;                  /* Temporary storage */
	float	determinat;            
	
	determinat = 1.0f;
	for (k=0; k<4; k++)  {
		/* Locate k'th pivot element */
		pvt_val=m._mat_2D[k][k];            /* Initialize for search */
		pvt_i[k]=k;
		pvt_j[k]=k;
		for (i=k; i<4; i++) {
			for (j=k; j<4; j++) {
				if (fabs(m._mat_2D[i][j]) > fabs(pvt_val)) {
					pvt_i[k] = i;
					pvt_j[k] = j;
					pvt_val = m._mat_2D[i][j];
				}
			}
		}
		
		/* Product of pivots, gives determinant when finished */
		determinat*=pvt_val;
		if (fabs(determinat) < MATH3D_EPSILON) {    
			return false;  /* Matrix is singular (zero determinant) */
		}
		
		/* "Interchange" rows (with sign change stuff) */
		i=pvt_i[k];
		if (i!=k) {               /* If rows are different */
			for (j=0; j<4; j++) {
				hold=-m._mat_2D[k][j];
				m._mat_2D[k][j]=m._mat_2D[i][j];
				m._mat_2D[i][j]=hold;
			}
		}
		
		/* "Interchange" columns */
		j=pvt_j[k];
		if (j!=k) {              /* If columns are different */
			for (i=0; i<4; i++) {
				hold=-m._mat_2D[i][k];
				m._mat_2D[i][k]=m._mat_2D[i][j];
				m._mat_2D[i][j]=hold;
			}
		}
		
		/* Divide column by minus pivot value */
		for (i=0; i<4; i++) {
			if (i!=k) m._mat_2D[i][k]/=( -pvt_val) ; 
		}
		
		/* Reduce the matrix */
		for (i=0; i<4; i++) {
			hold = m._mat_2D[i][k];
			for (j=0; j<4; j++) {
				if (i!=k && j!=k) 
					m._mat_2D[i][j] += hold * m._mat_2D[k][j];
			}
		}
		
		/* Divide row by pivot */
		for (j=0; j<4; j++) {
			if (j!=k) m._mat_2D[k][j]/=pvt_val;
		}
		
		/* Replace pivot by reciprocal (at last we can touch it). */
		m._mat_2D[k][k] = 1.0f/pvt_val;
	}
	
	/* That was most of the work, one final pass of row/column interchange */
	/* to finish */
	for (k=4-2; k>=0; k--) { /* Don't need to work with 1 by 1 corner*/
		i=pvt_j[k];            /* Rows to swap correspond to pivot COLUMN */
		if (i!=k) {            /* If rows are different */
			for(j=0; j<4; j++) {
				hold = m._mat_2D[k][j];
				m._mat_2D[k][j]=-m._mat_2D[i][j];
				m._mat_2D[i][j]=hold;
			}
		}
		
		j=pvt_i[k];           /* Columns to swap correspond to pivot ROW */
		if (j!=k)             /* If columns are different */
			for (i=0; i<4; i++) {
				hold=m._mat_2D[i][k];
				m._mat_2D[i][k]=-m._mat_2D[i][j];
				m._mat_2D[i][j]=hold;
			}
	}
	return true;
}

////////////////////////////////////////////////////////////////////

void CMatrix4x4::CameraMtx(CVector3 &pos, CVector3 &look_at, float roll)
{
	Identity();

	float		tanfov, focus;
	float		ax, ay, az;
	float		sinx, siny, sinz,
 			cosx, cosy, cosz;

	CVector3 Pivot = -pos;
	CVector3 c = -(look_at - pos);

	focus = c.Length();

	ax = - atan2(c.x, c.z);
	ay = asin(c.y / focus);
	az = - TO_RADIANS(roll);

	sinx = (float)sin(ax);
	cosx = (float)cos(ax);

	siny = (float)sin(ay);
	cosy = (float)cos(ay);

	sinz = (float)sin(az);
	cosz = (float)cos(az);

	_11 =  sinx * siny * sinz + cosx * cosz;
	_12 =  cosy * sinz;
	_13 =  sinx * cosz - cosx * siny * sinz;
	_21 =  sinx * siny * cosz - cosx * sinz;
	_22 =  cosy * cosz;
	_23 = -cosx * siny * cosz - sinx * sinz;
	_31 = -sinx * cosy;
	_32 =  siny;
	_33 =  cosx * cosy;

	_14 = _11*Pivot[0] + _12*Pivot[1] + _13*Pivot[2] + _14;
	_24 = _21*Pivot[0] + _22*Pivot[1] + _23*Pivot[2] + _24;
	_34 = _31*Pivot[0] + _32*Pivot[1] + _33*Pivot[2] + _34;
}

ostream& operator<< (ostream &out, CMatrix4x4 &m)
{
	out << "[" << m._11 << ", " << m._12 << ", " << m._13 << ", " << m._14 << "]" << endl;
	out << "[" << m._21 << ", " << m._22 << ", " << m._23 << ", " << m._24 << "]" << endl;
	out << "[" << m._31 << ", " << m._32 << ", " << m._33 << ", " << m._34 << "]" << endl;
	out << "[" << m._41 << ", " << m._42 << ", " << m._43 << ", " << m._44 << "]" << endl;
	return out;
}

////////////////////////////////////////////////////////////////////

bool operator!= (const CMatrix4x4 &a, const CMatrix4x4 &b)
{
	return !(a == b);
}

////////////////////////////////////////////////////////////////////

bool operator== (const CMatrix4x4 &a, const CMatrix4x4 &b)
{
	for (int i = 0; i < 16; i++)
		if (a._mat_1D[i] != b._mat_1D[i])
			return false;
	return true;
}

////////////////////////////////////////////////////////////////////

void CMatrix4x4::RotationMtx(const CQuaternion& quat)
{
	float			xx = quat.x*quat.x; 
	float			yy = quat.y*quat.y; 
	float			zz = quat.z*quat.z;
	float			xy = quat.x*quat.y; 
	float			xz = quat.x*quat.z; 
	float			yz = quat.y*quat.z;
	float			wx = quat.w*quat.x; 
	float			wy = quat.w*quat.y; 
	float			wz = quat.w*quat.z;
	CMatrix4x4		out(CMatrix4x4::INIT_ZEROS);
    
	_11 = 1 - 2 * ( yy + zz ); 
	_12 =     2 * ( xy - wz );
	_13 =     2 * ( xz + wy );

	_21 =     2 * ( xy + wz );
	_22 = 1 - 2 * ( xx + zz );
	_23 =     2 * ( yz - wx );

	_31 =     2 * ( xz - wy );
	_32 =     2 * ( yz + wx );
	_33 = 1 - 2 * ( xx + yy );

	_44 = 1.0f;
}
