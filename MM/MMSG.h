#pragma once
#include "CI/CISG.h"
#include "CI/CICubeMap.h"
#include "MMGrid.h"

namespace vcg {
class CICubeMapNode : public CICubeMap, public CISGNode
{
public:
	CICubeMapNode(){rr.SetIdentity();dirty=true;}
	virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement en, CISG *Base);
	virtual void drawGL(const float );
	virtual void  initializeGL(){Load(textureBaseName); dirty=false;}
	QString textureBaseName;
	bool dirty; // se c'e' bisogno di reinizializzare la cubemap (ad es. perche' e' cambiato il nome.)
	Matrix44f rr; // la matrice di rotazione da applicare alla cubemap di solito si prende dallo sg.
};

class CIGridNode : public MMGrid, public CISGNode
{
public:
	virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement en, CISG *Base);
	virtual void drawGL(const float ){ MMGrid::drawGL(); };
	virtual void initializeGL(){initGL();}
};

// Scene Graph specializzato per tenere anche un unico Grid;
class MMSG : public CISG
{
public :
	virtual CISGNode *Allocate(const QString &classname);

	CIGridNode *g;
	CICubeMapNode *c;
};


}// end namespace