#include "SPH.h"
#include <time.h>
#include <omp.h>

SPH::SPH()
{

}

SPH::SPH(int numparticle)
{
	MaxParticle = numparticle;
	index = 0;	
	rest_density = 4.2;
	mu = 0.8;	
	h = 1.0;
	k = 50.0;
}

SPH::~SPH()
{
	while (!particles.empty())
	{
		particles.pop_back();
	}
	particles.clear();
}

void SPH::resetParticle()
{
	index = 0;
	while (!particles.empty())
	{
		particles.pop_back();
	}
	while (!cloth_particles.empty())
	{
		cloth_particles.pop_back();
	}
	while (!sphere_particles.empty())
	{
		sphere_particles.pop_back();
	}
}

void SPH::init()
{
	resetParticle();
	damBreaking();

	for (int x = 0; x < GRIDSIZE; x++) {
		for (int y = 0; y < GRIDSIZE; y++) {
			for (int z = 0; z < GRIDSIZE; z++) {
				marchingData[x][y][z] = 0;
			}
		}
	}
	marchingPosition[0] = vec3(-10.0 + 0.5, -2.0, -10.0 + 1.0);
	marchingPosition[1] = vec3(-10.0 + 1.0, -2.0, -10.0 + 0.5);
	marchingPosition[2] = vec3(-10.0 + 0.5, -2.0, -10.0);
	marchingPosition[3] = vec3(-10.0, -2.0, -10.0 + 0.5);
	marchingPosition[4] = vec3(-10.0+0.5, -2.0+1.0, -10.0 + 1.0);
	marchingPosition[5] = vec3(-10.0 + 1.0, -2.0 + 1.0, -10.0 + 0.5);
	marchingPosition[6] = vec3(-10.0 + 0.5, -2.0 + 1.0, -10.0);
	marchingPosition[7] = vec3(-10.0, -2.0 + 1.0, -10.0 + 0.5);
	marchingPosition[8] = vec3(-10.0, -2.0+0.5, -10.0 + 1.0);
	marchingPosition[9] = vec3(-10.0+1.0, -2.0 + 0.5, -10.0 + 1.0);
	marchingPosition[10] = vec3(-10.0+1.0, -2.0 + 0.5, -10.0);
	marchingPosition[11] = vec3(-10.0, -2.0 + 0.5, -10.0);
	
	
}

void SPH::damBreaking()
{
	for (double y = 5.0; y < 15.0; y += 0.5) {
		for (double x = -5.0; x < 5.0; x += 0.5) {

			for (double z = -5.0; z < 5.0; z += 0.5) {
				if (particles.size() < MaxParticle)
				{
					Particle *p = new Particle(x, y, z, index++);
					p->randonColor = vec3(0.0f, 1.0f, 1.0f);
					p->water = true;
					particles.push_back(p);
				}
			}
		}
	}
	cout << "SPH" << particles.size() << " Paricles" << endl;
}

void SPH::pouring()
{
	if (particles.size() >= MaxParticle) {
		printf("Error : Too many particle\n");
		return;
	}

	for (double y = 13.0; y < 15.0; y += 0.5) {
		for (double x = -1.0; x < 1.0; x += 0.5) {
			for (double z = -3.0; z < -1.0; z += 0.5) {
				if (particles.size() < MaxParticle)
				{
					Particle *p = new Particle(x, y, z, index++);
					//p->velocity3.z = 7.0f;
					p->velocity3.y = -3.0f;
					p->randonColor = vec3(1.0f, 0.0f, 0.0f);
					p->water = true;
					particles.push_back(p);
				}
			}
		}
	}
	cout << "SPH" << particles.size() << " Paricles" << endl;
}

void SPH::update3(float dt, vec3 gravity)
{
	makeHashTable3();
	computeDensity();
	computeForce();
	integrate3(dt, gravity);
	marchingCube();
}

void SPH::draw(float rot1, float rot2)
{

	/*
	for (int i = 0; i < sphere_particles.size(); i++)
	{
		Particle *p = sphere_particles[i];
		p->draw(rot1, rot2, vec3(1.0f, 0.0f, 0.0f));
	}
	for (int i = 0; i < cloth_particles.size(); i++)
	{
		Particle *p = cloth_particles[i];
		p->draw(rot1, rot2, vec3(1.0f, 0.0f, 0.0f));
	}
	for (int i = 0; i < particles.size(); i++)
	{
		Particle *p = particles[i];
		p->draw(rot1, rot2, vec3(0.0f, 1.0f, 1.0f));
	}*/
	
	for (float x = 0; x < GRIDSIZE; x++) {
		for (float y = 0; y < GRIDSIZE; y++) {
			for (float z = 0; z < GRIDSIZE; z++) {
				for (int t = 0; t < 12; t++) {
					marchingNormal[(int)x][(int)y][(int)z][t] = vec3(0.0,0.0,0.0);
				}
			}
		}
	}
	for (float x = 0; x < GRIDSIZE; x++) {
		for (float y = 0; y < GRIDSIZE; y++) {
			for (float z = 0; z < GRIDSIZE; z++) {
				for (int t = 0; t < 16; t += 3) {
					if (triTable[marchingData[(int)x][(int)y][(int)z]][t] == -1)continue;

					vec3 trianglepos1 = vec3(x, y, z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t]];
					vec3 trianglepos2 = vec3(x, y, z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t + 1]];
					vec3 trianglepos3 = vec3(x, y, z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t + 2]];

					vec3 normal1 = trianglepos1 - trianglepos2;
					vec3 normal2 = trianglepos1 - trianglepos3;
					vec3 finalnormal = normal1.Cross(normal2);
					finalnormal.Normalize();
					
					for (int i = 0; i < 3; i++) {
						switch (triTable[marchingData[(int)x][(int)y][(int)z]][t + i])
						{
						case(0):
							marchingNormal[(int)x][(int)y][(int)z][0] += finalnormal;
							if (z+1<GRIDSIZE)marchingNormal[(int)x][(int)y][(int)z +1][2] += finalnormal;
							if (y > 0 && z + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y-1][(int)z +1][6] += finalnormal;
							if (y > 0)marchingNormal[(int)x][(int)y-1][(int)z][4] += finalnormal;
							break;
						case(1):
							marchingNormal[(int)x][(int)y][(int)z][1] += finalnormal;
							if (x + 1 < GRIDSIZE)marchingNormal[(int)x+1][(int)y][(int)z][3] += finalnormal;
							if (y > 0 && x + 1 < GRIDSIZE)marchingNormal[(int)x+1][(int)y - 1][(int)z][7] += finalnormal;
							if (y > 0)marchingNormal[(int)x][(int)y - 1][(int)z][5] += finalnormal;
							break;
						case(2):
							marchingNormal[(int)x][(int)y][(int)z][2] += finalnormal;
							if (z > 0)marchingNormal[(int)x][(int)y][(int)z - 1][0] += finalnormal;
							if (y > 0 && z > 0)marchingNormal[(int)x][(int)y - 1][(int)z-1][4] += finalnormal;
							if (y > 0)marchingNormal[(int)x][(int)y - 1][(int)z][6] += finalnormal;
							break;
						case(3):
							marchingNormal[(int)x][(int)y][(int)z][3] += finalnormal;
							if (x>0)marchingNormal[(int)x - 1][(int)y][(int)z][1] += finalnormal;
							if (y > 0 && x > 0)marchingNormal[(int)x - 1][(int)y - 1][(int)z][5] += finalnormal;
							if (y > 0)marchingNormal[(int)x][(int)y - 1][(int)z][7] += finalnormal;
							break;
						case(4):
							marchingNormal[(int)x][(int)y][(int)z][4] += finalnormal;
							if (z + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y][(int)z + 1][6] += finalnormal;
							if (y + 1 < GRIDSIZE && z + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y + 1][(int)z + 1][2] += finalnormal;
							if (y + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y + 1][(int)z][0] += finalnormal;
							break;
						case(5):
							marchingNormal[(int)x][(int)y][(int)z][5] += finalnormal;
							if (x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y][(int)z][7] += finalnormal;
							if (y + 1 < GRIDSIZE && x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y + 1][(int)z][3] += finalnormal;
							if (y + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y + 1][(int)z][1] += finalnormal;
							break;
						case(6):
							marchingNormal[(int)x][(int)y][(int)z][6] += finalnormal;
							if (z > 0)marchingNormal[(int)x][(int)y][(int)z - 1][4] += finalnormal;
							if (y + 1 < GRIDSIZE && z > 0)marchingNormal[(int)x][(int)y + 1][(int)z - 1][0] += finalnormal;
							if (y + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y + 1][(int)z][2] += finalnormal;
							break;
						case(7):
							marchingNormal[(int)x][(int)y][(int)z][7] += finalnormal;
							marchingNormal[(int)x][(int)y][(int)z][3] += finalnormal;
							if (x > 0)marchingNormal[(int)x - 1][(int)y][(int)z][5] += finalnormal;
							if (y + 1 < GRIDSIZE && x > 0)marchingNormal[(int)x - 1][(int)y + 1][(int)z][1] += finalnormal;
							if (y + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y + 1][(int)z][3] += finalnormal;
							break;
						case(8):
							marchingNormal[(int)x][(int)y][(int)z][8] += finalnormal;
							if (x > 0)marchingNormal[(int)x - 1][(int)y][(int)z][9] += finalnormal;
							if (z + 1 < GRIDSIZE && x > 0)marchingNormal[(int)x - 1][(int)y][(int)z + 1][10] += finalnormal;
							if (z + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y][(int)z + 1][11] += finalnormal;
							break;
						case(9):
							marchingNormal[(int)x][(int)y][(int)z][9] += finalnormal;
							if (x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y][(int)z][8] += finalnormal;
							if (z + 1 < GRIDSIZE && x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y][(int)z + 1][11] += finalnormal;
							if (z + 1 < GRIDSIZE)marchingNormal[(int)x][(int)y][(int)z + 1][10] += finalnormal;
							break;
						case(10):
							marchingNormal[(int)x][(int)y][(int)z][10] += finalnormal;
							if (x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y][(int)z][11] += finalnormal;
							if (z > 0 && x + 1 < GRIDSIZE)marchingNormal[(int)x + 1][(int)y][(int)z - 1][10] += finalnormal;
							if (z > 0)marchingNormal[(int)x][(int)y][(int)z - 1][9] += finalnormal;
							break;
						case(11):
							marchingNormal[(int)x][(int)y][(int)z][11] += finalnormal;
							if (x > 0)marchingNormal[(int)x - 1][(int)y][(int)z][10] += finalnormal;
							if (z > 0 && x > 0)marchingNormal[(int)x - 1][(int)y][(int)z - 1][9] += finalnormal;
							if (z > 0)marchingNormal[(int)x][(int)y][(int)z - 1][8] += finalnormal;
							break;
						default:
							break;
						}
					}

				}
			}
		}
	}
	for (float x = 0; x < GRIDSIZE; x++) {
		for (float y = 0; y < GRIDSIZE; y++) {
			for (float z = 0; z < GRIDSIZE; z++) {
				for (int t = 0; t < 12; t++) {
					marchingNormal[(int)x][(int)y][(int)z][t].Normalize();
				}
			}
		}
	}
	for (float x = 0; x < GRIDSIZE; x++) {
		for (float y = 0; y < GRIDSIZE; y++) {
			for (float z = 0; z < GRIDSIZE; z++) {
				glColor4f(0.2, 0.2, 0.7, 0.2);
				glBegin(GL_TRIANGLES);
				for (int t = 0; t < 16; t+=3) {
					if (triTable[marchingData[(int)x][(int)y][(int)z]][t] == -1)continue;

					vec3 trianglepos1 = vec3(x,y,z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t]];
					vec3 trianglepos2 = vec3(x, y, z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t+1]];
					vec3 trianglepos3 = vec3(x, y, z) + marchingPosition[triTable[marchingData[(int)x][(int)y][(int)z]][t+2]];

					vec3 normal1 = trianglepos1 - trianglepos2;
					vec3 normal2 = trianglepos1 - trianglepos3;
					vec3 finalnormal = normal1.Cross(normal2);
					finalnormal.Normalize();
					//glNormal3f(finalnormal.x, finalnormal.y, finalnormal.z);
					glNormal3f(marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t]].x, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t]].y, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t]].z);
					glVertex3f(trianglepos1.x, trianglepos1.y, trianglepos1.z);
					glNormal3f(marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+1]].x, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+1]].y, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+1]].z);
					glVertex3f(trianglepos2.x, trianglepos2.y, trianglepos2.z);
					glNormal3f(marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+2]].x, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+2]].y, marchingNormal[(int)x][(int)y][(int)z][triTable[marchingData[(int)x][(int)y][(int)z]][t+2]].z);
					glVertex3f(trianglepos3.x, trianglepos3.y, trianglepos3.z);

				
				}
				glEnd();
				
			}
		}
	}



}

double SPH::poly6Kernel(vec2 rij, double h)
{
	double temp = 0.0;
	double norm = rij.dist();
	temp = h*h - norm * norm;

	return 4.0 / (SPH_PI*h*h*h*h*h*h*h*h) * temp*temp*temp;
}

double SPH::poly6Kernel3(vec3 rij, double h)
{
	double temp = 0.0;
	double norm = rij.dist();
	temp = h * h - norm * norm;

	return 4.0 / (SPH_PI*h*h*h*h*h*h*h*h) * temp*temp*temp;
}
vec2 SPH::spikygradientKernel(vec2 rij, double q)
{
	double temp = 0.0;
	temp = 1.0 - q*q;
	temp = -30.0 / (SPH_PI*h*h*h*h) * (temp*temp)/q;

	return  vec2(temp*rij.x, temp*rij.y);
}


vec3 SPH::spikygradientKernel3(vec3 rij, double q)
{
	double temp = 0.0;
	temp = 1.0 - q * q;
	temp = -30.0 / (SPH_PI*h*h*h*h) * (temp*temp) / q;

	return  vec3(temp*rij.x, temp*rij.y, temp*rij.z);
}

double SPH::viscositylaplacianKernel(vec2 rij, double q)
{
	return 40.0 / (SPH_PI*h*h*h*h) * (1.0 - q);
}

double SPH::viscositylaplacianKernel3(vec3 rij, double q)
{
	return 40.0 / (SPH_PI*h*h*h*h) * (1.0 - q);
}

void SPH::computeDensity()
{
#pragma omp parallel for
	for (int x = 0; x < GRIDSIZE; x++)
	{
		for (int y = 0; y < GRIDSIZE; y++)
		{
			for (int z = 0; z < GRIDSIZE; z++)
			{
				vector<Particle*> ris;
				vector<Particle*> rjs = getNeighbor3(x, y, z, h, ris);

				for (int i = 0; i < ris.size(); i++)
				{
					Particle *pi = ris[i];
					if (pi->water == true)
						pi->density = 0.0;	//compute with poly6Kernel

					/*Implements - Compute Density 작성, 아래 density 초기화는 지울 것*/
					///*
					for (int j = 0; j < rjs.size(); j++) {
						Particle *pj = rjs[j];
						vec3 rij = pi->position3 - pj->position3;
						double q = rij.dist() / h;
						if (q >=0.0 && q < 1.0 && (pi->water ==true && pj->water == true)) {
							pi->density = pi->density + pj->mass*poly6Kernel3(rij, h);
						}

						
					}
					//*/
				}
			}
		}
	}
}
void SPH::computeForce() // Compute Pressure and Viscosity
{
#pragma omp parallel for
	for (int x = 0; x < GRIDSIZE; x++)
	{
		for (int y = 0; y < GRIDSIZE; y++)
		{
			for (int z = 0; z < GRIDSIZE; z++)
			{
				vector<Particle*> ris;
				vector<Particle*> rjs = getNeighbor3(x, y, z, h, ris);

				for (int i = 0; i < ris.size(); i++)
				{
					Particle *pi = ris[i];
					pi->fpressure3 = vec3(0.0, 0.0, 0.0);//compute with spikygradientKernel
					pi->fviscosity3 = vec3(0.0, 0.0, 0.0);//compute with viscositylaplacianKernel

					/*Implements - Compute Pressure and Viscosity Forces 작성*/
					for (int j = 0; j < rjs.size(); j++) {
						Particle *pj = rjs[j];
						vec3 rij = pi->position3 - pj->position3;
						double q = rij.dist() / h;
						if (q > 0.0 && q < 1.0 && (pi->water == true || pj->water == true)) {
							pi->fpressure3 = pi->fpressure3 + pj->mass
								* (k * ((pi->density - rest_density) + (pj->density - rest_density)) / (2.0* pj->density))
								* spikygradientKernel3(rij, q);//compute with spikygradientKernel
							pi->fviscosity3 = pi->fviscosity3 + pj->mass
								* ((pj->velocity3 - pi->velocity3) / pj->density)
								* viscositylaplacianKernel3(rij, q);//compute with viscositylaplacianKernel
						}
					}

					pi->fpressure3 = -1.0 * pi->fpressure3;
					pi->fviscosity3 = mu * pi->fviscosity3;
					//*/
				}
			}
		}
	}
}


void SPH::integrate(double dt, vec2 gravity)
{
#pragma omp parallel for
	for (int i = 0; i < particles.size(); i++)
	{
		Particle *p = particles[i];
		p->integrate(dt, gravity);
	}
}

void SPH::integrate3(double dt, vec3 gravity)
{
#pragma omp parallel for
	for (int i = 0; i < particles.size(); i++)
	{
		Particle *p = particles[i];
		
		p->integrate3(dt, gravity);
	}
}

void SPH::makeHashTable()
{
	for (int p = 0; p < GRIDSIZE; p++)
	{
		for (int q = 0; q < GRIDSIZE; q++)
		{
			hashGrid[p][q].clear();
		}
	}
	
	for (int i = 0; i < particles.size(); i++)
	{
		Particle *p = particles[i];
		double x = (p->getPosX() + GRIDSIZE / 2);
		double y = (p->getPosY() + GRIDSIZE / 2);
		int gridx = (int)(x);					
		int gridy = (int)(y);					

		if (gridx < 0) gridx = 0;
		if (gridx > GRIDSIZE - 1) gridx = GRIDSIZE - 1;
		if (gridy < 0) gridy = 0;
		if (gridy > GRIDSIZE - 1) gridy = GRIDSIZE - 1;

		hashGrid[gridx][gridy].push_back(p);
	}
}


void SPH::makeHashTable3()
{
	for (int p = 0; p < GRIDSIZE; p++)
	{
		for (int q = 0; q < GRIDSIZE; q++)
		{
			for (int r = 0; r < GRIDSIZE; r++) {
				hashGrid3[p][q][r].clear();
				hashGrid3_cloth[p][q][r].clear();
			}
		}
	}

	for (int i = 0; i < particles.size(); i++)
	{
		Particle *p = particles[i];
		
		double x = (p->position3.x + GRIDSIZE / 2);
		double y = (p->position3.y + GRIDSIZE / 2);
		double z = (p->position3.z + GRIDSIZE / 2);
		int gridx = (int)(x);
		int gridy = (int)(y-8);
		int gridz = (int)(z);

		if (gridx < 0) gridx = 0;
		if (gridx > GRIDSIZE - 1) gridx = GRIDSIZE - 1;
		if (gridy < 0) gridy = 0;
		if (gridy > GRIDSIZE - 1) gridy = GRIDSIZE - 1;
		if (gridz < 0) gridz = 0;
		if (gridz > GRIDSIZE - 1) gridz = GRIDSIZE - 1;

		hashGrid3[gridx][gridy][gridz].push_back(p);
		

	}

	for (int i = 0; i < sphere_particles.size(); i++)
	{
		Particle *p = sphere_particles[i];

		double x = (p->position3.x + GRIDSIZE / 2);
		double y = (p->position3.y + GRIDSIZE / 2);
		double z = (p->position3.z + GRIDSIZE / 2);
		int gridx = (int)(x);
		int gridy = (int)(y-8);
		int gridz = (int)(z);

		if (gridx < 0) gridx = 0;
		if (gridx > GRIDSIZE - 1) gridx = GRIDSIZE - 1;
		if (gridy < 0) gridy = 0;
		if (gridy > GRIDSIZE - 1) gridy = GRIDSIZE - 1;
		if (gridz < 0) gridz = 0;
		if (gridz > GRIDSIZE - 1) gridz = GRIDSIZE - 1;

		hashGrid3[gridx][gridy][gridz].push_back(p);


	}
	for (int i = 0; i < cloth_particles.size(); i++)
	{
		Particle *p = cloth_particles[i];

		double x = (p->position3.x + GRIDSIZE / 2);
		double y = (p->position3.y + GRIDSIZE / 2);
		double z = (p->position3.z + GRIDSIZE / 2);
		int gridx = (int)(x);
		int gridy = (int)(y-8);
		int gridz = (int)(z);

		if (gridx < 0) gridx = 0;
		if (gridx > GRIDSIZE - 1) gridx = GRIDSIZE - 1;
		if (gridy < 0) gridy = 0;
		if (gridy > GRIDSIZE - 1) gridy = GRIDSIZE - 1;
		if (gridz < 0) gridz = 0;
		if (gridz > GRIDSIZE - 1) gridz = GRIDSIZE - 1;


		hashGrid3[gridx][gridy][gridz].push_back(p);
	}
}


vector<Particle *> SPH::getNeighbor(int gridx, int gridy, double radius, vector<Particle*> &mine)
{
	vector<Particle *>res;
	mine.clear();
	for (int i = gridx - (int)radius; i <= gridx + (int)radius; i++)
	{
		for (int j = gridy - (int)radius; j <= gridy + (int)radius; j++)
		{
			if (i < 0 || i > GRIDSIZE - 1 || j < 0 || j > GRIDSIZE - 1)
				continue;

			for (int k = 0; k < hashGrid[i][j].size(); k++)
			{
				res.push_back(hashGrid[i][j][k]);

				if (i == gridx && j == gridy)
					mine.push_back(hashGrid[i][j][k]);
			}
		}
	}
	return res;
}

//cloth + sph particle (for sph simul)
vector<Particle *> SPH::getNeighbor3(int gridx, int gridy, int gridz, double radius, vector<Particle*> &mine)
{
	vector<Particle *>res;
	mine.clear();
	for (int i = gridx - (int)radius; i <= gridx + (int)radius; i++)
	{
		for (int j = gridy - (int)radius; j <= gridy + (int)radius; j++)
		{

			for (int n = gridz - (int)radius; n <= gridz + (int)radius; n++)
			{
				if (i < 0 || i > GRIDSIZE - 1 || j < 0 || j > GRIDSIZE - 1 || n < 0 || n > GRIDSIZE -1)
					continue;

				for (int k = 0; k < hashGrid3[i][j][n].size(); k++)
				{
					res.push_back(hashGrid3[i][j][n][k]);
					
					if (i == gridx && j == gridy && n == gridz)
						mine.push_back(hashGrid3[i][j][n][k]);
				}

				
				for (int k = 0; k < hashGrid3_cloth[i][j][n].size(); k++)
				{
					res.push_back(hashGrid3_cloth[i][j][n][k]);

					if (i == gridx && j == gridy && n == gridz)
						mine.push_back(hashGrid3_cloth[i][j][n][k]);
				}

				
			}
		}
	}
	return res;
}



void SPH::marchingCube() {
	for (int x = 0; x < GRIDSIZE; x++) {
		for (int y = 0; y < GRIDSIZE; y++) {
			for (int z = 0; z < GRIDSIZE; z++) {
				int value = 0;

				vector<Particle*> ris;
				vector<Particle*> rjs = getNeighbor3(x, y, z, h, ris);
				int count[8] = { 0 };
				int drawValue = 0;
				for (int i = 0; i < ris.size(); i++)
				{
					Particle *pi = ris[i];
					if (pi->water == false) continue;

					int rx = (int)round(pi->position3.x - (x-10));
					int ry = (int)round(pi->position3.y - (y-2));
					int rz = (int)round(pi->position3.z - (z-10));
					int sum = rx * 100 + ry * 10 + rz;

					switch (sum)
					{
					case(1):
						count[0]++;
						if (count[0] > drawValue)
							value |= 1;
						break;
					case(101):
						count[1]++;
						if (count[1] > drawValue)
							value |= 2;
						break;
					case(100):
						count[2]++;
						if (count[2] > drawValue)
							value |= 4;
						break;
					case(0):
						count[3]++;
						if (count[3] > drawValue)
							value |= 8;
						break;
					case(11):
						count[4]++;
						if (count[4] > drawValue)
							value |= 16;
						break;
					case(111):
						count[5]++;
						if (count[5] > drawValue)
							value |= 32;
						break;
					case(110):
						count[6]++;
						if (count[6] > drawValue)
							value |= 64;
						break;
					case(10):
						count[7]++;
						if (count[7] > drawValue)
							value |= 128;
						break;
					default:
						break;
					}


				}for (int i = 0; i < rjs.size(); i++)
				{
					Particle *pi = rjs[i];
					if (pi->water == false) continue;
					int rx = (int)round(pi->position3.x - (x - 10));
					int ry = (int)round(pi->position3.y - (y - 2));
					int rz = (int)round(pi->position3.z - (z - 10));
					int sum = rx * 100 + ry * 10 + rz;

					switch (sum)
					{
					case(1):
						count[0]++;
						if (count[0] > drawValue)
							value |= 1;
						break;
					case(101):
						count[1]++;
						if (count[1] > drawValue)
							value |= 2;
						break;
					case(100):
						count[2]++;
						if (count[2] > drawValue)
							value |= 4;
						break;
					case(0):
						count[3]++;
						if (count[3] > drawValue)
							value |= 8;
						break;
					case(11):
						count[4]++;
						if (count[4] > drawValue)
							value |= 16;
						break;
					case(111):
						count[5]++;
						if (count[5] > drawValue)
							value |= 32;
						break;
					case(110):
						count[6]++;
						if (count[6] > drawValue)
							value |= 64;
						break;
					case(10):
						count[7]++;
						if (count[7] > drawValue)
							value |= 128;
						break;
					default:
						break;
					}


				}
				marchingData[x][y][z] = value;

			}

		}
	}
}