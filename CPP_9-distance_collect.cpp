/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2023, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/
# define _CRT_SECURE_NO_WARNINGS
# include <cstdio>
# include <cstdlib>
# include "string.h"
# include "Class_Functions.h"
# include "Header_Option.h"

int clDistanceData_Colloct_Fun::distance_data_collect(Para_Pro pPPro, char* argv, FILE* flog) {
	//withdraw type: 1--single folder; 2--multi folders
	int gather_type;
	int err;
	int output_number, folder_number;
	char f_name[10];

	//input the withdraw type
	printf("-------------------------------------------------------------------------------------\n");
	printf("Please input the data withdraw type: 1--single folder; 2--multi folders\n");
	printf("-------------------------------------------------------------------------------------\n");
	err = scanf("%d", &gather_type);
	printf("-------------------------------------------------------------------------------------\n");

	//process of withdrawing data
	if (gather_type == 1) { //single folder
		//count number of total Step XXXX
		printf("-------------------------------------------------------------------------------------\n");
		printf("Please input the count number of total Step XXXX in Flowing distance.dat\n");
		printf("-------------------------------------------------------------------------------------\n");
		err = scanf("%d", &output_number);
		printf("-------------------------------------------------------------------------------------\n");
		//call data whithdrawing subroutine for single folder
		err = datawithdraw_single(output_number, (char*)("."));
		if (err != 0) {
			printf("Data withdrawing of single folder raised error.\n");
			return err;
		}
	}
	else if (gather_type == 2) { //multi folder
		//input the loop folder name
		printf("-------------------------------------------------------------------------------------\n");
		printf("Please input the folder name: less than 9 characters\n");
		printf("-------------------------------------------------------------------------------------\n");
		err = scanf("%s", f_name);
		printf("-------------------------------------------------------------------------------------\n");
		//input the loop number
		printf("-------------------------------------------------------------------------------------\n");
		printf("Please input the loop number for folders: > 1\n");
		printf("-------------------------------------------------------------------------------------\n");
		err = scanf("%d", &folder_number);
		printf("-------------------------------------------------------------------------------------\n");
		//count number of total Step XXXX
		printf("-------------------------------------------------------------------------------------\n");
		printf("Please input the count number of total Step XXXX in Flowing distance.dat\n");
		printf("-------------------------------------------------------------------------------------\n");
		err = scanf("%d", &output_number);
		printf("-------------------------------------------------------------------------------------\n");
		//call data whithdrawing subroutine for single folder
		err = datawithdraw_multi((char*)("."), f_name, folder_number, output_number);
		if (err != 0) {
			printf("Data withdrawing of multi folders raised error.\n");
			return err;
		}
	}
	else {
		printf("The gatherring type is wrong.\n");
		return 1;
	}

	return 0;
}

int clDistanceData_Colloct_Fun::datawithdraw_single(int output_number, char* folder) {
	char temp[100];
	memset(temp, 0, 100);

	//skipe number
	int skip_number;
	int i, j;
	int err;

	//data variables
	char a[20], b[20], c;
	double max_x[5], max_y[5], max_z[5];
	double min_x[5], min_y[5], min_z[5];

	//File
	FILE* fFld;
	FILE* fOut;

	//file for flowing distance input
	strcpy(temp, folder);
	if (win32) strcat(temp, "\\Flowing distance.dat");
	else strcat(temp, "/Flowing distance.dat");
	if ((fFld = fopen(temp, "r")) == NULL) {
		return 11;
	}
	memset(temp, 0, 100);

	//file for data output
	strcpy(temp, folder);
	if (win32) strcat(temp, "\\Arranged_distance.dat");
	else strcat(temp, "/Arranged_distance.dat");
	if ((fOut = fopen(temp, "w")) == NULL) {
		return 11;
	}
	memset(temp, 0, 100);

	//initializing of output file
	err = fprintf(fOut, "   No.,  max_x_water,  min_x_water,  max_y_water,  min_y_water,  max_z_water,  min_z_water,  \
max_x_soil,  min_x_soil,  max_y_soil,  min_y_soil,  max_z_soil,  min_z_soil,  \
max_x_air,  min_x_air,  max_y_air,  min_y_air,  max_z_air,  min_z_air,  \
max_x_stru,  min_x_stru,  max_y_stru,  min_y_stru,  max_z_stru,  min_z_stru\n");

	//data withdrawing process
	for (i = 0; i < output_number; i++) {
		skip_number = 7 * i + 3;

		for (j = 0; j < skip_number; j++) {
			err = fscanf(fFld, "%c%*[^\n]%*c", &c);
		}

		err = fscanf(fFld, "%s %s %lf %lf %lf %lf %lf %lf", a, b,
			&max_x[1], &min_x[1], &max_y[1], &min_y[1], &max_z[1], &min_z[1]);
		err = fscanf(fFld, "%s %s %lf %lf %lf %lf %lf %lf", a, b,
			&max_x[2], &min_x[2], &max_y[2], &min_y[2], &max_z[2], &min_z[2]);
		err = fscanf(fFld, "%s %s %lf %lf %lf %lf %lf %lf", a, b,
			&max_x[3], &min_x[3], &max_y[3], &min_y[3], &max_z[3], &min_z[3]);
		err = fscanf(fFld, "%s %lf %lf %lf %lf %lf %lf", a, 
			&max_x[4], &min_x[4], &max_y[4], &min_y[4], &max_z[4], &min_z[4]);

		err = fprintf(fOut, "%5d %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
%14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n",
			i, max_x[1], min_x[1], max_y[1], min_y[1], max_z[1], min_z[1],
			max_x[2], min_x[2], max_y[2], min_y[2], max_z[2], min_z[2], 
			max_x[3], min_x[3], max_y[3], min_y[3], max_z[3], min_z[3], 
			max_x[4], min_x[4], max_y[4], min_y[4], max_z[4], min_z[4]);

		rewind(fFld);
	}

	//close file pointers
	if (fFld != NULL) fclose(fFld);
	if (fOut != NULL) fclose(fOut);

	return 0;
}

int clDistanceData_Colloct_Fun::datawithdraw_multi(char* folder, char* f_name, int folder_number, int output_number) {
	char temp[100];
	char i_char[10];
	char c;
	memset(temp, 0, 100);
	//index for the folder loop
	int i, j;
	int err;
	//for input and output
	FILE* finp, * fout;
	//data
	int no;
	double max_x[5], max_y[5], max_z[5];
	double min_x[5], min_y[5], min_z[5];

	//data to Arranged_distance.dat for every folder
	for (i = 0; i < folder_number; i++) {
		//generating folder name
		strcpy(temp, folder);
		if (win32) strcat(temp, "\\");
		else strcat(temp, "/");
		strcat(temp, f_name);
		err = sprintf(i_char, "%d", i + 1);
		strcat(temp, i_char);
		//single folder operation
		err = datawithdraw_single(output_number, temp);
		if (err != 0) {
			printf("Data withdrawing of single folder raised error.\n");
			return 1;
		}
		memset(temp, 0, 100);
	}

	//file for output
	strcpy(temp, folder);
	if (win32) strcat(temp, "\\summarized flowing distance.txt");
	else strcat(temp, "/summarized flowing distance.txt");
	if ((fout = fopen(temp, "w")) == NULL) {
		return 11;
	}
	//initializing of output file
	err = fprintf(fout, " No. of foler,  max_x_water,  min_x_water,  max_y_water,  min_y_water,  max_z_water,  min_z_water,  \
max_x_soil,  min_x_soil,  max_y_soil,  min_y_soil,  max_z_soil,  min_z_soil,  \
max_x_air,  min_x_air,  max_y_air,  min_y_air,  max_z_air,  min_z_air,  \
max_x_stru,  min_x_stru,  max_y_stru,  min_y_stru,  max_z_stru,  min_z_stru\n");

	//data summary
	for (i = 0; i < folder_number; i++) {
		//generating folder name
		strcpy(temp, folder);
		if (win32) strcat(temp, "\\");
		else strcat(temp, "/");
		strcat(temp, f_name);
		err = sprintf(i_char, "%d", i + 1);
		strcat(temp, i_char);
		if (win32) strcat(temp, "\\Arranged_distance.dat");
	    else strcat(temp, "/Arranged_distance.dat");
		if ((finp = fopen(temp, "r")) == NULL) {
			return 11;
		}
		memset(temp, 0, 100);

		//read data
		for (j = 0; j < output_number; j++) {
			err = fscanf(finp, "%c%*[^\n]%*c", &c);
		}
		err = fscanf(finp, "%d %lf %lf %lf %lf %lf %lf \
%lf %lf %lf %lf %lf %lf \
%lf %lf %lf %lf %lf %lf \
%lf %lf %lf %lf %lf %lf",
			&no, &max_x[1], &min_x[1], &max_y[1], &min_y[1], &max_z[1], &min_z[1],
			&max_x[2], &min_x[2], &max_y[2], &min_y[2], &max_z[2], &min_z[2], 
			&max_x[3], &min_x[3], &max_y[3], &min_y[3], &max_z[3], &min_z[3], 
			&max_x[4], &min_x[4], &max_y[4], &min_y[4], &max_z[4], &min_z[4]);

		//write data
		strcat(temp, f_name);
		err = sprintf(i_char, "%d", i + 1);
		strcat(temp, i_char);
		err = fprintf(fout, "%s %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
 %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
 %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e \
 %14.6e %14.6e %14.6e %14.6e %14.6e %14.6e\n",
			temp, max_x[1], min_x[1], max_y[1], min_y[1], max_z[1], min_z[1],
			max_x[2], min_x[2], max_y[2], min_y[2], max_z[2], min_z[2],
			max_x[3], min_x[3], max_y[3], min_y[3], max_z[3], min_z[3],
			max_x[4], min_x[4], max_y[4], min_y[4], max_z[4], min_z[4]);
		memset(temp, 0, 100);

		//close file pointers
		if (finp != NULL) fclose(finp);
	}

	//close file pointers
	if (fout != NULL) fclose(fout);

	return 0;
}