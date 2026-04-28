#ifndef MESHVIEWER_APPLE_GLUT_COMPAT_H
#define MESHVIEWER_APPLE_GLUT_COMPAT_H

/* Shim autour du GLUT Apple (<GLUT/glut.h>) : glutBitmapString / molette. */

#include <GLUT/glut.h>

static inline void glutBitmapString(void *font, const unsigned char *string)
{
	while (*string)
	{
		glutBitmapCharacter(font, *string);
		++string;
	}
}

typedef void (*GLUTmousewheelfun)(int, int, int, int);

static inline void glutMouseWheelFunc(GLUTmousewheelfun func)
{
	(void)func;
}

#endif
