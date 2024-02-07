/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#ifndef CLASS_PROBLEM_H_INCLUDE
#define CLASS_PROBLEM_H_INCLUDE


class Para_Pro{
	//Problem parameters
public:
	int ndim; /*dimensions*/

	int fop; /*steps for output to file*/
	int inip; /*steps for initial stress of soil*/
	int loop; /*total loops*/
	int ntotal; /*total number of particles*/

	int nboun; /*number of boundary particles*/
	int nwater; /*number of water particles*/
	int nsoil; /*number of soil particles*/
	int nair; /*number of air particles*/

	int dttype;/*time step type 1-constant 2-variable*/
	double dr; /*particle spacing*/
	double dt; /*time increment*/

	double xsph[3]; /*xsph coefficient for the subroutine of xsph*/
	//0--water; 1--soil; 2--air
	double art_vis[3][2]; /*artificial viscosity coefficient*/
					//0--water; 1--soil; 2--air

	int l; /*current step*/
	double t; /*time*/

	//methods
	Para_Pro();
	~Para_Pro();
};
class Para_SVGPlot{// parameters for svg output
public:
    int flag; //svg plot or not
    int type; //controlling the type of countour color
    //1--red -> green -> blue; 2--blue -> green -> red; 3--white -> black
    int flag_dis; //countour plot for dispacement magnitude
    int flag_vel; //countour plot for velocity magnitude
    int flag_stress; //countour plot for second invariant of stress
    int flag_strain; //countour plot for second invariant of stain

    //methods
    Para_SVGPlot();
    ~Para_SVGPlot();
};

#endif // CLASS_PROBLEM_H_INCLUDE
