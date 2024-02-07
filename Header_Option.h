/************************************************************************************
SPH Data Post Viewer
--Copyright (c) 2018-2024, Weijie ZHANG, GeoHohai, Hohai University.
************************************************************************************/

#ifndef HEADER_OPTION_H_INCLUDED
#define HEADER_OPTION_H_INCLUDED

#define fmax(x,y) (x>y)?x:y;
#define fmin(x,y) (x<y)?x:y;

#ifdef __ANDROID__
#define win32 0
#elif __linux__
#define win32 0
#elif _WIN32
#define win32 1
#endif

#endif // HEADER_OPTION_H_INCLUDED
