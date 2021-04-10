#define GLM_ENABLE_EXPERIMENTAL

#include "Particle.h"
#include <GL/glut.h>
#include "cloth.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "Viewer.h"

#include <stdlib.h>
#include <Windows.h> 
#include <time.h>

void Particle::draw(float rot1, float rot2, vec3 color)
{
	
	glPushMatrix();
	//glEnable(GL_LIGHTING);

	glTranslatef(position3.x, position3.y, position3.z);


	glBegin(GL_TRIANGLE_FAN);
	float rad = 0.3f;

	//glColor4f(color.x, color.y, color.z, 0.3f);
	//glColor4f(randonColor.x, randonColor.y, randonColor.z, 0.3f);
	glColor4f(randonColor.x, randonColor.y, randonColor.z, 0.2f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	for (float i = 0.0; i <= 360; i += 10.0) {
		glm::vec3 rot = glm::vec3(rad * glm::cos(glm::radians(i)), rad * glm::sin(glm::radians(i)), 0.0f);

		rot = glm::rotateX(rot, glm::radians(rot1));
		rot = glm::rotateY(rot, glm::radians(rot2));
		rot.z = -rot.z;
		glVertex3f(rot.x, rot.y, rot.z);

	}
	glEnd();

	glTranslatef(-position3.x, -position3.y, -position3.z);

	glPopMatrix();
}

void Particle::draw_clothParticle()
{

	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(2.0f);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	//glVertex2f(getPosX(), getPosY());
	glVertex3f(position3.x, position3.y, position3.z);
	glEnd();
}
void mass_cloth::draw()
{
	//Basic Implements 3-3. Draw Call for Cloth
	
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	for (int i = 0; i < faces.size(); i += 3) {
		glTexCoord2f(faces[i]->tex_coord.x, faces[i]->tex_coord.y);
		//faces[i]의 normal은 전체 nodes의 normal이 normalize되며 해당 값을 따라 가지게 되므로 node의 normal값이다.
		glNormal3f(faces[i]->normal.x, faces[i]->normal.y, faces[i]->normal.z);
		glVertex3f(faces[i]->position.x, faces[i]->position.y, faces[i]->position.z);

		glTexCoord2f(faces[i + 1]->tex_coord.x, faces[i + 1]->tex_coord.y);
		glNormal3f(faces[i + 1]->normal.x, faces[i + 1]->normal.y, faces[i + 1]->normal.z);
		glVertex3f(faces[i + 1]->position.x, faces[i + 1]->position.y, faces[i + 1]->position.z);

		glTexCoord2f(faces[i + 2]->tex_coord.x, faces[i + 2]->tex_coord.y);
		glNormal3f(faces[i + 2]->normal.x, faces[i + 2]->normal.y, faces[i + 2]->normal.z);
		glVertex3f(faces[i + 2]->position.x, faces[i + 2]->position.y, faces[i + 2]->position.z);

	}
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	//Additional Implements 4-3. Texture Coordinate Mapping


	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}