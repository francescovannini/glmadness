#include <GL/glew.h>
#include <CI/CIMsg.h>

void CIMsg::checkGLError(const char* m, const char* title) {

  QString message(m);

  switch(glGetError()) {

  case GL_NO_ERROR:
    return;

  case GL_INVALID_ENUM:
    message+=(": invalid enum");
		QMessageBox::warning(0, title,message);
    break;

  case GL_INVALID_VALUE:
    message+=(": invalid value");
    QMessageBox::warning(0, title,message);
    break;

  case GL_INVALID_OPERATION:
    message+=(": invalid operation");
    QMessageBox::warning(0, title,message);
    break;

  case GL_STACK_OVERFLOW:
    message+=(": stack overflow");
    QMessageBox::warning(0, title,message);
    break;

  case GL_STACK_UNDERFLOW:
    message+=(": stack underflow");
    QMessageBox::warning(0, title,message);
    break;

  case GL_OUT_OF_MEMORY:
    message+=(": out of memory");
    QMessageBox::warning(0, title,message);
    break;
  }

}
