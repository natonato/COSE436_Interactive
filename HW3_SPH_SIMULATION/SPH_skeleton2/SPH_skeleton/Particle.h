#pragma once
#include <math.h>
#include <vector>
#include <algorithm>
#include "vector.h"
#include <Windows.h>
#include <stdlib.h>
class Particle
{
public:
	double	mass;
	vec2	position;
	vec2	velocity;
	vec2	acceleration;
	vec2	fpressure;
	vec2	fviscosity;

	vec3	position3;
	vec3	velocity3;
	vec3	acceleration3;
	vec3	fpressure3;
	vec3	fviscosity3;
	vec3	randonColor;
	double	density;
	int		idx;
	double  restitution;
	bool	water;
public:
	Particle(void)
	{
	}
	Particle(double _x, double _y)
	{
		position = vec2(_x, _y);
		velocity = vec2(0.0, 0.0);
		mass = 1.0;
		restitution = 0.5;
	}

	Particle(double _x, double _y, int _idx) : position(_x, _y), velocity(0.0, 0.0), acceleration(0.0, 0.0), mass(1.0)
	{
		fpressure = vec2(0.0, 0.0);
		fviscosity = vec2(0.0, 0.0);
		density = 0.0;
		idx = _idx;
		restitution = 0.5;
	}
	~Particle(void)
	{
	}
	Particle(double _x, double _y, double _z, int _idx) : position3(_x, _y, _z), velocity3(0.0, 0.0, 0.0), acceleration3(0.0, 0.0, 0.0), mass(1.0)
	{
		srand(_idx*_idx);
		randonColor = vec3((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);

		fpressure3 = vec3(0.0, 0.0, 0.0);
		fviscosity3 = vec3(0.0, 0.0, 0.0);
		density = 0.0;
		idx = _idx;
		restitution = 0.5;
	}

	double	getPosX(void) { return position.getX(); }
	double	getPosY(void) { return position.getY(); }

	void integrate(double dt, vec2 gravity)
	{
		double boxsize = 10.0f;
		vec2 fgrav = gravity * mass;

		// Update velocity and position
		acceleration = (fpressure + fviscosity) / density + fgrav;
		velocity = velocity + acceleration * dt;
		position = position + velocity * dt;

		// Boundary condition
		if (position.x < -boxsize && velocity.x < 0.0)
		{
			velocity.x *= -restitution;
			position.x = -boxsize + 0.1;
		}
		if (position.x > boxsize && velocity.x > 0.0)
		{
			velocity.x *= -restitution;
			position.x = boxsize - 0.1;
		}
		if (position.y < -boxsize && velocity.y < 0.0)
		{
			velocity.y *= -restitution;
			position.y = -boxsize + 0.1;
		}
		if (position.y > boxsize && velocity.y > 0.0)
		{
			velocity.y *= -restitution;
			position.y = boxsize - 0.1;
		}
	}




	void integrate3(double dt, vec3 gravity)
	{
		vec3 fgrav = gravity * mass;
		double boxsize = 10.0f;

		// Update velocity and position
		acceleration3 = (fpressure3 + fviscosity3) / density + fgrav;
		velocity3 = velocity3 + acceleration3 * dt;
		position3 = position3 + velocity3 * dt;

		// Boundary condition
		if (position3.x < -boxsize && velocity3.x < 0.0)
		{
			velocity3.x *= -restitution;
			position3.x = -boxsize + 0.1;
		}
		if (position3.x > boxsize && velocity3.x > 0.0)
		{
			velocity3.x *= -restitution;
			position3.x = boxsize - 0.1;
		}
		if (position3.y < -boxsize + 8.0f && velocity3.y < 0.0)
		{
			velocity3.y *= -restitution;
			position3.y = -boxsize + 8.0f + 0.1;
		}
		if (position3.y > boxsize + 8.0f && velocity3.y > 0.0)
		{
			velocity3.y *= -restitution;
			position3.y = boxsize + 8.0f - 0.1;
		}
		if (position3.z < -boxsize && velocity3.z < 0.0)
		{
			velocity3.z *= -restitution;
			position3.z = -boxsize + 0.1;
		}
		if (position3.z > boxsize && velocity3.z > 0.0)
		{
			velocity3.z *= -restitution;
			position3.z = boxsize - 0.1;
		}
	}

	void draw(float rot1, float rot2, vec3 color);
	void draw_clothParticle();
	void draw_cloth();
};
