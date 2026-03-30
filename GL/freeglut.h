#ifndef FREEGLUT_H
#define FREEGLUT_H

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
