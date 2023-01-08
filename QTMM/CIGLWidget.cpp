/****************************************************************
**
** Implementation of CIGLWIdget class
**
****************************************************************/

#include <GL/glew.h>
#include "CIGLWidget.h"
#include "MM/MMSG.h"

#include <QMessageBox>
#include <QDateTime>
#include <QMouseEvent>
#include <qcursor.h>

using namespace vcg;

CIGLWidget::CIGLWidget( QWidget *parent): QGLWidget(parent)
{
	sg=0;
	dirty=false;
	animBall=false;
	zoomFactor=1;
	rubberBanding=false;
	rbStart=Point2i(-1,-1);
}
void CIGLWidget::initializeGL()
{   
	setEditing(false);
	// questa chiamata va fatta almeno una volta per inizializzare le ext opengl.
	GLenum err = glewInit();

	// Set up the rendering context, define display lists etc.:
	glClearColor( 0.0, 1.0, 1.0, 0.0 );
	float spec[4]={.4f,.4f,.4f,1};
	float shin = 1;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR , spec);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS , shin);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE );
	glEnable(GL_COLOR_MATERIAL);
	
	// Abilita il la possibilita' di cambiare il alcune componenti colore del 
	// materiale tramite un semplice glcolor, anziche' fare glMaterial(...)
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Abilita il calcolo dell'illuminazione
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// anche le normali attraversano la pipeline, quindi devono essere  
	// rinormalizzate, prima del calcolo dell'illuminazione, altrimenti 
	// il calcolo dell'illuminazione viene sbagliato.
	glEnable(GL_NORMALIZE); 
	
	// Abilita il test sullo zbuffer e quindi l'eliminazione delle 
	// superfici nascoste
	glEnable(GL_DEPTH_TEST); 	

	// Disabilita lo shading di gouraud per le facce dei poligoni disegnati
	glShadeModel(GL_SMOOTH);
	
	// Se scommentato disegna solo in wireframe
	//glPolygonMode(GL_FRONT,GL_LINE);
	tb.Init(size().width(),size().height());
	rot.SetIdentity();

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if(sg) sg->root.initializeGL();
}

void CIGLWidget::resizeGL( int w, int h )
{
  // setup viewport, projection etc.:
	glMatrixMode (GL_PROJECTION);   
	glLoadIdentity ();
	float ratio=(float)w/(float)h;
	gluPerspective(60,ratio,2,20);
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_MODELVIEW);
	tb.Resize( w,h );
}

void CIGLWidget::setEditing(bool val)
{
	editing=val;
	if(val) setCursor(QCursor(Qt::PointingHandCursor));
	else setCursor(QCursor(Qt::SizeAllCursor));
}

void CIGLWidget::setDirty(bool val)
{
	dirty=val;
}

void CIGLWidget::resetTB()
{
	tb.Reset();
	tb.CalcRotMatrix(rot, 0,0);
}

void CIGLWidget::smooth()
{
	sg->g->smoothSelected();
}

void CIGLWidget::flatten()
{
	sg->g->flattenSelected();
}

void CIGLWidget::paintGL()
{
	QTime tm;
	tm.restart();

	if(dirty) 
		{
			initializeGL();
			dirty=false;
		}

	if(animBall)
		{
			sg->g->updateSpherePos(((float)tm.msecsSinceStartOfDay()) / 1000.0f);
		}

	glMatrixMode (GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT  );
	glLoadIdentity();
	gluLookAt(2,3,4,0,0,0,0,1,0);
	glMultMatrix(rot);
	//glRotatef(90,1,0,0);
	glColor3f(1.0f,.4f,.4f);
	static int cnt=0;
	glScalef(zoomFactor,zoomFactor,zoomFactor);
	
	if(sg) 	sg->drawGL(tm.elapsed()/1000.0f);
	if(rubberBanding && pickPos!=Point2i(0,0)){
			Point2i out;
			bool ret=sg->g->pickGL(pickPos,out);
			if(ret)  
			{
				if(rbStart==Point2i(-1,-1)) 
					rbStart=out;
				Point2i rbEnd= out;
				sg->g->clearSelection();
				sg->g->selectRect(rbStart,rbEnd);
				//if(ret) sg->g->G(out[0],out[1]).selected =! sg->g->G(out[0],out[1]).selected;
				pickPos = Point2i(0,0);
			}
	}
}
void CIGLWidget::mousePressEvent(QMouseEvent*e)
{
  if(!editing) {
		tb.MouseDown( e->x(), e->y(), 0 );
		update();
	}
	else
	{
		pickPos=Point2i( e->x(), e->y());
		rubberBanding=true;
		update();
	}
}

void CIGLWidget::mouseMoveEvent(QMouseEvent*e)
{
	if(!editing) {
		tb.CalcRotMatrix( rot, e->x(), e->y());
		update();
	}
	else
	{
		if(rubberBanding)
			pickPos=Point2i( e->x(), e->y());
	}
}

void CIGLWidget::mouseReleaseEvent(QMouseEvent*e)
{
  if(!editing) {
		tb.MouseUp( e->x(), e->y());
		update();
	}
	else
	{
		rubberBanding=false;
		rbStart=Point2i(-1,-1);
		update();
	}
}
void CIGLWidget::wheelEvent(QWheelEvent*e)
{
	const int WHEEL_DELTA =120;
	int notch = e->delta()/ WHEEL_DELTA;
	if(!editing) {
		for(int i=0; i<abs(notch);++i)
				if(notch<0) zoomFactor*=1.2f;
							 else zoomFactor/=1.2f;
		update();
	}
	else
	{
		sg->g->raiseSelected(notch);
		update();
	}
}
