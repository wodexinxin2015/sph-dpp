/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

int clRandomField_Out::RandomField_Inp(Para_Pro pPPro, char* argv, FILE* flog) {
	Para_Mat* pMat;
	int i;

	//initialization of array
	pMat = new Para_Mat[pPPro.ntotal];

	//read material parameters
	read_data_mat(pMat, pPPro, argv);

	//particle number of each phase
	for (i = 0; i < pPPro.ntotal; i++) {
		if (pMat[i].type == 0) pPPro.nboun += 1;
		if (pMat[i].type == 1) pPPro.nwater += 1;
		if (pMat[i].type == 2) pPPro.nsoil += 1;
		if (pMat[i].type == 3) pPPro.nair += 1;
		if (pMat[i].type == 4) pPPro.nsoil += 1;
	}

	//write data to inp files
	write_data(pMat, pPPro, argv);

	//memory free
	delete[]pMat;
	return 0;
}

void clRandomField_Out::read_data_mat(Para_Mat* pMat, Para_Pro pPPro, char* argv) {
	char file_name[100];
	FILE* finp;
	int i;
	char c[2];
	int err_t, d;

	//assemble file name
	memset(file_name, 0, 100);
	strcpy(file_name, argv);
	if (win32) strcat(file_name, "\\Parameters");
	else strcat(file_name, "/Parameters");
	strcat(file_name, ".txt");

	//Open file
	if ((finp = fopen(file_name, "r")) != NULL) {
		err_t = fscanf(finp, "%c%c%*[^\n]%*c", &c[0], &c[1]);
		for (i = 0; i < pPPro.ntotal; i++) {
			err_t = fscanf(finp, "%d%lf%lf%lf%lf%lf%lf%lf%d%d", 
				&d, &pMat[i].xp[0], &pMat[i].xp[1], &pMat[i].xp[2], &pMat[i].fai, 
				&pMat[i].c, &pMat[i].cop, &pMat[i].ds, &pMat[i].type, &pMat[i].matype);
			pMat[i].x[0] = pMat[i].xp[0];
			pMat[i].x[1] = pMat[i].xp[1];
			pMat[i].x[2] = pMat[i].xp[2];
		}
	}
}

void clRandomField_Out::write_data(Para_Mat* pMat, Para_Pro pPPro, char* argv) {

	char file_name[100];
	int i, temp1[5];

	/*File name*/
	FILE* fpw, * fps, * fpt, * fpa;

	memset(file_name, 0, 100);

	/*Open files*/
	strcpy(file_name, argv);
	if (win32) strcat(file_name, "\\Parameters");
	else strcat(file_name, "/Parameters");
	strcat(file_name, ".inp");

	fpt = fopen(file_name, "w");
	fprintf(fpt, "#%c\n", 1);
	fprintf(fpt, "%d  %d  %d  %d  %d\n", pPPro.ntotal, 0, 4, 0, 0);


	fpw = NULL;
	fps = NULL;
	fpa = NULL;

	if (pPPro.nwater > 0) {  ///< liquid//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Parameters-liquid");
		else  strcat(file_name, "/Parameters-liquid");
		strcat(file_name, ".inp");
		fpw = fopen(file_name, "w");
		fprintf(fpw, "#%c\n", 1);
		fprintf(fpw, "%d  %d  %d  %d  %d\n", pPPro.nwater + pPPro.nboun, 0, 2, 0, 0);
	}

	if (pPPro.nsoil > 0) {   ///< solid//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Parameters-solid");
		else  strcat(file_name, "/Parameters-solid");
		strcat(file_name, ".inp");
		fps = fopen(file_name, "w");
		fprintf(fps, "#%c\n", 1);
		fprintf(fps, "%d  %d  %d  %d  %d\n", pPPro.nsoil + pPPro.nboun, 0, 4, 0, 0);
	}

	if (pPPro.nair > 0) {  ///< gas//
		memset(file_name, 0, 100);
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\Parameters-gas");
		else  strcat(file_name, "/Parameters-gas");
		strcat(file_name, ".inp");
		fpa = fopen(file_name, "w");
		fprintf(fpa, "#%c\n", 1);
		fprintf(fpa, "%d  %d  %d  %d  %d\n", pPPro.nair + pPPro.nboun, 0, 2, 0, 0);
	}
	//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	///< coordinate information//
	for (i = 0; i < pPPro.ntotal; i++) {
		if (pMat[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
		}

		if (pMat[i].type == 1) {
			temp1[1] += 1;
			fprintf(fpw, "%5d %14.6e %14.6e %14.6e\n", temp1[1], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
		}

		if (pMat[i].type == 2) {
			temp1[2] += 1;
			fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
		}

		if (pMat[i].type == 3) {
			temp1[3] += 1;
			fprintf(fpa, "%5d %14.6e %14.6e %14.6e\n", temp1[3], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
		}

		if (pMat[i].type == 4) {
			temp1[2] += 1;
			fprintf(fps, "%5d %14.6e %14.6e %14.6e\n", temp1[2], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
		}

		temp1[0] += 1;
		fprintf(fpt, "%5d %14.6e %14.6e %14.6e\n", temp1[0], pMat[i].xp[0], pMat[i].xp[1], pMat[i].xp[2]);
	}
	if (pPPro.nwater > 0) {
		fprintf(fpw, "%d %d %d \n", 2, 1, 1);
		fprintf(fpw, "fai, \n c, pa\n");
	}
	if (pPPro.nsoil > 0) {
		fprintf(fps, "%d %d %d %d %d\n", 4, 1, 1, 1, 1);
		fprintf(fps, "fai, \n c, pa\n cop, m/s\n ds, m\n");
	}
	if (pPPro.nair > 0) {
		fprintf(fpa, "%d %d %d \n", 2, 1, 1);
		fprintf(fpa, "fai, \n c, pa\n");
	}
	{
		fprintf(fpt, "%d %d %d %d %d\n", 4, 1, 1, 1, 1);
		fprintf(fpt, "fai, \n c, pa\n cop, m/s\n ds, m\n");
	}
	///< data//
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = 0;
	temp1[3] = 0;
	for (i = 0; i < pPPro.ntotal; i++) {

		if (pMat[i].type == 0) {
			if (pPPro.nwater > 0) temp1[1] += 1;
			if (pPPro.nsoil > 0) temp1[2] += 1;
			if (pPPro.nair > 0) temp1[3] += 1;
			if (pPPro.nwater > 0) fprintf(fpw, "%5d %14.6e %14.6e\n", \
				temp1[1], pMat[i].fai, pMat[i].c);
			if (pPPro.nsoil > 0) fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pMat[i].fai, pMat[i].c, pMat[i].cop, pMat[i].ds);
			if (pPPro.nair > 0) fprintf(fpa, "%5d %14.6e %14.6e\n", \
				temp1[1], pMat[i].fai, pMat[i].c);
		}

		if (pMat[i].type == 1) {
			temp1[1] += 1;
			fprintf(fpw, "%5d %14.6e %14.6e\n", \
				temp1[1], pMat[i].fai, pMat[i].c);
		}

		if (pMat[i].type == 2) {
			temp1[2] += 1;
			fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pMat[i].fai, pMat[i].c, pMat[i].cop, pMat[i].ds);
		}

		if (pMat[i].type == 3) {
			temp1[3] += 1;
			fprintf(fpa, "%5d %14.6e %14.6e\n", \
				temp1[1], pMat[i].fai, pMat[i].c);
		}

		if (pMat[i].type == 4) {
			temp1[2] += 1;
			fprintf(fps, "%5d %14.6e %14.6e %14.6e %14.6e\n", \
				temp1[2], pMat[i].fai, pMat[i].c, pMat[i].cop, pMat[i].ds);
		}

		temp1[0] += 1;
		fprintf(fpt, "%5d %14.6e %14.6e %14.6e %14.6e\n", \
			temp1[2], pMat[i].fai, pMat[i].c, pMat[i].cop, pMat[i].ds);

	}

	/*Close files*/
	fclose(fpt);
	if (pPPro.nwater > 0) fclose(fpw);
	if (pPPro.nsoil > 0) fclose(fps);
	if (pPPro.nair > 0) fclose(fpa);
}