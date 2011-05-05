#ifndef DYNAMNIC_FUNCTION_H
#define DYNAMNIC_FUNCTION_H

#ifdef DLL
#define DLL_EXPORT   __declspec( dllexport )
#else
#define DLL_EXPORT   __declspec( dllimport )
#endif

DLL_EXPORT int dynamic_function();

#endif
