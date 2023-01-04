#include <GL/glew.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include<GL/gl.h>
#include<GL/glu.h>
#include <vcg/Point3.h>
#include <vcg/Matrix44.h>
#include <vector>
#include <qimage.h>
#include <qgl.h>

using namespace vcg;
using namespace std;
#define TRACE printf
#include "CIMd2.h"

bool	CIMd2::Load(const char *model_name, const char *new_tex_name)
{
	tex_name=string(new_tex_name);
	FILE *md2file = fopen(model_name, "rb");

	if (md2file == NULL){
//		TRACE("ERROR: model %s open faild\n", model_name);
					return false;
	}

	TRACE("\n%s -----------------------------\n", model_name);
	fread(&header, 1, sizeof(CIMd2::Header), md2file);

	static char ver[5];
	sprintf(ver, "%c%c%c%c", header.magic, header.magic>>8, header.magic>>16, header.magic>>24);

	if ( strcmp(ver, "IDP2") || header.version != 8){
		TRACE("Bad header for MD2 file.\n");
		fclose(md2file);
		return false;
	}
	
	TRACE("magic: %c%c%c%c\n", header.magic, header.magic>>8, header.magic>>16, header.magic>>24);
	TRACE("version: %d\n", header.version);

	TRACE("skinWidth: %d\n", header.skinWidth);
	TRACE("skinHeight: %d\n", header.skinHeight);
	TRACE("frameSize: %d\n", header.frameSize);
	
	TRACE("numSkins: %d\n", header.numSkins);
	TRACE("numvert: %d\n", header.numvert);
	TRACE("numTexCoords: %d\n", header.numTexCoords);
	TRACE("numTriangles: %d\n", header.numTriangles);
	TRACE("numGlCommands: %d\n", header.numvert);
	TRACE("numFrames: %d\n", header.numFrames);
	
	TRACE("offsetSkins: %d\n", header.offsetSkins);
	TRACE("offsetTexCoords: %d\n", header.offsetTexCoords);
	TRACE("offsetTriangles: %d\n", header.offsetTriangles);
	TRACE("offsetFrames: %d\n", header.offsetFrames);
	TRACE("offsetGlCommands: %d\n", header.offsetGlCommands);
	TRACE("offsetEnd: %d\n", header.offsetEnd);

	numFrames = header.numFrames;
//	curFrame = 0;					// frame 0
	//curAnim = 0;					// anim 0
	//nextFrame = +1;					// next frame
	//animStatus = 2;					// play
	//animSpeed = 0;					// anim speed
	//animSleep = 0;					// anim sleep
	//interp = 0;

	int i, j;

	TRACE("skins ....\n");
	fseek(md2file, header.offsetSkins, SEEK_SET);

	skins.resize(header.numSkins);
  
	for (i=0; i<header.numSkins; i++)
	{
		char buf[64];
		fread(buf, 64, 1, md2file);
		skins[i]=buf;
		TRACE("skins.name: %s\n", skins[i].c_str());
	}
	
	TRACE("texture coordinates ....\n");
	fseek(md2file, header.offsetTexCoords, SEEK_SET);

	texCoords.resize(header.numTexCoords);
	for (i=0; i<header.numTexCoords; i++)
		fread(&texCoords[i], sizeof(Md2TexCoord), 1, md2file);
	
	// for (i=0; i<header.numTexCoords; i++)
	//	TRACE("%3d %3d\n", texCoords[i].s, texCoords[i].t);
	TRACE("triangles ....\n");
	fseek(md2file, header.offsetTriangles, SEEK_SET);
	
	triangles.resize(header.numTriangles);
	
	for (i=0; i<header.numTriangles; i++)
		fread(&triangles[i], sizeof(Md2Triangle), 1, md2file);

	// for (i=0; i<header.numTriangles; i++)
	//  TRACE("%3d %3d %3d\n", triangles[i].vertexIndices[0], triangles[i].vertexIndices[1], triangles[i].vertexIndices[2]);
	
	TRACE("alias frames ....\n");

	fseek(md2file, header.offsetFrames, SEEK_SET);
	frames.resize(header.numFrames);
  vector<Md2CompressVertex> tmpvert(header.numvert);
	for (i=0; i<header.numFrames; i++)
	{
		Md2CompressFrame tt;
		fread(&tt.scale,3,sizeof(float),md2file);
		fread(&tt.translate,3,sizeof(float),md2file);
		fread(&tt.name[0],16,sizeof(char),md2file);
		fread(&tmpvert[0],header.numvert,sizeof(Md2CompressVertex),md2file);
		strcpy(frames[i].name, tt.name);
    frames[i].vert.resize(header.numvert);

		TRACE("frame %3d: %s\n", i, frames[i].name);
		//TRACE("tr: %f: %f %f\n", tt.translate[0], tt.translate[1], tt.translate[2]);
		//TRACE("sc: %f: %f %f\n", tt.scale[0], tt.scale[1], tt.scale[2]);
		for (j=0; j<header.numvert; j++)
		{
			frames[i].vert[j].v[0] = (float) ((int) tmpvert[j].v[0]) * tt.scale[0] + tt.translate[0];
			frames[i].vert[j].v[1] =-((float) ((int) tmpvert[j].v[1]) * tt.scale[1] + tt.translate[1]);
			frames[i].vert[j].v[2] = 24+(float) ((int) tmpvert[j].v[2]) * tt.scale[2] + tt.translate[2];
			
			SetNormal(tmpvert[j].lightNormalIndex,frames[i].vert[j].n);
			frames[i].vert[j].n[1]*=-1.0f;
			
		}
	}

	TRACE("gl command ....\n");
	fseek(md2file, header.offsetGlCommands, SEEK_SET);
	glcommands.resize(header.numGlCommands);
	fread(&glcommands[0], sizeof(int), header.numGlCommands, md2file);

	fclose(md2file);
	
	return true;
}
void	CIMd2:: DrawFrame(int curFrame)
{
	if(triangles.empty()) return;
	if(ti==0) LoadTextures();
		//ti=ilutGLLoadImage((ILstring) tex_name.c_str());

	int i = 0;
	int val = glcommands[i++];
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,ti);
	glColor3f(1.f, 1.f, 1.f);

	while (val != 0)
	{
		int count;

		if (val > 0)		{
			glBegin(GL_TRIANGLE_STRIP);
			count = val;
		}		else		{
			glBegin(GL_TRIANGLE_FAN);
			count = -val;
		}

		while (count--)
		{
			float s =  * (float *) &glcommands[i++];
			float t =  * (float *) &glcommands[i++];
			t=1.0f-t; // patch per rivoltare le texture
			int index = glcommands[i++];

			glTexCoord2f(s, t);
			glNormal(frames[curFrame].vert[index].n);
			glVertex(frames[curFrame].vert[index].v);
		}
		glEnd ();

		val = glcommands[i++];
	}
}

/////////////////////////////

void	CIMd2:: DrawTimedFrame(float curtime)
{
	if(triangles.empty()) return;
	//TRACE("(%3.2f)--", curtime);	
	float delta=curtime-CurTime;
	CurTime=curtime;
	float DeltaFrame=delta*FPS;
	CurFrame+=DeltaFrame;
	while(CurFrame>EndFrame+1) CurFrame=CurFrame-(1+EndFrame-StartFrame);
	assert(CurFrame>StartFrame);
	int next=floor(CurFrame+1);
	if(next>EndFrame) next=StartFrame;
	DrawFrame(floor(CurFrame),next,fmod(CurFrame,1));
}

/////////////////////////////

void CIMd2::LoadTextures()
{
			glGenTextures(1, &ti);
			QString texfilename=tex_name.c_str();
			texfilename.replace(".pcx",".png");
			QImage tt; 	tt.load(tex_name.c_str());
			QImage tx = QGLWidget::convertToGLFormat ( tt);
			glBindTexture(GL_TEXTURE_2D, ti);
			glTexImage2D( GL_TEXTURE_2D, 0, 3, tx.width(), tx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tx.width(), tx.height(), GL_RGBA, GL_UNSIGNED_BYTE, tx.bits() );
}

void	CIMd2:: DrawFrame(int curFrame, int nextFrame, float pol)
{
	if(triangles.empty()) return;
	//TRACE("%i %i (%3.2f)\n", curFrame,  nextFrame,  pol);
	if(ti==0) LoadTextures();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,ti);
	

	int i = 0;
	float pol1= 1-pol;
	int val = glcommands[i++];
	
  glColor3f(1.f, 1.f, 1.f);

	while (val != 0)
	{
		int count;

		if (val > 0){
			glBegin(GL_TRIANGLE_STRIP);
			count = val;
		}	else {
			glBegin(GL_TRIANGLE_FAN);
			count = -val;
		}

		while (count--)
		{
			float s =  * (float *) &glcommands[i++];
			float t =  * (float *) &glcommands[i++];
			t=1.0f-t; // patch per rivoltare le texture
			int index = glcommands[i++];

			glTexCoord2f(s, t);
			glNormal(frames[curFrame].vert[index].n*pol1 + frames[nextFrame].vert[index].n*pol);
			glVertex(frames[curFrame].vert[index].v*pol1 + frames[nextFrame].vert[index].v*pol);
		}

		glEnd ();

		val = glcommands[i++];
	}
}

