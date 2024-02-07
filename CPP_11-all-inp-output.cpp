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

//main subroutine of inp format output for microavs
int clInpOutput_Fun::all_inp_output(Para_Pro pPPro, char* argv, FILE* flog) {
	int nstep, i, bndy_flag, fop, loop;
	char file_name[100], temp[100];
	Particle* pPar;
	FILE* finp;
	int err_t;

	printf("-------------------------------------------------------------------\n");
	printf("Show the boundary: 0--no boundary; 1--boundary; 2--virtual boundary\n");
	err_t = scanf("%d", &bndy_flag);
	printf("-------------------------------------------------------------------\n");

	pPar = new Particle[pPPro.ntotal];
	finp = NULL;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

	fop = pPPro.fop;
	loop= pPPro.loop;

	for (nstep = fop; nstep <= loop; nstep += fop) {
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
		else if (bndy_flag == 1) {
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

	delete[]pPar;
	if (finp != NULL) fclose(finp);

	return 0;
}