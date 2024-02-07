/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <time.h>
#include "Class_Functions.h"
#include "Header_Option.h"

//initializing from input file for problems property
int clFIni_Fun::initial_pro(Para_Pro *pPPro, FILE *flog, FILE *finp)
{
	FILE *pFile;
	char c[2];
	time_t rawtime;
	struct tm *timeinfo;
	int err_t, step_regu;
	double temp1, temp2;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Current time: %s", asctime(timeinfo));
	printf("The problem parameters are initializing\n.......\n");
	fprintf(flog, "Current time: %s", asctime(timeinfo));
	fprintf(flog, "The problem parameters are initializing\n.......\n");

	pFile = finp;
	rewind(pFile);

	rewind(pFile);
	while (!feof(pFile))
	{
		err_t = fscanf(pFile, "%c%c%*[^\n]%*c", &c[0], &c[1]);
		if (c[0] == 'P' && c[1] == 'R')
		{ //problem parameters
			do
			{
				err_t = fscanf(pFile, "%c%*[^\n]%*c", &c[0]);
				if (c[0] == 'A')
				{
					err_t = fscanf(pFile, "%d%lf%lf", &pPPro->ndim, &temp1, &temp2);
					err_t = fscanf(pFile, "%d%d%d%d%d", &pPPro->fop, &pPPro->inip, &pPPro->loop, &pPPro->ntotal, &step_regu);
					err_t = fscanf(pFile, "%d%lf%lf", &pPPro->dttype, &pPPro->dr, &pPPro->dt);
					break;
				}
			} while (c[0] != 'E');
			break;
		}
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Current time: %s", asctime(timeinfo));
	printf("The problem parameters have been initialized!\n");
	printf("-------------------------------------------------------------------\n");

	fprintf(flog, "Current time: %s", asctime(timeinfo));
	fprintf(flog, "The problem parameters have been initialized!\n");
	fprintf(flog, "-------------------------------------------------------------------\n");

	return 0;
}