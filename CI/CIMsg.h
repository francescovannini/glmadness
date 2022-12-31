#pragma once
class CIMsg
{
	public:
	static void checkGLError(const char* m, const char* title);
	static void checkGLError(const char* m) {checkGLError(m,"GL error");};
};
