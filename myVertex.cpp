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
	if (originof == NULL) return;

	myVector3D sum(0.0, 0.0, 0.0);
	myHalfedge *current = originof;
	do {
		if (current->adjacent_face != NULL) {
			if (current->adjacent_face->normal == NULL)
				current->adjacent_face->computeNormal();
			myVector3D *fNormal = current->adjacent_face->normal;
			sum.dX += fNormal->dX;
			sum.dY += fNormal->dY;
			sum.dZ += fNormal->dZ;
		}
		if (current->twin != NULL) {
			current = current->twin->next;
		} else {
			break;
		}
	} while (current != originof);

	if (normal != NULL) {
		*normal = sum;
		normal->normalize();
	}
}
