/************************************************************************************
SPH Data Post Viewer

--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

# define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "Class_Functions.h"

void clErr_Fun::Err_Deal(int err_type, FILE * flog) {
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("\n");

	if (err_type == 1) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-F01 There is no input file.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-F01 There is no input file.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
    else if (err_type == 10) {
        printf("Current time: %s", asctime(timeinfo));
        printf("-------------------------------------------------------------------\n");
        printf("----||E-WD10: The working directory is not correct.\n");
        printf("-------------------------------------------------------------------\n");
    }
	else if (err_type == 11) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-WD11: The working directory is not correct.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-WD11: The working directory is not correct.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 12) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-WD12: The type of data output is wrong.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-WD12: The type of data output is wrong.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 13) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-SV13: contour type is not correct.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-SV13: contour type is not correct.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 14) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-IP14: step number is not correct.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-IP14: step number  is not correct.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 15) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-SV15: can not open data-range file.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-SV15: can not open data-range file.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 16) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-DW16: can not open time serie file.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-DW16: can not open time serie file.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 17) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-DW17: x_id must be less than or equal to the problem dimensions.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-DW17: x_id must be less than or equal to the problem dimensions.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}
	else if (err_type == 18) {
		printf("Current time: %s", asctime(timeinfo));
		printf("-------------------------------------------------------------------\n");
		printf("----||E-DW18: x_poi must be in the problem domain.\n");
		printf("-------------------------------------------------------------------\n");

		fprintf(flog, "Current time: %s", asctime(timeinfo));
		fprintf(flog, "-------------------------------------------------------------------\n");
		fprintf(flog, "----||E-DW18: x_poi must be in the problem domain.\n");
		fprintf(flog, "-------------------------------------------------------------------\n");

		fclose(flog);
	}

}
