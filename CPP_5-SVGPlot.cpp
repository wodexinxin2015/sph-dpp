/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_DEPRECATE
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

//main subroutine of svg format output for IE, Chrome and Firefox
int clSvgPlot_Fun::svg_outplot(Para_Pro pPPro, char *argv, FILE *flog) {
	int nstep, i;
	char file_name[100], temp[100];
    int err_t;
	Para_SVGPlot psvg;
	Particle *pPar;
	FILE *finp, *fdat;
	printf("-------------------------------------------------------------------\n");
	printf("Please input step number: > 0--svg plot; 0--exit\n");
    err_t = scanf("%d", &nstep);
	printf("-------------------------------------------------------------------\n");
	printf("Please input the flag of svg:\n");
	printf("--svg (0 or 1) contour (1,2, or 3) :\n");
	printf("1--red -> green -> blue; 2--blue -> green -> red; 3--white -> black\n");
	printf("-------------------------------------------------------------------\n");
    err_t = scanf("%d%d", &psvg.flag, &psvg.type);
	printf("-------------------------------------------------------------------\n");
	printf("Please input the flag of svg:\n");
	if (psvg.type > 0 && psvg.type < 4) {
		printf(" dis      vel      strain      stress (1--output or 0--not output)\n");
        err_t = scanf("%d%d%d%d", &psvg.flag_dis, &psvg.flag_vel, &psvg.flag_strain, &psvg.flag_stress);
	}
	else {
		return 13;
	}
	printf("-------------------------------------------------------------------\n");

 	pPar = new Particle[pPPro.ntotal];
	finp = NULL;
	fdat = NULL;

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
		memset(file_name, 0, 100);

		//read file
		strcpy(file_name, argv);
		if (win32) strcat(file_name, "\\data-range");
		else strcat(file_name, "/data-range");
		strcat(file_name, temp);
		strcat(file_name, ".txt");

		if ((fdat = fopen(file_name, "w")) == NULL) {
			return 15;
		}
		//read data from file
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
		if (psvg.flag == 1) {
			Plot_SVG(pPar, pPPro, psvg, argv);
			Plot_SVG_contour(pPar, pPPro, psvg, argv, fdat);
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
	if (fdat != NULL) fclose(fdat);

	return 0;
}

//plot configurations
void clSvgPlot_Fun::Plot_SVG(Particle *pVelPar, const Para_Pro pPPro, const Para_SVGPlot pPSVG,
                          char *argv) {
    char file_name[100], temp[100];
    int i, base_p[3], maxyp, rgb[3];
	double b_max[3], b_min[3], tx[3], dot_density;
    FILE *fpt, *fpw, *fps, *fpa;

    if(pPPro.ndim == 2)
    {
        base_p[0] = 60;
        base_p[1] = 20;
        base_p[2] = 0;
    }
    else{
        base_p[0] = 150;
        base_p[1] = 20;
        base_p[2] = 0;
    }

    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;

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
    if(win32) strcat(file_name, "\\svg-step");
    else strcat(file_name, "/svg-step");
    strcat(file_name, temp);
    strcat(file_name, ".svg");

    fpt = fopen(file_name, "w");

    fpw = NULL;
    fps = NULL;
    fpa = NULL;

    if (pPPro.nwater > 0) {  ///< liquid//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
        if (win32) strcat(file_name, "\\svg-liquid");
        else  strcat(file_name, "/svg-liquid");
        strcat(file_name, temp);
        strcat(file_name, ".svg");
        fpw = fopen(file_name, "w");
    }

    if (pPPro.nsoil > 0) {   ///< solid//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
        if (win32) strcat(file_name, "\\svg-solid");
        else  strcat(file_name, "/svg-solid");
        strcat(file_name, temp);
        strcat(file_name, ".svg");
        fps = fopen(file_name, "w");
    }

    if (pPPro.nair > 0) {  ///< gas//
        memset(file_name, 0, 100);
        strcpy(file_name, argv);
        if (win32) strcat(file_name, "\\svg-gas");
        else  strcat(file_name, "/svg-gas");
        strcat(file_name, temp);
        strcat(file_name, ".svg");
        fpa = fopen(file_name, "w");
    }
    //calculating the maximum and minimum of x, y and z
    b_max[0] = 0.0;
    b_max[1] = 0.0;
    b_max[2] = 0.0;
    b_min[0] = 0.0;
    b_min[1] = 0.0;
    b_min[2] = 0.0;

    for(i = 0; i < pPPro.ntotal; i++){
        if(pVelPar[i].type == 0){
            b_max[0] = fmax(b_max[0], pVelPar[i].xp[0]);
            b_max[1] = fmax(b_max[1], pVelPar[i].xp[1]);
            b_max[2] = fmax(b_max[2], pVelPar[i].xp[2]);

            b_min[0] = fmin(b_min[0], pVelPar[i].xp[0]);
            b_min[1] = fmin(b_min[1], pVelPar[i].xp[1]);
            b_min[2] = fmin(b_min[2], pVelPar[i].xp[2]);
        }
    }
    if(pPPro.ndim == 2) {
        dot_density = 1200 / (b_max[1] - b_min[1]);
        maxyp = (int) ((b_max[1] - b_min[1]) * dot_density) + base_p[1];
    }
    else{
        dot_density = 1200 / (b_max[1] - b_min[1]);
        maxyp = (int) ((b_max[2] - b_min[2]) * dot_density) + base_p[1];
    }
    //write to svg file
    if (pPPro.ndim == 2) {
        printf_start(fpt, dot_density, b_max[0], b_min[0]);
        if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[0], b_min[0]);
        if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[0], b_min[0]);
        if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[0], b_min[0]);
    }
    else {
        printf_start(fpt, dot_density, b_max[1], b_min[1]);
        if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[1], b_min[1]);
        if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[1], b_min[1]);
        if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[1], b_min[1]);
    }

    if(pPPro.ndim == 2){
		printf_boundary2d(fpt, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nwater > 0) printf_boundary2d(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nsoil > 0) printf_boundary2d(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nair > 0) printf_boundary2d(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
    }
    else if(pPPro.ndim == 3){
        if(pPSVG.type > 0) printf_boundary3dleft(fpt, b_max, b_min, pPPro.dr, base_p, dot_density);
        else if (pPSVG.type < 0) printf_boundary3dright(fpt, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nwater > 0 && pPSVG.type > 0) printf_boundary3dleft(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
        else if (pPPro.nwater > 0 && pPSVG.type < 0) printf_boundary3dright(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nsoil > 0 && pPSVG.type > 0) printf_boundary3dleft(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
        else if(pPPro.nsoil > 0 && pPSVG.type < 0) printf_boundary3dright(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
        if (pPPro.nair > 0 && pPSVG.type > 0) printf_boundary3dleft(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        else if(pPPro.nair > 0 && pPSVG.type < 0) printf_boundary3dright(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
    }

    for(i = 0; i < pPPro.ntotal; i++){
        if(pVelPar[i].type == 1 ){
            rgb[0] = 150;
            rgb[1] = 210;
            rgb[2] = 250;
            if(pPPro.ndim == 3){
                if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                printf_particles(fpw, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
            else{
                printf_particles(fpw, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
        }
        else if(pVelPar[i].type == 2 ){
            rgb[0] = 150;
            rgb[1] = 130;
            rgb[2] = 100;
            if (pPPro.ndim == 3) {
                if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
            else{
                printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
        }
        else if(pVelPar[i].type == 3 ){
            rgb[0] = 240;
            rgb[1] = 200;
            rgb[2] = 220;
            if (pPPro.ndim == 3) {
                if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                printf_particles(fpa, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
            else{
                printf_particles(fpa, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                printf_particles(fpt, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
            }
        }
		else if (pVelPar[i].type == 4) {
			rgb[0] = 150;
			rgb[1] = 130;
			rgb[2] = 100;
			if (pPPro.ndim == 3) {
				if (pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
				else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
				printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				printf_particles(fpt, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
			}
			else {
				printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				printf_particles(fpt, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
			}
		}
    }
    printf_end(fpt);
	if (pPPro.nwater > 0) printf_end(fpw);
	if (pPPro.nsoil > 0) printf_end(fps);
	if (pPPro.nair > 0) printf_end(fpa);
    /*Close files*/
    fclose(fpt);
    if (pPPro.nwater > 0) fclose(fpw);
    if (pPPro.nsoil > 0) fclose(fps);
    if (pPPro.nair > 0) fclose(fpa);
}
//plot contour of variables
void clSvgPlot_Fun::Plot_SVG_contour(Particle *pVelPar, const Para_Pro pPPro,
                                   const Para_SVGPlot pPSVG, char *argv, FILE *fsvg) {
    char file_name[100], temp[100];
    int i, j, base_p[3], maxyp, type;
    double b_max[3], b_min[3], tx[3], prod, dot_density;
    double max_v[4][4], min_v[4][4];
    FILE *fpw, *fps, *fpa;

    int *rgb = new int[3];
    double (*val)[4] = new double[pPPro.ntotal][4];

    if(pPPro.ndim == 2)
    {
        base_p[0] = 60;
        base_p[1] = 20;
        base_p[2] = 0;
    }
    else{
        base_p[0] = 150;
        base_p[1] = 20;
        base_p[2] = 0;
    }

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

    //calculating the maximum and minimum of x, y and z
    b_max[0] = 0.0;
    b_max[1] = 0.0;
    b_max[2] = 0.0;
    b_min[0] = 0.0;
    b_min[1] = 0.0;
    b_min[2] = 0.0;

    for(i = 0; i < 4; i ++){
        for(j = 0; j < 4; j ++){
            max_v[i][j] = 0.0;
            min_v[i][j] = 0.0;
        }
    }

    for(i = 0; i < pPPro.ntotal; i++){
        if(pVelPar[i].type == 0){
            b_max[0] = fmax(b_max[0], pVelPar[i].xp[0]);
            b_max[1] = fmax(b_max[1], pVelPar[i].xp[1]);
            b_max[2] = fmax(b_max[2], pVelPar[i].xp[2]);

            b_min[0] = fmin(b_min[0], pVelPar[i].xp[0]);
            b_min[1] = fmin(b_min[1], pVelPar[i].xp[1]);
            b_min[2] = fmin(b_min[2], pVelPar[i].xp[2]);
        }
        else if (pVelPar[i].type != 7){ //memory leak
            prod = pVelPar[i].ux[0] * pVelPar[i].ux[0]
            + pVelPar[i].ux[1] * pVelPar[i].ux[1] + pVelPar[i].ux[2] * pVelPar[i].ux[2];
            val[i][0] = sqrt(prod);
            prod = pVelPar[i].vx[0] * pVelPar[i].vx[0]
            + pVelPar[i].vx[1] * pVelPar[i].vx[1] + pVelPar[i].vx[2] * pVelPar[i].vx[2];
            val[i][1] = sqrt(prod);
            val[i][2] = pVelPar[i].divq;
            val[i][3] = pVelPar[i].meps;

            type = pVelPar[i].type;

            max_v[type][0] = fmax(max_v[type][0], val[i][0]);
            max_v[type][1] = fmax(max_v[type][1], val[i][1]);
            max_v[type][2] = fmax(max_v[type][2], val[i][2]);
            max_v[type][3] = fmax(max_v[type][3], val[i][3]);

            min_v[type][0] = fmin(min_v[type][0], val[i][0]);
            min_v[type][1] = fmin(min_v[type][1], val[i][1]);
            min_v[type][2] = fmin(min_v[type][2], val[i][2]);
            min_v[type][3] = fmin(min_v[type][3], val[i][3]);
        }
    }
    if(pPPro.ndim == 2) {
        dot_density = 1200 / (b_max[1] - b_min[1]);
        maxyp = (int) ((b_max[1] - b_min[1]) * dot_density) + base_p[1];
    }
    else{
        dot_density = 1200 / (b_max[1] - b_min[1]);
        maxyp = (int) ((b_max[2] - b_min[2]) * dot_density) + base_p[1];
    }
    if(pPSVG.flag_dis == 1){
        /*Open files*/
        fpw = NULL;
        fps = NULL;
        fpa = NULL;

        if (pPPro.nwater > 0) {  ///< liquid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cdisliquid");
            else  strcat(file_name, "/svg-cdisliquid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpw = fopen(file_name, "w");
        }

        if (pPPro.nsoil > 0) {   ///< solid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cdissolid");
            else  strcat(file_name, "/svg-cdissolid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fps = fopen(file_name, "w");
        }

        if (pPPro.nair > 0) {  ///< gas//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cdisgas");
            else  strcat(file_name, "/svg-cdisgas");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpa = fopen(file_name, "w");
        }

        //writting svg file
        if (pPPro.ndim == 2) {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[0], b_min[0]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[0], b_min[0]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[0], b_min[0]);
        }
        else {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[1], b_min[1]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[1], b_min[1]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[1], b_min[1]);
        }

        //write to svg file
        if(pPPro.ndim == 2){
            if (pPPro.nwater > 0) printf_boundary2d(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0) printf_boundary2d(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0) printf_boundary2d(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }
        else if(pPPro.ndim == 3){
            if (pPPro.nwater > 0 && pPSVG.type > 0) printf_boundary3dleft(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if (pPPro.nwater > 0 && pPSVG.type < 0) printf_boundary3dright(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0 && pPSVG.type > 0) printf_boundary3dleft(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nsoil > 0 && pPSVG.type < 0) printf_boundary3dright(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0 && pPSVG.type > 0) printf_boundary3dleft(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nair > 0 && pPSVG.type < 0) printf_boundary3dright(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }

        for(i = 0; i < pPPro.ntotal; i++){
            if(pVelPar[i].type == 1 ){
                colour_cal(val[i][0], max_v[1][0], min_v[1][0], rgb, abs(pPSVG.type));
                if(pPPro.ndim == 3){
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpw, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpw, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 2 ){
                colour_cal(val[i][0], max_v[2][0], min_v[2][0], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 3 ){
                colour_cal(val[i][0], max_v[3][0], min_v[3][0], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpa, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpa, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);

                }
            }
			else if (pVelPar[i].type == 4) {
				colour_cal(val[i][0], max_v[2][0], min_v[2][0], rgb, abs(pPSVG.type));
				if (pPPro.ndim == 3) {
					if (pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
					else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
					printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
				else {
					printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
			}
        }
        if (pPPro.nwater > 0) printf_end(fpw);
        if (pPPro.nsoil > 0) printf_end(fps);
        if (pPPro.nair > 0) printf_end(fpa);
        /*Close files*/
        if (pPPro.nwater > 0) fclose(fpw);
        if (pPPro.nsoil > 0) fclose(fps);
        if (pPPro.nair > 0) fclose(fpa);
    }

    if(pPSVG.flag_vel == 1){
        /*Open files*/
        fpw = NULL;
        fps = NULL;
        fpa = NULL;

        if (pPPro.nwater > 0) {  ///< liquid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cvelliquid");
            else  strcat(file_name, "/svg-cvelliquidr");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpw = fopen(file_name, "w");
        }

        if (pPPro.nsoil > 0) {   ///< solid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cvelsolid");
            else  strcat(file_name, "/svg-cvelsolid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fps = fopen(file_name, "w");
        }

        if (pPPro.nair > 0) {  ///< gas//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cvelgas");
            else  strcat(file_name, "/svg-cvelgas");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpa = fopen(file_name, "w");
        }

        //write to svg file
        if (pPPro.ndim == 2) {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[0], b_min[0]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[0], b_min[0]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[0], b_min[0]);
        }
        else {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[1], b_min[1]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[1], b_min[1]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[1], b_min[1]);
        }

        if(pPPro.ndim == 2){
            if (pPPro.nwater > 0) printf_boundary2d(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0) printf_boundary2d(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0) printf_boundary2d(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }
        else if(pPPro.ndim == 3){
            if (pPPro.nwater > 0 && pPSVG.type > 0) printf_boundary3dleft(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if (pPPro.nwater > 0 && pPSVG.type < 0) printf_boundary3dright(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0 && pPSVG.type > 0) printf_boundary3dleft(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nsoil > 0 && pPSVG.type < 0) printf_boundary3dright(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0 && pPSVG.type > 0) printf_boundary3dleft(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nair > 0 && pPSVG.type < 0) printf_boundary3dright(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }

        for(i = 0; i < pPPro.ntotal; i++){
            if(pVelPar[i].type == 1 ){
                colour_cal(val[i][1], max_v[1][1], min_v[1][1], rgb, abs(pPSVG.type));
                if(pPPro.ndim == 3){
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpw, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpw, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 2 ){
                colour_cal(val[i][1], max_v[2][1], min_v[2][1], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 3 ){
                colour_cal(val[i][1], max_v[3][1], min_v[3][1], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpa, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpa, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);

                }
            }
			else if (pVelPar[i].type == 4) {
				colour_cal(val[i][1], max_v[2][1], min_v[2][1], rgb, abs(pPSVG.type));
				if (pPPro.ndim == 3) {
					if (pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
					else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
					printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
				else {
					printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
			}
        }
        if (pPPro.nwater > 0) printf_end(fpw);
        if (pPPro.nsoil > 0) printf_end(fps);
        if (pPPro.nair > 0) printf_end(fpa);
        /*Close files*/
        if (pPPro.nwater > 0) fclose(fpw);
        if (pPPro.nsoil > 0) fclose(fps);
        if (pPPro.nair > 0) fclose(fpa);
    }

    if(pPSVG.flag_stress == 1){
        /*Open files*/
        fpw = NULL;
        fps = NULL;
        fpa = NULL;

        if (pPPro.nwater > 0) {  ///< liquid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-csigliquid");
            else  strcat(file_name, "/svg-csigliquid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpw = fopen(file_name, "w");
        }

        if (pPPro.nsoil > 0) {   ///< solid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-csigsolid");
            else  strcat(file_name, "/svg-csigsolid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fps = fopen(file_name, "w");
        }

        if (pPPro.nair > 0) {  ///< gas//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-csiggas");
            else  strcat(file_name, "/svg-csiggas");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpa = fopen(file_name, "w");
        }

        //write to svg file
        if (pPPro.ndim == 2) {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[0], b_min[0]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[0], b_min[0]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[0], b_min[0]);
        }
        else {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[1], b_min[1]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[1], b_min[1]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[1], b_min[1]);
        }

        if(pPPro.ndim == 2){
            if (pPPro.nwater > 0) printf_boundary2d(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0) printf_boundary2d(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0) printf_boundary2d(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }
        else if(pPPro.ndim == 3){
            if (pPPro.nwater > 0 && pPSVG.type > 0) printf_boundary3dleft(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if (pPPro.nwater > 0 && pPSVG.type < 0) printf_boundary3dright(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0 && pPSVG.type > 0) printf_boundary3dleft(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nsoil > 0 && pPSVG.type < 0) printf_boundary3dright(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0 && pPSVG.type > 0) printf_boundary3dleft(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nair > 0 && pPSVG.type < 0) printf_boundary3dright(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }

        for(i = 0; i < pPPro.ntotal; i++){
            if(pVelPar[i].type == 1 ){
                colour_cal(val[i][2], max_v[1][2], min_v[1][2], rgb, abs(pPSVG.type));
                if(pPPro.ndim == 3){
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpw, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpw, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 2 ){
                colour_cal(val[i][2], max_v[2][2], min_v[2][2], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 3 ){
                colour_cal(val[i][2], max_v[3][2], min_v[3][2], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpa, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpa, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);

                }
            }
			else if (pVelPar[i].type == 4) {
				colour_cal(val[i][2], max_v[2][2], min_v[2][2], rgb, abs(pPSVG.type));
				if (pPPro.ndim == 3) {
					if (pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
					else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
					printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
				else {
					printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
			}
        }
        if (pPPro.nwater > 0) printf_end(fpw);
        if (pPPro.nsoil > 0) printf_end(fps);
        if (pPPro.nair > 0) printf_end(fpa);
        /*Close files*/
        if (pPPro.nwater > 0) fclose(fpw);
        if (pPPro.nsoil > 0) fclose(fps);
        if (pPPro.nair > 0) fclose(fpa);
    }

    if(pPSVG.flag_strain == 1){
        /*Open files*/
        fpw = NULL;
        fps = NULL;
        fpa = NULL;

        if (pPPro.nwater > 0) {  ///< liquid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cepsliquid");
            else  strcat(file_name, "/svg-cepsliquid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpw = fopen(file_name, "w");
        }

        if (pPPro.nsoil > 0) {   ///< solid//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cepssolid");
            else  strcat(file_name, "/svg-cepssolid");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fps = fopen(file_name, "w");
        }

        if (pPPro.nair > 0) {  ///< gas//
            memset(file_name, 0, 100);
            strcpy(file_name, argv);
            if (win32) strcat(file_name, "\\svg-cepsgas");
            else  strcat(file_name, "/svg-cepsgas");
            strcat(file_name, temp);
            strcat(file_name, ".svg");
            fpa = fopen(file_name, "w");
        }
        //write to svg file
        if (pPPro.ndim == 2) {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[0], b_min[0]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[0], b_min[0]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[0], b_min[0]);
        }
        else {
            if (pPPro.nwater > 0) printf_start(fpw, dot_density, b_max[1], b_min[1]);
            if (pPPro.nsoil > 0) printf_start(fps, dot_density, b_max[1], b_min[1]);
            if (pPPro.nair > 0) printf_start(fpa, dot_density, b_max[1], b_min[1]);
        }

        if(pPPro.ndim == 2){
            if (pPPro.nwater > 0) printf_boundary2d(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0) printf_boundary2d(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0) printf_boundary2d(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }
        else if(pPPro.ndim == 3){
            if (pPPro.nwater > 0 && pPSVG.type > 0) printf_boundary3dleft(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if (pPPro.nwater > 0 && pPSVG.type < 0) printf_boundary3dright(fpw, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nsoil > 0 && pPSVG.type > 0) printf_boundary3dleft(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nsoil > 0 && pPSVG.type < 0) printf_boundary3dright(fps, b_max, b_min, pPPro.dr, base_p, dot_density);
            if (pPPro.nair > 0 && pPSVG.type > 0) printf_boundary3dleft(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
            else if(pPPro.nair > 0 && pPSVG.type < 0) printf_boundary3dright(fpa, b_max, b_min, pPPro.dr, base_p, dot_density);
        }

        for(i = 0; i < pPPro.ntotal; i++){
            if(pVelPar[i].type == 1 ){
                colour_cal(val[i][3], max_v[1][3], min_v[1][3], rgb, abs(pPSVG.type));
                if(pPPro.ndim == 3){
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpw, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpw, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 2 ){
               colour_cal(val[i][3], max_v[2][3], min_v[2][3], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
            }
            else if(pVelPar[i].type == 3 ){
                colour_cal(val[i][3], max_v[3][3], min_v[3][3], rgb, abs(pPSVG.type));
                if (pPPro.ndim == 3) {
                    if(pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
                    else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
                    printf_particles(fpa, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
                }
                else{
                    printf_particles(fpa, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);

                }
            }
			else if (pVelPar[i].type == 4) {
				colour_cal(val[i][3], max_v[2][3], min_v[2][3], rgb, abs(pPSVG.type));
				if (pPPro.ndim == 3) {
					if (pPSVG.type > 0) d3dleft_transformation(pVelPar[i].x, tx);
					else if (pPSVG.type < 0) d3dright_transformation(pVelPar[i].x, tx);
					printf_particles(fps, tx, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
				else {
					printf_particles(fps, pVelPar[i].x, pPPro.dr, b_min, base_p[0], maxyp, dot_density, rgb);
				}
			}
        }
        if (pPPro.nwater > 0) printf_end(fpw);
        if (pPPro.nsoil > 0) printf_end(fps);
        if (pPPro.nair > 0) printf_end(fpa);
        /*Close files*/
        if (pPPro.nwater > 0) fclose(fpw);
        if (pPPro.nsoil > 0) fclose(fps);
        if (pPPro.nair > 0) fclose(fpa);
    }

    //write maxmimun and minimun values of contour plot to file svgdata.dat
    fprintf(fsvg, "Step %d\n",pPPro.l);
    fprintf(fsvg, "----Dis----Vel----Stress----Strain\n");
    fprintf(fsvg, "Liquid phase max: %14.5e %14.5e %14.5e %14.5e \n", max_v[1][0], max_v[1][1], max_v[1][2], max_v[1][3]);
    fprintf(fsvg, "Liquid phase min: %14.5e %14.5e %14.5e %14.5e \n", min_v[1][0], min_v[1][1], min_v[1][2], min_v[1][3]);
    fprintf(fsvg, "Solid phase max : %14.5e %14.5e %14.5e %14.5e \n", max_v[2][0], max_v[2][1], max_v[2][2], max_v[2][3]);
    fprintf(fsvg, "Solid phase min : %14.5e %14.5e %14.5e %14.5e \n", min_v[2][0], min_v[2][1], min_v[2][2], min_v[2][3]);
    fprintf(fsvg, "Gas phase max  : %14.5e %14.5e %14.5e %14.5e \n", max_v[3][0], max_v[3][1], max_v[3][2], max_v[3][3]);
    fprintf(fsvg, "Gas phase min  : %14.5e %14.5e %14.5e %14.5e \n", min_v[3][0], min_v[3][1], min_v[3][2], min_v[3][3]);
    fprintf(fsvg, "--------------------------------------------------------------------------------------------------\n");

    delete []rgb;
    delete []val;
}
//generating start content of SVG
void clSvgPlot_Fun::printf_start(FILE *fp, double dot_density, double max_x, double min_x){
    int maxxp = (int)((max_x - min_x) * dot_density) + 200;

    fprintf(fp, "<!DOCTYPE html>");
    fprintf(fp, "   <html>\n        <body>\n");
    fprintf(fp, "               <svg width=\"%d\" height=\"1600\" viewBox=\"0,0,%d,1600\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n",
        maxxp,maxxp);
}
//generating end content of SVG
void clSvgPlot_Fun::printf_end(FILE *fp){
    fprintf(fp, "           </svg>\n        </body>\n   </html>\n");
}
//plot boundary particles for 2D case
void clSvgPlot_Fun::printf_boundary2d(FILE *fp, double b_max[3], double b_min[3],
                                   double r, int base_p[3], double dot_density){

    int cc_x, cc_y, cc_r;

    cc_x = (int)((b_max[0] - b_min[0]) * dot_density);
    cc_y = (int)((b_max[1] - b_min[1]) * dot_density);
    cc_r = (int) (r * dot_density);
	if (cc_r < 1) cc_r = 1;

    //plot boundary
    fprintf(fp, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:none;stroke-width:%d;stroke:rgb(0,0,0)\"/>\n",base_p[0], base_p[1], cc_x, cc_y, cc_r);
}
//plot boundary particles for 3D case if moving particles at left side
void clSvgPlot_Fun::printf_boundary3dleft(FILE *fp, double b_max[3], double b_min[3],
                                   double r, int base_p[3], double dot_density){

    int cc_x, cc_y, cc_z, cc_r;

    cc_x = (int)((b_max[1] - b_min[1]) * dot_density) + base_p[0];
    cc_y = (int)((b_max[2] - b_min[2]) * dot_density) + base_p[1];
    cc_z = (int)((b_max[0] - b_min[0]) * dot_density * 0.5);
    cc_r = (int) (r * dot_density);
	if (cc_r < 1) cc_r = 1;

    //plot boundary front rectangle
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0] - cc_z, base_p[1] + cc_z, base_p[0] - cc_z, cc_y + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0] - cc_z, cc_y + cc_z, cc_x - cc_z, cc_y + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x - cc_z, cc_y + cc_z, cc_x - cc_z, base_p[1] + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x - cc_z,base_p[1] + cc_z, base_p[0] - cc_z, base_p[1] + cc_z, cc_r);
    //plot boundary back rectangle
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], base_p[1], base_p[0], cc_y, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], cc_y, cc_x , cc_y, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, cc_y, cc_x, base_p[1], cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x,base_p[1], base_p[0], base_p[1], cc_r);
    //form the box
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], base_p[1], base_p[0] - cc_z, base_p[1] + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], cc_y, base_p[0] - cc_z, cc_y + cc_z,cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, cc_y, cc_x - cc_z, cc_y + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, base_p[1], cc_x - cc_z, base_p[1] + cc_z, cc_r);
}
//plot boundary particles for 3D case if moving particles at right side
void clSvgPlot_Fun::printf_boundary3dright(FILE *fp, double b_max[3], double b_min[3],
                                   double r, int base_p[3], double dot_density){

    int cc_x, cc_y, cc_z, cc_r;

    cc_x = (int)((b_max[1] - b_min[1]) * dot_density) + base_p[0];
    cc_y = (int)((b_max[2] - b_min[2]) * dot_density) + base_p[1];
    cc_z = (int)((b_max[0] - b_min[0]) * dot_density * 0.5);
    cc_r = (int) (r * dot_density);
	if (cc_r < 1) cc_r = 1;

    //plot boundary back rectangle
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], base_p[1], base_p[0], cc_y, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], cc_y, cc_x, cc_y, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, cc_y, cc_x, base_p[1], cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x,base_p[1], base_p[0], base_p[1], cc_r);
    //plot boundary front rectangle
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0]+cc_z, base_p[1]+cc_z, base_p[0]+cc_z, cc_y+cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0]+cc_z, cc_y+cc_z, cc_x+cc_z, cc_y+cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x+cc_z, cc_y+cc_z, cc_x+cc_z, base_p[1]+cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x+cc_z,base_p[1]+cc_z, base_p[0]+cc_z, base_p[1]+cc_z, cc_r);
    //form the box
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], base_p[1], base_p[0] + cc_z, base_p[1] + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", base_p[0], cc_y, base_p[0] + cc_z, cc_y + cc_z,cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, cc_y, cc_x + cc_z, cc_y + cc_z, cc_r);
    fprintf(fp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(0,0,0);stroke-width:%d\"/>\n", cc_x, base_p[1], cc_x + cc_z, base_p[1] + cc_z, cc_r);
}
//plot moving particle
void clSvgPlot_Fun::printf_particles(FILE *fp, double x[3], double r, double base[3],
                                  int base_xp, int max_yp, double dot_density,
                                  int rgb[3]){
    int cc_x, cc_y, cc_r;

    cc_x = (int) ((x[0] - base[0]) * dot_density) + base_xp;
    cc_y = (int) (- (x[1] - base[1]) * dot_density) + max_yp;
    cc_r = (int) (r * dot_density);
	if (cc_r < 1) cc_r = 1;

    fprintf(fp, "           <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:rgb(%d,%d,%d);fill-opacity:0.8\"/>\n",
            cc_x, cc_y, cc_r, rgb[0], rgb[1], rgb[2]);
}
//coordinate transformation for 3D case if moving particles at left side
void clSvgPlot_Fun::d3dleft_transformation(double *src, double *dest){

    dest[0] = src[1] - src[0] * 0.5;
    dest[1] = src[2] - src[0] * 0.5;
    dest[2] = 0.0;
}
//coordinate transformation for 3D case if moving particles at right side
void clSvgPlot_Fun::d3dright_transformation(double *src, double *dest){

    dest[0] = src[1] + src[0] * 0.5;
    dest[1] = src[2] - src[0] * 0.5;
    dest[2] = 0.0;
}
//calculating rgb value for contour plotting
void clSvgPlot_Fun::colour_cal(double val, double max, double min, int *rgb, int type){

    double mean, grad;
    mean = (max + min) / 2.0;
    grad = 0.0;

    if(type == 1){//red (255, 0, 0) -> green (0, 255, 0) -> blue (0, 0, 255)
        if(val < mean && (mean - min) > 0.0 ){
            grad = (val - min) / (mean - min);
            rgb[0] = (int)(255 * (1 - grad));
            rgb[1] = (int)(255 * grad);
            rgb[2] = 0;
        }
        else if (val >= mean && (max - mean) > 0.0 ) {
            grad = (val - mean) / (max - mean);
            rgb[0] = 0;
            rgb[1] = (int)(255 * (1 - grad));
            rgb[2] = (int)(255 * grad);
        }
        else {
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }
    }
    else if (type == 2){//blue (0, 0, 255) -> green (0, 255, 0) -> red (255, 0, 0)
        if(val < mean && (mean - min) > 0.0 ){
            grad = (val - min) / (mean - min);
            rgb[0] = 0;
            rgb[1] = (int)(255 * grad);
            rgb[2] = (int)(255 * (1 - grad));
        }
        else if (val >= mean && (max - mean) > 0.0 ) {
            grad = (val - mean) / (max - mean);
            rgb[0] = (int)(255 * grad);
            rgb[1] = (int)(255 * (1 - grad));
            rgb[2] = 0;
        }
        else {
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }
    }
    else if(type == 3){// gray white (255, 255, 255) -> black (0, 0, 0)
        if((max - min) > 0.0){
            grad = (val - min)/(max - min);
            rgb[0] = (int)(255 * (1 - grad));
            rgb[1] = (int)(255 * (1 - grad));
            rgb[2] = (int)(255 * (1 - grad));
        }
        else{
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }
    }
}

void clSvgPlot_Fun::read_data(Particle * pPar, Para_Pro pPPro, FILE * finp) {
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
