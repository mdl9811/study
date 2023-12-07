#ifndef SRC_CONFIG_CONFIG_SITE_H_
#define SRC_CONFIG_CONFIG_SITE_H_

#define _LIB_NAMESPACE study
#define _LIB_VERSION 1

#include "build/build_config.h"

#if BUILDFLAG(IS_WIN)
#if defined(_MSC_VER)
#if defined(STUDY_BUILD_DLL)
#define STUDY_DLL __declspec(dllexport)
#elif defined(STUDY_CONSUME_DLL)
#define STUDY_DLL __declspec(dllimport)
#else
#define STUDY_DLL
#endif
#else
#define STUDY_DLL
#endif  // defined(_MSC_VER)
#endif

#endif