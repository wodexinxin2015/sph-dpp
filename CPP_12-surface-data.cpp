/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/
# define _CRT_SECURE_NO_DEPRECATE
# include <stdio.h>
# include <cmath>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

int clSurfaceData_Out::SurfaceData_Out(Para_Pro pPPro, char* argv, FILE* flog) {
	int nstep, i, j, surface_type;
	char file_name[100], temp[100];
	Particle* pPar;
	FILE* finp;
	int err_t;

	printf("-------------------------------------------------------------------\n");
	printf("Please input step number: > 0--inp output; 0--exit\n");
	err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");
	printf("Please input type of surface extracting surface: 1--water; 2--soil; 3--water and soil together\n");
	err_t = scanf("%d", &surface_type);
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
		//reading particle data from odb file
		read_data(pPar, pPPro, finp);

		//close the file
		if(finp != NULL) fclose(finp);

		//extract the surface particles
		int ndim = pPPro.ndim;
		int axia_compress = ndim - 1;
		//for 2D problem, the surface is extracted along y direction (1);
		//for 3D problem, the surface is extracted along z direction (2);

		//calculate the minimum and maximum location
		double xmin[3], xmax[3];
		xmin[0] = 1000000;
		xmin[1] = 1000000;
		xmin[2] = 1000000;
		xmax[0] = -1000000;
		xmax[1] = -1000000;
		xmax[2] = -1000000;

		for (i = 0; i < pPPro.ntotal; i++) {
			for (j = 0; j < 2; j++) {
				xmin[j] = (pPar[i].xp[j] < xmin[j]) ? pPar[i].xp[j] : xmin[j];
				xmax[j] = (pPar[i].xp[j] > xmax[j]) ? pPar[i].xp[j] : xmax[j];
			}
		}

		int nXm, nYm, ctotal;  //define the cell number in each coordinate
		double dr = 2.0 * pPPro.dr;  // particle spacing

		nXm = int((xmax[0] - xmin[0]) / dr) + 1;
		nYm = int((xmax[1] - xmin[1]) / dr) + 1;
		ctotal = (ndim == 2) ? nXm : nXm * nYm;
		int* cell_parti_index = new int[ctotal + 1];
		int* cell_bndy_index = new int[ctotal + 1];
		Particle* surface_par = new Particle[ctotal * 2];
		int nx_t[2], cell_id, temp_id;

		//initial of surface index from pPar
		for (i = 0; i <= ctotal; i++) {
			cell_parti_index[i] = -1;
			cell_bndy_index[i] = -1;
		}

		//compress data to surface 
		if (surface_type == 1) {
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type == 1) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_parti_index[cell_id] < 0) cell_parti_index[cell_id] = i;
					else {
						temp_id = cell_parti_index[cell_id];
						cell_parti_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
				else if (pPar[i].type == 0) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_bndy_index[cell_id] < 0) cell_bndy_index[cell_id] = i;
					else {
						temp_id = cell_bndy_index[cell_id];
						cell_bndy_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
			}
		}
		else if (surface_type == 2) {
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type == 2) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25* dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_parti_index[cell_id] < 0) cell_parti_index[cell_id] = i;
					else {
						temp_id = cell_parti_index[cell_id];
						cell_parti_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
				else if (pPar[i].type == 0) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_bndy_index[cell_id] < 0) cell_bndy_index[cell_id] = i;
					else {
						temp_id = cell_bndy_index[cell_id];
						cell_bndy_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
			}
		}
		else if (surface_type == 3) {
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type == 1 || pPar[i].type == 2) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_parti_index[cell_id] < 0) cell_parti_index[cell_id] = i;
					else {
						temp_id = cell_parti_index[cell_id];
						cell_parti_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
				else if (pPar[i].type == 0) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = (ndim == 2) ? nx_t[0] : nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_bndy_index[cell_id] < 0) cell_bndy_index[cell_id] = i;
					else {
						temp_id = cell_bndy_index[cell_id];
						cell_bndy_index[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
			}
		}

		//transport the original data to the surface data
		int total = 0;
		for (i = 0; i <= ctotal; i++) {
			temp_id = cell_parti_index[i];
			if (temp_id >= 0) {
				surface_par[total] = pPar[temp_id];
				total++;
			}
		}
		for (i = 0; i <= ctotal; i++) {
			temp_id = cell_bndy_index[i];
			if (temp_id >= 0) {
				surface_par[total] = pPar[temp_id];
				total++;
			}
		}

		//output data to new odb file
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\surf-data-");
		else strcat(file_name, "/surf-data-");
		strcat(file_name, temp);
		strcat(file_name, ".odb");

		if ((finp = fopen(file_name, "w")) == NULL) {
			return 14;
		}
		write_data(surface_par, finp, total);
		if (finp != NULL) fclose(finp);

		//output data to surface file
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\surf-bndy-");
		else strcat(file_name, "/surf-bndy-");
		strcat(file_name, temp);
		strcat(file_name, ".txt");

		if ((finp = fopen(file_name, "w")) == NULL) {
			return 14;
		}
		write_surface_bndy(surface_par, finp, total);
		if (finp != NULL) fclose(finp);

		if (surface_type == 1) {
			memset(file_name, 0, 100);
			strcpy(file_name, argv);
			if (win32) strcat(file_name, "\\surf-water-");
			else strcat(file_name, "/surf-water-");
			strcat(file_name, temp);
			strcat(file_name, ".txt");

			if ((finp = fopen(file_name, "w")) == NULL) {
				return 14;
			}
			write_surface_water(surface_par, finp, total);
			if (finp != NULL) fclose(finp);
		}
		else if (surface_type == 2) {
			memset(file_name, 0, 100);
			strcpy(file_name, argv);
			if (win32) strcat(file_name, "\\surf-soil-");
			else strcat(file_name, "/surf-soil-");
			strcat(file_name, temp);
			strcat(file_name, ".txt");

			if ((finp = fopen(file_name, "w")) == NULL) {
				return 14;
			}
			write_surface_soil(surface_par, finp, total);
			if (finp != NULL) fclose(finp);
		}
		else if (surface_type == 3) {
			memset(file_name, 0, 100);
			strcpy(file_name, argv);
			if (win32) strcat(file_name, "\\surf-water-");
			else strcat(file_name, "/surf-water-");
			strcat(file_name, temp);
			strcat(file_name, ".txt");

			if ((finp = fopen(file_name, "w")) == NULL) {
				return 14;
			}
			write_surface_water(surface_par, finp, total);

			if (finp != NULL) fclose(finp);
			memset(file_name, 0, 100);
			strcpy(file_name, argv);
			if (win32) strcat(file_name, "\\surf-soil-");
			else strcat(file_name, "/surf-soil-");
			strcat(file_name, temp);
			strcat(file_name, ".txt");

			if ((finp = fopen(file_name, "w")) == NULL) {
				return 14;
			}
			write_surface_soil(surface_par, finp, total);
			if (finp != NULL) fclose(finp);
		}

		//free memory
		delete[]surface_par;
		delete[]cell_parti_index;
		delete[]cell_bndy_index;
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
	return 0;
}

void clSurfaceData_Out::read_data(Particle* pPar, Para_Pro pPPro, FILE* finp) {
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

void clSurfaceData_Out::write_data(Particle* pVelPar, FILE* finp, int total) {
	int temp1 = 0;
	for (int i = 0; i < total; i++)
	{
		temp1 = temp1 + 1;
		fprintf(finp, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e  %14.6e %14.6e \
%14.6e %14.6e %14.6e \
%14.6e %4d %4d\n",
temp1, pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2], pVelPar[i].ux[0], pVelPar[i].ux[1], pVelPar[i].ux[2],
sqrt(pVelPar[i].ux[0] * pVelPar[i].ux[0] + pVelPar[i].ux[1] * pVelPar[i].ux[1] + pVelPar[i].ux[2] * pVelPar[i].ux[2]),
pVelPar[i].interfss[0], pVelPar[i].interfss[1], pVelPar[i].interfss[2], pVelPar[i].interf[0], pVelPar[i].interf[1], pVelPar[i].interf[2],
pVelPar[i].pre, pVelPar[i].prea, pVelPar[i].prew,
pVelPar[i].sig[0], pVelPar[i].sig[1], pVelPar[i].sig[2], pVelPar[i].sig[3], pVelPar[i].sig[4], pVelPar[i].sig[5],
pVelPar[i].strep[0], pVelPar[i].strep[1], pVelPar[i].strep[2],
pVelPar[i].eps[0], pVelPar[i].eps[1], pVelPar[i].eps[2], pVelPar[i].eps[3], pVelPar[i].eps[4], pVelPar[i].eps[5],
pVelPar[i].adps[0], pVelPar[i].adps[1], pVelPar[i].adps[2], pVelPar[i].adps[3], pVelPar[i].adps[4], pVelPar[i].adps[5],
pVelPar[i].vsig, pVelPar[i].veps, pVelPar[i].meps, pVelPar[i].divq, pVelPar[i].divr, pVelPar[i].satu,
pVelPar[i].suct, pVelPar[i].interfss[0], pVelPar[i].interfss[1], pVelPar[i].interfss[2], pVelPar[i].type, pVelPar[i].matype);
	}
}

void clSurfaceData_Out::write_surface_bndy(Particle* pVelPar, FILE* finp, int total) {
	for (int i = 0; i < total; i++)
	{
		if(pVelPar[i].type == 0) fprintf(finp, "%14.6e %14.6e %14.6e\n", pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
	}
}

void clSurfaceData_Out::write_surface_water(Particle* pVelPar, FILE* finp, int total) {
	for (int i = 0; i < total; i++)
	{
		if (pVelPar[i].type == 1) fprintf(finp, "%14.6e %14.6e %14.6e\n", pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
	}
}

void clSurfaceData_Out::write_surface_soil(Particle* pVelPar, FILE* finp, int total) {
	for (int i = 0; i < total; i++)
	{
		if (pVelPar[i].type == 2) fprintf(finp, "%14.6e %14.6e %14.6e\n", pVelPar[i].xp[0], pVelPar[i].xp[1], pVelPar[i].xp[2]);
	}
}
