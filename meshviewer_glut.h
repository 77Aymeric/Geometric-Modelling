#pragma once

#if defined(__APPLE__) && defined(MESHVIEWER_USE_APPLE_GLUT)
#include "GL/apple_glut_compat.h"
#else
#include <GL/freeglut.h>
#  if defined(MESHVIEWER_FREEGLUT_COCOA_EMBEDDED)
#    include <GL/freeglut_ext.h> /* glutInitContextVersion, profil compat. */
#  endif
#endif
