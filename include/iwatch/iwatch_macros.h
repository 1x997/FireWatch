//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_MACROS_H
#define ENEYES_IWATCH_API_IWATCH_MACROS_H
#define IWATCH_API

#ifndef _WIN32

#define IWATCH_API

#elif defined IWATCH_EXPORTS

#define IWATCH_API __declspec(dllexport)

#else

#define IWATCH_API

#endif

#endif //ENEYES_IWATCH_API_IWATCH_MACROS_H
