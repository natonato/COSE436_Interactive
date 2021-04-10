
#define GLM_ENABLE_EXPERIMENTAL
#include"Simulator.h"
#include <GL/glut.h>
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <time.h>
#include "teapot.h"

Simulator::Simulator()
{
	timsStep = 0.001f;
	mySPH = new SPH(10000);	// the number of particles
}
Simulator::~Simulator()
{

}

void Simulator::Initialize()
{
	timsStep = 0.006;
	mySPH->iteration_n = 6;
	mySPH->init();

	wall = vec3(10.2, 0.0, 0.0);
	wall2 = vec3(0.0, 0.0, 10.2);
	ground = vec3(0.0, -10.2, 0.0);
	cloth = new mass_cloth();

	cloth->dx = 1;
	cloth->dy = 1;
	cloth->dz = 1;
	cloth->size_x = 15;
	cloth->size_y = 15;
	cloth->size_z = 1;
	cloth->structural_coef = 1500;
	cloth->shear_coef = 50;
	cloth->bending_coef = 1000;
	cloth->iteration_n = 6;
	cloth->drawMode = 0;

	cloth->init();
	

}


void Simulator::SphereParticle() {
	spherePos = vec3(0.0f, -2.0f, 0.0f);
	sphereRad = 3.0f;
	int i = 0;
	for (float x = 0.0; x < 90; x += 5.0) {
        float tempRad = sphereRad * glm::cos(glm::radians(x));
        float tempY = sphereRad * glm::sin(glm::radians(x));

		for (float y = 0.0; y <= 360; y += 5.0) {

            glm::vec3 rot = glm::vec3(tempRad * glm::cos(glm::radians(y)), 0.0f, tempRad * glm::sin(glm::radians(y)));
           
            //glVertex3f(rot.x, rot.y, rot.z);
			//Particle* p = new Particle(x,y,z, i++);
            rot.x += spherePos.x;
            rot.z += spherePos.z;
            rot.y += (tempY + spherePos.y);

            Particle* p = new Particle(rot.x, rot.y, rot.z, i++);
			p->water = false;
			p->density = 15.0;
			mySPH->sphere_particles.push_back(p);
            //mySPH->particles.push_back(p);
		}
	}
	Particle* p = new Particle(spherePos.x, spherePos.y + sphereRad, spherePos.z, i++);
	p->water = false;
	p->density = 15.0;
	mySPH->sphere_particles.push_back(p);
	for (float x = 0.0; x < 90; x += 5.0) {
		float tempRad = (sphereRad-1.0) * glm::cos(glm::radians(x));
		float tempY = (sphereRad-1.0) * glm::sin(glm::radians(x));

		for (float y = 0.0; y <= 360; y += 5.0) {

			glm::vec3 rot = glm::vec3(tempRad * glm::cos(glm::radians(y)), 0.0f, tempRad * glm::sin(glm::radians(y)));

			//glVertex3f(rot.x, rot.y, rot.z);
			//Particle* p = new Particle(x,y,z, i++);
			rot.x += spherePos.x;
			rot.z += spherePos.z;
			rot.y += (tempY + spherePos.y);

			Particle* p = new Particle(rot.x, rot.y, rot.z, i++);
			p->water = false;
			p->density = 15.0;
			mySPH->sphere_particles.push_back(p);
			//mySPH->particles.push_back(p);
		}
	}
	p = new Particle(spherePos.x, spherePos.y + (sphereRad-1.0), spherePos.z, i++);
	p->water = false;
	p->density = 15.0;
	mySPH->sphere_particles.push_back(p);

}

void Simulator::SphereParticleUpdate() {
	int i = 0;
	for (float x = 0.0; x < 90; x += 5.0) {
		float tempRad = sphereRad * glm::cos(glm::radians(x));
		float tempY = sphereRad * glm::sin(glm::radians(x));

		for (float y = 0.0; y <= 360; y += 5.0) {

			glm::vec3 rot = glm::vec3(tempRad * glm::cos(glm::radians(y)), 0.0f, tempRad * glm::sin(glm::radians(y)));

			rot.x += spherePos.x;
			rot.z += spherePos.z;
			rot.y += (tempY + spherePos.y);
			mySPH->sphere_particles[i++]->position3 = vec3(rot.x, rot.y, rot.z);
		}
	}
	mySPH->sphere_particles[i++]->position3 = vec3(spherePos.x, spherePos.y + sphereRad, spherePos.z);
	for (float x = 0.0; x < 90; x += 5.0) {
		float tempRad = (sphereRad-1.0) * glm::cos(glm::radians(x));
		float tempY = (sphereRad - 1.0) * glm::sin(glm::radians(x));

		for (float y = 0.0; y <= 360; y += 5.0) {

			glm::vec3 rot = glm::vec3(tempRad * glm::cos(glm::radians(y)), 0.0f, tempRad * glm::sin(glm::radians(y)));

			rot.x += spherePos.x;
			rot.z += spherePos.z;
			rot.y += (tempY + spherePos.y);
			mySPH->sphere_particles[i++]->position3 = vec3(rot.x, rot.y, rot.z);
		}
	}
	mySPH->sphere_particles[i++]->position3 = vec3(spherePos.x, spherePos.y + (sphereRad - 1.0), spherePos.z);
	

}

void Simulator::ClothParticle() {
	//particle in node
	int str_spring_num = cloth->size_x * (cloth->size_y - 1) + cloth->size_y * (cloth->size_x - 1);
	for (int i = 0; i < cloth->nodes.size(); i++) {

		Particle *p = new Particle(cloth->nodes[i]->position.x, cloth->nodes[i]->position.y, cloth->nodes[i]->position.z, i);
		p->water = false;
		p->density = 15.0;
	
		mySPH->cloth_particles.push_back(p);
	}
	//particle between node(middle of structural spring)
	for (int i = 0; i < str_spring_num; i++) {
		if (cloth->spring[i]->spring_coef == cloth->structural_coef) {

			Particle *p = new Particle((cloth->spring[i]->p1->position.x + cloth->spring[i]->p2->position.x) / 2.0f,
				(cloth->spring[i]->p1->position.y + cloth->spring[i]->p2->position.y) / 2.0f,
				(cloth->spring[i]->p1->position.z + cloth->spring[i]->p2->position.z) / 2.0f,
				i);
			p->water = false;
			p->density = 15.0;
			mySPH->cloth_particles.push_back(p);
		}
	}
	//particle between node(middle of shear spring)
	
	for (int i = 0; i < (cloth->size_x-1) * (cloth->size_y-1); i++) {
		int i2 = str_spring_num + i * 2;
		if (cloth->spring[i2]->spring_coef == cloth->shear_coef) {
			Particle *p = new Particle((cloth->spring[i2]->p1->position.x + cloth->spring[i2]->p2->position.x) / 2.0f,
				(cloth->spring[i2]->p1->position.y + cloth->spring[i2]->p2->position.y) / 2.0f,
				(cloth->spring[i2]->p1->position.z + cloth->spring[i2]->p2->position.z) / 2.0f,
				i);
			p->water = false;
			p->density = 15.0;
			mySPH->cloth_particles.push_back(p);
		}
	}
	vec3 initPos = vec3(-12.0f, 5.0f, 0.0f);
	for (int i = 0; i < teapot_count; i += 9) {

		glm::vec3 rot = glm::rotateX(glm::vec3(teapot[i], teapot[i + 1], teapot[i + 2]), glm::radians(180.0f));
		Particle *p = new Particle(3 * rot.x + initPos.x, 3 * rot.y + initPos.y, 3 * rot.z + initPos.z, i);

		p->water = false;
		p->density = 5.0;
		mySPH->cloth_particles.push_back(p);
	}

	SphereParticle();
}


void Simulator::Update()
{
	vec3 gravity(0.0, -9.8 / mySPH->iteration_n, 0.0);

	SphereParticleUpdate();
	//Update
	for (int iter = 0; iter < mySPH->iteration_n; iter++)
	{
		mySPH->update3(timsStep, gravity);
	}
	for (int iter = 0; iter < cloth->iteration_n; iter++)
	{
		compute_cloth_particle();
		cloth->compute_force(timsStep, gravity);
		cloth->integrate(timsStep);
		cloth->collision_response(ground);// , spherePos, sphereRad);
		cloth->integrate2(timsStep);


		for (int i = 0; i < cloth->nodes.size(); i++) {
			mySPH->cloth_particles[i]->position3 = cloth->nodes[i]->position;
			mySPH->cloth_particles[i]->velocity3 = cloth->nodes[i]->velocity;
		}
		int str_spring_num = cloth->size_x * (cloth->size_y - 1) + cloth->size_y * (cloth->size_x - 1);
		for (int i = cloth->nodes.size(); i < cloth->nodes.size() + str_spring_num; i++) {
			int i1 = i - cloth->nodes.size();
			mySPH->cloth_particles[i]->position3 = vec3((cloth->spring[i1]->p1->position.x + cloth->spring[i1]->p2->position.x) / 2.0f,
				(cloth->spring[i1]->p1->position.y + cloth->spring[i1]->p2->position.y) / 2.0f,
				(cloth->spring[i1]->p1->position.z + cloth->spring[i1]->p2->position.z) / 2.0f);

			mySPH->cloth_particles[i]->velocity3 = (cloth->spring[i1]->p1->velocity + cloth->spring[i1]->p2->velocity)/2.0f;
		}
		
		for (int i = 0; i < (cloth->size_x - 1) * (cloth->size_y - 1); i++)	{
			int i1 = cloth->nodes.size() + str_spring_num + i;
			int i2 = str_spring_num + i * 2;
			mySPH->cloth_particles[i1]->position3 = vec3((cloth->spring[i2]->p1->position.x + cloth->spring[i2]->p2->position.x) / 2.0f,
				(cloth->spring[i2]->p1->position.y + cloth->spring[i2]->p2->position.y) / 2.0f,
				(cloth->spring[i2]->p1->position.z + cloth->spring[i2]->p2->position.z) / 2.0f);


			mySPH->cloth_particles[i]->velocity3 = (cloth->spring[i2]->p1->velocity + cloth->spring[i2]->p2->velocity + cloth->spring[i2+1]->p1->velocity + cloth->spring[i2+1]->p2->velocity) / 4.0f;
		}
	}
	cloth->computeNormal();
}

void Simulator::Render(float rot1, float rot2)
{

	Lighting();
	DrawThings();
	DrawGround();
	DrawSphere();
	cloth->draw();
	DrawWall();
	mySPH->draw(rot1, rot2);


}




void Simulator::Lighting()
{
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	float light_pos[] = { 0.0, 1500.0, 0.0, 1.0 };
	float light_dir[] = { 0, -1, 0 , 0.0 };
	float light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
	float light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	float light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float frontColor[] = { 0.8, 0.8, 0.8, 1 };

	float matShininess = 20;
	float noMat[] = { 0, 0, 0, 1 };

	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, noMat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_AMBIENT, frontColor);					// Set ambient property frontColor.
	glMaterialfv(GL_FRONT, GL_DIFFUSE, frontColor);					// Set diffuse property frontColor.
	glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 80.0f);                  // 80도 원뿔
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 80.0f);                 // 초점 설정
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void Simulator::DrawGround(void) {

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cloth->texture[2]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	float textureDensity = 32.0f;
	for (int y = 0; y < 64; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * z, -10.2 + 30.0f / 64 * y, -15.0);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * (z + 1), -10.2 + 30.0f / 64 * y, -15.0);
			glTexCoord2f((float)(z+1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * (z + 1), -10.2 + 30.0f / 64 * (y + 1), -15.0);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * z, -10.2 + 30.0f / 64 * (y + 1), -15.0);
		}
	}
	for (int y = 0; y < 64; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(30.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(30.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(30.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(30.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * z);
		}
	}
	for (int y = 0; y < 64; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * (z + 1));
		}
	}
	for (int y = 0; y < 64; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * z, -10.2 + 30.0f / 64 * y, 45.0);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * z, -10.2 + 30.0f / 64 * (y + 1), 45.0);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * (z + 1), -10.2 + 30.0f / 64 * (y + 1), 45.0);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.0f / 64 * (z + 1), -10.2 + 30.0f / 64 * y, 45.0);
		}
	}

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cloth->texture[3]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int x = 0; x < 128; x++) {
		for (int y = 0; y < 128; y++) {
			glNormal3f(0.0, 1.0, 0.0);
			glTexCoord2f((float)x / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0f + 30.0f / 64 * x, ground.y, -15.0f + 30.0f / 64 * y);
			glTexCoord2f((float)(x + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0f + 30.0f / 64 * (x + 1), ground.y, -15.0f + 30.0f / 64 * y);
			glTexCoord2f((float)(x + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0f + 30.0f / 64 * (x + 1), ground.y, -15.0f + 30.0f / 64 * (y + 1));
			glTexCoord2f((float)x / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0f + 30.0f / 64 * x, ground.y, -15.0f + 30.0f / 64 * (y + 1));
		}
	}
	glEnd();


	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);	
	glPopMatrix();
}


void Simulator::DrawWall(void) {

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, cloth->texture[4]);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);

	

	glColor4f(1.0, 1.0, 1.0, 1.0);
	int textureDensity = 64;
	for (int y = 0; y < 96; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f((float)y / textureDensity, (float)z / textureDensity);
			glVertex3f(wall.x, -10.2 + 10.2f / 64 * y, -10.2 + 10.2f / 64 * z);
			glTexCoord2f((float)(y+1) / textureDensity, (float)z / textureDensity);
			glVertex3f(wall.x, -10.2 + 10.2f / 64 * (y + 1), -10.2 + 10.2f / 64 * z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(wall.x, -10.2 + 10.2f / 64 * (y + 1), -10.2 + 10.2f / 64 * (z + 1));
			glTexCoord2f((float)y / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(wall.x, -10.2 + 10.2f / 64 * y, -10.2 + 10.2f / 64 * (z + 1));
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f((float)y / textureDensity, (float)z / textureDensity);
			glVertex3f(-wall.x, -10.2 + 10.2f / 64 * y, -10.2 + 10.2f / 64 * z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)z / textureDensity);
			glVertex3f(-wall.x, -10.2 + 10.2f / 64 * (y + 1), -10.2 + 10.2f / 64 * z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-wall.x, -10.2 + 10.2f / 64 * (y + 1), -10.2 + 10.2f / 64 * (z + 1));
			glTexCoord2f((float)y / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-wall.x, -10.2 + 10.2f / 64 * y, -10.2 + 10.2f / 64 * (z + 1));
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f((float)y / textureDensity, (float)z / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * z, -10.2 + 10.2f / 64 * y, wall2.z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)z / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * z, -10.2 + 10.2f / 64 * (y + 1), wall2.z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * (z + 1), -10.2 + 10.2f / 64 * (y + 1), wall2.z);
			glTexCoord2f((float)y / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * (z + 1), -10.2 + 10.2f / 64 * y, wall2.z);
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)y / textureDensity, (float)z / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * z, -10.2 + 10.2f / 64 * y, -wall2.z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)z / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * z, -10.2 + 10.2f / 64 * (y + 1), -wall2.z);
			glTexCoord2f((float)(y + 1) / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * (z + 1), -10.2 + 10.2f / 64 * (y + 1), -wall2.z);
			glTexCoord2f((float)y / textureDensity, (float)(z + 1) / textureDensity);
			glVertex3f(-10.2 + 10.2f / 64 * (z + 1), -10.2 + 10.2f / 64 * y, -wall2.z);
			
		}
	}
	glEnd();


	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}



void Simulator::compute_cloth_particle() {
	//particle on each node
	for (int i = 0; i < cloth->nodes.size(); i++) {
		cloth->nodes[i]->particle_accel +=
		((mySPH->cloth_particles[i]->fpressure3 + mySPH->cloth_particles[i]->fviscosity3) / mySPH->cloth_particles[i]->density);
	}
	//particle on structual spring(1/2 to each node)
	int str_spring_num = cloth->size_x * (cloth->size_y - 1) + cloth->size_y * (cloth->size_x - 1);
	for (int i = cloth->nodes.size(); i < cloth->nodes.size() + str_spring_num; i++) {
		int i1 = i - cloth->nodes.size();
		cloth->spring[i1]->p1->particle_accel +=
		((mySPH->cloth_particles[i]->fpressure3 + mySPH->cloth_particles[i]->fviscosity3) / (2.0f * mySPH->cloth_particles[i]->density));
		cloth->spring[i1]->p2->particle_accel +=
		((mySPH->cloth_particles[i]->fpressure3 + mySPH->cloth_particles[i]->fviscosity3) / (2.0f * mySPH->cloth_particles[i]->density));

	}

	for (int i = 0; i < (cloth->size_x - 1) * (cloth->size_y - 1); i++) {
		int i1 = cloth->nodes.size() + str_spring_num + i;
		int i2 = str_spring_num + i * 2;
		cloth->spring[i2]->p1->particle_accel +=
		((mySPH->cloth_particles[i1]->fpressure3 + mySPH->cloth_particles[i1]->fviscosity3) / (4.0f*mySPH->cloth_particles[i]->density));
		cloth->spring[i2]->p2->particle_accel +=	
		((mySPH->cloth_particles[i1]->fpressure3 + mySPH->cloth_particles[i1]->fviscosity3) / (4.0f*mySPH->cloth_particles[i]->density));
		cloth->spring[i2+1]->p1->particle_accel +=
		((mySPH->cloth_particles[i1]->fpressure3 + mySPH->cloth_particles[i1]->fviscosity3) / (4.0f*mySPH->cloth_particles[i]->density));
		cloth->spring[i2+1]->p2->particle_accel +=
		((mySPH->cloth_particles[i1]->fpressure3 + mySPH->cloth_particles[i1]->fviscosity3) / (4.0f*mySPH->cloth_particles[i]->density));
	}
}


void Simulator::Pouring() {
	mySPH->pouring();
}


void Simulator::DrawSphere(void) {

	glPushMatrix();

	glTranslatef(spherePos.x, spherePos.y, spherePos.z);
	glColor3f(0.1f, 0.1f, 0.1f);
	glutSolidSphere(sphereRad+0.3, 60, 60);
	glPopMatrix();

}


void Simulator::DrawThings() {

	glPushMatrix();

	//glColor4f(0.7f, 0.9f, 0.6f, 1.0f);
	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	vec3 initPos = vec3(-12.0f, 4.8f, 0.0f);
	for (int i = 0; i < teapot_count; i += 9) {
		glm::vec3 rot1 = glm::rotateX(glm::vec3(teapot[i], teapot[i + 1], teapot[i + 2]), glm::radians(180.0f));
		glm::vec3 rot2 = glm::rotateX(glm::vec3(teapot[i+3], teapot[i + 4], teapot[i + 5]), glm::radians(180.0f));
		glm::vec3 rot3 = glm::rotateX(glm::vec3(teapot[i+6], teapot[i + 7], teapot[i + 8]), glm::radians(180.0f));
		

		vec3 nor1 = vec3((rot1 - rot2).x, (rot1 - rot2).y, (rot1 - rot2).z);
		vec3 nor2 = vec3((rot1 - rot3).x, (rot1 - rot3).y, (rot1 - rot3).z);

		nor1.Cross(nor2);
		
		if (i % 2 ==0) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		}
		//glNormal3f(nor1.x, nor1.y, nor1.z);
		glVertex3f(3 * rot1.x +initPos.x, 3 * rot1.y + initPos.y, 3 * rot1.z + initPos.z);
		if ((i+3) % 2 == 0) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		}
		glVertex3f(3 * rot2.x + initPos.x, 3 * rot2.y + initPos.y, 3 * rot2.z + initPos.z);
		if ((i + 6) % 2 == 0) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		}
		glVertex3f(3 * rot3.x + initPos.x, 3 * rot3.y + initPos.y, 3 * rot3.z + initPos.z);
	}
	glEnd();


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cloth->texture[2]);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	int textureDensity = 32;
	for (int y = 0; y < 32; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(15.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(15.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(15.0f, -10.2 + 30.0f / 64 * (y + 1), -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(15.0f, -10.2 + 30.0f / 64 * y, -15.0 + 30.0f / 64 * (z + 1));
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cloth->texture[3]);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int y = 0; y < 32; y++) {
		for (int z = 0; z < 128; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(15.0 + 30.0f / 64 * y, 4.8f, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(15.0 + 30.0f / 64 * (y + 1), 4.8f, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(15.0 + 30.0f / 64 * (y + 1), 4.8f, -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(15.0 + 30.0f / 64 * y, 4.8f, -15.0 + 30.0f / 64 * (z + 1));
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//sink left upside
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cloth->texture[5]);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int y = 0; y < 42; y++) {
		for (int z = 0; z< 54; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, 4.8f, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), 4.8f, -15.0 + 30.0f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), 4.8f, -15.0 + 30.0f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, 4.8f, -15.0 + 30.0f / 64 * (z + 1));
		}
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cloth->texture[2]);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int y = 0; y < 42; y++) {
		for (int z = 0; z < 32; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, -10.2 + 30.0f / 64 * z, -15.0 + 30.0f / 64 * 53);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), -10.2 + 30.0f / 64 * z, -15.0 + 30.0f / 64 * 53);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), -10.2 + 30.0f / 64 * (z+1), -15.0 + 30.0f / 64 * 53);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, -10.2 + 30.0f / 64 * (z+1), -15.0 + 30.0f / 64 * 53);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cloth->texture[4]);
	glEnable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int y = 42; y < 85; y++) {
		for (int z = 0; z < 43; z++) {
			glNormal3f(0.0, 0.0, -1.0);
			glTexCoord2f((float)z / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, -2.0f, -10.2 + 30.2f / 64 * z);
			glTexCoord2f((float)z / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), -2.0f, -10.2 + 30.2f / 64 * z);
			glTexCoord2f((float)(z + 1) / textureDensity, (float)(y + 1) / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * (y + 1), -2.0f, -10.2 + 30.2f / 64 * (z + 1));
			glTexCoord2f((float)(z + 1) / textureDensity, (float)y / textureDensity);
			glVertex3f(-30.0 + 30.2f / 64 * y, -2.0f, -10.2 + 30.2f / 64 * (z + 1));
		}
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);



	glPopMatrix();


}



