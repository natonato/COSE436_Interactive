
#pragma once
 
#include <iostream>
#include "Node.h"
using namespace std;

class mass_spring{
public:
	double		spring_coef;
	double		damping_coef;
	Node	*p1;
	Node	*p2;
	double		initial_length;
 
public:
 
	mass_spring(Node *p1, Node *p2)
	{
		damping_coef = 5.0;
		this->p1 = p1;
		this->p2 = p2;
		init();
	}

	void init()
	{
		vec3 S_length = (p2->position - p1->position);
		initial_length = S_length.length();
	}

	void internal_force(double dt)
	{
		//Basic Implements 2-1. Compute Spring Force
		/*add hook_force and damping force*/
		double length = (p1->position - p2->position).length();
		vec3 force = (p1->position - p2->position) / length *
			(spring_coef *(length - initial_length) +
				damping_coef * dot((p1->velocity - p2->velocity), (p1->position - p2->position) / length));

		p1->add_force(-1 * force);
		p2->add_force(force);

	}

	double dot(vec3 a, vec3 b) {
		double result;
		result = a.x * b.x + a.y * b.y + a.z * b.z;

		return result;
	}

	void draw();
	void drawBend();
};