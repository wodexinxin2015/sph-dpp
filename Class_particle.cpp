/************************************************************************************
SPH Data Post Viewer

--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#include <cmath>
#include "Class_particle.h"

/*initializing of mass*/

Particle::Particle()
{
	int i;

	for (i = 0; i < 3; i++)
	{
		this->x[i] = 0.0;
		this->xp[i] = 0.0;
		this->vx[i] = 0.0;
		this->vxp[i] = 0.0;
		this->ax[i] = 0.0;
		this->axp[i] = 0.0;

		this->ux[i] = 0.0; /*2016-7-5*/

		this->strep[i] = 0.0; /*2016-7-7*/

		this->divx[i][0] = 0.0;
		this->divx[i][1] = 0.0;
		this->divx[i][2] = 0.0;

		this->interf[i] = 0.0;
		this->interfss[i] = 0.0;

		this->weps[i][0] = 0.0;
		this->weps[i][1] = 0.0;
		this->weps[i][2] = 0.0;

		this->beta[i][0] = 0.0;
		this->beta[i][1] = 0.0;
		this->beta[i][2] = 0.0;

		this->beta_ts[i][0] = 0.0;
		this->beta_ts[i][1] = 0.0;
		this->beta_ts[i][2] = 0.0;
	}

	tsqc = 0.0;

	rho = 0.0;
	rhop = 0.0;
	mass = 0.0;
	hl = 0.0;

	pre = 0.0;
	prep = 0.0;

	e = 0.0;

	for (i = 0; i < 6; i++)
	{
		this->eps[i] = 0.0;
		this->deps[i] = 0.0;
		this->sig[i] = 0.0;
		this->dsig[i] = 0.0;
		this->tssig[i] = 0.0;
		this->adps[i] = 0.0;
	}
	ff_sig[0] = 0.0;
	ff_sig[1] = 0.0;
	ff_sig[2] = 0.0;

	satu = 0.0;
	dsatu = 0.0;
	suct = 0.0;
	cop = 0.0;

	vsig = 0.0;
	veps = 0.0;
	divq = 0.0;
	divr = 0.0;
	meps = 0.0;

	roue = 0.0;
	rous = 0.0;
	zeta = 0.0;
	rsta = 0.0;
	prew = 0.0;
	prea = 0.0;

	porosity = 0.0;
	cd = 0.0;
	poro_cop = 0.0;

	type = 0;
	matype = 0;
	permtype = 0;
	etype = 0;
}

Particle::~Particle()
{
}

Para_Mat::Para_Mat()
{
    xp[0]=0.0;
    xp[1] = 0.0;
    xp[2] = 0.0;
    x[0]=0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    dens=0.0;
    fai=0.0;
    c=0.0;
    cop=0.0;
    em=0.0;
    lamda=0.0;
    kapa=0.0;
    visc=0.0;
    ds=0.0;
    type=0;
    matype=0;
}

Para_Mat::~Para_Mat()
{
}
