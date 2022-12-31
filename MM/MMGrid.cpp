#include <GL/glew.h>
#include <QImage>
#include <QGL>
#include <vcg/Point3.h>
#include <QString>
#include <QTextStream>
#include "MMGrid.h"

using namespace std;
using namespace vcg;


/* 
	MMGrid 
*/
MMGrid::MMGrid(){
	resize(10,10);
	setDefaultTexture();
	inGame=false;
}
MMGrid::MMGrid(const int _sx, const int _sy) {
	resize(_sx,_sy);
	setDefaultTexture();
	inGame=false;
}
void MMGrid::setTexture(int s, QString t){
	if (s<TEXARRAYSIZE) 
		texFiles[s]=t;
}
void MMGrid::setDefaultTexture(){
	texFiles[0]="../data/normal.png";
	texFiles[1]="../data/start.png";
	texFiles[2]="../data/finish.png";
	texFiles[3]="../data/glue.png";
	texFiles[4]="../data/ice.png";
	texFiles[5]="../data/vface.png";
}
void MMGrid::drawGL()
{
	glPushMatrix();
	float s=2.0f/max(sx,sy);
	glRotatef(-90,1,0,0);
	if (!inGame) glTranslatef(-1.0f,-1.0f,0); 
	glScalef(s,s,s);
	if (inGame) glTranslatef(-sp.p.x(),-sp.p.y(),-sp.p.z());
	
	vector<MMCell>::iterator ci;
	glGetDoublev(GL_MODELVIEW_MATRIX,mm); 
	glGetDoublev(GL_PROJECTION_MATRIX,mp); 
	glGetIntegerv(GL_VIEWPORT,vp);
		
	for(ci=g.begin();ci!=g.end();++ci)
		(*ci).drawGL(tex[(*ci).type],tex[5]);
	
	sp.drawGL();
	glPopMatrix();
}

bool MMGrid::pickGL(Point2i in, Point2i &pos)
{
	float z;
	glReadPixels(in[0],vp[3]-in[1],1,1,GL_DEPTH_COMPONENT ,GL_FLOAT,&z);
	return pickGL(Point3d(in[0],vp[3]-in[1],z),pos);
}

bool MMGrid::pickGL(Point3d in, Point2i &pos)
{
	Point3d o;
	gluUnProject(in[0],in[1],in[2],mm,mp,vp,&o[0],&o[1],&o[2]);
 	qDebug("picked %f %f %f\n",o[0],o[1],o[2]);
	if(o[0]<0 || o[0]>sx ) return false;
	if(o[1]<0 || o[1]>sy ) return false;
	pos[0]=o[0];
	pos[1]=o[1]; 
	
	return true;
}

void MMGrid::resize(const int _sx, const int _sy) 
{
	sx=_sx; sy=_sy;
	g.resize(sx*sy);
	int i,j;
	for(i=0;i<sx;++i)
		for(j=0;j<sy;++j)
		{
			G(i,j).V[0]=Point3f(i+0,j+0,0);
			G(i,j).V[1]=Point3f(i+1,j+0,0);
			G(i,j).V[2]=Point3f(i+1,j+1,0);
			G(i,j).V[3]=Point3f(i+0,j+1,0);
			G(i,j).ComputeNormal();
			G(i,j).selected=false;
			G(i,j).type=0;
		}
}
void MMGrid::initGL()
{
	QImage tt;
	QImage tx;
    int i;

	glEnable(GL_TEXTURE_2D);

	/* tex array loader */
	for (i=0;i<TEXARRAYSIZE;i++) {
		if (tt.load(texFiles[i])) {
			glGenTextures(1, &tex[i]);
			tx = QGLWidget::convertToGLFormat(tt);
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tx.width(), tx.height(), GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );
		} else	//File mancante */
			tex[i]=0;
	}
}

void MMGrid::clearSelection()
{
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) (*ii).selected=false;
 }

void MMGrid::selectRect(Point2i s, Point2i e)
{
	int i,j;
	if(s[0]>e[0]) swap(s[0],e[0]);
	if(s[1]>e[1]) swap(s[1],e[1]);
	for(i=s[0];i<=e[0];++i)
		for(j=s[1];j<=e[1];++j)
			G(i,j).selected=true;	
}

void MMGrid::raiseSelected(float f)
{
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) 
		if((*ii).selected)
			for(int k=0;k<4;++k)
				(*ii).V[k].z()+=f;
}

void MMGrid::setCellTypeSelection(int t)
{
	vector<MMCell>::iterator ii;
	bool flag=false;
	for (ii=g.begin();ii!=g.end();++ii) 
		if ( ((*ii).selected) && (!flag) ) {
			if (t==1) flag=true;
			(*ii).type=t;
		} else 
			if (t==CELL_TYPE_START) /* Solo uno START ammesso */
				if ((*ii).type==CELL_TYPE_START) (*ii).type=CELL_TYPE_NORMAL;
}

void MMGrid::generateFace()
{
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) 
		(*ii).generateFace();	
}



void MMGrid::smoothSelected()
{
	//  ----- ----- -----
	// | 3 2 | 3 2 | 3 2 | 
	// | 0 1 | 0 1 | 0 1 |
	//  ----- ===== ----- 
	// | 3 2 " 3 2 " 3 2 | 
	// | 0 1 " 0 1 " 0 1 |
	//  ----- ===== ----- 
	// | 3 2 | 3 2 | 3 2 | 
	// | 0 1 | 0 1 | 0 1 |
	//  ----- ----- ----- 

	// I vertici di ogni cella sono indiciati come sopra.
	//
	// L'alg di smooth serve a far si che in una zona selezionata, 
	// tutti i vertici che condividono le stesse xy 
	// (eccetto quelli sul bordo della zona selezionata)
	// abbiano la stessa z (la media delle z medie delle celle coinvolte)
	//

	// 1) si calcola il valor medio e lo si stora in una var temporanea G(i,j).tmpH[k]
	int bp[4][2]={{ 0, 0},{ 1, 0},{1,1},{ 0,1}}; // base position of each vertex of the cell
	int dp[4][2]={{-1, -1},{ 0, -1 },{0,0},{ -1, 0}}; // delta of the four adj of a cell vertex
	int i,j,k,q;
	int cnt;
	float tH;
	for(i=0;i<sx;++i)
		for(j=0;j<sy;++j)
			if(G(i,j).selected)
				for(k=0;k<4;++k) //for each cell vertex;
				{
					tH=0;
					cnt=0;
					for(q=0;q<4;++q) // foreach adj of a cell vertexe
						{ 
              int adjX=i+dp[k][0]+bp[q][0];
              int adjY=j+dp[k][1]+bp[q][1];

							if(adjX>=0 && adjX<sx && adjY>=0 && adjY<sy) 
                {
                  tH+=G(adjX,adjY).avgH();
							    ++cnt;
               }
						}
					G(i,j).tmpH[k]=tH/cnt;
				}
	
	// 1) si aggiorna tutti vertici non di bordo
	for(i=0;i<sx;++i)
		for(j=0;j<sy;++j)
			if(G(i,j).selected)
				for(k=0;k<4;++k) 
        {
          bool tt=true;
	        for(q=0;q<4;++q)
          {
            int adjX=i+dp[k][0]+bp[q][0];
            int adjY=j+dp[k][1]+bp[q][1];
            if(!(adjX>=0 && adjX<sx && adjY>=0 && adjY<sy) ) tt=false;
            else if(!G(adjX,adjY).selected) tt=false;
          }
          if(tt) G(i,j).V[k][2]=G(i,j).tmpH[k]; 
        }
}

void MMGrid::smoothVerticalSelected()
{
	int i,j,k;
	bool insel;
	int vb,ve;

	for(i=0;i<sx;++i){
		insel=false;
		for(j=0;j<sy;++j){
			if(G(i,j).selected){
				if (!insel) {	/* Siamo nella prima cella della selezione, v0 e v1 non si calcolano */
					insel=true;
					G(i,j).tmpH[0]=G(i,j).V[0][2];
					G(i,j).tmpH[1]=G(i,j).V[1][2];
				} else {
					G(i,j).tmpH[0]=(G(i,j-1).V[0][2]+G(i,j).V[3][2])/2;
					G(i,j).tmpH[1]=(G(i,j-1).V[1][2]+G(i,j).V[2][2])/2;
				}
				
				if ((j==sy-1) || (!G(i,j+1).selected)) { /* Siamo nell'ultima cella selezionata/della colonna, v2 e v3 non si calcolano */
					G(i,j).tmpH[2]=G(i,j).V[2][2];
					G(i,j).tmpH[3]=G(i,j).V[3][2];
				} else { 
					G(i,j).tmpH[2]=(G(i,j+1).V[2][2]+G(i,j).V[1][2])/2;
					G(i,j).tmpH[3]=(G(i,j+1).V[3][2]+G(i,j).V[0][2])/2;
				}
			}
		}
	}

	for(i=0;i<sx;++i){
		insel=false; vb=0;ve=3;
		for(j=0;j<sy;++j){
			if(G(i,j).selected){
				if (!insel) {
					insel=true;
					vb=2; ve=3; /*Aggiorna solo v2 e v3 della cella corrente (la prima della selezione) */
				} else {
					vb=0;	
					if (!G(i,j+1).selected) /* La cella e' l'ultima della selezione */
						ve=1;
					else 
						ve=3;
				}
				
				for(k=vb;k<=ve;++k)
						G(i,j).V[k][2]=G(i,j).tmpH[k];
			}
		}
	}
}

void MMGrid::smoothHorizontalSelected()
{
	int i,j;
	bool insel;

	for(j=0;j<sy;++j){
		insel=false;
		for(i=0;i<sx;++i){
			if(G(i,j).selected){
				if (!insel) {	/* Siamo nella prima cella della selezione, v0 e v3 non si calcolano */
					insel=true;
					G(i,j).tmpH[0]=G(i,j).V[0][2];
					G(i,j).tmpH[3]=G(i,j).V[3][2];
				} else {
					G(i,j).tmpH[0]=(G(i-1,j).V[0][2]+G(i,j).V[2][2])/2;
					G(i,j).tmpH[3]=(G(i-1,j).V[3][2]+G(i,j).V[1][2])/2;
				}
				
				if ((i==sx-1) || (!G(i+1,j).selected)) { /* Siamo nell'ultima cella selezionata/della colonna, v2 e v1 non si calcolano */
					G(i,j).tmpH[2]=G(i,j).V[2][2];
					G(i,j).tmpH[1]=G(i,j).V[1][2];
				} else { 
					G(i,j).tmpH[2]=(G(i+1,j).V[2][2]+G(i,j).V[3][2])/2;
					G(i,j).tmpH[1]=(G(i+1,j).V[1][2]+G(i,j).V[0][2])/2;
				}
			}
		}
	}

	for(j=0;j<sy;++j){
		insel=false; 
		for(i=0;i<sx;++i){
			if(G(i,j).selected){
				if (!insel) {
					insel=true;
					/*Aggiorna solo v1 e v2 della cella corrente (la prima della selezione) */
					G(i,j).V[1][2]=G(i,j).tmpH[1];
					G(i,j).V[2][2]=G(i,j).tmpH[2];
				} else {
					G(i,j).V[0][2]=G(i,j).tmpH[0];
					G(i,j).V[3][2]=G(i,j).tmpH[3];
					if (G(i+1,j).selected) {
						G(i,j).V[1][2]=G(i,j).tmpH[1];
						G(i,j).V[2][2]=G(i,j).tmpH[2];
					}
				}
			}
		}
	}
}

void MMGrid::flattenSelected()
{
	// first step compute average height
	int i,j;
	int cnt=0;
	float avgH=0;
	for(i=0;i<sx;++i)
		for(j=0;j<sy;++j)
			if(G(i,j).selected)	{
				avgH+=G(i,j).avgH();
				++cnt;
			}
	avgH/=cnt;
	// second step apply the compute average height
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) 
		if((*ii).selected)			
			for(int k=0;k<4;++k) 
					(*ii).V[k][2]=avgH;
}

void MMGrid::setBallOnSelectionCenter()
{
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) 
		if((*ii).selected)			
		{
			sp.p=((*ii).V[0]+(*ii).V[1]+(*ii).V[2]+(*ii).V[3])/4.0;
			sp.p[2]+=3;
			sp.v=Point3f(0,0,0);
			return;
		}
   sp.p=G(sx/2,sy/2).V[0]+Point3f(0.5,0.5,0);
   sp.p[2]+=3;
   sp.v=Point3f(0,0,0);
}

bool MMGrid::setBallAtStart()
{
	vector<MMCell>::iterator ii;
	for (ii=g.begin();ii!=g.end();++ii) 
		if((*ii).type==CELL_TYPE_START){
			sp.p=((*ii).V[0]+(*ii).V[1]+(*ii).V[2]+(*ii).V[3])/4.0;
			sp.p[2]+=2;
			sp.v=Point3f(0,0,0);
			return true;
		}
   sp.p=Point3f(0.5,0.5,3);
   sp.v=Point3f(0,0,0);
   return false;
}

void MMGrid::setBallMouseVector(int bx, int by, int ex, int ey)
{
	Point3d o1,o2;
	Point3f V;
	bool flag=true;
	float forceScale=40.0f;

	if (!gluUnProject(bx,vp[3]-by,1,mm,mp,vp,&o1[0],&o1[1],&o1[2])) flag=false;
	if (!gluUnProject(ex,vp[3]-ey,1,mm,mp,vp,&o2[0],&o2[1],&o2[2])) flag=false;
	
	if (!flag) {
		fprintf(stderr,"setBallMouseVector: error on gluUnProject\n");
	} else {
		if (ballOnIce) forceScale*=100.0f;
		V.x()=(o2[0] - o1[0])/forceScale;
		V.y()=(o2[1] - o1[1])/forceScale;
		V.z()=0;
		sp.v=sp.v+V;
	}	
}

void MMGrid::ballWin(float docTime)
{
	static float lastTime=0;
	if(!lastTime) lastTime=docTime;
	float dt=docTime-lastTime;
	lastTime=docTime;
	float tot=0;
	const float miniDt=(float)0.010; 
	static bool colliding=false;
	bool collision=false;

	sp.v=Point3f(0,0,sp.v[2]);
	if (sp.v.Norm()<0.01){
		sp.v[2]=5;
	}
	
	while (tot < dt && !collision){
		tot+=miniDt;
		sp.a=Point3f(0,0,-6);
		sp.v = sp.v + sp.a*miniDt;
		sp.p = sp.p + sp.v*miniDt;

		vector<MMCell>::iterator ii;
		Point3f bestpt,pt;
		Point3f bestnt,nt;
		float mindist=10000;
		bool found=false;
		int ft,facetype;
  
		for (ii=g.begin();ii!=g.end();++ii){
			if( (*ii).collideSphere(&sp,pt,nt,mindist,ft) ){
				bestpt=pt;
				bestnt=nt;
				facetype=ft;
				found=true;
			}
		}
	
		/* Collisione con una faccia */
		if (found){
			sp.p=bestpt+bestnt*sp.rad;
			float vel=sp.v*bestnt;
			sp.v=sp.v - bestnt*(2.0f * vel);
			if(sp.v.Norm()<0.01) sp.v.Zero();
		}
	}
}
int MMGrid::updateSpherePos(float docTime)
{
	/*	Aggiorna la posizione della palla nel gioco
		Ritorna:
		   -2:	Palla esplosa perche' fuori dal piano
		   -1:	Palla esplosa per impatto 
			0:	Nessun evento, tutto ok
			1:	Palla sul Finish
	*/

	static float lastTime=0;
	if(!lastTime) lastTime=docTime;
	float dt=docTime-lastTime;
	lastTime=docTime;
	float tot=0;
	const float miniDt=(float)0.010; 
	static bool colliding=false;
	bool collision=false;

	while (tot < dt && !collision){
		tot+=miniDt;
		sp.a=Point3f(0,0,-0.98f);
		sp.v = sp.v + sp.a*miniDt;
		sp.p = sp.p + sp.v*miniDt;

		if ((sp.p[0]>sx) || (sp.p[1]>sy) || (sp.p[0]<0) || (sp.p[1]<0) || (sp.p[2]<0)) return -2;
		vector<MMCell>::iterator ii;
		Point3f bestpt,pt;
		Point3f bestnt,nt;
		float mindist=10000;
		bool found=false;
		int ft,facetype;
  
		for (ii=g.begin();ii!=g.end();++ii){
			if( (*ii).collideSphere(&sp,pt,nt,mindist,ft) ){
				bestpt=pt;
				bestnt=nt;
				facetype=ft;
				found=true;
			}
		}
	
		/* Collisione con una faccia */
		if (found){
			/* Se la velocita' di impatto e' troppo alta la palla si rompe */
			if (-sp.v[2]>sp.maxSpeed) return -1; 
			
			sp.p=bestpt+bestnt*sp.rad;
			float vel=sp.v*bestnt;
			sp.v=sp.v - bestnt*(2.0f * vel * .7);
			if(sp.v.Norm()<0.01) sp.v.Zero();
			
			/* Gestione Finish, Ghiaccio, Colla */
			ballOnIce=false;
			switch (facetype) {
				case CELL_TYPE_FINISH:
					return 1;
				break;
	
				case CELL_TYPE_GLUE:
					sp.v=sp.v * 0.95f;
				break;
					
				case CELL_TYPE_ICE:
					ballOnIce=true;
				break;
			}
		}
	}

	return 0;
}

/* 
	MMFace
*/
MMFace::MMFace(Point3f * v0, Point3f * v1, Point3f * v2)
{
	v[0]=v0;
	v[1]=v1;
	v[2]=v2;
	ComputeE();
}

void MMFace::ComputeE()
{	
	  flags=0;
			// Primo calcolo degli edges
		edge[0] = (*(v[1])); edge[0] -= (*(v[0]));
		edge[1] = (*(v[2])); edge[1] -= (*(v[1]));
		edge[2] = (*(v[0])); edge[2] -= (*(v[2]));
			// Calcolo di plane
		plane.n = edge[0]^edge[1];
		plane.d = plane.n * (*(v[0]));
		plane.Normalize();
		n=plane.n;
			// Calcolo migliore proiezione
		float nx = Abs(plane.n[0]);
		float ny = Abs(plane.n[1]);
		float nz = Abs(plane.n[2]);
		float d;
		if(nx>ny && nx>nz) { flags |= NORMX; d = 1/plane.n[0]; }
		else if(ny>nz)     { flags |= NORMY; d = 1/plane.n[1]; }
		else               { flags |= NORMZ; d = 1/plane.n[2]; }

			// Scalatura spigoli
		edge[0] *= d;
		edge[1] *= d;
		edge[2] *= d;
	}

bool MMFace::Dist(const Point3f & q, float & dist, Point3f & p)
	{
		const float EPSILON = float(- 0.000001);
		float b;
			// Calcolo distanza punto piano
		float d = Distance( plane, q );
		if( d>dist || d<-dist )			// Risultato peggiore: niente di fatto
			return false;

			// Calcolo del punto sul piano
		// NOTA: aggiunto un '-d' in fondo Paolo C.
		Point3f t = plane.n;
		t[0] *= -d;
		t[1] *= -d;
		t[2] *= -d;
		p = q; p += t;
		
	#define PP(i)	(*(v[i]))
	#define E(i)    (edge[i])
    
		switch( flags & (NORMX|NORMY|NORMZ) )
		{
		case NORMX:
			b = E(1)[1]*(p[2] - PP(1)[2]) - E(1)[2]*(p[1] - PP(1)[1]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[1])),(*(v[2])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(2)[1]*(p[2] - PP(2)[2]) - E(2)[2]*(p[1] - PP(2)[1]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[2])),(*(v[0])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(0)[1]*(p[2] - PP(0)[2]) - E(0)[2]*(p[1] - PP(0)[1]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[0])),(*(v[1])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			break;

		  case NORMY:
			b = E(1)[2]*(p[0] - PP(1)[0]) - E(1)[0]*(p[2] - PP(1)[2]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[1])),(*(v[2])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(2)[2]*(p[0] - PP(2)[0]) - E(2)[0]*(p[2] - PP(2)[2]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[2])),(*(v[0])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(0)[2]*(p[0] - PP(0)[0]) - E(0)[0]*(p[2] - PP(0)[2]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[0])),(*(v[1])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			break;

		  case NORMZ:
			b = E(1)[0]*(p[1] - PP(1)[1]) - E(1)[1]*(p[0] - PP(1)[0]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[1])),(*(v[2])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(2)[0]*(p[1] - PP(2)[1]) - E(2)[1]*(p[0] - PP(2)[0]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[2])),(*(v[0])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			b = E(0)[0]*(p[1] - PP(0)[1]) - E(0)[1]*(p[0] - PP(0)[0]);
			if(b<-EPSILON)
			{
				b = PSDist(q,(*(v[0])),(*(v[1])),p);
				if(dist>b) { dist = b; return true; }
				else return false;
			}
			break;
		}

	#undef E
	#undef PP

		dist = fabs(d);
		//dist = Distance(p,q);
		return true;
	}

/* 
	MMCell 
*/
void MMCell::drawGL(GLuint topTex, GLuint sideTex)
{
	/* Se selezionata la cella appare piu' scura */
	if (selected) 
		glColor3f(0.3f,0.3f,0.3f);
	else 
		glColor3f(1,1,1);
	
	glEnable(GL_TEXTURE_2D);

	if (topTex) 
		glBindTexture(GL_TEXTURE_2D,topTex);
	else
		qDebug("Warning! Missing texture %d!\n",topTex);
    
	/* Disegna la faccia orizzontale della cella */
	glBegin(GL_QUADS);
	glNormal(n);
	glTexCoord2f(0,0); 
	glVertex(V[0]);
	glTexCoord2f(1,0);
	glVertex(V[1]);
	glTexCoord2f(1,1);
	glVertex(V[2]);
	glTexCoord2f(0,1);
	glVertex(V[3]);
	glEnd();

	/* Disegna i lati della cella */
	glEnable(GL_TEXTURE_2D);
	if (sideTex) 
		glBindTexture(GL_TEXTURE_2D,sideTex);
	else
		qDebug("Warning! Missing texture %d!\n",sideTex);
		
	glBegin(GL_QUADS);
	glNormal3f(0,-1,0);
	glTexCoord2f(0,V[0][2]);
	glVertex(V[0]);
	glTexCoord2f(0,0);
	glVertex3f(V[0][0], V[0][1], 0);
	glTexCoord2f(1,0);
	glVertex3f(V[1][0], V[1][1], 0);
	glTexCoord2f(1,V[1][2]);
	glVertex(V[1]);
	
	glNormal3f(1,0,0);
	glTexCoord2f(0,V[1][2]);
	glVertex(V[1]);
	glTexCoord2f(0,0);
	glVertex3f(V[1][0], V[1][1], 0);
	glTexCoord2f(1,0);
	glVertex3f(V[2][0], V[2][1], 0);
	glTexCoord2f(1,V[2][2]);
	glVertex(V[2]);
	
	glNormal3f(0,1,0);
	glTexCoord2f(0,V[2][2]);
	glVertex(V[2]);
	glTexCoord2f(0,0);
	glVertex3f(V[2][0], V[2][1], 0);
	glTexCoord2f(1,0);
	glVertex3f(V[3][0], V[3][1], 0);
	glTexCoord2f(1,V[3][2]);
	glVertex(V[3]);

	glNormal3f(-1,0,0);
	glTexCoord2f(0,V[3][2]);
	glVertex(V[3]);
	glTexCoord2f(0,0);
	glVertex3f(V[3][0], V[3][1], 0);
	glTexCoord2f(1,0);
	glVertex3f(V[0][0], V[0][1], 0);
	glTexCoord2f(1,V[0][2]);
	glVertex(V[0]);
	
	glEnd();

}

void MMCell::ComputeNormal()
{
	Point3f e01=V[1]-V[0];
	Point3f e02=V[2]-V[0];
	n=e01^e02;
	n.Normalize();
}

void MMCell::generateFace()
{ 
	ComputeNormal();
	face.clear();
	int k;
	for(k=0;k<4;++k) B[k]=Point3f(V[k][0],V[k][1],0);

	face.push_back(MMFace(&(V[0]),&(V[1]),&(V[2])));
	face.push_back(MMFace(&(V[0]),&(V[2]),&(V[3])));

	if(fabs(V[0][2]-B[0][2])>0.01) face.push_back(MMFace(&(V[0]),&(B[0]),&(V[1])));
	if(fabs(V[1][2]-B[1][2])>0.01) face.push_back(MMFace(&(V[1]),&(B[0]),&(B[1])));
	if(fabs(V[1][2]-B[1][2])>0.01) face.push_back(MMFace(&(V[1]),&(B[1]),&(V[2])));
	if(fabs(V[2][2]-B[2][2])>0.01) face.push_back(MMFace(&(V[2]),&(B[1]),&(B[2])));
	if(fabs(V[2][2]-B[2][2])>0.01) face.push_back(MMFace(&(V[2]),&(B[2]),&(V[3])));
	if(fabs(V[3][2]-B[3][2])>0.01) face.push_back(MMFace(&(V[3]),&(B[2]),&(B[3])));
	if(fabs(V[3][2]-B[3][2])>0.01) face.push_back(MMFace(&(V[3]),&(B[3]),&(V[0])));
	if(fabs(V[0][2]-B[0][2])>0.01) face.push_back(MMFace(&(V[0]),&(B[3]),&(B[0])));
	//qDebug("Generate %i faces\n",face.size());
	
}
void MMCell::XMLWrite(QTextStream &xstrm)
{
	xstrm << V[0][2] << " " << V[1][2] << " " << V[2][2] << " " << V[3][2] << " " << type << " \n";
}	

bool MMCell::XMLRead(QTextStream &xstrm)
{
	xstrm >> V[0][2] >>  V[1][2] >> V[2][2] >> V[3][2] >> type;
	return true;
}	

bool MMCell::collideSphere(MMSphere *s, Point3f &impact, Point3f &normal, float &mindist, int &facetype)
{
	bool found=false;
	impact.Zero();
	normal.Zero();
	
	for(unsigned int i=0;i<face.size();++i){
		float dist=mindist;
		Point3f closest;
		bool fnd=face[i].Dist(s->p,dist,closest);
		if(fnd && fabs(dist)<s->rad){
			dist=fabs(dist);
			impact=closest;
			if(dist<mindist) mindist=dist;
			normal=(s->p-closest);
		 	//qDebug("impact %f %f %f\n",impact[0],impact[1],impact[2]);
			found=true;
			facetype=type;
		}
	}
	normal.Normalize();
	return found;
}

/* 
	MMSphere
*/
void MMSphere::drawGL()
{
	if (!dl) {
		dl=glGenLists(1);
		glNewList(dl,GL_COMPILE);
		glColor3f(1.0f,0.0f,0.0f);
		GLUquadricObj *qobj=gluNewQuadric();
		gluSphere(qobj,rad,12,6);
		glEndList();
    }
	glPushMatrix();
	glTranslate(p);
	glCallList(dl);
	glPopMatrix();
}

