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
 *					                                                         *
 *****************************************************************************/
/*#**************************************************************************
  History

...
****************************************************************************/

#ifndef __VCGLIB_POINT4
#define __VCGLIB_POINT4

#include <vcg/Point3.h>

#include <assert.h>
#ifndef __VCGLIB_UTILITY
#include <vcg/Utility.h>
#endif

namespace vcg {


template<class T>
class MagnitudoComparer
{
public:
	inline bool operator() ( const T a, const T b )
	{
		return fabs(a)>fabs(b);
	}
};

template <class T> class Point4
{
protected:
	T _v[4];

public:
	typedef T scalar;


	inline Point4 () { }
	inline Point4 ( const T nx, const T ny, const T nz , const T nw )
	{
		_v[0] = nx; _v[1] = ny; _v[2] = nz; _v[3] = nw;
	}
	inline Point4 ( const T  p[4] )
	{   
		_v[0] = p[0]; _v[1]= p[1]; _v[2] = p[2]; _v[3]= p[3];
	}
	inline Point4 ( const Point4 & p )
	{   
		_v[0]= p._v[0]; _v[1]= p._v[1]; _v[2]= p._v[2]; _v[3]= p._v[3];
	}
	inline Point4 ( const Point3<T> & p )
	{
		_v[0] = p.V(0);
		_v[1] = p.V(1);
		_v[2] = p.V(2);
		_v[3] = 1.0;
	}
	inline Point4 & operator = ( const Point4 & p )
	{
		_v[0]= p._v[0]; _v[1]= p._v[1]; _v[2]= p._v[2]; _v[3]= p._v[3];
		return *this;
	}
	inline T &x() {return _v[0];}
	inline T &y() {return _v[1];}
	inline T &z() {return _v[2];}
	inline T &w() {return _v[3];}
	inline const T & operator [] ( const int i ) const
	{
		assert(i>=0 && i<4);
		return _v[i];
	}
	inline T & operator [] ( const int i )
	{
		assert(i>=0 && i<4);
		return _v[i];
	}
	inline T const * V() const
	{
		return _v;
	}
	inline const T & V ( const int i ) const
	{
		assert(i>=0 && i<4);
		return _v[i];
	}
	inline T & V ( const int i )
	{
		assert(i>=0 && i<4);
		return _v[i];
	}
	
	inline Point4 operator + ( const Point4 & p) const
	{ 
		return Point4( _v[0]+p._v[0], _v[1]+p._v[1], _v[2]+p._v[2], _v[3]+p._v[3] );
	}
	inline Point4 operator - ( const Point4 & p) const
	{
		return Point4( _v[0]-p._v[0], _v[1]-p._v[1], _v[2]-p._v[2], _v[3]-p._v[3] );
	}
	inline Point4 operator * ( const T s ) const
	{
		return Point4( _v[0]*s, _v[1]*s, _v[2]*s, _v[3]*s );
	}
	inline Point4 operator / ( const T s ) const
	{
		return Point4( _v[0]/s, _v[1]/s, _v[2]/s, _v[3]/s );
	}
	inline T operator * ( const Point4 & p ) const
	{
		return _v[0]*p._v[0] + _v[1]*p._v[1] + _v[2]*p._v[2] + _v[3]*p._v[3];
	} 
	inline Point4 & operator += ( const Point4 & p)
	{
		_v[0] += p._v[0]; _v[1] += p._v[1]; _v[2] += p._v[2]; _v[3] += p._v[3];
		return *this;
	}
	inline Point4 & operator -= ( const Point4 & p )
	{
		_v[0] -= p._v[0]; _v[1] -= p._v[1]; _v[2] -= p._v[2]; _v[3] -= p._v[3];
		return *this;
	}
	inline Point4 & operator *= ( const T s )
	{
		_v[0] *= s; _v[1] *= s; _v[2] *= s; _v[3] *= s;
		return *this;
	}
	inline Point4 & operator /= ( const T s )
	{
		_v[0] /= s; _v[1] /= s; _v[2] /= s; _v[3] /= s;
		return *this;
	}
	inline T Norm() const
	{
		return Sqrt( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3] );
	}
	inline T SquaredNorm() const
	{
		return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
	}
	inline Point4 & Normalize()
	{
		T n = Sqrt(_v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3] );
		if(n>0.0) {	_v[0] /= n;	_v[1] /= n;	_v[2] /= n; _v[3] /= n; }
		return *this;
	}
	inline Point4 operator - () const
	{
		return Point4( -_v[0], -_v[1], -_v[2], -_v[3] );
	}
	inline bool operator == (  const Point4& p ) const
	{
		return _v[0]==p._v[0] && _v[1]==p._v[1] && _v[2]==p._v[2] && _v[3]==p._v[3];
	} 
	inline bool operator != ( const Point4 & p ) const
	{
		return _v[0]!=p._v[0] || _v[1]!=p._v[1] || _v[2]!=p._v[2] || _v[3]!=p._v[3];
	}
	inline bool operator <  ( Point4 const & p ) const
	{
		return	(_v[3]!=p._v[3])?(_v[3]<p._v[3]):
				(_v[2]!=p._v[2])?(_v[2]<p._v[2]):
				(_v[1]!=p._v[1])?(_v[1]<p._v[1]):
				(_v[0]<p._v[0]);
	}
	inline bool operator >  ( const Point4 & p ) const
	{
		return	(_v[3]!=p._v[3])?(_v[3]>p._v[3]):
				(_v[2]!=p._v[2])?(_v[2]>p._v[2]):
				(_v[1]!=p._v[1])?(_v[1]>p._v[1]):
				(_v[0]>p._v[0]);
	}
	inline bool operator <= ( const Point4 & p ) const
	{
		return	(_v[3]!=p._v[3])?(_v[3]< p._v[3]):
				(_v[2]!=p._v[2])?(_v[2]< p._v[2]):
				(_v[1]!=p._v[1])?(_v[1]< p._v[1]):
				(_v[0]<=p._v[0]);
	}
	inline bool operator >= ( const Point4 & p ) const
	{
		return	(_v[3]!=p._v[3])?(_v[3]> p._v[3]):
				(_v[2]!=p._v[2])?(_v[2]> p._v[2]):
				(_v[1]!=p._v[1])?(_v[1]> p._v[1]):
				(_v[0]>=p._v[0]);
	}
		/// Questa funzione estende il vettore ad un qualsiasi numero di dimensioni
		/// paddando gli elementi estesi con zeri
	inline T Ext( const int i ) const
	{
		if(i>=0 && i<=3) return _v[i];
		else             return 0;
	}

	T stable_dot ( const Point4<T> & p ) const
	{
		T k[4];

		k[0] = _v[0]*p._v[0];
		k[1] = _v[1]*p._v[1];
		k[2] = _v[2]*p._v[2];
		k[3] = _v[3]*p._v[3];
		sort(k+0,k+4, MagnitudoComparer<T>() );
		T q = k[0];
		q += k[1];
		q += k[2];
		q += k[3];
		return q;
	}  

	template <class Q>
	inline void Import( const Point4<Q> & b )
	{
		_v[0] = T(b[0]);
		_v[1] = T(b[1]);
		_v[2] = T(b[2]);
		_v[3] = T(b[3]);
	}

}; // end class definition

template <class T>
T Angle( const Point4<T>& p1, const Point4<T>  & p2 )
{
	T w = p1.Norm()*p2.Norm();
	if(w==0) return -1;
	T t = (p1*p2)/w;
	if(t>1) t=1;
    return T( acos(t) );
}


template <class T>
inline T Norm( const Point4<T> & p )
{
	return p.Norm();
}

template <class T>
inline T SquaredNorm( const Point4<T> & p )
{
    return p.SquaredNorm();
}

template <class T>
inline T Distance( const Point4<T> & p1, const Point4<T> & p2 )
{
    return Norm(p1-p2);
}

template <class T>
inline T SquaredDistance( const Point4<T> & p1, const Point4<T> & p2 )
{
    return SquaredNorm(p1-p2);
}


#ifdef __GL_H__
inline void glVertex(Point4<int> const & p)   { glVertex4iv(p.V());}
inline void glVertex(Point4<short> const & p) { glVertex4sv(p.V());}
inline void glVertex(Point4<float> const & p) { glVertex4fv(p.V());}
inline void glVertex(Point4<double> const & p){ glVertex4dv(p.V());}
inline void glTexCoord(Point4<int> const & p)   { glTexCoord4iv(p.V());}
inline void glTexCoord(Point4<short> const & p) { glTexCoord4sv(p.V());}
inline void glTexCoord(Point4<float> const & p) { glTexCoord4fv(p.V());}
inline void glTexCoord(Point4<double> const & p){ glTexCoord4dv(p.V());}
#endif

typedef Point4<short>  Point4s;
typedef Point4<int>	   Point4i;
typedef Point4<float>  Point4f;
typedef Point4<double> Point4d;


} // end namespace
#endif
