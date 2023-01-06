#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdio>
#include <GL/glew.h>
#include <SDL.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdom.h>
#include <QDateTime>
#include <QDir>
#include <QGL>
#include <utility>
#include <vcg/Point3.h>
#include <MM/MMSG.h>
#include "../bmf/BMF_font.h"

GLuint menuTex,consoleTex,chooseLevelTex,creditsTex;
MMSG sg;
float rx,ry;
float scale;
BMF_Font *bfont;

float pi;
bool gamequit=false;

void Enter2D(void) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
}

void Exit2D(void) {
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void showGameOver(int gs){

	BMF_Begin(bfont);
	BMF_SetVAlign(BMF_ALIGN_CENTER);
	BMF_SetHAlign(BMF_ALIGN_CENTER);
	
	if (gs>0) {		/* Schermata di vittoria */
		BMF_SetScale(2.3f, 2.3f);
		BMF_SetPos(320,280);
		BMF_SetColor(1, 0.4f, 0.15f, 1);
		BMF_Print("CONGRATULATIONS!");
		BMF_SetScale(1.5f, 1.5f);
		BMF_SetPos(320,240);
		BMF_SetColor(0, 0, 0, 1);
		BMF_Print("LEVEL COMPLETED!");
	} else {		/* Schermata di game over */
		BMF_SetScale(4, 4);
		BMF_SetColor(0, 0, 0, 1);
		BMF_SetPos(320,280);
		BMF_Print("GAME");
		BMF_SetPos(320,240);
		BMF_Print("OVER");
		BMF_SetScale(1.3f, 1.5f);
		BMF_SetPos(320,205);
		switch (gs){
			case -1:
				BMF_Print("BALL BROKEN!");
			break;

			case -2:
				BMF_Print("BALL OUT OF PLANE!");
			break;

			case -3:
				BMF_Print("TIMEOUT!");
			break;
		}
	}
	
	BMF_SetScale(1, 1);
	BMF_SetColor(0, 0, 0, 0.5);
	BMF_SetPos(320,180);
	BMF_Print("PRESS ESC TO CONTINUE");
	BMF_End();
	
}
void drawConsole(int time){
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,consoleTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); 
	glVertex3f(0,0,1);
	glTexCoord2f(1,0);
	glVertex3f(640,0,1);
	glTexCoord2f(1,1);
	glVertex3f(640,32,1);
	glTexCoord2f(0,1);
	glVertex3f(0,32,1);
	glEnd();
	BMF_Begin(bfont);
	BMF_SetScale(1.2f, 1.2f);
	if (time<10) 
		BMF_SetColor(1, 0, 0, 1);
	else
		BMF_SetColor(0, 0, 0, 1);
	BMF_SetPos(530,10);
	BMF_Print("TIME: %i",time);
	BMF_End();
}

void DrawGLScene(int gs, int rtime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	float spec[4]={.4f,.4f,.4f,1};
	float shin = 1;
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR , spec);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS , shin);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	float lightpos[4]={0,0,1,0};
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0,GL_POSITION,lightpos); 
	
	if (rx>360) rx=360.0f-rx;
	if (rx<0) rx=360.0f+rx;

	if (ry>89.0f) ry=89.0f;
	if (ry<-89.0f) ry=-89.0f;

	/* 
		Anziche' usare una rotazione per inclinare il piano
		si potrebbe spostare la camera lungo una semicirconferenza 
		il cui centro e' la pallina.

		gluLookAt(sin((pi/2.0f)-(0.01745*ry)),sin(0.01745*ry),0,0,0,0,0,1,0);
		glPushMatrix();
		glRotatef(rx,0,1,0);
		glScalef(scale,scale,scale);
		sg.drawGL(clock()/1000.0f);
		glPopMatrix();
	*/

	gluLookAt(1.0f,0,0,0,0,0,0,1,0);
	glPushMatrix();
	glRotatef(-ry,0,0,1);
	glRotatef(rx,0,1,0);
	glScalef(scale,scale,scale);
	sg.drawGL(clock()/1000.0f);
	glPopMatrix();
	
	Enter2D();
		if (gs!=0) {
			showGameOver(gs);
		}
		drawConsole(rtime);
	Exit2D();
	
	SDL_GL_SwapBuffers();
}

GLuint loadImage(QString filename){
	QImage tt,tx;
	GLuint tex;

	if (tt.load(filename)) {
		glGenTextures(1, &tex);
		tx = QGLWidget::convertToGLFormat(tt);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tx.width(), tx.height(), GL_RGBA, GL_UNSIGNED_BYTE, tx.bits());
		return tex;
	} else
		fprintf(stderr,"%s image not found!\n",filename.toStdString().c_str());
	return 0;
}

void glInit()
{
	glMatrixMode(GL_PROJECTION);   
	glLoadIdentity ();
	float ratio=640.0f/480.0f;
	gluPerspective(45,ratio,0.1f,60);
	glViewport (0, 0, 640, 480); 
	glMatrixMode(GL_MODELVIEW);
	
	glewInit();
	// Abilita il la possibilita' di cambiare alcune componenti colore del 
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

	// abilita i texture environment necessari
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
}

void playLevel(QString levelfile){
	float ctime,otime;
	int mousex,mousey;
	bool mousedrag=false;
	int gameState=0;
	SDL_Event event;
	int endTime;

	/* Caricamento livello */
 	sg.XMLRead(std::move(levelfile));
	sg.root.initializeGL();
	sg.g->generateFace();
	sg.g->setBallAtStart();
	sg.g->inGame = true;

	int rtime;
	int done = 0;
	otime = 0;

	/* Istante nel quale finira' il tempo dello schema */
	endTime=clock()/1000 + sg.g->time;

	/* Zoom e rotazioni predefinite */
	rx=135.0f;
	ry=45.0f;
	scale=1.0f;

	while ((!done) && (!gamequit)){
		if (SDL_PollEvent(&event)) {
			switch(event.type){ 		
				case SDL_QUIT:
					gamequit=true;
				break;
				case SDL_MOUSEBUTTONDOWN:
					if ((event.button.button==SDL_BUTTON_LEFT) && (gameState==0)) 
						if (!mousedrag) {
							mousedrag=true;
							mousex=event.motion.x;
							mousey=event.motion.y;
						}
				break;

				case SDL_MOUSEBUTTONUP:
					if ((gameState==0) && (mousedrag)) {
						int tx, ty;
						tx=event.motion.x;	
						ty=event.motion.y;
						mousedrag=false;
						sg.g->setBallMouseVector(mousex,mousey,tx,ty);		
					}
				break;
						
				case SDL_KEYDOWN: {
					switch(event.key.keysym.sym){

						case SDLK_ESCAPE :
							done=1;
						break;		

						case SDLK_UP :
							ry=ry+1.0f;
						break;
						
						case SDLK_DOWN :
							ry=ry-1.0f;
						break;
						
						case SDLK_LEFT :
							rx=rx+1.0f;
						break;
						
						case SDLK_RIGHT :
							rx=rx-1.0f;
						break;

						case SDLK_PAGEUP :
							if (scale<5.0f) scale*=1.1f;
						break;
						
						case SDLK_PAGEDOWN :
							if (scale>0.1f) scale/=1.1f;
						break;
					
					} /* END switch(event.key.keysym.sym) */
				}
				break; 

			} /* END switch(event.type) */

		} else {  /* di if (SDL_PollEvent(&event)) */
		
			ctime= clock_ms(&initial_time) / 1000.0f;
					
			if (gameState==0) {
				rtime=endTime - ctime;
				gameState=sg.g->updateSpherePos(ctime);
				if (rtime<=0) gameState=-3;
			} else {
				/* Spin dello schema alla fine del gioco */
				if (otime)
					rx=rx-(ctime-otime)*5.0f;
				otime=ctime;

				if (gameState>0){	/* Schema completato */
					sg.g->ballWin(ctime);
				} 
			}

			DrawGLScene(gameState,rtime, &initial_time);
		}
  }
}
void showCredits(){
	bool creditsDone=false;
	SDL_Event evt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,creditsTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); 
	glVertex3f(0,0,0);
	glTexCoord2f(1,0);
	glVertex3f(640,0,0);
	glTexCoord2f(1,1);
	glVertex3f(640,480,0);
	glTexCoord2f(0,1);
	glVertex3f(0,480,0);
	glEnd();

	SDL_GL_SwapBuffers();
	
	while ((!creditsDone) && (!gamequit)){
		if(SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					gamequit=true;
				break;
				case SDL_KEYDOWN: 
					creditsDone=true;
				break;
			}
		}
	}				
}
void drawChooseLevelMenu(QStringList files, int sel){
	int py=200;
	int px=320;
	unsigned int x,c;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,chooseLevelTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); 
	glVertex3f(0,0,0);
	glTexCoord2f(1,0);
	glVertex3f(640,0,0);
	glTexCoord2f(1,1);
	glVertex3f(640,480,0);
	glTexCoord2f(0,1);
	glVertex3f(0,480,0);
	glEnd();

	BMF_Begin(bfont);
	BMF_SetScale(1, 1);
	BMF_SetColor(0, 0, 0, 1.0f);
	BMF_SetVAlign(BMF_ALIGN_CENTER);
	BMF_SetHAlign(BMF_ALIGN_CENTER);

	c=0;	/* Disegna la lista dei files, il primo (quello selezionato) ha alpha=1 */
	for (x=sel;((x<files.count()) && (c<8));x++){	
		BMF_SetPos(px,py-c*15);
		if (c>0) BMF_SetColor(0, 0, 0, 0.3f);
		BMF_Print("%s",files.at(x).toStdString().c_str());
		c++;
	}	
	BMF_End();

	SDL_GL_SwapBuffers();
}

bool chooseLevelMenu(QString &fn){
	QDir d("../levels/");
	
	bool menuDone=false;
	bool confirmed=false;
	
	QStringList dirList;
   	SDL_Event evt;
	unsigned int selectedEntry=0;	

	printf("Entering levels menu...\n");
	
	dirList=d.entryList(QStringList("*.xml"));
	
	while ((!menuDone) && (!gamequit)){
		if(SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					gamequit=true;
				break;
				case SDL_KEYDOWN: {
					switch(evt.key.keysym.sym ) {
						case SDLK_UP:
							if (selectedEntry>0) selectedEntry--;
						break;
						case SDLK_DOWN:
							if (selectedEntry<dirList.count()-1) selectedEntry++;
						break;
						case SDLK_ESCAPE:
							menuDone=true;
							confirmed=false;
						break;
						case SDLK_RETURN:
							menuDone=true;
							confirmed=true;
						break;
					}

				break;
				}
			}
			drawChooseLevelMenu(dirList,selectedEntry);
		}
	}
	if (confirmed) {
		fn="../levels/";
		fn.append(dirList.at(selectedEntry).toStdString().c_str());
	}
	printf("Exiting levels menu...\n");
	return confirmed;
}

void drawMainMenu(int sel){
	int py=201;
	int px=160;

	py-=(sel * 50);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,menuTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); 
	glVertex3f(0,0,0);
	glTexCoord2f(1,0);
	glVertex3f(640,0,0);
	glTexCoord2f(1,1);
	glVertex3f(640,480,0);
	glTexCoord2f(0,1);
	glVertex3f(0,480,0);
	glEnd();

	/* Disegna la selezione (due rettangoli neri) */
	glColor3f(0,0,0);
	glBegin(GL_QUADS);
	glVertex3f(px,py,1);
	glVertex3f(px+320,py,1);
	glVertex3f(px+320,py+5,1);
	glVertex3f(px,py+5,1);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(px,py+35,1);
	glVertex3f(px+320,py+35,1);
	glVertex3f(px+320,py+5+35,1);
	glVertex3f(px,py+5+35,1);
	glEnd();

	SDL_GL_SwapBuffers();
}

void mainMenu(){
	bool menuDone=false;
	SDL_Event evt;
	int selectedOption=0;

	printf("Entering main menu...\n");

	Enter2D();

	while ((!menuDone) && (!gamequit)){
		if(SDL_PollEvent(&evt))
			switch (evt.type) {
				case SDL_QUIT:
					gamequit=true;
				break;
				case SDL_KEYDOWN: {
					switch(evt.key.keysym.sym ) {
						case SDLK_UP:
							selectedOption--;
						break;
						case SDLK_DOWN:
							selectedOption++;
						break;
						case SDLK_ESCAPE:
							menuDone=true;					
						break;
						case SDLK_RETURN:
							if (selectedOption==0) {
								QString fn;
								bool r;
								r=chooseLevelMenu(fn);
								if (r) {
									Exit2D();
									printf("Playing level %s\n",fn.data());
									playLevel(fn);
									Enter2D();
								}	
							}
							if (selectedOption==1) showCredits();
							if (selectedOption==2) menuDone=true;
						break;
					}

					if (selectedOption>2) selectedOption=0;
					if (selectedOption<0) selectedOption=2;

				break;
				}
			}
		drawMainMenu(selectedOption);
	}	
}

int main(int argc, char **argv) 
{
	bool noFS=false;
	int a;
	QString f;

	pi=4.0f*atan(1.0f);
	
    printf("GLMadness started!\n");
	
	if (!SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL))
		fprintf(stderr,"Unable to set key autorepeat...\n");
	
	SDL_WM_GrabInput(SDL_GRAB_ON);
	
	for (a=1;(a<argc) && (a<=2);a++){
		if (!strcmp(argv[a],"-nofs")){
			printf("Running in windowed mode...\n");
			noFS=true;
		} else 
			f=QString(argv[a]);
	}

	/* Initialize SDL for video output */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	/* Specify the requirements for the gl window*/
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);   
  
	/* Create a 640x480 OpenGL screen */
	if ( SDL_SetVideoMode(640, 480, 0, SDL_OPENGL | (noFS ? 0 : SDL_FULLSCREEN) ) == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	
	/* Load fonts and images */
	printf("Loading font...\n");
	bfont = BMF_Load("../bmf/data/fnt1.bmf");
	printf("Loading images...\n");
	consoleTex=loadImage("../data/console.png");
	menuTex=loadImage("../data/mainmenu.png");
	chooseLevelTex=loadImage("../data/chooselevel.png");
	creditsTex=loadImage("../data/credits.png");

	glInit();
	if (f.isEmpty()) 
		mainMenu();
	else
		playLevel(f);
	SDL_Quit();
	return 0;
}
