/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/
#ifndef CLASS_FUNCTIONS_H_INCLUDED
#define CLASS_FUNCTIONS_H_INCLUDED
#include <stdio.h>
#include "Class_particle.h"
#include "Class_Problem.h"

class clFIni_Fun {
public:
	int initial_pro(Para_Pro *, FILE *, FILE *);
};

class clErr_Fun {
public:
	void Err_Deal(int , FILE *);
};

class clInpOutput_Fun {
public:
	int inp_output(Para_Pro pPPro, char *argv, FILE *flog);
	int all_inp_output(Para_Pro pPPro, char* argv, FILE* flog);
private:
	void read_data(Particle *pPar, Para_Pro pPPro, FILE *);
	void output_velocity(Particle *, const Para_Pro &, char *);
	void output_stress(Particle *, const Para_Pro &, char *);
	void output_velocity_bndy(Particle*, const Para_Pro&, char*);
	void output_stress_bndy(Particle*, const Para_Pro&, char*);
	void output_velocity_virtbndy(Particle*, const Para_Pro&, char*);
	void output_stress_virtbndy(Particle*, const Para_Pro&, char*);
};

class clSvgPlot_Fun {
public:
	int svg_outplot(Para_Pro pPPro, char *argv, FILE *flog);
private:
	void read_data(Particle *, Para_Pro , FILE *);
	void Plot_SVG(Particle *, const Para_Pro , const Para_SVGPlot , char *);
	void Plot_SVG_contour(Particle *, const Para_Pro , const Para_SVGPlot , char *, FILE *);
    void printf_start(FILE* fp, double dot_density, double max_x, double min_x);
	void printf_boundary2d(FILE *, double [], double [], double, int [], double );
    void printf_boundary3dleft(FILE *, double [], double [], double, int [], double);
    void printf_boundary3dright(FILE *, double [], double [], double, int [], double);
    void printf_particles(FILE *, double [], double, double [], int , int , double , int []);
    void d3dleft_transformation(double *, double *);
    void d3dright_transformation(double *, double *);
    void colour_cal(double , double , double , int *, int );
	void printf_end(FILE*);
};
class clTimeS_Fun {
public:
	int time_series(Para_Pro pPPro, char *argv, FILE *flog);
private:
	void out_data(Particle par, FILE *fop, int step);
	void read_data(Particle *, int , FILE * );
 };

class clCrossOutput_Fun {
public:
	int cross_output(Para_Pro pPPro, char *argv, FILE *flog);
private:
	void read_data(Particle *pPar, Para_Pro pPPro, FILE *);
	void output_velocity(Particle *, const Para_Pro &, double, char *);
	void output_stress(Particle *, const Para_Pro &, double, char *);
	void check_position(Particle *InpPar, Particle *OutPar, 
		const Para_Pro &InPPro, Para_Pro *OutPPro, int x_id, double x_poi);
};
class clSoiltoFluid_Fun {
public:
	int soil_to_fluid(Para_Pro pPPro, char* argv, FILE* flog);
private:
	void read_data(Particle* pPar, Para_Pro pPPro, FILE*);
	void out_data_2D(Particle* pPar, Para_Pro pPPro, FILE*, double dis_value, int dis_type);
	void out_data_3D(Particle* pPar, Para_Pro pPPro, FILE*, double dis_value, int dis_type);
};

class clDistanceData_Colloct_Fun {
public:
	int distance_data_collect(Para_Pro pPPro, char* argv, FILE* flog);
private:
	int datawithdraw_multi(char* folder, char* f_name, int folder_number, int output_number);
	int datawithdraw_single(int output_number, char* folder);
};

class clRandomField_Out {
public:
	int RandomField_Inp(Para_Pro pPPro, char* argv, FILE* flog);
private:
	void read_data_mat(Para_Mat* pMat, Para_Pro pPPro, char* argv);
	void write_data(Para_Mat* pMat, Para_Pro pPPro, char* argv);
};

class clSurfaceData_Out {
public:
	int SurfaceData_Out(Para_Pro pPPro, char* argv, FILE* flog);
private:
	void read_data(Particle* pPar, Para_Pro pPPro, FILE*);
	void write_data(Particle* pPar, FILE* finp, int total);
	void write_surface_bndy(Particle* pVelPar, FILE* finp, int total);
	void write_surface_water(Particle* pVelPar, FILE* finp, int total);
	void write_surface_soil(Particle* pVelPar, FILE* finp, int total);
};

class clArea_Vol_Data {
public:
	int Area_Vol_Data_Out(Para_Pro pPPro, char* argv, FILE* flog);
private:
	void read_data(Particle* pPar, Para_Pro pPPro, FILE*);
	void write_data(FILE*, double area, double proj_area, double volume, double height, int type);
};

#endif // CLASS_FUNCTIONS_H_INCLUDED
