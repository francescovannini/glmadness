#pragma once
#include <list>
#include <vcg/Matrix44.h>
#include <qtextstream.h>
#include <qdom.h>
namespace vcg {

class CISG;

class CISGNode 
{
public:
	virtual	~CISGNode(void){};
	
	virtual void drawGL(const float DocTime)=0;
	virtual void initializeGL(){};
	virtual void XMLWrite(QTextStream &xstrm)=0;
	virtual bool XMLRead(QDomElement n, CISG *Base)=0;
};

class CISGGroup :public CISGNode 
{
	public:
  virtual ~CISGGroup();
	typedef std::list<CISGNode *>::iterator iterator;
  
  
	std::list<CISGNode *> Sons;
	virtual void drawGL(const float DocTime);
	virtual void initializeGL();
	virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);

};

class CISGTransformation :public CISGNode 
{
	public:
  Matrix44f m;
	virtual void drawGL(const float );
	virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);

};

class CISGTranslation :public CISGNode 
{
	public:
	CISGTranslation::CISGTranslation() :V(Point3f(0,0,0)){};
	CISGTranslation::CISGTranslation(Point3f &v): V(v) {};
  Point3f V;
	virtual void drawGL(const float ) { glTranslate(V); };
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);

};

class CISGRotation :public CISGNode 
{
	public:
	CISGRotation::CISGRotation() :Axis(Point3f(0,0,1)),AngleDeg(0){};
	CISGRotation::CISGRotation(float ad, Point3f &v): Axis(v),AngleDeg(ad) {};

  Point3f Axis;
	float AngleDeg; 
  virtual void drawGL(const float ){ glRotatef(AngleDeg,Axis[0],Axis[1],Axis[2]); };
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);
};

class CISGScaling :public CISGNode 
{
	public:
		CISGScaling():axis(1,1,1){};
		CISGScaling(const float v):axis(v,v,v){};
		CISGScaling(Point3f &v):axis(v){};

  Point3f axis;
	virtual void drawGL(const float ){glScale(axis);};
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);
};

class CISGAnimRotation :public CISGNode 
{
	public:
  Point3f axis;
	float AngularSpeedDPS; //Degree Per Sec;
  float StartAngleDeg; 
	virtual void drawGL(const float DocTime);
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);
};

class CISGAnimZPrecession :public CISGNode 
{
	public:
  CISGAnimZPrecession();
 
  float DeclinationDeg;
	float AngularSpeedDPS; //Degree Per Sec;
  float StartAngleDeg; 
	virtual ~CISGAnimZPrecession(){};
	virtual void drawGL(const float DocTime);
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);
};

class CISGGround :public CISGNode 
{
	public:
		CISGGround(){};
 
	virtual ~CISGGround(){};
	virtual void drawGL(const float );
  virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QDomElement n, CISG *Base);
};



// Minimal Scene graph
// Solo i gruppi definiscono uno scope di trasformazioni
// l'ordine dei figli e' significativo.
class CISG
{
public:
	CISG(void);
	~CISG(void);
	void drawGL(const float DocTime);
	CISGGroup root;
	void XMLWrite(QTextStream &xstrm);
	void XMLRead(QDomDocument &xd);
	virtual CISGNode *CISG::Allocate(const QString &classname);
	bool XMLRead(QString filename);
	bool XMLRead(QIODevice &io);
};


} // end namespace