#include <GL/glew.h>
#include <math.h>
#include <vcg/Point3.h>
#include <algorithm>
#include <vcg/Matrix44.h>
#include "CITrackBall.h"
using namespace vcg;
/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
#define TRACKBALLSIZE  (0.8f)
#define DELTASPIN		4

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
static float TBProjectToSphere(float r, float x, float y)
{
    float d, t, z;

    d = (float) sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
	z = (float) sqrt(r*r - d*d);
    } else {           /* On hyperbola */
        t = r / 1.41421356237309504880f;
        z = t*t / d;
    }
    return z;
}

/*
 * Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitued will
 * renormalize them.
 *
 * Note: See the following for more information on quaternions:
 * 
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */
static void NormalizeQuat(Point4f &q)
{
    int i;
    float mag;

    mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (i = 0; i < 4; i++) q[i] /= mag;
}

void CITrackball::Init( GLint width, GLint height )
{
    m_giWidth  = width;
    m_giHeight = height;

    CalcQuat( m_curquat, 0.0f, 0.0f, 0.0f, 0.0f );
}

void CITrackball::Reset()
{
	m_glMouseDownX=m_glMouseDownY=0;
    CalcQuat( m_curquat, 0.0f, 0.0f, 0.0f, 0.0f );
}
void CITrackball::Resize( GLint width, GLint height )
{
    m_giWidth  = width;
    m_giHeight = height;
}


void CITrackball::MouseDown( int glMouseDownX, int glMouseDownY, int /*nButton*/ )
{
	m_glMouseDownX = glMouseDownX;
	m_glMouseDownY = glMouseDownY;
}

void CITrackball::MouseUp( int /*posx*/, int /*posy*/ )
{
	/*
	if( (abs(posx-m_glMouseDownX)+abs(posy-m_glMouseDownY)) >=DELTASPIN )
		m_bSpinning = true;
    else
		m_bSpinning = false;

	m_glMouseDownX = posx;
    m_glMouseDownY = posy;
	*/
}


void CITrackball::CalcRotMatrix(Matrix44f &matRot, int posx, int posy )
{
		// If mouse has moved since button was pressed, change quaternion.
	if (posx != m_glMouseDownX || posy != m_glMouseDownY)
    {
			// negate all params for proper operation with glTranslate(-z)
		CalcQuat(m_lastquat,
			-(2.0f * ( m_giWidth - m_glMouseDownX ) / m_giWidth - 1.0f),
            -(2.0f * m_glMouseDownY / m_giHeight - 1.0f),
            -(2.0f * ( m_giWidth - posx ) / m_giWidth - 1.0f),
            -(2.0f * posy / m_giHeight - 1.0f)
        );
		m_bSpinning = true;
    }
    else
		m_bSpinning = false;

	m_glMouseDownX = posx;
    m_glMouseDownY = posy;

	if (m_bSpinning)
		AddQuats(m_lastquat, m_curquat, m_curquat);
	BuildRotMatrix(matRot, m_curquat);
}

void CITrackball::CalcRotMatrix( Matrix44f & matRot  )
{
	if (m_bSpinning)
		AddQuats(m_lastquat, m_curquat, m_curquat);
	BuildRotMatrix(matRot, m_curquat);
}

void CITrackball::CalcScaVector( GLfloat v[3], int /*posx*/, int posy )
{
	if (posy != m_glMouseDownY)
	{
		v[2] += /* 2.0f* */ float(posy-m_glMouseDownY)/m_giHeight;
		m_glMouseDownY = posy;
	}
}

void CITrackball::CalcTraVector( GLfloat v[3], int posx, int posy )
{
	if (posx!= m_glMouseDownX || posy != m_glMouseDownY)
	{
		v[0] -= /* 2.0f* */ float(m_glMouseDownX-posx)/m_giWidth;
		v[1] += /* 2.0f* */ float(m_glMouseDownY-posy)/m_giHeight;
		m_glMouseDownX = posx;
		m_glMouseDownY = posy;
	}
}

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 * 
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void CITrackball::CalcQuat(Point4f &q, float p1x, float p1y, float p2x, float p2y)
{
    Point3f a; /* Axis of rotation */
    float phi;  /* how much to rotate about axis */
    Point3f p1 , p2 , d ;
    float t;

    if (p1x == p2x && p1y == p2y) {
				/* Zero rotation */
  			q=Point4f(0,0,0,1.0f); 
        return;
    }

    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */
    p1=Point3f(p1x,p1y,TBProjectToSphere(TRACKBALLSIZE,p1x,p1y));
    p2=Point3f(p2x,p2y,TBProjectToSphere(TRACKBALLSIZE,p2x,p2y));

    /*
     *  Now, we want the cross product of P1 and P2
     */
    a=p2^p1;

    /*
     *  Figure out how much to rotate around that axis.
     */
    d=p1-p2;
    t = d.Norm() / (2.0f*TRACKBALLSIZE);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0f) t = 1.0f;
    if (t < -1.0f) t = -1.0f;
    phi = 2.0f * (float) asin(t);

    AxisToQuat(a,phi,q);
}

/*
 *  Given an axis and angle, compute quaternion.
 */
void CITrackball::AxisToQuat(Point3f &a, const float phi, Point4f &q)
{
    a.Normalize();
    q=Point4f(a[0],a[1],a[2],1.0f);
    q*=(float) sin(phi/2.0f);
    q[3] = (float) cos(phi/2.0f);
}


/*
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into dest.
 * 
 * This routine also normalizes the result every RENORMCOUNT times it is
 * called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as dest (or each other).
 */

#define RENORMCOUNT 97

void CITrackball::AddQuats(const Point4f &q1, const Point4f &q2, Point4f &dest)
{
    static int count=0;
    Point3f t1,t2,t3,tf;
		float d;

		t1[0] = q1[0];
		t1[1] = q1[1];
		t1[2] = q1[2];

		t2[0] = q2[0];
		t2[1] = q2[1];
		t2[2] = q2[2];

		d  = t1 * t2;
		t3 = t2 ^ t1;
		t1 *= q2.V(3);
		t2 *= q1.V(3);
		tf = t1 + t2 +t3;

		dest.V(0) = tf[0];
		dest.V(1) = tf[1];
		dest.V(2) = tf[2];
		dest.V(3) = q1.V(3) * q2.V(3) - d;
		

    if (++count > RENORMCOUNT) {
        count = 0;
        NormalizeQuat(dest);
    }
}

/*
 * Build a rotation matrix, given a quaternion rotation.
 *
 */
void CITrackball::BuildRotMatrix(Matrix44f &m, Point4f &q)
{
    m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
    m[1][0] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
    m[2][0] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
    m[3][0] = 0.0f;
				
    m[0][1] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
    m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
    m[2][1] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
    m[3][1] = 0.0f;
				
    m[0][2] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
    m[1][2] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
    m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
    m[3][2] = 0.0f;
				
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}
