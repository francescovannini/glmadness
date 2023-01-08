/****************************************************************
**
** Definition of CIGLWidget class 
**
****************************************************************/

#ifndef CIGLWIDGET_H
#define CIGLWIDGET_H
#include <GL/glew.h>
#include <qgl.h>
#include <CI/CITrackBall.h>
#include <MM/MMSG.h>


class CIGLWidget : public  QGLWidget 
{ 
	Q_OBJECT	

public:
	CIGLWidget( QWidget *parent=0);
	void initializeGL();
	void resizeGL( int w, int h );
	void paintGL();
	vcg::MMSG *sg;
	bool editing;
	bool dirty;
	bool animBall;
	void resetTB();

public slots:
	void setEditing(bool);
	void setDirty(bool);
	void smooth();
	void flatten();

protected:
	void mousePressEvent(QMouseEvent*e);
	void mouseMoveEvent(QMouseEvent*e);
	void mouseReleaseEvent(QMouseEvent*e);
	void wheelEvent(QWheelEvent*e); 

private:
	vcg::Matrix44f rot;
	vcg::CITrackball tb;
	vcg::Point2i pickPos;
	float zoomFactor;
	bool rubberBanding;
	vcg::Point2i rbStart;
};

#endif
