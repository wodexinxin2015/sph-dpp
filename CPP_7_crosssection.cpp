/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <math.h>
# include "string.h"
# include "Header_Option.h"
# include "Class_Functions.h"

//cross section data output for the 2D and 3D cases
int clCrossOutput_Fun::cross_output(Para_Pro pPPro, char *argv, FILE *flog) {
	int nstep, i, x_id;
	double x_poi, xmax[3], xmin[3];
	char file_name[100], temp[100];
	Particle *pPar, *cutPar;
	Para_Pro cutPro;
	FILE *finp;
	int err_t;

	//input of output steps
	printf("-------------------------------------------------------------------\n");
	printf("Please input step number: > 0--inp output; 0--exit\n");
	err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");

	//input of cutting coordinate
	printf("-------------------------------------------------------------------\n");
	printf("Please input id of cutting coordinate (x_id): 1--x; 2--y; 3--z\n");
	printf("x_id must be less than or equal to the problem dimensions.\n");
	err_t = scanf("%d", &x_id);
	printf("-------------------------------------------------------------------\n");

	if (x_id < 1 || x_id > pPPro.ndim) {
		return 17;
	}

	//initializing of problem domain
	for (i = 0; i < 3; i++) {
		xmax[i] = -10000.0;
		xmin[i] = 10000.0;
	}

	//memory allocation
	pPar = new Particle[pPPro.ntotal];
	cutPar = new Particle[int (pPPro.ntotal / 3)];
	finp = NULL;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

	//reading data, cutting section and output
	if (nstep > 0 && nstep <= pPPro.loop) {
		pPPro.l = nstep;
		if (nstep < 10)
		{
			sprintf(temp, "000000000%d", nstep);
		}
		else if (nstep < 100)
		{
			sprintf(temp, "00000000%d", nstep);
		}
		else if (nstep < 1000)
		{
			sprintf(temp, "0000000%d", nstep);
		}
		else if (nstep < 10000)
		{
			sprintf(temp, "000000%d", nstep);
		}
		else if (nstep < 100000)
		{
			sprintf(temp, "00000%d", nstep);
		}
		else if (nstep < 1000000)
		{
			sprintf(temp, "0000%d", nstep);
		}
		else if (nstep < 10000000)
		{
			sprintf(temp, "000%d", nstep);
		}
		else if (nstep < 100000000)
		{
			sprintf(temp, "00%d", nstep);
		}
		//read file
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\data-");
		else strcat(file_name, "/data-");
		strcat(file_name, temp);
		strcat(file_name, ".odb");

		if ((finp = fopen(file_name, "r")) == NULL) {
			return 14;
		}

		//reading data
		read_data(pPar, pPPro, finp);

		//initializing of pPPro numbers
		pPPro.nboun = 0;
		pPPro.nwater = 0;
		pPPro.nsoil = 0;
		pPPro.nair = 0;

		//counting number of particles for each phase
		for (i = 0; i < pPPro.ntotal; i++) {
			if (pPar[i].type == 0) pPPro.nboun += 1;
			if (pPar[i].type == 1) pPPro.nwater += 1;
			if (pPar[i].type == 2) pPPro.nsoil += 1;
			if (pPar[i].type == 3) pPPro.nair += 1;
			if (pPar[i].type == 4) pPPro.nsoil += 1;

			xmax[0] = fmax(xmax[0], pPar[i].xp[0]);
			xmax[1] = fmax(xmax[1], pPar[i].xp[1]);
			xmax[2] = fmax(xmax[2], pPar[i].xp[2]);

			xmin[0] = fmin(xmin[0], pPar[i].xp[0]);
			xmin[1] = fmin(xmin[1], pPar[i].xp[1]);
			xmin[2] = fmin(xmin[2], pPar[i].xp[2]);
		}

		//selecting particlesby checking xp[x_id - 1] = x_poi
		printf("-------------------------------------------------------------------\n");
		printf("Please input id of cutting position (x_poi): \n");
		printf("x_poi must be between %10.5f and %10.5f.\n", xmin[x_id - 1], xmax[x_id - 1]);
		err_t = scanf("%lf", &x_poi);
		printf("-------------------------------------------------------------------\n");

		//check if x_poi is correct
		if (x_poi > xmax[x_id - 1] || x_poi < xmin[x_id - 1]) {
			return 18;
		}

		//cutting section
		check_position(pPar, cutPar, pPPro, &cutPro, x_id, x_poi);

		//ouput to svg
		output_velocity(cutPar, cutPro, x_poi, argv);
		output_stress(cutPar, cutPro, x_poi, argv);

		memset(file_name, 0, 100);
		memset(temp, 0, 100);

		if (finp != NULL) fclose(finp);

	}
	else {
		printf("-------------------------------------------------------------------\n");
		printf("Step No. is not correct (total steps: %d).\n", pPPro.loop);
		printf("-------------------------------------------------------------------\n");
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "Step No. is not correct (total steps: %d).\n", pPPro.loop);
		fprintf(flog, "-------------------------------------------------------------------\n");
	}

	//memory free
	delete[]pPar;
	delete[]cutPar;

	return 0;
}

void clCrossOutput_Fun::read_data(Particle * pPar, Para_Pro pPPro, FILE * finp) {
	int temp, i;
	char c[2];
	int err_t;
	double uxp;

	for (i = 1; i < 3; i++) {
		err_t = fscanf(finp, "%c%c%*[^\n]%*c", &c[0], &c[1]);
	}

	for (i = 0; i < pPPro.ntotal; i++) {
		err_t = fscanf(finp, "%d%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%d%d",
&temp, &pPar[i].xp[0], &pPar[i].xp[1], &pPar[i].xp[2], &pPar[i].ux[0], &pPar[i].ux[1], &pPar[i].ux[2],
&uxp, &pPar[i].axp[0], &pPar[i].axp[1], &pPar[i].axp[2], &pPar[i].vxp[0], &pPar[i].vxp[1], &pPar[i].vxp[2],
&pPar[i].pre, &pPar[i].prea, &pPar[i].prew,
&pPar[i].sig[0], &pPar[i].sig[1], &pPar[i].sig[2], &pPar[i].sig[3], &pPar[i].sig[4], &pPar[i].sig[5],
&pPar[i].strep[0], &pPar[i].strep[1], &pPar[i].strep[2],
&pPar[i].eps[0], &pPar[i].eps[1], &pPar[i].eps[2], &pPar[i].eps[3], &pPar[i].eps[4], &pPar[i].eps[5],
&pPar[i].adps[0], &pPar[i].adps[1], &pPar[i].adps[2], &pPar[i].adps[3], &pPar[i].adps[4], &pPar[i].adps[5],
&pPar[i].vsig, &pPar[i].veps, &pPar[i].meps, &pPar[i].divq, &pPar[i].divr, &pPar[i].satu,
&pPar[i].suct, &pPar[i].interfss[0], &pPar[i].interfss[1], &pPar[i].interfss[2], &pPar[i].type, &pPar[i].matype);
		pPar[i].x[0] = pPar[i].xp[0];
		pPar[i].x[1] = pPar[i].xp[1];
		pPar[i].x[2] = pPar[i].xp[2];

		pPar[i].vx[0] = pPar[i].vxp[0];
		pPar[i].vx[1] = pPar[i].vxp[1];
		pPar[i].vx[2] = pPar[i].vxp[2];


		pPar[i].ax[0] = pPar[i].axp[0];
		pPar[i].ax[1] = pPar[i].axp[1];
		pPar[i].ax[2] = pPar[i].axp[2];
	}
}

/*Save the information of velocity particles to files*/
void clCrossOutput_Fun::output_velocity(Particle *pVelPar, const Para_Pro &pPPro, double x_poi, char *argv) {

	char file_name[100], temp[100];
	int i, temp1[5];
	double utotal;
	double vxtotal;

	/*File name*/
	FILE *fpw, *fps, *fpt, *fpa;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

	if (pPPro.l < 10)
	{
		sprintf(temp, "000000000%d", pPPro.l);
	}
	else if (pPPro.l < 100)
	{
		sprintf(temp, "00000000%d", pPPro.l);
	}
	else if (pPPro.l < 1000)
	{
		sprintf(temp, "0000000%d", pPPro.l);
	}
	else if (pPPro.l < 10000)
	{
		sprintf(temp, "000000%d", pPPro.l);
	}
	else if (pPPro.l < 100000)
	{
		sprintf(temp, "00000%d", pPPro.l);
	}
	else if (pPPro.l < 1000000)
	{
		sprintf(temp, "0000%d", pPPro.l);
	}
	else if (pPPro.l < 10000000)
	{
		sprintf(temp, "000%d", pPPro.l);
	}
	else if (pPPro.l < 100000000)
	{
		sprintf(temp, "00%d", pPPro.l);
	}

	/*Open files*/
	strcpy(file_name, argv);
	if(win32) strcat(file_name, "\\velocity-step");
	else strcat(file_name, "/velocity-step");
	strcat(file_name, temp);
	strcat(file_name, "+Loc=");
	sprintf(temp, "%f", x_poi);
	strcat(file_name, temp);
	strcat(file_name, ".inp");

	fpt = fopen(file_name, "w");
	fprintf(fpt, "#%s\n", temp);
	fprintf(fpt, "%d  %d  %d  %d  %d\n", pPPro.ntotal, 0, 12, 0, 0);
	

	fpw = NULL;
	fps = NULL;
	fpa = NULL;

	if (pPPro.nwater > 0) {  ///< liquid//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\velocity-liquid");
		else  strcat(file_name, "/velocity-liquid");
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fpw = fopen(file_name, "w");
		fprintf(fpw, "#%s\n", temp);
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater + pPPro.nboun, 0, 11, 0, 0);
	}

	if (pPPro.nsoil > 0) {   ///< solid//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\velocity-solid");
		else  strcat(file_name, "/velocity-solid");
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fps = fopen(file_name, "w");
		fprintf(fps, "#%s\n", temp);
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil + pPPro.nboun, 0, 11, 0, 0);
	}

	if (pPPro.nair > 0) {  ///< gas//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\velocity-gas");
		else  strcat(file_name, "/velocity-gas");
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fpa = fopen(file_name, "w");
		fprintf(fpa, "#%s\n", temp);
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair + pPPro.nboun, 0, 11, 0, 0);
	}
	//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
		///< coordinate information//
	for (i = 0; i < pPPro.ntotal; i++) {
		if (pVelPar[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 1) {
			temp1[1] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 2) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 3) {
			temp1[3] += 1;
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 4) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		temp1[0] += 1;
		fprintf(fpt, "%5d %14.6e %14.6e %14.6e\n", temp1[0], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);

	}

	if (pPPro.nwater > 0) {
		fprintf(fpw, "%d %d %d %d %d %d %d %d %d %d %d %d\n", 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		fprintf(fpw, "ux,m\n uy,m\n uz,m\n u,m\n vx,m/s\n vy,m/s\n vz,m/s/s\n v,m/s\n ax,m/s/s\n ay,m/s/s\n az,m/s/s\n");
	}
	if (pPPro.nsoil > 0) {
		fprintf(fps, "%d %d %d %d %d %d %d %d %d %d %d %d\n", 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		fprintf(fps, "ux,m\n uy,m\n uz,m\n u,m\n vx,m/s\n vy,m/s\n vz,m/s\n v,m/s\n ax,m/s/s\n ay,m/s/s\n az,m/s/s\n");
	}
	if (pPPro.nair > 0) {
		fprintf(fpa, "%d %d %d %d %d %d %d %d %d %d %d %d\n", 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		fprintf(fpa, "ux,m\n uy,m\n uz,m\n u,m\n vx,m/s\n vy,m/s\n vz,m/s\n v,m/s\n ax,m/s/s\n ay,m/s/s\n az,m/s/s\n");
	}
	{
		fprintf(fpt, "%d %d %d %d %d %d %d %d %d %d %d %d %d\n", 12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		fprintf(fpt, "ux,m\n uy,m\n uz,m\n u,m\n vx,m/s\n vy,m/s\n vz,m/s\n v,m/s\n ax,m/s/s\n ay,m/s/s\n az,m/s/s \n type,1\n");
	}
	///< data//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	for (i = 0; i < pPPro.ntotal; i++) {
		utotal = pVelPar[i].ux[0] * pVelPar[i].ux[0]
			+ pVelPar[i].ux[1] * pVelPar[i].ux[1] + pVelPar[i].ux[2] * pVelPar[i].ux[2];
		utotal = sqrt(utotal);
		vxtotal= pVelPar[i].vxp[0] * pVelPar[i].vxp[0]
			+ pVelPar[i].vxp[1] * pVelPar[i].vxp[1] + pVelPar[i].vxp[2] * pVelPar[i].vxp[2];
		vxtotal = sqrt(vxtotal);

		if (pVelPar[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[1], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal,
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal,
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[3], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal,
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
		}

		if (pVelPar[i].type == 1) {
			temp1[1] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[1], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal, 
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
		}

		if (pVelPar[i].type == 2) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal, 
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
		}

		if (pVelPar[i].type == 3) {
			temp1[3] += 1;
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[3], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal, 
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
		}

		if (pVelPar[i].type == 4) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal, 
				pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
				pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2]);
		}

		temp1[0] += 1;
		fprintf(fpt, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %d \n", \
			temp1[0], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2], utotal, 
			pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], vxtotal,
			pVelPar[i].ax[0], pVelPar[i].ax[1], pVelPar[i].ax[2],
			pVelPar[i].type);

	}

	/*Close files*/
	fclose(fpt);
	if (pPPro.nwater > 0) fclose(fpw);
	if (pPPro.nsoil > 0) fclose(fps);
	if (pPPro.nair > 0) fclose(fpa);

}

/*Save the information of stress particles to files*/
void clCrossOutput_Fun::output_stress(Particle *pVelPar, const Para_Pro &pPPro, double x_poi, char *argv) {

	char file_name[100], temp[100];
	int i, temp1[5];

	/*File name*/
	FILE* fpw, * fps, * fpa;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

	if (pPPro.l < 10)
	{
		sprintf(temp, "000000000%d", pPPro.l);
	}
	else if (pPPro.l < 100)
	{
		sprintf(temp, "00000000%d", pPPro.l);
	}
	else if (pPPro.l < 1000)
	{
		sprintf(temp, "0000000%d", pPPro.l);
	}
	else if (pPPro.l < 10000)
	{
		sprintf(temp, "000000%d", pPPro.l);
	}
	else if (pPPro.l < 100000)
	{
		sprintf(temp, "00000%d", pPPro.l);
	}
	else if (pPPro.l < 1000000)
	{
		sprintf(temp, "0000%d", pPPro.l);
	}
	else if (pPPro.l < 10000000)
	{
		sprintf(temp, "000%d", pPPro.l);
	}
	else if (pPPro.l < 100000000)
	{
		sprintf(temp, "00%d", pPPro.l);
	}

	/*Open files*/

	fpw = NULL;
	fps = NULL;
	fpa = NULL;

	if (pPPro.nwater > 0) {  ///< liquid//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Stress-liquid");
		else  strcat(file_name, "/Stress-liquid");
		strcat(file_name, temp);
		strcat(file_name, "+Loc=");
		sprintf(temp, "%f", x_poi);
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fpw = fopen(file_name, "w");
		fprintf(fpw, "#%s\n", temp);
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater + pPPro.nboun, 0, 4, 0, 0);;
	}

	if (pPPro.nsoil > 0) {   ///< solid//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Stress-solid");
		else  strcat(file_name, "/Stress-solid");
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fps = fopen(file_name, "w");
		fprintf(fps, "#%s\n", temp);
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil + pPPro.nboun, 0, 34, 0, 0);
	}

	if (pPPro.nair > 0) {  ///< gas//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Stress-gas");
		else  strcat(file_name, "/Stress-gas");
		strcat(file_name, temp);
		strcat(file_name, ".inp");
		fpa = fopen(file_name, "w");
		fprintf(fpa, "#%s\n", temp);
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair + pPPro.nboun, 0, 4, 0, 0);
	}

	///< coordinate information//
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	for (i = 0; i < pPPro.ntotal; i++) {

		if (pVelPar[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 1) {
			temp1[1] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 2) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 3) {
			temp1[3] += 1;
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

		if (pVelPar[i].type == 4) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
		}

	}
	if (pPPro.nwater > 0) {
		fprintf(fpw, "%d %d %d %d %d\n", 4, 1, 1, 1, 1);
		fprintf(fpw, "vx,m/s\n vy,m/s\n vz,m/s\n pre,pa\n");
	}
	if (pPPro.nsoil > 0) {
		fprintf(fps, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			34, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		fprintf(fps, " vx,m/s\n vy,m/s\n vz,m/s\n prea,pa\n prew,pa\n sxx,pa\n syy,pa\n szz,pa\n sxy,pa\n syz,pa\n szx,pa\n");
		fprintf(fps, " psxx,pa\n psyy,pa\n pszz,pa\n");
		fprintf(fps, " txx,1\n tyy,1\n tzz,1\n txy,1\n tyz,1\n tzx,1\n apsxx,1\n apsyy,1\n apszz,1\n apsxy,1\n apsyz,1\n apszx,1\n ");
		fprintf(fps, " vsig,pa\n veps,1\n i2eps,1\n divq,pa\n divr,1\n satu,1\n suct,pa\n matype,1\n");
	}
	if (pPPro.nair > 0) {
		fprintf(fpa, "%d %d %d %d %d\n", 4, 1, 1, 1, 1);
		fprintf(fpa, "vx,m/s\n vy,m/s\n vz,m/s\n pre,pa\n");
	}
	///< data//
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	for (i = 0; i < pPPro.ntotal; i++) {

		if (pVelPar[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
				temp1[1], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], 0.0);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e %14.6e %3d\n", \
temp1[2], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
pVelPar[i].strep[0], pVelPar[i].strep[1], pVelPar[i].strep[2],
0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
pVelPar[i].adps[0], pVelPar[i].adps[1], pVelPar[i].adps[2], pVelPar[i].adps[3], pVelPar[i].adps[4], pVelPar[i].adps[5],
pVelPar[i].vsig, pVelPar[i].veps, pVelPar[i].meps, pVelPar[i].divq, pVelPar[i].divr, pVelPar[i].satu,
pVelPar[i].suct, pVelPar[i].matype);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
				temp1[3], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], 0.0);

		}

		if (pVelPar[i].type == 1) {
			temp1[1] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
				temp1[1], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], pVelPar[i].pre);
		}

		if (pVelPar[i].type == 2) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e %14.6e %3d\n", \
temp1[2], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], pVelPar[i].prea, pVelPar[i].prew,
pVelPar[i].sig[0], pVelPar[i].sig[1], pVelPar[i].sig[2], pVelPar[i].sig[3], pVelPar[i].sig[4], pVelPar[i].sig[5],
pVelPar[i].strep[0], pVelPar[i].strep[1], pVelPar[i].strep[2],
pVelPar[i].eps[0], pVelPar[i].eps[1], pVelPar[i].eps[2], pVelPar[i].eps[3], pVelPar[i].eps[4], pVelPar[i].eps[5],
pVelPar[i].adps[0], pVelPar[i].adps[1], pVelPar[i].adps[2], pVelPar[i].adps[3], pVelPar[i].adps[4], pVelPar[i].adps[5],
pVelPar[i].vsig, pVelPar[i].veps, pVelPar[i].meps, pVelPar[i].divq, pVelPar[i].divr, pVelPar[i].satu,
pVelPar[i].suct, pVelPar[i].matype);
		}

		if (pVelPar[i].type == 3) {
			temp1[3] += 1;
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
				temp1[3], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], pVelPar[i].pre);
		}

		if (pVelPar[i].type == 4) {
			temp1[2] += 1;
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e %14.6e %3d\n", \
temp1[2], pVelPar[i].vxp[0], pVelPar[i].vxp[1], pVelPar[i].vxp[2], pVelPar[i].prea, pVelPar[i].prew,
pVelPar[i].sig[0], pVelPar[i].sig[1], pVelPar[i].sig[2], pVelPar[i].sig[3], pVelPar[i].sig[4], pVelPar[i].sig[5],
pVelPar[i].strep[0], pVelPar[i].strep[1], pVelPar[i].strep[2],
pVelPar[i].eps[0], pVelPar[i].eps[1], pVelPar[i].eps[2], pVelPar[i].eps[3], pVelPar[i].eps[4], pVelPar[i].eps[5],
pVelPar[i].adps[0], pVelPar[i].adps[1], pVelPar[i].adps[2], pVelPar[i].adps[3], pVelPar[i].adps[4], pVelPar[i].adps[5],
pVelPar[i].vsig, pVelPar[i].veps, pVelPar[i].meps, pVelPar[i].divq, pVelPar[i].divr, pVelPar[i].satu,
pVelPar[i].suct, pVelPar[i].matype);
		}

	}

	/*Close files*/
	if (pPPro.nwater > 0) fclose(fpw);
	if (pPPro.nsoil > 0) fclose(fps);
	if (pPPro.nair > 0) fclose(fpa);
}

//selecting particles from the total set of particles: xp[x_id-1]=x_poi?
void clCrossOutput_Fun::check_position(Particle *InpPar, Particle *OutPar, const Para_Pro &InPPro, Para_Pro *OutPPro, 
	int x_id, double x_poi){
	int ntotal, id, ntotal1, i;
	int nbound, nwater, nsoil, nair;
	double dist_stand = 0.2*InPPro.dr;
	double dist_cal;

	ntotal = InPPro.ntotal; //number of total particles 
	id = x_id - 1;

	ntotal1 = 0; //total number of selected particles
	nbound = 0;  //total number of selected boundary particles
	nwater = 0;  //total number of selected water particles
	nsoil = 0;  //total number of selected soil particles
	nair = 0;  //total number of selected air particles

	//selecting particles by checking xp[x_id-1]=x_poi
	for (i = 0; i < ntotal; i++) {
		dist_cal = fabs(InpPar[i].xp[id] - x_poi);
		if(dist_cal < dist_stand) {
			ntotal1 += 1;
			OutPar[ntotal1 - 1] = InpPar[i];
			if (OutPar[ntotal1 - 1].type == 0) {
				nbound += 1;
			}
			else if (OutPar[ntotal1 - 1].type == 1) {
				nwater += 1;
			}
			else if (OutPar[ntotal1 - 1].type == 2) {
				nsoil += 1;
			}
			else if (OutPar[ntotal1 - 1].type == 3) {
				nair += 1;
			}
			else if (OutPar[ntotal1 - 1].type == 4) {
				nsoil += 1;
			}
		}
	}

	//change data in OutPPro
	OutPPro->ntotal = ntotal1;
	OutPPro->nboun = nbound;
	OutPPro->nwater = nwater;
	OutPPro->nsoil = nsoil;
	OutPPro->nair = nair;
	OutPPro->l = InPPro.l;
}