/*#***************************************************************************
 * VCGLib                                                                    *
 *																			 *
 * Visual Computing Group                                                o>  *
 * IEI Institute, CNUCE Institute, CNR Pisa                             <|   *
 *                                                                      / \  *
 * Copyright(C) 1999 by Paolo Cignoni, Claudio Rocchini                      *
 * All rights reserved.                                                      *
 *																			 *
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

 1999 May 05 First Working release 

 2000	Jan 26 inserito include condizionale
						 corretto Distance() e init()
			Jan 27 Corretto Planef,d --> Plane3d,3f
			Feb 04 Aggiunto Costruttore(n,d) e viceversa.
			Jun 22 Aggiunta intersezione di tre piani
			    22 Aggiunta OriginNearest()
			    26 Update di allineamento
  		Jul 04 Modificata normalize (non viene espansa inline)
			       Modificata Distance (espansa inline + efficiente)
 2001   May 30 Aggiunto project (CR)
        Oct  6 Aggiunto init da normale ed un punto (CR)
		Dec 10 Cambiata normalize (CR)
		    19 corretto un prodotto scalare vettore in vettore*scalare (pc)
****************************************************************************/

#ifndef __VCGLIB_PLANE3
#define __VCGLIB_PLANE3

//#include <limits>
#include <assert.h>
#ifndef __VCGLIB_UTILITY
#include <vcg/Utility.h>
#endif
#ifndef __VCGLIB_POINT3
#include <vcg/Point3.h>
#endif

namespace vcg {
/** @name Plane3
    Class Plane3.
    This is the class for definition of a plane.
		@param FLTYPE (Temaplete Parameter) Specifies the scalar field.
 */
template<class FLTYPE> class Plane3
{
public:
	/// The point type
	typedef Point3<FLTYPE> PTYPE;
	/// Normal to plane (not normalized)
	PTYPE  n;		// Normale al piano (non normalizzata)
	/// Distance between the plane and the origin of axis
	FLTYPE d;		// Distanza del piano dall'origine (x in p -> n*x-d=0)

  inline Plane3<FLTYPE> (){ }
			
	inline Plane3<FLTYPE> (  Point3<FLTYPE> const & _n,  const FLTYPE _d){
			n=_n; d=_d;
	}
	inline Plane3<FLTYPE> (  const FLTYPE _d,  Point3<FLTYPE> const & _n){
			n=_n; d=_d;
	}
	inline Plane3<FLTYPE> (  Plane3<FLTYPE> const & p){
			n=p.n; d=p.d;
	}
	

		// Ritorna il punto del piano piu' vicino all'origine
	inline Point3<FLTYPE> OriginNearest() const
	{
		return n*(d/n.SquaredNorm());
	}

		/// Proietta un punto sul piano
	void Project( PTYPE & p )
	{
		FLTYPE k = p*n-d;
		p -= n*k;
	}

/// Function to normalize n
void Normalize()
{
	FLTYPE x = n.Norm();
	if(x>0)
	{
		n[0] /= x;
		n[1] /= x;
		n[2] /= x;
		d /= x;
	}
}

		// Calcola il piano passante per 3 punti
/// Calculates the plane passing through three points
inline void init(
		const PTYPE & p0,
		const PTYPE & p1,
		const PTYPE & p2 )
{
	n = (p2 - p0) ^ (p1 - p0);
	d = p0 * n;
}


/// Calculates the plane passing through a point and the normal
inline void init(
		const PTYPE & p0,
		const PTYPE & norm )
{
	n = norm;
	d = p0 * n;
}


};	// end class Plane3


	// Intersezione di tre piani
template<class T>
inline bool Intersection( const Plane3<T> & p1, const Plane3<T> & p2, const Plane3<T> & p3,
						  Point3<T> & pt )
{
	double d = p1.n[0] * ( p2.n[1]*p3.n[2] - p2.n[2]*p3.n[1] ) -
               p1.n[1] * ( p2.n[0]*p3.n[2] - p2.n[2]*p3.n[0] ) +
			   p1.n[2] * ( p2.n[0]*p3.n[1] - p2.n[1]*p3.n[0] ) ;

	if(d==0) return false;

	pt = ((p2.n ^ p3.n)* p1.d +
		  (p3.n ^ p1.n)* p2.d +
		  (p1.n ^ p2.n)* p3.d ) / d;
	return true;
}

	//Distanza piano - punto
/** Function to calculate the distance beetween the plane pl and the point po
		@param pl The plane
		@param po The point
	*/
template<class FLTYPE>
inline FLTYPE Distance( const Plane3<FLTYPE> & pl, const Point3<FLTYPE> & po )
{
	return pl.n[0]*po[0] + pl.n[1]*po[1] + pl.n[2]*po[2] - pl.d;
//	return pl.n*po - pl.d; 
}

	/// Distanza punto - piano
/** Function to calculate the distance beetween the point po and the plane pl
		@param po The point
		@param pl The plane
	*/
template<class FLTYPE>
inline FLTYPE Distance( const Point3<FLTYPE> & po, const Plane3<FLTYPE> & pl )
{
	return pl.n[0]*po[0] + pl.n[1]*po[1] + pl.n[2]*po[2] - pl.d;
//	return pl.n*po - pl.d; 
}


typedef Plane3<float>  Plane3f;
typedef Plane3<double> Plane3d;


} // end namespace


#endif