#define NUMVERTEXNORMALS 162

#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_vert		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_vert * 4 + 128)

#include <string>

class CIMd2
{
private: 
	typedef unsigned char byte;
class Header
{ public:
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numvert; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
} ;

class Md2CompressVertex
{ public:
   byte v[3];
   byte lightNormalIndex;
} ;

class Md2Vertex
{ public:
  vcg::Point3f v,n;
};

class Md2Triangle
{ public:
   short vertexIndices[3];
   short textureIndices[3];
};

class Md2TexCoord
{ public:
   short s, t;
};

class Md2CompressFrame
{ public:
   vcg::Point3f scale;
   vcg::Point3f translate;
   char name[16];
//	 Md2CompressVertex *vert;
} ;


class MD2Frame
{ public:
   char name[16];
	 vector<Md2Vertex> vert;
} ;
class MD2GLCommand
 {public:
   float s, t;
   int vertexIndex;
};
/*

typedef struct
{
	md2_header_t			header;
	md2_skin_t				*skins;
	md2_textureCoordinate_t	*texCoords;
	md2_triangle_t			*triangles;
	md2_frame_t				*frames;
	int						*glCommandBuffer;
} Info;

typedef struct animation_s {
	char	nameAnimation[16];
	int		firstFrame;
	int		lastFrame;
} animation_t;
public:
*/
public:
	CIMd2(){ti=0;CurTime=0;};
~CIMd2(){};
Header header;
// Dati animazione
float FPS;
int StartFrame;
int EndFrame;
float CurFrame;
float CurTime;



int			numFrames, curFrame, nextFrame, nextiFrame;
//int			animStatus, animSleep, animSpeed, curAnim;
int			interp;
//
	vector<string> skins;
	vector<Md2TexCoord> texCoords;
	vector<Md2Triangle> triangles;
	vector<MD2Frame> frames;
	vector<int> glcommands;
	GLuint ti; // texture index
	string tex_name;
	void SetNormal(const int i, vcg::Point3f &n) const;
	bool Load(const char *basename, const char *texturename);
	void DrawFrame(int frm);
	void DrawFrame(int curFrame, int nextFrame, float pol);
	void DrawTimedFrame(float time);
	void SetAnim(int sf, int ef){CurFrame=float(sf); StartFrame=sf; EndFrame=ef; }
	void SetFPS(float fps) {FPS=fps;};
	void LoadTextures();
};
