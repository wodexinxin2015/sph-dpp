/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_DEPRECATE
# include <stdio.h>
# include <cmath>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

//main subroutine of inp format output for microavs
int clInpOutput_Fun::inp_output(Para_Pro pPPro, char *argv, FILE *flog) {
	int nstep, i, bndy_flag;
	char file_name[100], temp[100];
	Particle *pPar;
	FILE *finp;
	int err_t;

	printf("-------------------------------------------------------------------\n");
	printf("Show the boundary: 0--no boundary; 1--boundary; 2--virtual boundary\n");
	err_t = scanf("%d", &bndy_flag);
	printf("-------------------------------------------------------------------\n");

	printf("-------------------------------------------------------------------\n");	
	printf("Please input step number: > 0--inp output; 0--exit\n");
	err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");

	pPar = new Particle[pPPro.ntotal];
	finp = NULL;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

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

		read_data(pPar, pPPro, finp);

		pPPro.nboun = 0;
		pPPro.nwater = 0;
		pPPro.nsoil = 0;
		pPPro.nair = 0;

		for (i = 0; i < pPPro.ntotal; i++) {
			if (pPar[i].type == 0) pPPro.nboun += 1;
			if (pPar[i].type == 1) pPPro.nwater += 1;
			if (pPar[i].type == 2) pPPro.nsoil += 1;
			if (pPar[i].type == 3) pPPro.nair += 1;
			if (pPar[i].type == 4) pPPro.nsoil += 1;
		}

		//ouput to svg
		if (bndy_flag == 0) {
			output_velocity(pPar, pPPro, argv);
			output_stress(pPar, pPPro, argv);
		}
		else if(bndy_flag == 1) {
			output_velocity_bndy(pPar, pPPro, argv);
			output_stress_bndy(pPar, pPPro, argv);
		}
		else if (bndy_flag == 2) {
			output_velocity_virtbndy(pPar, pPPro, argv);
			output_stress_virtbndy(pPar, pPPro, argv);
		}

		memset(file_name, 0, 100);
		memset(temp, 0, 100);

	}
	else {
		printf("-------------------------------------------------------------------\n");
		printf("Step No. is not correct (total steps: %d).\n", pPPro.loop);
		printf("-------------------------------------------------------------------\n");
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "Step No. is not correct (total steps: %d).\n", pPPro.loop);
		fprintf(flog, "-------------------------------------------------------------------\n");
	}

	delete[]pPar;
	if (finp != NULL) fclose(finp);

	return 0;
}

/*Save the information of velocity particles to inp file with boundary particles*/
void clInpOutput_Fun::output_velocity_virtbndy(Particle* pVelPar, const Para_Pro& pPPro, char* argv) {

	char file_name[100], temp[100];
	int i, temp1[5];
	double utotal;
	double vxtotal;
	Particle bndy_par[2];
	double xmax[3], xmin[3];

	/*File name*/
	FILE* fpw, * fps, * fpt, * fpa;

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

	/*determine the problem domain*/
	xmax[0] = -10000.0;
	xmax[1] = -10000.0;
	xmax[2] = -10000.0;
	xmin[0] = 10000.0;
	xmin[1] = 10000.0;
	xmin[2] = 10000.0;
	for (i = 0; i < pPPro.ntotal; i++) {
		xmax[0] = fmax(xmax[0], pVelPar[i].xp[0]);
		xmax[1] = fmax(xmax[1], pVelPar[i].xp[1]);
		xmax[2] = fmax(xmax[2], pVelPar[i].xp[2]);
		xmin[0] = fmin(xmin[0], pVelPar[i].xp[0]);
		xmin[1] = fmin(xmin[1], pVelPar[i].xp[1]);
		xmin[2] = fmin(xmin[2], pVelPar[i].xp[2]);
	}

	bndy_par[0].xp[0] = xmin[0];
	bndy_par[0].xp[1] = xmin[1];
	bndy_par[0].xp[2] = xmin[2];
	bndy_par[1].xp[0] = xmax[0];
	bndy_par[1].xp[1] = xmax[1];
	bndy_par[1].xp[2] = xmax[2];

	/*Open files*/
	strcpy(file_name, argv);
	if (win32) strcat(file_name, "\\velocity-step");
	else strcat(file_name, "/velocity-step");
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
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater + 2, 0, 11, 0, 0);
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
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil + 2, 0, 11, 0, 0);
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
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair + 2, 0, 11, 0, 0);
	}
	//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	///< coordinate information//
	if (pPPro.nwater > 0) {
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
	if (pPPro.nsoil> 0) {
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
	if (pPPro.nair > 0) {
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
		
	for (i = 0; i < pPPro.ntotal; i++) {
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

	if (pPPro.nwater > 0) {
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[1], bndy_par[0].ux[0], bndy_par[0].ux[1], bndy_par[0].ux[2], 0.0,
			bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], 0.0,
			bndy_par[0].ax[0], bndy_par[0].ax[1], bndy_par[0].ax[2]);
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[1], bndy_par[1].ux[0], bndy_par[1].ux[1], bndy_par[1].ux[2], 0.0,
			bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], 0.0,
			bndy_par[1].ax[0], bndy_par[1].ax[1], bndy_par[1].ax[2]);
	}
	if (pPPro.nsoil > 0) {
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[2], bndy_par[0].ux[0], bndy_par[0].ux[1], bndy_par[0].ux[2], 0.0,
			bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], 0.0,
			bndy_par[0].ax[0], bndy_par[0].ax[1], bndy_par[0].ax[2]);
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[2], bndy_par[1].ux[0], bndy_par[1].ux[1], bndy_par[1].ux[2], 0.0,
			bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], 0.0,
			bndy_par[1].ax[0], bndy_par[1].ax[1], bndy_par[1].ax[2]);
	}
	if (pPPro.nair > 0) {
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[3], bndy_par[0].ux[0], bndy_par[0].ux[1], bndy_par[0].ux[2], 0.0,
			bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], 0.0,
			bndy_par[0].ax[0], bndy_par[0].ax[1], bndy_par[0].ax[2]);
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[3], bndy_par[1].ux[0], bndy_par[1].ux[1], bndy_par[1].ux[2], 0.0,
			bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], 0.0,
			bndy_par[1].ax[0], bndy_par[1].ax[1], bndy_par[1].ax[2]);
	}

	for (i = 0; i < pPPro.ntotal; i++) {
		utotal = pVelPar[i].ux[0] * pVelPar[i].ux[0]
			+ pVelPar[i].ux[1] * pVelPar[i].ux[1] + pVelPar[i].ux[2] * pVelPar[i].ux[2];
		utotal = sqrt(utotal);
		vxtotal = pVelPar[i].vxp[0] * pVelPar[i].vxp[0]
			+ pVelPar[i].vxp[1] * pVelPar[i].vxp[1] + pVelPar[i].vxp[2] * pVelPar[i].vxp[2];
		vxtotal = sqrt(vxtotal);

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

/*Save the information of stress particles to inp file with boundary particles*/
void clInpOutput_Fun::output_stress_virtbndy(Particle* pVelPar, const Para_Pro& pPPro, char* argv) {

	char file_name[100], temp[100];
	int i, temp1[5];

	/*File name*/
	FILE* fpw, * fps, * fpa;
	Particle bndy_par[2];
	double xmax[3], xmin[3];

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

	/*determine the problem domain*/
	xmax[0] = -10000.0;
	xmax[1] = -10000.0;
	xmax[2] = -10000.0;
	xmin[0] = 10000.0;
	xmin[1] = 10000.0;
	xmin[2] = 10000.0;
	for (i = 0; i < pPPro.ntotal; i++) {
		xmax[0] = fmax(xmax[0], pVelPar[i].xp[0]);
		xmax[1] = fmax(xmax[1], pVelPar[i].xp[1]);
		xmax[2] = fmax(xmax[2], pVelPar[i].xp[2]);
		xmin[0] = fmin(xmin[0], pVelPar[i].xp[0]);
		xmin[1] = fmin(xmin[1], pVelPar[i].xp[1]);
		xmin[2] = fmin(xmin[2], pVelPar[i].xp[2]);
	}

	bndy_par[0].xp[0] = xmin[0];
	bndy_par[0].xp[1] = xmin[1];
	bndy_par[0].xp[2] = xmin[2];
	bndy_par[1].xp[0] = xmax[0];
	bndy_par[1].xp[1] = xmax[1];
	bndy_par[1].xp[2] = xmax[2];

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
		strcat(file_name, ".inp");
		fpw = fopen(file_name, "w");
		fprintf(fpw, "#%s\n", temp);
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater + 2, 0, 4, 0, 0);;
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
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil + 2, 0, 34, 0, 0);
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
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair + 2, 0, 4, 0, 0);
	}

	///< coordinate information//
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	if (pPPro.nwater > 0) {
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
	if (pPPro.nsoil > 0) {
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
	if (pPPro.nair > 0) {
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], bndy_par[0].xp[0], bndy_par[0].xp[1], bndy_par[0].xp[2]);
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], bndy_par[1].xp[0], bndy_par[1].xp[1], bndy_par[1].xp[2]);
	}
	for (i = 0; i < pPPro.ntotal; i++) {
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
	if (pPPro.nwater > 0) {
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
			temp1[1], bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], bndy_par[0].pre);
		temp1[1] += 1;
		fprintf(fpw, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
			temp1[1], bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], bndy_par[1].pre);
	}
	if (pPPro.nsoil > 0) {
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e %14.6e %3d\n", \
temp1[2], bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], bndy_par[0].prea, bndy_par[0].prew,
bndy_par[0].sig[0], bndy_par[0].sig[1], bndy_par[0].sig[2], bndy_par[0].sig[3], bndy_par[0].sig[4], bndy_par[0].sig[5],
bndy_par[0].strep[0], bndy_par[0].strep[1], bndy_par[0].strep[2],
bndy_par[0].eps[0], bndy_par[0].eps[1], bndy_par[0].eps[2], bndy_par[0].eps[3], bndy_par[0].eps[4], bndy_par[0].eps[5],
bndy_par[0].adps[0], bndy_par[0].adps[1], bndy_par[0].adps[2], bndy_par[0].adps[3], bndy_par[0].adps[4], bndy_par[0].adps[5],
bndy_par[0].vsig, bndy_par[0].veps, bndy_par[0].meps, bndy_par[0].divq, bndy_par[0].divr, bndy_par[0].satu,
bndy_par[0].suct, bndy_par[0].matype);
		temp1[2] += 1;
		fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e %14.6e %3d\n", \
temp1[2], bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], bndy_par[1].prea, bndy_par[1].prew,
bndy_par[1].sig[0], bndy_par[1].sig[1], bndy_par[1].sig[2], bndy_par[1].sig[3], bndy_par[1].sig[4], bndy_par[1].sig[5],
bndy_par[1].strep[0], bndy_par[1].strep[1], bndy_par[1].strep[2],
bndy_par[1].eps[0], bndy_par[1].eps[1], bndy_par[1].eps[2], bndy_par[1].eps[3], bndy_par[1].eps[4], bndy_par[1].eps[5],
bndy_par[1].adps[0], bndy_par[1].adps[1], bndy_par[1].adps[2], bndy_par[1].adps[3], bndy_par[1].adps[4], bndy_par[1].adps[5],
bndy_par[1].vsig, bndy_par[1].veps, bndy_par[1].meps, bndy_par[1].divq, bndy_par[1].divr, bndy_par[1].satu,
bndy_par[1].suct, bndy_par[1].matype);
	}
	if (pPPro.nair > 0) {
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
			temp1[3], bndy_par[0].vxp[0], bndy_par[0].vxp[1], bndy_par[0].vxp[2], bndy_par[0].pre);
		temp1[3] += 1;
		fprintf(fpa, "%5d %14.6e %14.6e %14.6e %14.6e \n", \
			temp1[3], bndy_par[1].vxp[0], bndy_par[1].vxp[1], bndy_par[1].vxp[2], bndy_par[1].pre);
	}

	for (i = 0; i < pPPro.ntotal; i++) {
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

/*Save the information of velocity particles to inp file with boundary particles*/
void clInpOutput_Fun::output_velocity_bndy(Particle *pVelPar, const Para_Pro &pPPro, char *argv) {

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

/*Save the information of stress particles to inp file with boundary particles*/
void clInpOutput_Fun::output_stress_bndy(Particle *pVelPar, const Para_Pro &pPPro, char *argv) {

	char file_name[100], temp[100];
	int i, temp1[5];

	/*File name*/
	FILE *fpw, *fps, *fpa;

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
			if (pPPro.nair> 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
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

/*Save the information of velocity particles to inp file without boundary particles*/
void clInpOutput_Fun::output_velocity(Particle* pVelPar, const Para_Pro& pPPro, char* argv) {

	char file_name[100], temp[100];
	int i, temp1[5];
	double utotal;
	double vxtotal;

	/*File name*/
	FILE* fpw, * fps, * fpt, * fpa;

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
	if (win32) strcat(file_name, "\\velocity-step");
	else strcat(file_name, "/velocity-step");
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
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater, 0, 11, 0, 0);
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
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil, 0, 11, 0, 0);
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
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair, 0, 11, 0, 0);
	}
	//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	///< coordinate information//
	for (i = 0; i < pPPro.ntotal; i++) {

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
		vxtotal = pVelPar[i].vxp[0] * pVelPar[i].vxp[0]
			+ pVelPar[i].vxp[1] * pVelPar[i].vxp[1] + pVelPar[i].vxp[2] * pVelPar[i].vxp[2];
		vxtotal = sqrt(vxtotal);

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

/*Save the information of stress particles to inp file without boundary particles*/
void clInpOutput_Fun::output_stress(Particle* pVelPar, const Para_Pro& pPPro, char* argv) {

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
		strcat(file_name, ".inp");
		fpw = fopen(file_name, "w");
		fprintf(fpw, "#%s\n", temp);
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater, 0, 4, 0, 0);;
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
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil, 0, 34, 0, 0);
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
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair, 0, 4, 0, 0);
	}

	///< coordinate information//
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	for (i = 0; i < pPPro.ntotal; i++) {

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

void clInpOutput_Fun::read_data(Particle *pPar, Para_Pro pPPro, FILE *finp) {
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
