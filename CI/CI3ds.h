#ifndef CI3DS_H
#define CI3DS_H

#include <lib3ds/node.h>

	// Trackbal code by Claudio Rocchini
namespace vcg {

class CI3ds
{
public:
	inline CI3ds( void ) {  }
	inline ~CI3ds( void ) { }

	bool Open(const char *filename);
	void Draw(int frame =0);
	int FrameNum() ;
//	Point3f Center();
	float radius();
private:
	Lib3dsFile *file;
	void CI3ds::renderNode(Lib3dsNode *node);
};


} // end namespace

#endif