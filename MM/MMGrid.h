#pragma once
#include <vector>
#include <vcg/Point2.h>
#include <vcg/Plane3.h>
#include <vcg/Matrix44.h>
#include <QString>
#include <QTextStream>
#include <qdom.h> 

#define TEXARRAYSIZE 6

#define	CELL_TYPE_NORMAL 0
#define	CELL_TYPE_START  1
#define	CELL_TYPE_FINISH 2
#define	CELL_TYPE_GLUE   3
#define	CELL_TYPE_ICE    4

namespace vcg {

class MMSphere 
{			/* MaxSpeed rappresenta la resistenza della palla alla caduta dall'alto */
	public:
		MMSphere():p(0,0,0),v(0,0,0),rad(0.25f),maxSpeed(4){dl=0;};	
	float rad;
	Point3f p;
	Point3f v;
	Point3f a;
	GLuint dl;
	int maxSpeed;
	virtual ~MMSphere(){};
	virtual void drawGL();
};

class MMFace
{
public:
	MMFace(){};
	MMFace(Point3f * v0, Point3f * v1, Point3f * v2);
	enum {// Face Orientation Flags, used efficiently compute point face distance  
		NORMX		= 0x00000200,
		NORMY		= 0x00000400,
		NORMZ		= 0x00000800,
	};

	Point3f *v[3];
	Point3f n;
	Point3f edge[3];
	Plane3f plane;
	int flags;
   
	void ComputeE();

	// Point face distance
	bool Dist( const Point3f & q, float & dist, Point3f & p );
};

class MMCell 
{
public:
	bool selected;
	int type;
	GLuint oFaceTex,vFaceTex;
	Point3f V[4];
	Point3f B[4];
	Point3f n;
	float tmpH[4];
	void ComputeNormal();
	void drawGL(GLuint topTex, GLuint sideTex);
	float avgH() {return (V[0][2]+V[1][2]+V[2][2]+V[3][2])/4.0f;}
	std::vector<MMFace> face;
	void generateFace();
	void updateSpherePos(MMSphere *sp);
	bool collideSphere(MMSphere *s,Point3f &impact, Point3f &Normal, float &mindist,int &ft);
 	virtual void XMLWrite(QTextStream &xstrm);
	virtual bool XMLRead(QTextStream &xstrm);
};


class MMGrid 
{
public:
	MMGrid();	
	MMGrid(const int _sx, const int _sy);
	
	/* Array for textures (GL refs and filenames) 

		Slot assignment:
		0: Normal
		1: Start
		2: Finish
		3: Glue
		4: Ice
		5: Vertical cell face 

	*/
	GLuint tex[TEXARRAYSIZE];
	QString texFiles[TEXARRAYSIZE];

	bool inGame;
	int time;
	inline MMCell &G(const int i, const int j){return g[j*sx+i];}
	void setTexture(int s, QString t);
	void setDefaultTexture();
	void initGL();
	void resize(const int sx, const int sy)  ;
	void drawGL();
	bool pickGL(Point2i in, Point2i &pos);
	bool pickGL(Point3d in, Point2i &pos);
	void clearSelection();
	void selectRect(Point2i s, Point2i e);
	void raiseSelected(float f);
	void setCellTypeSelection(int t);
	void smoothSelected();
	void smoothVerticalSelected();
	void smoothHorizontalSelected();
	void flattenSelected();
	void generateCellVertAndFace();
	int updateSpherePos(float dt);
	void setBallOnSelectionCenter();
	bool setBallAtStart();
	void setBallMouseVector(int bx, int by, int ex, int ey);
	void generateFace();
	void MMGrid::ballWin(float docTime);

protected:
	std::vector<MMCell> g;
	int sx,sy;
    bool ballOnIce;
	// for the picking
	double mm[16],mp[16];
	int vp[4]; // the viewport	
	MMSphere sp;
	
};
} //end namespace
