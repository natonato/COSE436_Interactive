#pragma once   
#include <iostream>
#include <GL/glut.h>
#include "SPH.h"
#include "cloth.h"

using namespace std;


class Simulator
{
public:
	Simulator(void);
	~Simulator(void);

public:
	void					Initialize();
	void					Update();
	void					Render(float rot1, float rot2);
	void					Lighting(void);
	void					DrawGround(void);
	void					DrawWall(void);
	void					DrawSphere(void);
	void					DrawThings();
	void					ClothParticle();
	void					SphereParticle();
	void					SphereParticleUpdate();
	void					compute_cloth_particle();
	void					Pouring();
	

public:
	mass_cloth				*cloth;
	SPH						*mySPH;
	vec3					ground;
	vec3					wall;
	vec3					wall2;
	vec3					spherePos;
	double					sphereRad;
	double					timsStep;
};

