#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <gl\gl.h>

#include <lib3ds/file.h>                        
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>
#include <string.h>
#include <config.h>
#include <stdlib.h>
#include <math.h>

#include <vcg/CI3ds.h>
using namespace vcg; 

bool CI3ds::Open(const char *filename)
{
	file=0;
	file=lib3ds_file_load(filename);
	if(!file) return false;
	return true;
}

int CI3ds::FrameNum() {
		if(file) return file->frames;
			else return 0;
	}
void CI3ds::Draw(int current_frame)
 {
	 glPushAttrib(GL_ENABLE_BIT );
	 	glDisable(GL_COLOR_MATERIAL);
	lib3ds_file_eval(file, current_frame);
	 if(file==0) return;
    Lib3dsNode *p;
    for (p=file->nodes; p!=0; p=p->next) {
      renderNode(p);
    }
		glPopAttrib();
  }

void CI3ds::renderNode(Lib3dsNode *node)
{
  ASSERT(file);

  {
    Lib3dsNode *p;
    for (p=node->childs; p!=0; p=p->next) {
      renderNode(p);
    }
  }
  if (node->type==LIB3DS_OBJECT_NODE) {
    if (strcmp(node->name,"$$$DUMMY")==0) {
      return;
    }

    if (!node->user.d) {
      Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
      ASSERT(mesh);
      if (!mesh) {
        return;
      }

      node->user.d=glGenLists(1);
      glNewList(node->user.d, GL_COMPILE);

      {
        unsigned p;
        Lib3dsVector *normalL=( Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);

        {
          Lib3dsMatrix M;
          lib3ds_matrix_copy(M, mesh->matrix);
          lib3ds_matrix_inv(M);
          glMultMatrixf(&M[0][0]);
        }
        lib3ds_mesh_calculate_normals(mesh, normalL);

        for (p=0; p<mesh->faces; ++p) {
          Lib3dsFace *f=&mesh->faceL[p];
          Lib3dsMaterial *mat=0;
          if (f->material[0]) {
            mat=lib3ds_file_material_by_name(file, f->material);
          }

          if (mat) {
            static GLfloat a[4]={0,0,0,1};
            float s;
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
            s = pow(2, 10.0*mat->shininess);
            if (s>128.0) {
              s=128.0;
            }
            glMaterialf(GL_FRONT, GL_SHININESS, s);
          }
          else {
            Lib3dsRgba a={0.2f, 0.2f, 0.2f, 1.0f};
            Lib3dsRgba d={0.8f, 0.8f, 0.8f, 1.0f};
            Lib3dsRgba s={0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_AMBIENT, a);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
            glMaterialfv(GL_FRONT, GL_SPECULAR, s);
          }
          {
            int i;
            glBegin(GL_TRIANGLES);
              glNormal3fv(f->normal);
              for (i=0; i<3; ++i) {
                glNormal3fv(normalL[3*p+i]);
								if(mesh->texels== mesh->points) glTexCoord2fv(mesh->texelL[f->points[i] ] );
                glVertex3fv(mesh->pointL[f->points[i]].pos);
              }
            glEnd();
          }
        }

        free(normalL);
      }

      glEndList();
    }

    if (node->user.d) {
      Lib3dsObjectData *d;

      glPushMatrix();
      d=&node->data.object;
      glMultMatrixf(&node->matrix[0][0]);
      glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
      glCallList(node->user.d);
      /*glutSolidSphere(50.0, 20,20);*/
      glPopMatrix();
    }
  }
}
