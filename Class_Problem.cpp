/************************************************************************************
SPH Data Post Viewer

--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Class_Problem.h"

Para_Pro::Para_Pro(){
	ndim = 1;

	fop = 0;
	inip = 0;
	loop = 0;
	ntotal = 0;

	nboun = 0;
	nwater = 0;
	nsoil = 0;
	nair = 0;

	xsph[0]=0.0;
	xsph[1]=0.0;
	xsph[2]=0.0;

	art_vis[0][0] = 0.0;
	art_vis[0][1] = 0.0;
	art_vis[1][0] = 0.0;
	art_vis[1][1] = 0.0;
	art_vis[2][0] = 0.0;
	art_vis[2][1] = 0.0;

	dttype = 0;
	dr = 0.0;
	dt = 0.0;

	l = 1;
	t = 0.0;
}

Para_Pro::~Para_Pro(){

}


Para_SVGPlot::Para_SVGPlot(){
    flag = 0;
    type = 0;
    flag_dis = 0;
    flag_vel = 0;
    flag_stress = 0;
    flag_strain = 0;
}

Para_SVGPlot::~Para_SVGPlot(){

}
