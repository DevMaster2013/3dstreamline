#pragma once

#ifdef RECONSTRUCTIONUTILITIES_EXPORTS
#define RECONUTIL_API __declspec(dllexport)
#else
#define RECONUTIL_API __declspec(dllimport)
#endif