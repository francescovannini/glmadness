/*#***************************************************************************
 * VCGLib                                                            o o     *
 *																																 o     o   *
 * Visual Computing Group                                          _  O  _   *
 * ISTI Institute, CNR Pisa                                         \/)\/    *
 *                                                                 /\/|      *
 * Copyright(C) 1999-2003 by Paolo Cignoni, Claudio Rocchini          |      *
 * All rights reserved.                                               \      *
 *																																					 *
 * Permission  to use, copy, modify, distribute  and sell this  software and *
 * its documentation for any purpose is hereby granted without fee, provided *
 * that  the above copyright notice appear  in all copies and that both that *
 * copyright   notice  and  this  permission  notice  appear  in  supporting *
 * documentation. the author makes  no representations about the suitability *
 * of this software for any purpose. It is provided  "as is" without express *
 * or implied warranty.                                                      *
 *					                                                                 *
 *****************************************************************************/
/****************************************************************************
  History
 2002 Nov 02 Release for the CI course,
 ... omissis...
 1999 Feb 02 First Draft.
****************************************************************************/

#ifndef __VCGLIB_UTILITY
#define __VCGLIB_UTILITY

#include <math.h>
#include <assert.h>


namespace vcg {

  inline int		Sqrt(int v)	   { return int(::sqrt(float(v))); }
  inline short  Sqrt(short v)  { return short(::sqrtf(float(v))); }
  inline float  Sqrt(float v)  { return ::sqrtf(v); }
  inline double Sqrt(double v) { return ::sqrt(v); }
  inline int		Abs(int v)	   { return ::abs(v); }
  inline short  Abs(short v)   { return short(::abs(v)); }
  inline float  Abs(float v)   { return ::fabsf(v); }
  inline double Abs(double v)  { return ::fabs(v); }
	inline int		Sin(int /*v*/)		 { assert(0); return 0; }
  inline short	Sin(short /*v*/)	 { assert(0); return 0; }
	inline float  Sin(float v)   { return float(::sin(double(v))); }
  inline double Sin(double v)  { return ::sin(v); }
	inline int		Cos(int /*v*/)		 { assert(0); return 0; }
  inline short	Cos(short /*v*/)	 { assert(0); return 0; }
	inline float  Cos(float v)   { return float(::cos(double(v))); }
  inline double Cos(double v)  { return ::cos(v); }

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


inline float ToDeg(const float &a){return a*180.0f/float(M_PI);}
inline float ToRad(const float &a){return float(M_PI)*a/180.0f;}
inline double ToDeg(const double &a){return a*180.0/M_PI;}
inline double ToRad(const double &a){return M_PI*a/180.0;}

// Generic Callback function:
// Used to make algorithms interumpable
// Return value: true continue, false break
// The second callback is to know where we are (useful for progress bar)
typedef bool CallBack( const char * str );
typedef bool CallBackPos(const int pos, const char * str );

inline bool DummyCallBack( const char * ) {return true;}
inline bool DummyCallBackPos(const int,  const char * ) {return true;}
	
}	// End namespace


#endif