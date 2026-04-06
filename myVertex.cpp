#include "myVertex.h"
#include "myVector3D.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}


void myVertex::computeNormal()
{
	if (originof == NULL)
	return;
	/**** TODO ****/
	normal->clear();
	myHalfedge *h = originof;
	do {
		*normal +=*(h->adjacent_face->normal);
		h = h->twin->next;
	} while (h != originof);
	normal->normalize();
}

