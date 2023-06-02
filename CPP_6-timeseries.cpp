/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

int clTimeS_Fun::time_series(Para_Pro pPPro, char *argv, FILE *flog) {
	int pnum, loop;
	char temp[100], file_name[100];
	FILE *fdop, *finp;
	Particle pPar;
	int err_t;

	memset(file_name, 0, 100);
	memset(temp, 0, 100);

	printf("-------------------------------------------------------------------\n");
	printf("Please input the particle number: > 0--data withdraw; 0--exit\n");
	printf("-------------------------------------------------------------------\n");
	err_t = scanf("%d", &pnum);
	printf("-------------------------------------------------------------------\n");

	if (pnum > 0 && pnum <= pPPro.ntotal) {

		//output file pointers
		if (pnum < 10)
		{
			sprintf(temp, "000000000%d", pnum);
		}
		else if (pnum < 100)
		{
			sprintf(temp, "00000000%d", pnum);
		}
		else if (pnum < 1000)
		{
			sprintf(temp, "0000000%d", pnum);
		}
		else if (pnum < 10000)
		{
			sprintf(temp, "000000%d", pnum);
		}
		else if (pnum < 100000)
		{
			sprintf(temp, "00000%d", pnum);
		}
		//file pointers for output
		strcpy(file_name, argv);
		if (win32)
			strcat(file_name, "\\Series-");
		else
			strcat(file_name, "/Series-");
		strcat(file_name, temp);
		strcat(file_name, ".txt");

		if ((fdop = fopen(file_name, "w")) == NULL)
		{
			return 16;
		}
		memset(file_name, 0, 100);
		memset(temp, 0, 100);

		fprintf(fdop, "   Step   x  y  z  ux  uy  uz  ax  ay  az  vx  vy  vz epsxx epsyy epszz \
epsxy epsyz epszx sigxx sigyy sigzz sigxy sigyz sigzx  pre prew prea satu \n");

		//main code
		for (loop = 1; loop <= pPPro.loop; loop++) {
			if (loop % pPPro.fop == 0) {
				if (loop < 10)
				{
					sprintf(temp, "000000000%d", loop);
				}
				else if (loop < 100)
				{
					sprintf(temp, "00000000%d", loop);
				}
				else if (loop < 1000)
				{
					sprintf(temp, "0000000%d", loop);
				}
				else if (loop < 10000)
				{
					sprintf(temp, "000000%d", loop);
				}
				else if (loop < 100000)
				{
					sprintf(temp, "00000%d", loop);
				}
				else if (loop < 1000000)
				{
					sprintf(temp, "0000%d", loop);
				}
				else if (loop < 10000000)
				{
					sprintf(temp, "000%d", loop);
				}
				else if (loop < 100000000)
				{
					sprintf(temp, "00%d", loop);
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

				read_data(&pPar, pnum, finp);
				memset(file_name, 0, 100);
				memset(temp, 0, 100);

				if (finp != NULL) fclose(finp);
				out_data(pPar, fdop, loop);
			}
		}

		if (fdop != NULL) fclose(fdop);
	}
	else {
		printf("-------------------------------------------------------------------\n");
		printf("Particle No. is not correct (total number: %d).\n", pPPro.ntotal);
		printf("-------------------------------------------------------------------\n");
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "Particle No. is not correct (total number: %d).\n", pPPro.ntotal);
		fprintf(flog, "-------------------------------------------------------------------\n");
	}

	return 0;
}

//output
void clTimeS_Fun::out_data(Particle par, FILE *fop, int step) {
	/*  Step   x  y  z  ux  uy  uz  ax   ay   az   vx  vy  vz epsxx epsyy epszz \
	epsxy epsyz epszx sigxx sigyy sigzz sigxy sigyz sigzx  pre prew prea satu */
	fprintf(fop, "%9d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e\n",
step, par.xp[0], par.xp[1], par.xp[2], par.ux[0], par.ux[1], par.ux[2], 
par.axp[0], par.axp[1], par.axp[2], par.vxp[0], par.vxp[1], par.vxp[2],
par.eps[0], par.eps[1], par.eps[2], par.eps[3], par.eps[4], par.eps[5],
par.sig[0], par.sig[1], par.sig[2], par.sig[3], par.sig[4], par.sig[5],
par.pre, par.prew, par.prea, par.satu);
}

void clTimeS_Fun::read_data(Particle *pPar, int pnum, FILE *finp) {
	int line_no, temp, i;
	char c[2];
	line_no = 2 + pnum;
	int err_t;
	double uxp;

	for (i = 1; i<line_no; i++) {
		err_t = fscanf(finp, "%c%c%*[^\n]%*c", &c[0], &c[1]);
	}
	err_t = fscanf(finp, "%d%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%lf%lf\
%lf%lf%lf%lf%d%d",
&temp, &pPar->xp[0], &pPar->xp[1], &pPar->xp[2], &pPar->ux[0], &pPar->ux[1], &pPar->ux[2],
&uxp, &pPar->axp[0], &pPar->axp[1], &pPar->axp[2], &pPar->vxp[0], &pPar->vxp[1], &pPar->vxp[2],
&pPar->pre, &pPar->prea, &pPar->prew,
&pPar->sig[0], &pPar->sig[1], &pPar->sig[2], &pPar->sig[3], &pPar->sig[4], &pPar->sig[5],
&pPar->strep[0], &pPar->strep[1], &pPar->strep[2],
&pPar->eps[0], &pPar->eps[1], &pPar->eps[2], &pPar->eps[3], &pPar->eps[4], &pPar->eps[5],
&pPar->adps[0], &pPar->adps[1], &pPar->adps[2], &pPar->adps[3], &pPar->adps[4], &pPar->adps[5],
&pPar->vsig, &pPar->veps, &pPar->meps, &pPar->divq, &pPar->divr, &pPar->satu,
&pPar->suct, &pPar->interfss[0], &pPar->interfss[1], &pPar->interfss[2], &pPar->type, &pPar->matype);

	pPar->x[0] = pPar->xp[0];
	pPar->x[1] = pPar->xp[1];
	pPar->x[2] = pPar->xp[2];

	pPar->vx[0] = pPar->vxp[0];
	pPar->vx[1] = pPar->vxp[1];
	pPar->vx[2] = pPar->vxp[2];


	pPar->ax[0] = pPar->axp[0];
	pPar->ax[1] = pPar->axp[1];
	pPar->ax[2] = pPar->axp[2];
}
