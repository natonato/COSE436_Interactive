
#pragma once
#pragma warning(disable : 4996)
#include "SPH.h"
#include "spring.h"
#include "Node.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <GL/glut.h>
class mass_cloth
{
public:

	std::vector<Node *> nodes;
	std::vector<mass_spring *> spring;
	std::vector<Node*> faces;

	GLuint		texture[6];
	int			size_x, size_y, size_z;
	double		dx, dy, dz;
	double		structural_coef;
	double		shear_coef;
	double		bending_coef;
	int			iteration_n;
	int			drawMode;
	
	
	mass_cloth()
	{ 	 
	}
	~mass_cloth()
	{ 
		for (int i = 0; i < nodes.size(); i++){ delete nodes[i]; }
		for (int i = 0; i < spring.size(); i++){ delete spring[i]; }
		nodes.clear();
		spring.clear();
		faces.clear();
	}
	enum DrawModeEnum{
		DRAW_MASS_NODES,
		DRAW_SPRINGS,
		DRAW_FACES
	};

 
public:
	void init()
	{
		int startx, starty, startz;
		startx = -8;
		starty = 8;
		startz = -5;
		//텍스쳐 로딩
		LoadTexture("texture1.bmp", 0);
		LoadTexture("texture.bmp", 1);
		LoadTexture("texture2.bmp", 2);
		LoadTexture("wood.bmp", 3);
		LoadTexture("steel.bmp", 4);
		LoadTexture("marble.bmp", 5);


		//Basic Implements 1. Init Nodes and Shear and Structural Springs
		//Additional Implements 1. Init Bending Spring

		//노드 생성 저장
		for (int i = startx; i < startx+size_x; i++) {
			for (int j = startz; j < startz +size_y; j++) {
				//Node *xp1 = new Node(vec3(starty, j, i));
				Node *xp1 = new Node(vec3(j, starty, i));
				if ((i == startx && j == startz + size_y -1) || (i == startx && j == startz) || (i == startx + size_x-1 && j == startz)) {
					xp1->isFixed = true;
				}

				if ((i == startx + size_x - 1 && j == startz + size_y - 1)) {
					xp1->isFixed = true;
				}
				vec2 tex = vec2((i - startx) / (float)size_x, (j- startz) / (float)size_y);
				xp1->tex_coord = tex;
				nodes.push_back(xp1);
				//nodes[i * 50 + j] = xp1;

			}
		}
		//좌우인접 노드 스프링
		
		for (int i = 0; i< size_x; i++) {
			for (int j = 0; j< size_y; j++) {
				//둘다 최대니 끝
				if (i == size_x-1 && j == size_y-1) {
					break;
				}
				//x값은 최대니 y값으로만 spring 생성
				if (i == size_x-1) {
					mass_spring *sp = new mass_spring(nodes[i*size_x +j], nodes[i * size_y + j+1]);
					sp->spring_coef = structural_coef;
					spring.push_back(sp);
				}
				//y값은 최대니 x값으로만 spring 생성
				else if (j == size_y-1) {
					mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[(i+1) * size_y + j]);
					sp->spring_coef = structural_coef;
					spring.push_back(sp);

				}
				//둘다 최대가 아니니 ㄴ자 모양으로 spring 생성.
				else {
					mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[i * size_y + j + 1]);
					sp->spring_coef = structural_coef;
					spring.push_back(sp);
					sp = new mass_spring(nodes[i * size_x + j], nodes[(i + 1) * size_y + j]);
					sp->spring_coef = structural_coef;
					spring.push_back(sp);
				}
			}
		}
		
		//shear 스프링 생성 저장
		for (int i = 0; i < size_x-1; i++) {
			for (int j = 0; j < size_y-1; j++) {
				mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[(i+1) * size_y + j + 1]);
				sp->spring_coef = shear_coef;
				spring.push_back(sp);
				sp = new mass_spring(nodes[(i+1) * size_x + j], nodes[i * size_y + j+1]);
				sp->spring_coef = shear_coef;
				spring.push_back(sp);

			}
		}
		//bending 스프링
		for (int i = 0; i < size_x; i++) {
			for (int j = 0; j < size_y; j++) {
				//둘다 최대니 끝
				if (i >= size_x-2 && j >= size_y-2) {
					break;
				}
				//x값은 최대니 y값으로만 spring 생성
				if (i >= size_x-2) {
					mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[i * size_y + j + 2]);
					sp->spring_coef = bending_coef;
					spring.push_back(sp);

				}
				else if (j >= size_y-2) {
					mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[(i + 2) * size_y + j]);
					sp->spring_coef = bending_coef;
					spring.push_back(sp);

				}
				else {
					mass_spring *sp = new mass_spring(nodes[i * size_x + j], nodes[i * size_y + j + 2]);
					sp->spring_coef = bending_coef;
					spring.push_back(sp);
					sp = new mass_spring(nodes[i * size_x + j], nodes[(i + 2) * size_y + j]);
					sp->spring_coef = bending_coef;
					spring.push_back(sp);
				}
			}
		}
		//face 저장
		for (int i = 0; i < size_x-1; i++) {
			for (int j = 0; j < size_y-1; j++) {
				//아래쪽 삼각형
				faces.push_back(nodes[i * size_x + j + 1]);
				faces.push_back(nodes[i * size_x + j]);
				faces.push_back(nodes[(i + 1) * size_x + j]);
				//위쪽 삼각형
				faces.push_back(nodes[i * size_x + j + 1]);
				faces.push_back(nodes[(i + 1) * size_x + j]);
				faces.push_back(nodes[(i + 1) * size_x + j + 1]);
				
			}
		}
	
	}
	
	void computeNormal()
	{

		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->normal = vec3(0,0,0);

		}
		for (int i = 0; i < faces.size(); i+=3)
		{
			vec3 a = faces[i+1]->position - faces[i]->position;
			vec3 b = faces[i+2]->position - faces[i]->position;
			vec3 c = vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
			faces[i]->normal += c;
			faces[i+1]->normal += c;
			faces[i+2]->normal += c;
			

		}
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->normal.Normalize();			
		}
	}

	void add_force(vec3 additional_force)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->add_force(additional_force);

		}
	}

	void compute_force(double dt, vec3 gravity)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->add_force(gravity * nodes[i]->mass);
		}
		/* Compute Force for all springs */
		for (int i = 0; i < spring.size(); i++)
		{
			spring[i]->internal_force(dt);	
		}
	}

	void integrate(double dt)
	{
		/* integrate Nodes*/
		
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->integrate(dt);
		}

	}
	void integrate2(double dt)
	{
		/* integrate Nodes*/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->integrate2(dt);
			
		}
		
	}
	float tansung = 0.3f; // 임의의 탄성계수
	void collision_response(vec3 ground)//, vec3 spherePos, float sphereRad)
	{
		//Basic Implements 4. Collision Check with ground
		//Additional Implements 2. Collision Check with Sphere
		//Additional Implements 3. Collision Check with Mesh Object
		for (int i = 0; i < nodes.size(); i++)
		{
			//바닥의 법선벡터는 0,1,0
			if (dot(nodes[i]->position - ground, vec3(0, 1, 0)) < 0.4f && dot(nodes[i]->velocity, vec3(0,1,0))<0) {
				vec3 vn = dot(vec3(0, 1, 0), nodes[i]->velocity) * vec3(0, 1, 0);
				vec3 vt = nodes[i]->velocity - vn;
				nodes[i]->velocity = vt - vn * tansung;
			}
			/*
			vec3 sphereNom = nodes[i]->position - spherePos;
			sphereNom.Normalize();
			if (dot(nodes[i]->position - spherePos, sphereNom) < sphereRad + 0.4f && dot(nodes[i]->velocity, sphereNom) < 0) {

				vec3 vn = dot(sphereNom, nodes[i]->velocity) * sphereNom;
				vec3 vt = nodes[i]->velocity - vn;
				nodes[i]->velocity = vt - vn *tansung;
			}
			*/

			
		}
		
		

	}
	void RK2_velocity(double dt)
	{
		/* integrate Nodes*/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->RK2_velocity(dt);

		}

	}
	void RK2_position(double dt)
	{
		/* integrate Nodes*/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->RK2_position(dt);

		}

	}




	double dot(vec3 a, vec3 b) {
		double result;
		result = a.x * b.x + a.y * b.y + a.z * b.z;

		return result;
	}

	void LoadTexture(const char * filename, int index)
	{

		//GLuint texture;

		unsigned char header[54]; 
		unsigned int dataPos;  
		unsigned int width, height;
		unsigned int imageSize; 
		unsigned char * data;

		FILE * file;
		file = fopen(filename, "rb");
		if (!file) {
			printf("Image could not be opened\n");
			return;
		}
		if (fread(header, 1, 54, file) != 54) { 
			printf("Not a correct BMP file\n");
			return;
		}
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);
		if (imageSize == 0)    imageSize = width * height * 3;
		if (dataPos == 0)      dataPos = 54;
		data = new unsigned char[imageSize];
		fread(data, width * height * 3, 1, file);
		fclose(file);

		for (int i = 0; i < width * height; ++i)
		{
			int index = i * 3;
			unsigned char B, R;
			B = data[index];
			R = data[index + 2];

			data[index] = R;
			data[index + 2] = B;

		}

		glGenTextures(1, &texture[index]);
		glBindTexture(GL_TEXTURE_2D, texture[index]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);




		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


		free(data);

		//return index;
	}



	void draw();
};