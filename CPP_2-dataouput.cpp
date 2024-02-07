/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

void statement_data(FILE *flog) {
	/*program information*/
	/*the version of program must be reflected in this function*/
	printf("-------------------------------------------------------------------\n");
	printf("SPH Data Post Viewer V1.11.0\n");
	printf("--Copyright (c) 2018-2024 Weijie ZHANG, GeoHohai, Hohai University.\n");
	printf("-------------------------------------------------------------------\n");
	printf("2022-05-09: v1.8.1 add the continuous output of inp files from odb files\n");
	printf("2022-06-05: v1.9.1 revise the random field to inp file\n");
	printf("2023-04-11: v1.10.0 add the extraction module of surface data from the odb file\n");
	printf("2023-04-11: v1.10.0 add the area and volume calculations module\n");
	printf("-------------------------------------------------------------------\n");
	fprintf(flog, "-------------------------------------------------------------------\n");
	fprintf(flog, "SPH Data Post Viewer V1.11.0\n");
	fprintf(flog, "--Copyright (c) 2018-2024 Weijie ZHANG, GeoHohai, Hohai University.\n");
	fprintf(flog, "-------------------------------------------------------------------\n");
	fprintf(flog, "2022-05-09: v1.8.1 add the continuous output of inp files from odb files\n");
	fprintf(flog, "2022-06-05: v1.9.1 revise the random field to inp file\n");
	fprintf(flog, "2023-04-11: v1.10.0 add the extraction module of surface data from the odb file\n");
	fprintf(flog, "2023-04-11: v1.10.0 add the area and volume calculations module\n");
	fprintf(flog, "-------------------------------------------------------------------\n");
}

int data_output(int argc, char *argv) {
	FILE *finp, *flog;
	//working directory initializing
	int err_t, dwflag;

	char temp[100], file_name[100];
	memset(temp, 0, 100);
	memset(file_name, 0, 100);

	//problem global parameters
	Para_Pro *pPPro = new Para_Pro();

	finp = NULL;

	//Function instances
	clFIni_Fun ItFini;
	clErr_Fun ItErrD;
	clTimeS_Fun ItTimeS;
	clInpOutput_Fun ItInpOutput;
	clSvgPlot_Fun ItSvgPlot;
	clCrossOutput_Fun ItCrossOut;
	clSoiltoFluid_Fun ItStoF;
	clDistanceData_Colloct_Fun ItDisColl;
	clRandomField_Out ItRandomField;
	clSurfaceData_Out ItSurfaceData;
	clArea_Vol_Data ItAreaVolData;

	//path for windows
	if (win32) {
		//file for log output
		strcpy(temp, argv);
		strcat(temp, "\\dwlog.txt");
		if ((flog = fopen(temp, "w")) == NULL) {
			ItErrD.Err_Deal(10, flog);
			return 10;
		}
		memset(temp, 0, 100);
	}
	//path for Linux
	else {
		//file for log output
		strcpy(temp, argv);
		strcat(temp, "/dwlog.txt");
		if ((flog = fopen(temp, "w")) == NULL) {
			ItErrD.Err_Deal(10, flog);
			return 10;
		}
		memset(temp, 0, 100);
	}
	/*statement*/
	statement_data(flog);

	//path for windows
	if (win32) {
		//Initializing of input file
		strcpy(temp, argv);
		strcat(temp, "\\input.dat");
		if ((finp = fopen(temp, "r")) == NULL) {
			ItErrD.Err_Deal(1, flog);
			return 1;
		}
		memset(temp, 0, 100);
	}
	//path for Linux
	else {
		//Initializing of input file
		strcpy(temp, argv);
		strcat(temp, "/input.dat");
		if ((finp = fopen(temp, "r")) == NULL) {
			ItErrD.Err_Deal(1, flog);
			return 1;
		}
		memset(temp, 0, 100);
	}
	//initializing from input file for problems and material property
	err_t = ItFini.initial_pro(pPPro, flog, finp);
	if (err_t > 0) {
		ItErrD.Err_Deal(err_t, flog);
		return err_t;
	}
	dwflag = 0;
	//input controlling
	do {
		//information on screen
		printf("-------------------------------------------------------------------\n");
		printf("Please select the data ouput type:\n");
		printf("	1--output to .inp files (Micro AVS);\n");
		printf("	2--output to .svg files (IE, Chrome or Firefox);\n");
		printf("	3--output the time series of variables;\n");
		printf("	4--output the cross section of 2D and 3D cases;\n");
		printf("	5--change the sliding area of soil slope to fluid particles;\n");
		printf("	6--arrange the distance data from Flowing distance.dat;\n");
		printf("	7--generate the inp file from parameters.txt;\n");
		printf("	8--continuous output of inp files from odb files;\n");
		printf("	9--extracting the surface data from the odb file;\n");
		printf("	10--get the area (2D-mass volume or 3D-surface area) \n		and volume (3D-mass volume);\n");
		printf("	0--exit.\n");
        printf("-------------------------------------------------------------------\n");
		err_t = scanf("%d", &dwflag);

		if (dwflag == 1) {//output to .inp files (Micro AVS)
			err_t = ItInpOutput.inp_output(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 2) {//output to .svg files (IE, Chrome or Firefox)
			err_t = ItSvgPlot.svg_outplot(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 3) {//output the time serie of variables
			err_t = ItTimeS.time_series(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 4) {//output the cross section of 2D and 3D cases
			err_t = ItCrossOut.cross_output(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 5) {//change the sliding area of soil slope to fluid particles
			err_t = ItStoF.soil_to_fluid(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 6) {//collect flowing distance information from seperate or multi folders
			err_t = ItDisColl.distance_data_collect(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 7) {//output the random field to inp file
			err_t = ItRandomField.RandomField_Inp(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 8) {//continuous output of inp files from odb files
			err_t = ItInpOutput.all_inp_output(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 9) {//extracting the surface data from the odb file
			err_t = ItSurfaceData.SurfaceData_Out(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag == 10) {//calculate the area and volume of landslide mass
			err_t = ItAreaVolData.Area_Vol_Data_Out(*pPPro, argv, flog);
			if (err_t > 0) {
				ItErrD.Err_Deal(err_t, flog);
				return err_t;
			}
		}
		else if (dwflag < 0 || dwflag > 10) {//exception deal
			ItErrD.Err_Deal(12, flog);
			return 12;
		}
	} while (dwflag > 0 && dwflag < 11);

	printf("-------------------------------------------------------------------\n");
	printf("Data withdraw is finished!\n");
	printf("-------------------------------------------------------------------\n");

	fprintf(flog, "-------------------------------------------------------------------\n");
	fprintf(flog, "Data withdraw is finished!\n");
	fprintf(flog, "-------------------------------------------------------------------\n");

	//close file
	fclose(flog);
	fclose(finp);

	delete pPPro;

	return 0;
}
