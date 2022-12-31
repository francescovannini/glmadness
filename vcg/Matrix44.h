/*****************************************************************************
 * VCGLib                                                                    *
 *																																					 *
 * Visual Computing Group                                                o>  *
 * IEI Institute, CNUCE Institute, CNR Pisa                             <|   *
 *                                                                      / \  *
 * Copyright(C) 1999 by Paolo Cignoni, Claudio Rocchini                      *
 * All rights reserved.                                                      *
 *																																					 *
 * Permission  to use, copy, modify, distribute  and sell this  software and *
 * its documentation for any purpose is hereby granted without fee, provided *
 * that  the above copyright notice appear  in all copies and that both that *
 * copyright   notice  and  this  permission  notice  appear  in  supporting *
 * documentation. the author makes  no representations about the suitability *
 * of this software for any purpose. It is provided  "as is" without express *
 * or implied warranty.                                                      *
 *					                                                         *
 *****************************************************************************/
/****************************************************************************
  History

... 
****************************************************************************/
/*
	Annotations:
Opengl stores matrix in  column-major order. That is, the matrix is stored as:

	a0  a4  a8  a12
	a1  a5  a9  a13
	a2  a6  a10 a14
	a3  a7  a11 a15
e.g. glTranslate generate a matrix that is

	1 0 0 tx
	0 1 0 ty 
	0 0 1 tz
	0 0 0  1

Qui invece si tengono in row major order (per fare l'accesso piu'facile)
come 4 vettori di 4 float (ogni vettore e' una riga);

	a0  a1  a2  a3
	a4  a5  a6  a7
	a8  a9  a10 a11
	a12 a13 a14 a15

Quindi prima di passarle ad opengl si deve fare una transpose;


*/


#ifndef __VCGLIB_MATRIX44_H
#define __VCGLIB_MATRIX44_H

#ifndef __VCGLIB_POINT3
#include <vcg/Point3.h>
#endif

#ifndef __VCGLIB_POINT4
#include <vcg/Point4.h>
#endif

#include <algorithm>

namespace vcg {

template <class FLTYPE> 
/** @name Matrix44
	Class Matrix44.
    This is the class for definition of a matrix 4x4.	
	@param FLTYPE (Templete Parameter) Specifies the scalar field.
*/
class Matrix44 
{
public:	
/// Elementi della matrice
    FLTYPE a[4][4];
/// The matrix type
typedef Matrix44<FLTYPE> Matrix44_FT;

    	
	/// Default constructor
	inline  Matrix44( void ) {};
	/// Default dinstructor
	inline ~Matrix44( void ) {};

	/// Copy constructor
	inline Matrix44( const Matrix44 & m ){
		int i,j;
    
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				a[i][j] = m.a[i][j];
	}

	/// Vector constructor
	inline Matrix44( const FLTYPE v[] ){
		int i,j;
    
		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			a[i][j] = v[i+4*j];
	}

	/// Assignment operator
    Matrix44 & operator = ( const Matrix44 & m ){
		int i,j;
    
		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			a[i][j] = m.a[i][j];
		return *this;
	}

	/// Operatore di indicizzazione
	inline FLTYPE * operator [] ( const int i )
	{
		return a[i];
	}

	/// Operatore const di indicizzazione
	inline const FLTYPE * operator [] ( const int i ) const
	{
		return a[i];
	}

  inline bool operator == (const  Matrix44 & m) const
	{
    int i,j;
		for(i=0;i<4;i++)
      for(j=0;j<4;j++)
        if(a[i][j] != m.a[i][j]) return false;
    return true;
	}
  inline bool operator != (const  Matrix44 & m) const
	{
    int i,j;
		for(i=0;i<4;i++)
      for(j=0;j<4;j++)
        if(a[i][j] != m.a[i][j]) return true;
    return false;
	}

	// Operatori aritmetici
    /// Operatore di somma	
	Matrix44 operator + ( const Matrix44 & m ) const { 	
    Matrix44 r;
    int i,j;
    
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
        r.a[i][j] = a[i][j] + m.a[i][j];
    return r;
		}

	/// Operatore di sottrazione
	Matrix44 operator - ( const Matrix44 & m ) const { 
    Matrix44 r;
    int i,j;
    
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
        r.a[i][j] = a[i][j] - m.a[i][j];
    return r;
		}

	/// Operatore prodotto
	Matrix44 operator * ( const Matrix44 & m ) const {
		Matrix44 r;
    int i,j,k;
   
    for(i=0;i<4;i++)
      for(j=0;j<4;j++){
        FLTYPE t = 0.0;
        for(k=0;k<4;k++)
          t += a[i][k] * m.a[k][j];
        r.a[i][j] = t;
      }
    return r;
		}

	/// Operatore prodotto matrice per Point4
	Point4<FLTYPE> operator * ( const Point4<FLTYPE> &v ) const {
		Point4<FLTYPE> r;
    int i,k;
   
    for(i=0;i<4;i++){
        FLTYPE t = 0.0;
        for(k=0;k<4;k++)
          t += a[i][k] * v[k];
        r[i] = t;
      }
    return r;
		}
		
		 /// Operatore prodotto matrice per Point3 
	Point3<FLTYPE> operator * ( const Point3<FLTYPE> &v ) const {
	Point3<FLTYPE> r;
   int i,k;
   
    for(i=0;i<3;i++){
        r[i]=a[i][3];
        for(k=0;k<3;k++)
          r[i] += a[i][k] * v[k];
      }
			return r;
		}
  
  // Modificatore prodotto per scalare per matrici 4x4
  Matrix44 & operator *= ( const FLTYPE k )		// Prodotto per scalare
	{
		int i,j;
		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			a[i][j] *= k;
		return *this;
	}
    	/// Costruttori speciali
	void Zero()
	{
		int i,j;
		for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			a[i][j] = 0.0;
	}

	void Scale( const FLTYPE sx, const FLTYPE sy, const FLTYPE sz )
	{
		Zero();
		a[0][0] = sx;
		a[1][1] = sy;
		a[2][2] = sz;
		a[3][3] = 1.0;
	}

	void Translate (const Point3<FLTYPE> &t)	{	Translate(t[0],t[1],t[2]);	}
	void Translate( const FLTYPE sx, const FLTYPE sy, const FLTYPE sz )
	{
		SetIdentity();
		a[0][3] = sx;
		a[1][3] = sy;
		a[2][3] = sz;
	}

	void Rotate( FLTYPE angle, const Point3<FLTYPE> & axis )
	{
		angle = angle*(FLTYPE)3.14159265358979323846/180;
		FLTYPE c = Cos(angle);
		FLTYPE s = Sin(angle);
		FLTYPE q = 1-c;
		Point3<FLTYPE> t = axis;
		t.Normalize();
		a[0][0] = t[0]*t[0]*q + c;
		a[0][1] = t[0]*t[1]*q - t[2]*s;
		a[0][2] = t[0]*t[2]*q + t[1]*s;
		a[0][3] = 0;
		a[1][0] = t[1]*t[0]*q + t[2]*s;
		a[1][1] = t[1]*t[1]*q + c;
		a[1][2] = t[1]*t[2]*q - t[0]*s;
		a[1][3] = 0;
		a[2][0] = t[2]*t[0]*q -t[1]*s;
		a[2][1] = t[2]*t[1]*q +t[0]*s;
		a[2][2] = t[2]*t[2]*q +c;
		a[2][3] = 0;
		a[3][0] = 0;
		a[3][1] = 0;									
		a[3][2] = 0;
		a[3][3] = 1;
	}

Matrix44 & SetIdentity(){return SetScalar(1.0);}	// Matrice identita'
Matrix44 & SetScalar( const FLTYPE k ) {
    int i,j;
    
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
        if(i==j) a[i][j] = k;
        else     a[i][j] = 0;
    return *this;
}
Matrix44 & Transpose() {
    int i,j;
    
    for(i=1;i<4;i++)
      for(j=0;j<i;j++)
				std::swap(a[i][j],a[j][i]);
    return *this;
}
  		
FLTYPE Det() const
{
 return 
   a[0][0] * (  a[1][1]*a[2][2]*a[3][3] - a[1][1]*a[2][3]*a[3][2] - a[2][1]*a[1][2]*a[3][3] 
			  + a[2][1]*a[1][3]*a[3][2] + a[3][1]*a[1][2]*a[2][3] - a[3][1]*a[1][3]*a[2][2]) 
 - a[0][1] * (  a[1][0]*a[2][2]*a[3][3] - a[1][0]*a[2][3]*a[3][2] - a[2][0]*a[1][2]*a[3][3]
			  + a[2][0]*a[1][3]*a[3][2] + a[3][0]*a[1][2]*a[2][3] - a[3][0]*a[1][3]*a[2][2]) 
 + a[0][2] * (  a[1][0]*a[2][1]*a[3][3] - a[1][0]*a[2][3]*a[3][1] - a[2][0]*a[1][1]*a[3][3] 
			  + a[2][0]*a[1][3]*a[3][1] + a[3][0]*a[1][1]*a[2][3] - a[3][0]*a[1][3]*a[2][1]) 
 - a[0][3] * (  a[1][0]*a[2][1]*a[3][2] - a[1][0]*a[2][2]*a[3][1] - a[2][0]*a[1][1]*a[3][2] 
			  + a[2][0]*a[1][2]*a[3][1] + a[3][0]*a[1][1]*a[2][2] - a[3][0]*a[1][2]*a[2][1]);
}
		
Matrix44 & Invert()
{
   FLTYPE det;
   FLTYPE d01, d02, d12, d13, d23, d30;
   Matrix44 T; 
   /* Inverse = adjoint / det. (See linear algebra texts.)*/

   /* pre-compute 2x2 dets for last two rows when computing */
   /* cofactors of first two rows. */
   d01 = (a[2][0]*a[3][1]-a[3][0]*a[2][1]);
   d02 = (a[2][0]*a[3][2]-a[3][0]*a[2][2]);
   d12 = (a[2][1]*a[3][2]-a[3][1]*a[2][2]);
   d13 = (a[2][1]*a[3][3]-a[3][1]*a[2][3]);
   d23 = (a[2][2]*a[3][3]-a[3][2]*a[2][3]);
   d30 = (a[2][3]*a[3][0]-a[3][3]*a[2][0]);

   T.a[0][0] =  (a[1][1] * d23 - a[1][2] * d13 + a[1][3] * d12);
   T.a[0][1] = -(a[1][0] * d23 + a[1][2] * d30 + a[1][3] * d02);
   T.a[0][2] =  (a[1][0] * d13 + a[1][1] * d30 + a[1][3] * d01);
   T.a[0][3] = -(a[1][0] * d12 - a[1][1] * d02 + a[1][2] * d01);

   /* Compute determinant as early as possible using these cofactors. */
   det = a[0][0] * T.a[0][0] + a[0][1] * T.a[0][1] + a[0][2] * T.a[0][2] + a[0][3] * T.a[0][3];

   /* Run singularity test. */
   if (det == 0.0) {
      /* printf("invert_matrix: Warning: Singular matrix.\n"); */
	   SetIdentity();
	}
   else {
      FLTYPE invDet = FLTYPE(1.0) / det;
      /* Compute rest of inverse. */
      T.a[0][0] *= invDet;  T.a[0][1] *= invDet;
      T.a[0][2] *= invDet;  T.a[0][3] *= invDet;

      T.a[1][0] = -(a[0][1] * d23 - a[0][2] * d13 + a[0][3] * d12) * invDet;
      T.a[1][1] =  (a[0][0] * d23 + a[0][2] * d30 + a[0][3] * d02) * invDet;
      T.a[1][2] = -(a[0][0] * d13 + a[0][1] * d30 + a[0][3] * d01) * invDet;
      T.a[1][3] =  (a[0][0] * d12 - a[0][1] * d02 + a[0][2] * d01) * invDet;

      /* Pre-compute 2x2 dets for first two rows when computing */
      /* cofactors of last two rows. */
      d01 = a[0][0]*a[1][1]-a[1][0]*a[0][1];
      d02 = a[0][0]*a[1][2]-a[1][0]*a[0][2];
      d12 = a[0][1]*a[1][2]-a[1][1]*a[0][2];
      d13 = a[0][1]*a[1][3]-a[1][1]*a[0][3];
      d23 = a[0][2]*a[1][3]-a[1][2]*a[0][3];
      d30 = a[0][3]*a[1][0]-a[1][3]*a[0][0];

      T.a[2][0] =  (a[3][1] * d23 - a[3][2] * d13 + a[3][3] * d12) * invDet;
      T.a[2][1] = -(a[3][0] * d23 + a[3][2] * d30 + a[3][3] * d02) * invDet;
      T.a[2][2] =  (a[3][0] * d13 + a[3][1] * d30 + a[3][3] * d01) * invDet;
      T.a[2][3] = -(a[3][0] * d12 - a[3][1] * d02 + a[3][2] * d01) * invDet;
      T.a[3][0] = -(a[2][1] * d23 - a[2][2] * d13 + a[2][3] * d12) * invDet;
      T.a[3][1] =  (a[2][0] * d23 + a[2][2] * d30 + a[2][3] * d02) * invDet;
      T.a[3][2] = -(a[2][0] * d13 + a[2][1] * d30 + a[2][3] * d01) * invDet;
      T.a[3][3] =  (a[2][0] * d12 - a[2][1] * d02 + a[2][2] * d01) * invDet;
	
	  int i,j;
	  for(i=0;i<4;i++)
		  for(j=0;j<4;j++)
			  a[i][j] = T.a[j][i];
   }
 return *this;
}


bool IsSymmetric()
{
	for(int i=0;i<3;++i)
		for(int j=i+1;j<4;++j)
			if(a[i][j] != a[j][i]) return false;
	
	return true;
}
};// End of class

typedef Matrix44<short>  Matrix44s;
typedef Matrix44<int>	   Matrix44i;
typedef Matrix44<float>  Matrix44f;
typedef Matrix44<double> Matrix44d;



#ifdef __GL_H__
inline void glMultMatrix(Matrix44<double> & M) { 	M.Transpose(); glMultMatrixd(&(M.a[0][0])); M.Transpose();}
inline void glMultMatrix(Matrix44<float>  & M) { 	M.Transpose(); glMultMatrixf(&(M.a[0][0])); M.Transpose();}
inline void glLoadMatrix(Matrix44<double> & M) { 	M.Transpose(); glLoadMatrixd(&(M.a[0][0])); M.Transpose();}
inline void glLoadMatrix(Matrix44<float>  & M) { 	M.Transpose(); glLoadMatrixf(&(M.a[0][0])); M.Transpose();}

inline void glGetModelViewMatrix(Matrix44<float>  & M) { 
	glGetFloatv(GL_MODELVIEW_MATRIX,(float *)&(M.a[0][0])); M.Transpose();
}
inline void glGetModelViewMatrix(Matrix44<double>  & M) { 
	glGetDoublev(GL_MODELVIEW_MATRIX,(double *)&(M.a[0][0])); M.Transpose();
}
inline void glGetProjectionMatrix(Matrix44<float>  & M) { 
	glGetFloatv(GL_PROJECTION_MATRIX,(float *)&(M.a[0][0])); M.Transpose();
}
inline void glGetProjectionMatrix(Matrix44<double>  & M) { 
	glGetDoublev(GL_PROJECTION_MATRIX,(double *)&(M.a[0][0])); M.Transpose();
}
#endif


} // end of namespace
#endif