

#pragma once
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "vector.h"
class Node
{
public:
	double	mass;
	vec3	force;
	vec3	particle_accel;
	vec3	last_pos;
	vec3	position;
	vec3	last_vel;
	vec3	velocity;
	vec3	acceleration;
	vec3	next_acceleration;
	vec3	normal;
	bool	isFixed;
	vec2	tex_coord;
	vec3	temp_vel;

public:
	Node(void)
	{
		isFixed = false;
	}
	Node(vec3 init_pos)
	{
		isFixed = false;
		position = init_pos;
		mass = 1.0;
	}
 
	~Node(void)
	{
	}

	double	getPosX(void) { return position.getX(); }
	double	getPosY(void) { return position.getY(); }
	double	getPosZ(void){ return position.getZ(); }

	void add_force(vec3 additional_force)
	{
		force += additional_force;

	}

	void integrate(double dt)
	{
		if (!isFixed)
		{

			last_vel = velocity; //vt
			acceleration = force / mass; // at
			acceleration += particle_accel / 4;
			velocity = velocity + dt * acceleration; // v*
		}
		/*initialize Force*/
		force.x = force.y = force.z= 0.0;
		particle_accel.x = particle_accel.y = particle_accel.z = 0.0;
	}
	//collision 결과 velocity가 0일때, 중력으로 인한 accel 발생으로 서서히 하락하는 문제 해결을 위해.
	void integrate2(double dt) {
		if (!isFixed)
		{
			last_pos = position;
			position = position + dt * velocity;
			
		}
	}
	void RK2_velocity(double dt) {
		if (!isFixed)
		{	
			next_acceleration = force / mass; // a*
			temp_vel = velocity;
			velocity = last_vel + dt * (acceleration + next_acceleration) / 2.0f; // (a* + at) / 2

		}
		force.x = force.y = force.z = 0.0;
	}
	//RK2적용. 가속도만 필요하다.
	void RK2_position(double dt) {
		if (!isFixed)
		{
			position = last_pos + dt * (temp_vel + last_vel) / 2.0f; // (v* + vt) /2
		}
	}

	void draw();
};
