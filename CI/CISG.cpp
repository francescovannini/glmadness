#include <GL/glew.h>	
#include <iostream>	
#include <qfile.h>

#include "CISG.h"

using namespace vcg;
using namespace std;
QTextStream& operator<< ( QTextStream& os, Matrix44f& m )
{
  for(int i=0; i<4;++i)
		for(int j=0; j<4;++j)
			os<< m[i][j];
	 return os;
}
QTextStream& operator<< ( QTextStream& os, Point3f& m )
{
   os<< m[0] << " " << m[1] << " " << m[2]  << " " ;
	 return os;
}
QTextStream& operator>> ( QTextStream& os, Point3f& m )
{
   os >> m[0] >>  m[1] >> m[2];
	 return os;
}


CISG::CISG(void)
{
}

CISG::~CISG(void)
{
}

void CISG::drawGL(const float DocTime)
{
	root.drawGL(DocTime);
}

void CISG::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISG>\n";
	root.XMLWrite(xstrm);
	xstrm << "</CISG>\n";
}


CISGNode *CISG::Allocate(const QString &classname)
{
	CISGNode *pt=0;
  if(classname=="CISGGroup") pt= new CISGGroup;
  if(classname=="CISGGround") pt= new CISGGround;
  if(classname=="CISGRotation") pt= new CISGRotation;
  if(classname=="CISGTranslation") pt= new CISGTranslation;
  if(classname=="CISGScaling") pt= new CISGScaling;
  if(classname=="CISGTransformation") pt= new CISGTransformation;
  if(classname=="CISGAnimRotation") pt= new CISGAnimRotation;
  if(classname=="CISGAnimZPrecession") pt= new CISGAnimZPrecession;
	return pt;
}

bool CISG::XMLRead(QString filename)
{
	QFile qf(filename);
  if ( !qf.open( QFile::ReadOnly ) )   return false;
	bool ret=XMLRead(qf) ; 
	qf.close();
	return ret;
}

bool CISG::XMLRead(QIODevice &io)
{
 QDomDocument doc;
  if ( !doc.setContent( &io ) )    return false;

  QDomElement docElem = doc.documentElement();
	if(docElem.tagName() != "CISG" ) return false;

  QDomNode xroot = docElem.firstChild();
	if(xroot.isNull()) return false;
	QDomElement xre = xroot.toElement(); // try to convert the node to an element.
  if( xre.tagName()!= "CISGGroup") return false;
  return root.XMLRead(xre,this);
}


/********** GROUP *************/
CISGGroup::~CISGGroup()
{
  iterator i;
	for(i=Sons.begin();i!=Sons.end();++i)
		delete (*i);
}

void CISGGroup::drawGL(const float DocTime)
{
	glPushMatrix();
  iterator i;
	for(i=Sons.begin();i!=Sons.end();++i)
		(*i)->drawGL(DocTime);
	glPopMatrix();
}
void CISGGroup::initializeGL()
{
	glPushMatrix();
  iterator i;
	for(i=Sons.begin();i!=Sons.end();++i)
		(*i)->initializeGL();
	glPopMatrix();
}

void CISGGroup::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGGroup>\n";
	iterator i;
	for(i=Sons.begin();i!=Sons.end();++i)
		(*i)->XMLWrite(xstrm);
	xstrm << "</CISGGroup>\n";
}

bool CISGGroup::XMLRead(QDomElement en, CISG *Base)
{
	Sons.clear();
	if(en.tagName() !="CISGGroup") return false;
  QDomNode n=en.firstChild();
	while( !n.isNull() ) {
		QDomElement e = n.toElement(); // try to convert the node to an element.
    if( !e.isNull() ) {
				CISGNode *np=Base->Allocate(e.tagName());
				if(!np) return false;
				cout << e.tagName().toStdString().c_str() << endl; // the node really is an element.
				np->XMLRead(e,Base);
				Sons.push_back(np);
       }
     n = n.nextSibling();
    }
	return true;
}

/********** CISGTransformation *************/
void CISGTransformation::drawGL(const float ){ glMultMatrix(m); };
void CISGTransformation::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGTransformation>\n";
	xstrm << m;
	xstrm << "</CISGTransformation>\n";
}
bool CISGTransformation::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGRotation" || !n.isText()) return false;
	return true;
}

/********** CISGRotation *************/
void CISGRotation::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGRotation>\n";
  	xstrm << Axis << " " << AngleDeg ;
	xstrm << "</CISGRotation>\n";
}

bool CISGRotation::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGRotation" || !n.firstChild().isText() ) 
		return false;
	cout << n.tagName().toStdString().c_str() << ":" << n.text().toStdString().c_str() << endl;
    QTextStream textStream(&n.text());
    textStream >> Axis >> AngleDeg;
  return true;
}

/********** CISGScaling *************/
void CISGScaling::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGScaling>\n";
	xstrm << axis;
	xstrm << "</CISGScaling>\n";
}
bool CISGScaling::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGScaling" || !n.firstChild().isText()) return false;
	cout << n.tagName().toStdString().c_str() << ":" << n.text().toStdString().c_str() << endl;
	QTextStream textStream(&n.text());
    textStream >> axis;
	return true;
}
/********** CISGTranslation *************/
void CISGTranslation::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGTranslation>\n";
  xstrm << V;
	xstrm << "</CISGTranslation>\n";
}

bool CISGTranslation::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGTranslation" || !n.firstChild().isText()) return false;
	cout << n.tagName().toStdString().c_str() << ":" << n.text().toStdString().c_str() << endl;
	QTextStream textStream(&n.text());
    textStream >> V;
	return true;
}
/********** CISGAnimRotation *************/
void CISGAnimRotation ::drawGL(const float DocTime) 
{
	float CurAngleRad=ToRad(StartAngleDeg + DocTime*AngularSpeedDPS);
	glRotatef( CurAngleRad, axis[0],axis[1],axis[2]);  
};
void CISGAnimRotation::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGAnimRotation>\n";
	xstrm << "</CISGAnimRotation>\n";
}
bool CISGAnimRotation::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGAnimRotation" || !n.isText()) return false;
	return true;
}

/********** CISGAnimZPrecession *************/
CISGAnimZPrecession::CISGAnimZPrecession()
{
	StartAngleDeg=0;
	AngularSpeedDPS=50;
	DeclinationDeg=30;
}
 	
void CISGAnimZPrecession::drawGL(const float DocTime) 
{
	// moto di precessione: una rotazione il cui asse ruota intorno all'asse z
	float CurAngleRad=ToRad(StartAngleDeg + DocTime*AngularSpeedDPS);
	glRotated( DeclinationDeg,cos(CurAngleRad), sin(CurAngleRad),0);  
};

void CISGAnimZPrecession::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGAnimZPrecession>\n";
  xstrm << StartAngleDeg << AngularSpeedDPS << DeclinationDeg;
	xstrm << "</CISGAnimZPrecession>\n";
}
bool CISGAnimZPrecession::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGAnimZPrecession" || !n.firstChild().isText()) return false;
	QTextStream(&n.text()) >> StartAngleDeg >> AngularSpeedDPS >> DeclinationDeg;
	return true;
}

/********** CISGGround *************/	
void CISGGround::drawGL(const float )
{
	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glTexCoord2f(0,  0);
	  glVertex3f(-1,-1,0);
	glTexCoord2f( 1, 0);
	  glVertex3f( 1,-1,0);
	glTexCoord2f( 1, 1);
	  glVertex3f( 1, 1,0);
	glTexCoord2f( 0, 1);
	  glVertex3f(-1, 1,0);
	glEnd();
}
void CISGGround::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CISGGround>\n";
	xstrm << "</CISGGround>\n";
}
bool CISGGround::XMLRead(QDomElement n, CISG *)
{
	if(n.tagName() !="CISGGround" || !n.isText()) return false;
	return true;
}

