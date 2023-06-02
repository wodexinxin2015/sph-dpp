/************************************************************************************
SPH Data Post Viewer
--Copyright(c) 2018 - 2021, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <math.h>
# include "string.h"
# include "Header_Option.h"
# include "Class_particle.h"
# include "Class_Functions.h"

int clSoiltoFluid_Fun::soil_to_fluid(Para_Pro pPPro, char* argv, FILE* flog) {
	Particle* pPar;
	FILE* finp, *fout;
	char file_name[100], temp[100];
	int err_t, nstep, dis_type;
	double dis_value;

	pPar = new Particle[pPPro.ntotal];
	finp = NULL;
	fout = NULL;

	printf("-------------------------------------------------------------------\n");
	printf("Please input step number: > 0--inp output; 0--exit\n");
	err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");

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

		dis_type = 0;
		printf("-------------------------------------------------------------------\n");
		printf("Please input the type of displacement: default-0-total displacement;\n1-3: displacement of x, y and z\n");
		err_t = scanf("%d", &dis_type);
		printf("-------------------------------------------------------------------\n");
		if (dis_type < 0 || dis_type > 3) {
			printf("the type of displacement is not correct and set to 0-total displacement.\n");
			dis_type = 0;
		}

		printf("-------------------------------------------------------------------\n");
		printf("Please input the minimum displacement: unit: m\n");
		err_t = scanf("%lf", &dis_value);
		printf("-------------------------------------------------------------------\n");

		//output file
		memset(file_name, 0, 100);

		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\soil_to_fluid-");
		else strcat(file_name, "/soil_to_fluid-");
		strcat(file_name, temp);
		strcat(file_name, ".txt");

		if ((fout = fopen(file_name, "w")) == NULL) {
			return 14;
		}

		if (pPPro.ndim == 2) {
			out_data_2D(pPar, pPPro, fout, dis_value, dis_type);
		}
		else if (pPPro.ndim == 3) {
			out_data_3D(pPar, pPPro, fout, dis_value, dis_type);
		}
	}
	else {
		printf("-------------------------------------------------------------------\n");
		printf("Step No. is not correct (total steps: %d).\n", pPPro.loop);
		printf("-------------------------------------------------------------------\n");
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "Step No. is not correct (total steps: %d).\n", pPPro.loop);
		fprintf(flog, "-------------------------------------------------------------------\n");
	}

	delete []pPar;
	if (finp != NULL) fclose(finp);
	if (fout != NULL) fclose(fout);

	return 0;
}

void clSoiltoFluid_Fun::read_data(Particle* pPar, Para_Pro pPPro, FILE* finp) {
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

void clSoiltoFluid_Fun::out_data_2D(Particle* pPar, Para_Pro pPPro, FILE* flt, double dis_value, int dis_type) {
	int i;
	double ux = 0.0;

	for (i = 0; i < pPPro.ntotal; i++) {
		if (pPar[i].type == 2) {

			if (dis_type == 0) {
				ux = sqrt(pPar[i].ux[0] * pPar[i].ux[0] + pPar[i].ux[1] * pPar[i].ux[1] + pPar[i].ux[2] * pPar[i].ux[2]);
			}
			else if (dis_type == 1) {
				ux = pPar[i].ux[0];
			}
			else if (dis_type == 2) {
				ux = pPar[i].ux[1];
			}
			else if (dis_type == 3) {
				ux = pPar[i].ux[2];
			}

			if (ux >= dis_value) {
				fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
					pPar[i].xp[0], pPar[i].xp[1], 0.0, 0.0, 1, 0, 0, 1);
			}
			else {
				fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
					pPar[i].xp[0], pPar[i].xp[1], 0.0, 0.0, 0, 0, 0, 0);
			}
		}
		else {
			fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
				pPar[i].xp[0], pPar[i].xp[1], 0.0, 0.0, 0, 0, 0, 0);
		}
	}
}

void clSoiltoFluid_Fun::out_data_3D(Particle* pPar, Para_Pro pPPro, FILE* flt, double dis_value, int dis_type) {
	int i;
	double ux = 0.0;

	for (i = 0; i < pPPro.ntotal; i++) {
		if (pPar[i].type == 2) {

			if (dis_type == 0) {
				ux = sqrt(pPar[i].ux[0] * pPar[i].ux[0] + pPar[i].ux[1] * pPar[i].ux[1] + pPar[i].ux[2] * pPar[i].ux[2]);
			}
			else if (dis_type == 1) {
				ux = pPar[i].ux[0];
			}
			else if (dis_type == 2) {
				ux = pPar[i].ux[1];
			}
			else if (dis_type == 3) {
				ux = pPar[i].ux[2];
			}

			if (ux >= dis_value) {
				fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
					pPar[i].xp[0], pPar[i].xp[1], pPar[i].xp[2], 0.0, 0.0, 0.0, 1, 0, 0, 1);
			}
			else {
				fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
					pPar[i].xp[0], pPar[i].xp[1], pPar[i].xp[2], 0.0, 0.0, 0.0, 0, 0, 0, 0);
			}
		}
		else {
			fprintf(flt, "%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %5d %5d %5d %5d\n",
				pPar[i].xp[0], pPar[i].xp[1], pPar[i].xp[2], 0.0, 0.0, 0.0, 0, 0, 0, 0);
		}
	}
}
