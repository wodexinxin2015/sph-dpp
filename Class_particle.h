/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#ifndef CLASS_PARTICLE_H_INCLUDE
#define CLASS_PARTICLE_H_INCLUDE

//particles
class Particle
{
public:
	//variables
	double x[3];   /*position vector */
	double xp[3];  /*previous position vector */
	double vx[3];  /*velocity vector or boundary virtual velocity for water*/
	double vxp[3]; /*previous velocity vector or boundary virtual velocity for soil*/
	double ax[3];  /*acceleration vector or boundary virtual velocity for air*/
	double axp[3]; /*previous acceleration vector or boundary virtual velocity for structure*/

	double ux[3]; /*displacement*/

	double strep[3]; /*eig value for stress also principle stress*/
	/*2016-7-7*/

	double divx[3][3];	/*velocity derivatives*/
	double interf[3];	/*interaction forces
					  or boundary wall velocity for seismic or moving acceleration*/
	double interfss[3]; /*interaction forces of soil-structure considering the frictional force
						or boundary wall velocity for seismic or moving velocity*/

	double rho;	 /*density */
	double rhop; /*previous  density*/
	double mass; /*mass */
	double hl;	 /*smooth length */

	double pre;	 /*dynamic pressure*/
	double prep; /*previous pressure*/

	double e; /*void ratio*/

	double eps[6];	   /*strain */
	double deps[6];	   /*strain rate*/
	double sig[6];	   /*stress*/
	double dsig[6];	   /*stress rate*/
	double ff_sig[3];  /*stress component for free field particles*/
	double weps[3][3]; /*derivate strain rate */
	double adps[6];	   /*accumulated plastic strain*/
	/*V2.7 2016-7-25*/

	double tssig[6]; /*stress after TS transformation*/
	double tsqc;	 /*deviatoric stress q after TS transformation*/

	double satu;  /*degree of saturation */
	double dsatu; /*degree of saturation */
	double suct;  /*suction */
	double cop;	  /*permeability coefficient */

	double vsig; /*volumetric stress*/
	double veps; /*volumetric strain*/
	double divq; /*deviatoric stress q */
	double divr; /*deviatoric stress ratio ita*/
	double meps; /*maximium of shear strain*/

	double roue;	 /*state variable for overconsolidation*/
	double rous;	 /*state variable for degree of saturation*/
	double zeta;	 /*state variable for anisotropy*/
	double rsta;	 /*state variable for structure*/
	double prew;	 /*water pressure for soil*/
	double prea;	 /*air pressure for soil*/
	double porosity; /*porosity*/
	double cd;		 /*damping coefficient*/
	double poro_cop; /*void ratio change by the soil deformation*/

	double beta[3][3];	  /*transformation matrix of stress tensor*/
	double beta_ts[3][3]; /*transposed transformation matrix of stress tensor*/

	int type; /*type of particle */
	//0, 1, 2, 3, 4, and 7
	int matype; /*sub type of material */
	/*0, 10, 20; 1, 11, 21; 2, 12, 22; 3, 13, 23, 33;4, 14, 24;*/
	/*5, 15, 25; 6, 16, 26; 7, 17, 27; 8, 18, 28; 9, 19, 29*/
	//>10 indicates different layers
	int permtype; /*permeability type*/

	int etype; /*existing type: 0--not exist; 1--exist*/

	Particle();
	~Particle();
};


class Para_Mat
{
public:
	double xp[3];
	double x[3];
	double dens;
	double fai;
	double c;
	double cop;
	double em;
	double lamda;
	double kapa;
	double visc;
	double ds;
	int type;
	int matype;

	//methods
	Para_Mat();
	~Para_Mat();
};

#endif // CLASS_PARTICLE_H_INCLUDE