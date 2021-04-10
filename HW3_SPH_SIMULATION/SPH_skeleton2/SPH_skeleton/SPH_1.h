#pragma once
#include "Particle.h"

using namespace std;
#define GAS_CONSTANT 50.0
#define GRIDSIZE 40
#define HASHSIZE GRIDSIZE+2

class SPH
{
public:
	vector<Particle *> particles;
	int index = 0;		//particle index
	int PI = 3.1415926;
	int MaxParticle;
	int iteration_n;
private:
	double rest_density = 4.2;
	double u = 0.8;	// viscosity constant
	double h = 1.0; // kernel radius
	enum kernels {
		poly6 = 0,
		spiky,
		viscosity
	};
public:
	SPH();
	SPH(int numparticle);
	~SPH();

	void ResetParticle();
	void Init();
	void DamBreaking();
	void Pouring();
	void Update(float dt, vec2 gravity);
	void Draw();
public:
	void HashTable();
	void computeDensity();
	void compute_force();
	void Integrate(double dt, vec2 gravity);

private:
	//poly6 kernel
	double cpoly6 = 4.0 / PI;
	double cpoly6G = -24.0 / PI;
	double cpoly6L = -48.0 / PI;
	double cspikyG = -30.0 / PI;
	double cviscosityL = 40 / PI;
	double Kernel(vec2 rij, double norm, kernels kernel);
	vec2 gradientKernel(vec2 rij, double norm, kernels kernel);
	double laplacianKernel(vec2 rij, double norm, kernels kernel);
	double calcSubpressure(double gas_constant, Particle *pi, Particle *pj, double rest_density);
	vec2 calcSubviscosity(Particle* pi, Particle* pj);
private:
	vector<Particle *> hashGrid[HASHSIZE][HASHSIZE];
	vector<Particle*> GetNeighbor(int gridx, int gridy, double radius, vector<Particle*>& mine);
};