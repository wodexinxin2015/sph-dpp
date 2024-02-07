/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/
/************************************************************************************
revision:
2022-05-09: v1.8.1 add the continuous output of inp files from odb files
2022-06-05: v1.9.1 revise the random field to inp file
2023-04-11: v1.10.0 add the extraction module of surface data from the odb file
2023-04-11: v1.10.0 add the area and volume calculations module
************************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <time.h>
#include "Header_Option.h"

int data_output(int argc, char *argv);

int main(int argc, char *argv[])
{

	int i, run_result;

	run_result = 0;

	/*SPH simulation process with arguments: argc, argv*/
	if (argc > 2)
	{

		printf("-------------------------------------------------------------------\n");
		printf("--||The simulation has    %3d working project.||----------\n", (argc - 2) < 1 ? 1 : (argc - 2));
		printf("-------------------------------------------------------------------\n");

		if (argv[1][0] == '-' && argv[1][1] == 'd')
		{
			for (i = 2; i < argc; i++)
			{
				printf("-------------------------------------------------------------------\n");
				printf("--||The data withdraw of Project No.%3d is now running.||---\n", i - 1);
				printf("--<%53s>--\n", argv[i]);
				printf("-------------------------------------------------------------------\n");

				run_result = data_output(argc, argv[i]);

				if (run_result == 0)
				{
					printf("-------------------------------------------------------------------\n");
					printf("--||Data withdraw of Project No.%3d is now finished.||--\n", i - 1);
					printf("-------------------------------------------------------------------\n");
				}
				else
				{
					printf("-------------------------------------------------------------------\n");
					printf("--||Data withdraw of Project No.%3d raised an error.||--\n", i - 1);
					printf("-------------------------------------------------------------------\n");
				}
			}
		}
		else
		{
			printf("Error 200: option is not -d.\n");
			printf("-------------------------------------------------------------------\n");
			printf("SPH Data Post Viewer\n");
			printf("--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.\n");
			printf("-------------------------------------------------------------------\n");
			printf("     Please using no arguments or following arguments.\n");
			printf("-------------------------------------------------------------------\n");
			printf("     Argument list: -d directory1 directory2 ...\n");
			printf("               -d         ---specifying data withoraw directory;\n");
			printf("               directory1 ---directory path 1;\n");
			printf("               directory2 ---directory path 2;\n");
			printf("               ...        ---more directory path.\n");
			printf("-------------------------------------------------------------------\n");

			return 200;
		}
	}
	else if (argc == 2)
	{
		printf("Error 100: Argument is not correct, Exit.\n");
		printf("-------------------------------------------------------------------\n");
		printf("SPH Data Post Viewer\n");
		printf("--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.\n");
		printf("-------------------------------------------------------------------\n");
		printf("     Please using no arguments or following arguments.\n");
		printf("-------------------------------------------------------------------\n");
		printf("     Argument list: -d directory1 directory2 ...\n");
		printf("               -d         ---specifying data withoraw directory;\n");
		printf("               directory1 ---directory path 1;\n");
		printf("               directory2 ---directory path 2;\n");
		printf("               ...        ---more directory path.\n");
		printf("-------------------------------------------------------------------\n");

		return 100;
	}
	else if (argc == 1)
	{
		printf("-------------------------------------------------------------------\n");
		printf("---||No specifying working directory!||--------------------\n");
		printf("---||using the directory of executable file!||-------------\n");
		printf("-------------------------------------------------------------------\n");

		printf("-------------------------------------------------------------------\n");
		printf("--||The data withdraw of current directory is now running.||---\n");
		printf("-------------------------------------------------------------------\n");

		run_result = data_output(argc, (char *)("."));

		if (run_result == 0)
		{
			printf("-------------------------------------------------------------------\n");
			printf("--||Data withdraw is now finished.||--\n");
			printf("-------------------------------------------------------------------\n");
		}
		else
		{
			printf("-------------------------------------------------------------------\n");
			printf("--||Data withdraw raised an error.||--\n");
			printf("-------------------------------------------------------------------\n");
		}
	}

	return 0;
}
