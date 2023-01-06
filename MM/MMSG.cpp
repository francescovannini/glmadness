#include <GL/glew.h>
#include "CI/CISG.h"
#include "CI/CICubeMap.h"
#include "CI/CIMsg.h"
#include "MMGrid.h"
#include "MMSG.h"

using namespace vcg ;
using namespace std ;

void CICubeMapNode::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CICubeMapNode>\n";
	xstrm << textureBaseName ;
	xstrm << "</CICubeMapNode>\n";
}

bool CICubeMapNode::XMLRead(QDomElement n, CISG *) 
{
	if(n.tagName() !="CICubeMapNode" || !n.firstChild().isText() ) 
		return false;
	qDebug("< %s > %s ...\n",n.tagName().toStdString().c_str(),n.text().toStdString().c_str());
	QTextStream(&n.text()) >> textureBaseName;
  return true;
}

void CICubeMapNode::drawGL(const float )
{
	if(dirty) initializeGL();
	CIMsg::checkGLError("drawGL");
	// estrae la componente rotazione dalla modelview corrente.
	Matrix44f mm;
	glGetModelViewMatrix(mm);
	mm.a[0][3]=0;
	mm.a[1][3]=0;
	mm.a[2][3]=0;
	mm.a[3][3]=1;
	float dt=1.0f/pow(mm.Det(),1.0f/3.0f);
	Matrix44f ss; ss.Scale(dt,dt,dt);
	rr=mm*ss;
	dt=rr.Det();
	DrawEnvCube(rr);
}


void CIGridNode::XMLWrite(QTextStream &xstrm)
{
	xstrm << "<CIGridNode sx=\"" << sx << "\" sy=\"" << sy << "\" time=\"" << time << "\">\n";
	vector<MMCell>::iterator ii;
	for(ii=g.begin();ii!=g.end();++ii)
		(*ii).XMLWrite(xstrm);
	xstrm << "</CIGridNode>\n";
}

bool CIGridNode::XMLRead(QDomElement en, CISG *) 
{
	if(en.tagName() !="CIGridNode" || !en.firstChild().isText() )
		return false;
	qDebug("< %s > %s ...\n",en.tagName().toStdString().c_str(),en.text().toStdString().c_str());
	sx = en.attribute("sx","10").toInt();
	sy = en.attribute("sy","10").toInt();
	time = en.attribute("time","60").toInt();

	resize(sx,sy);

	vector<MMCell>::iterator ii;
	QTextStream is(en.text().toStdString().c_str());
	for(ii=g.begin();ii!=g.end();++ii)
		(*ii).XMLRead(is);
	return true;
}


CISGNode *MMSG::Allocate(const QString &classname)
{
	CISGNode *ret=CISG::Allocate(classname);		
	if(classname=="CIGridNode") {
		g=new CIGridNode();
		ret=g;
		}
	if(classname=="CICubeMapNode") {
		c=new CICubeMapNode();
		ret=c;
		}
	return ret;
}	

