#pragma once

#ifdef ETHEREALENGINE_EXPORTS
#define ETHEREAL_API __declspec(dllexport)
#else
#define ETHEREAL_API __declspec(dllimport)
#endif
