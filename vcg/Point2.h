/*#***************************************************************************
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
 *																																					 *
 *****************************************************************************/
/*#**************************************************************************
  History

 1999 Feb 02 First Draft.
			Mar 15 First Working release 
			Jul 10 Reindented and reformatted
						 Fixed != bug
						 Fixed Eq bug
							
 2000	Jan 26 inserito include condizionale
						 corretto Distance() e init()
			Feb 15 Aggiunte versioni const di x() e y()			
			Feb 23 Cambiato vcg::abs e vcg::sqrt -> Abs e Sqrt
		  Nov 16 Corretta Angle (C.R.)
 2001 May 21 Aggiunto typedef scalar (CR)
             Aggiunto Zero
			       Corretto SquaredDistance
  		Jun 25 Aggiunti Cartesian2Polar e Polar2Cartesian (CR)
	  	Jul 04 aggiunta rotate (CR)
		         corretta angle (CR)
		      09 Aggiunta Ext (CR)
			    17 Corretta la sintassi completamente (CR)
      Nov 17 Aggiunte glMultiTexCoordARB tra le funzioni inline (PC)

****************************************************************************/

#ifndef __VCGLIB_POINT2
#define __VCGLIB_POINT2

//#include <limits>
#include <assert.h>
#ifndef __VCGLIB_UTILITY
#include <vcg/Utility.h>
#endif

namespace vcg {

template <class FLTYPE> class Point2
{
public:

	FLTYPE v[2];

	typedef FLTYPE scalar;

	inline const FLTYPE &x() const {return v[0];} 
	inline const FLTYPE &y() const {return v[1];}
	inline FLTYPE &x() {return v[0];}
	inline FLTYPE &y() {return v[1];}
	inline const FLTYPE & operator [] ( const int i ) const
	{
		assert(i>=0 && i<2);
		return v[i];
	}
	inline FLTYPE & operator [] ( const int i )
	{
		assert(i>=0 && i<2);
		return v[i];
	}
	

	inline Point2 () { }
	inline Point2 ( const FLTYPE nx, const FLTYPE ny )
	{
			v[0] = nx; v[1] = ny;
	}
	inline Point2 ( Point2 const & p)
	{   
			v[0]= p.v[0];    v[1]= p.v[1];
	}
	inline Point2 & operator =( Point2 const & p)
	{
			v[0]= p.v[0]; v[1]= p.v[1];
			return *this;
	}

	inline void Zero()
	{
		v[0] = 0;
		v[1] = 0;
	}

	inline Point2 operator + ( Point2 const & p) const
	{ 
			return Point2<FLTYPE>( v[0]+p.v[0], v[1]+p.v[1] );
	}
	inline Point2 operator - ( Point2 const & p) const
	{
			return Point2<FLTYPE>( v[0]-p.v[0], v[1]-p.v[1] );
	}
	inline Point2 operator * ( const FLTYPE s ) const
	{
			return Point2<FLTYPE>( v[0] * s, v[1] * s );
	}
	inline Point2 operator / ( const FLTYPE s ) const
	{
			return Point2<FLTYPE>( v[0] / s, v[1] / s );
	}
	inline FLTYPE operator * ( Point2 const & p ) const
	{
			return ( v[0]*p.v[0] + v[1]*p.v[1] );
	}

	inline FLTYPE operator ^ ( Point2 const & p ) const
	{
			return v[1]*p.v[0] - v[0]*p.v[1];
	} 

	inline Point2 & operator += ( Point2 const & p)
	{
			v[0] += p.v[0];    v[1] += p.v[1];
			return *this;
	}
	inline Point2 & operator -= ( Point2 const & p)
	{
			v[0] -= p.v[0];    v[1] -= p.v[1];
			return *this;
	}
	inline Point2 & operator *= ( const FLTYPE s )
	{
			v[0] *= s;    v[1] *= s;
			return *this;
	}
	inline Point2 & operator /= ( const FLTYPE s )
	{
			v[0] /= s;    v[1] /= s;
			return *this;
	}
	inline FLTYPE Norm( void ) const
	{
			return Sqrt( v[0]*v[0] + v[1]*v[1] );
	}
	inline FLTYPE SquaredNorm( void ) const
	{
			return ( v[0]*v[0] + v[1]*v[1] );
	}
	inline Point2 & Scale( const FLTYPE sx, const FLTYPE sy );

	inline Point2 & Normalize( void )
	{
			FLTYPE n = Sqrt(v[0]*v[0] + v[1]*v[1]);
			if(n>0.0) {	v[0] /= n;	v[1] /= n; }
			return *this;
	}
	inline bool operator == ( Point2 const & p ) const
	{
			return (v[0]==p.v[0] && v[1]==p.v[1]);
	} 
	inline bool operator != ( Point2 const & p ) const
	{
			return ( (v[0]!=p.v[0]) || (v[1]!=p.v[1]) );
	}
	inline bool operator <  ( Point2 const & p ) const
	{
			return	(v[1]!=p.v[1])?(v[1]<p.v[1]):
							(v[0]<p.v[0]);
	}
	inline bool operator >  ( Point2 const & p ) const
	{
			return	(v[1]!=p.v[1])?(v[1]>p.v[1]):
							(v[0]>p.v[0]);
	}

	inline bool operator <= ( Point2 const & p ) const
	{
			return	(v[1]!=p.v[1])?(v[1]< p.v[1]):
							(v[0]<=p.v[0]);
	}

	inline bool operator >= ( Point2 const & p ) const
	{
			return	(v[1]!=p.v[1])?(v[1]> p.v[1]):
							(v[0]>=p.v[0]);
	}
	inline FLTYPE Distance( Point2 const & p ) const
	{
			return Norm(*this-p);
	}

	inline FLTYPE SquaredDistance( Point2 const & p ) const
	{
			return Norm2(*this-p);
	}	

	inline Point2 & Cartesian2Polar()
	{
		FLTYPE t = (FLTYPE)atan2(v[1],v[0]);
		v[0] = Sqrt(v[0]*v[0]+v[1]*v[1]);
		v[1] = t;
		return *this;
	}

	inline Point2 & Polar2Cartesian()
	{
		FLTYPE l = v[0];
		v[0] = (FLTYPE)(l*cos(v[1]));
		v[1] = (FLTYPE)(l*sin(v[1]));
		return *this;
	}


	inline Point2 & rotate( const FLTYPE a )
	{
		FLTYPE t = v[0];
		FLTYPE s = sin(a);
		FLTYPE c = cos(a);

		v[0] = v[0]*c - v[1]*s;
		v[1] =   t *s + v[1]*c;

		return *this;
	}

	/// Questa funzione estende il vettore ad un qualsiasi numero di dimensioni
	/// paddando gli elementi estesi con zeri
	inline FLTYPE Ext( const int i ) const
	{
		if(i>=0 && i<2) return v[i];
		else            return 0;
	}

#if 0
	inline bool Eq( Point2<FLTYPE> const & p ) const
	{
	  return (Abs(v[0]-p.v[0]) < std::numeric_limits<float>::epsilon()*4 &&
			  Abs(v[1]-p.v[1]) < std::numeric_limits<float>::epsilon()*4 );
	}   

	inline bool NotEq( Point2<FLTYPE> const & p ) const
	{
	  return (Abs(v[0]-p.v[0]) > std::numeric_limits<float>::epsilon()*4 ||
			  Abs(v[1]-p.v[1]) > std::numeric_limits<float>::epsilon()*4 );
	}
#endif

}; // end class definition


template <class FLTYPE>
inline FLTYPE Angle( Point2<FLTYPE> const & p1, Point2<FLTYPE> const & p2 )
{
	return atan2(p2[1],p2[0]) - atan2(p1[1],p1[0]);
}

template <class FLTYPE>
inline Point2<FLTYPE> operator - ( Point2<FLTYPE> const & p ){
    return Point2<FLTYPE>( -p.v[0], -p.v[1] );
}

template <class FLTYPE>
inline Point2<FLTYPE> operator * ( const FLTYPE s, Point2<FLTYPE> const & p ){
    return Point2<FLTYPE>( p.v[0] * s, p.v[1] * s  );
}

template <class FLTYPE>
inline FLTYPE Norm( Point2<FLTYPE> const & p ){
		return Sqrt( p.v[0]*p.v[0] + p.v[1]*p.v[1] );
}

template <class FLTYPE>
inline FLTYPE Norm2( Point2<FLTYPE> const & p ){
    return ( p.v[0]*p.v[0] + p.v[1]*p.v[1] );
}

template <class FLTYPE>
inline Point2<FLTYPE> & Normalize( Point2<FLTYPE> & p ){
		FLTYPE n = Sqrt( p.v[0]*p.v[0] + p.v[1]*p.v[1] );
    if(n>0.0) p/=n;
    return p;
}

template <class FLTYPE>
inline FLTYPE Distance( Point2<FLTYPE> const & p1,Point2<FLTYPE> const & p2 ){
    return Norm(p1-p2);
}

template <class FLTYPE>
inline FLTYPE SquaredDistance( Point2<FLTYPE> const & p1,Point2<FLTYPE> const & p2 ){
    return Norm2(p1-p2);
}


#ifdef __GL_H__
inline void glVertex(Point2<int> const & p)   { glVertex2iv(p.v);}
inline void glVertex(Point2<short> const & p) { glVertex2sv(p.v);}
inline void glVertex(Point2<float> const & p) { glVertex2fv(p.v);}
inline void glVertex(Point2<double> const & p){ glVertex2dv(p.v);}
inline void glTexCoord(Point2<int> const & p)   { glTexCoord2iv(p.v);}
inline void glTexCoord(Point2<short> const & p) { glTexCoord2sv(p.v);}
inline void glTexCoord(Point2<float> const & p) { glTexCoord2fv(p.v);}
inline void glTexCoord (Point2<double> const & p){ glTexCoord2dv(p.v);}

#ifdef GL_TEXTURE0_ARB
//inline void glMultiTexCoordARB (GLenum ind, Point2<double> const & p){ glMultiTexCoord2dvARB(ind,p.v);}
//\inline void glMultiTexCoordARB (GLenum ind, Point2<float> const & p){ glMultiTexCoord2fvARB(ind,p.v);}
#endif

#endif

typedef Point2<short>  Point2s;
typedef Point2<int>	   Point2i;
typedef Point2<float>  Point2f;
typedef Point2<double> Point2d;


} // end namespace
#endif
