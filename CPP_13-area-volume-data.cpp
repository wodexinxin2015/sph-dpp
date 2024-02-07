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

int clArea_Vol_Data::Area_Vol_Data_Out(Para_Pro pPPro, char* argv, FILE* flog) {
	int nstep, i, j, area_type;
	char file_name[100], temp[100];
	Particle* pPar;
	FILE* finp;
	int err_t;
	double height_val, area, proj_area, volume;

	printf("-------------------------------------------------------------------\n");
	printf("Please input step number: > 0--inp output; 0--exit\n");
	err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");
	printf("Please input type of area calculation: 1--2D area of sliding mass;\n \
 2--3D projected area on the X-Y Plane;\n 3-3D volume ofsliding mass\n");
	err_t = scanf("%d", &area_type);
	printf("-------------------------------------------------------------------\n");
	printf("Please specify the height for truncating:\n");
	err_t = scanf("%lf", &height_val);
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
		if (finp != NULL) fclose(finp);

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
		int* cell_parti_index_top = new int[ctotal + 1];
		int* cell_parti_top = new int[nXm + 1];
		int* cell_parti_bottom = new int[nXm + 1];
		int nx_t[2], cell_id, temp_id;

		//initial of surface index from pPar
		for (i = 0; i <= ctotal; i++) {
			cell_parti_index_top[i] = -1;
		}

		//calculate the area or volume
		area = 0.0;
		proj_area = 0.0;
		volume = 0.0;
		if (area_type == 1) { //2D area.
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type != 0 && pPar[i].xp[axia_compress] < height_val) {
					area += dr * dr / 4.0;
				}
			}
		}
		else if (area_type == 2) {  //3D projected area on the X-Y plane
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type != 0 && pPar[i].xp[axia_compress] < height_val) {
					nx_t[0] = int((pPar[i].xp[0] - xmin[0] + 0.25 * dr) / dr);
					nx_t[1] = int((pPar[i].xp[1] - xmin[1] + 0.25 * dr) / dr);
					cell_id = nx_t[1] * nXm + nx_t[0];
					// transport the surface particle id to the cell_parti_index by compare the xp[axia_compress]
					if (cell_parti_index_top[cell_id] < 0) cell_parti_index_top[cell_id] = i;
					else {
						temp_id = cell_parti_index_top[cell_id];
						cell_parti_index_top[cell_id] = (pPar[i].xp[axia_compress] > pPar[temp_id].xp[axia_compress]) ?
							i : temp_id;
					}
				}
			}

			//colleting the surface particle
			for (i = 0; i <= nXm; i++) {
				cell_parti_top[i] = -1;
				cell_parti_bottom[i] = -1;
			}
			for (i = 0; i <= ctotal; i++) {
				temp_id = cell_parti_index_top[i];
				if (temp_id >= 0) {
					nx_t[0] = int((pPar[temp_id].xp[0] - xmin[0] + 0.25 * dr) / dr);
					cell_id = nx_t[0];

					// transport the surface particle id to top and bottom array on the XY plane
					if (cell_parti_top[cell_id] < 0)
					{
						cell_parti_top[cell_id] = i;
					}
					else {
						temp_id = cell_parti_top[cell_id];
						cell_parti_top[cell_id] = (pPar[i].xp[1] > pPar[temp_id].xp[1]) ?
							i : temp_id;
					}
					// transport the surface particle id to top and bottom array on the XY plane
					if (cell_parti_bottom[cell_id] < 0)
					{
						cell_parti_bottom[cell_id] = i;
					}
					else {
						temp_id = cell_parti_bottom [cell_id];
						cell_parti_bottom[cell_id] = (pPar[i].xp[1] < pPar[temp_id].xp[1]) ?
							i : temp_id;
					}
				}
			}

			//calculating the projected area
			for (i = 0; i <= nXm; i++) {
				int top_id = cell_parti_top[i];
				int bottom_id = cell_parti_bottom[i];
				if(top_id >= 0 && bottom_id >= 0)
					proj_area = proj_area + (pPar[top_id].xp[1] - pPar[bottom_id].xp[1]) * dr / 2.0;
			}
		}
		else if (area_type == 3) {   //3D volume
			for (i = 0; i < pPPro.ntotal; i++) {
				//calculate the cell id
				if (pPar[i].type != 0 && pPar[i].xp[axia_compress] < height_val) {
					volume += dr * dr * dr / 8.0;
				}
			}
		}

		//output to txt file
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\area-");
		else strcat(file_name, "/area-");
		strcat(file_name, temp);
		strcat(file_name, ".txt");

		if ((finp = fopen(file_name, "w")) == NULL) {
			return 14;
		}

		write_data(finp, area, proj_area, volume, height_val, area_type);

		if (finp != NULL) fclose(finp);

		delete[]cell_parti_index_top;
		delete[]cell_parti_top;
		delete[]cell_parti_bottom;
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

void clArea_Vol_Data::read_data(Particle* pPar, Para_Pro pPPro, FILE* finp) {
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

void clArea_Vol_Data::write_data(FILE* finp, double area, double proj_area, double volume, double height, int type) {
	if (type == 1) { //2D area
		fprintf(finp, "The 2D area of sliding mass is %10.6e m^2 below the truncating height of %8.4f m. \n",
			area, height);
	}
	else if (type == 2) {  //3D projected area on the X-Y plane
		fprintf(finp, "The projected area of sliding mass on X-Y plane is %10.6e m^2 below the truncating height of %8.4f m. \n",
			proj_area, height);
	}
	else if (type == 3) {   //3D volume
		fprintf(finp, "The 3D volume of sliding mass is %10.6e m^3 below the truncating height of %8.4f m. \n",
			volume , height);
	}
}