#ifndef CITRACKBALL_H
#define CITRACKBALL_H
#include <vcg/Matrix44.h>

namespace vcg {
class CITrackball
{
public:
	inline CITrackball( void ) { m_bSpinning = false; }
	inline ~CITrackball( void ) { }
	void Init( GLint width, GLint height );
	void Reset();
	void Resize( GLint width, GLint height );
	void MouseDown( int glMouseDownX, int glMouseDownY, int nButton );
	void MouseUp( int posx, int posy );
	void CalcRotMatrix( Matrix44f& rr, int posx, int posy );
	void CalcRotMatrix( Matrix44f&  matRot );		// Per spin
	void CalcScaVector( GLfloat vecSca[3], int posx, int posy );
	void CalcTraVector( GLfloat vecTra[3], int posx, int posy );
	bool IsSpin( void ) const { return m_bSpinning; }
	void SetSpin( bool bS ) { m_bSpinning = bS; }

	inline Point4f &GetCurQuat() { return m_curquat; }
	inline Point4f &GetLastQuat() { return m_lastquat; }

private:
    GLint	m_giWidth;
    GLint	m_giHeight;
	Point4f   m_curquat;
	Point4f   m_lastquat;
	int		m_glMouseDownX;
	int		m_glMouseDownY;
	bool	m_bSpinning;

	void CalcQuat(Point4f &q, float p1x, float p1y, float p2x, float p2y);
	void AxisToQuat(Point3f &a, const float phi, Point4f &q);
	void AddQuats(const Point4f &q1, const Point4f &q2, Point4f &dest);

	void BuildRotMatrix(Matrix44f &mm, Point4f &q);
};
}

#endif