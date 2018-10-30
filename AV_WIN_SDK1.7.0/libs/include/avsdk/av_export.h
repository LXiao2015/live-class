#ifndef AV_EXPORT_H_
#define AV_EXPORT_H_

#include "build_config.h"

#ifdef OS_WIN
#if defined(AV_IMPLEMENTATION)
#define AV_EXPORT __declspec(dllexport)
#else
#define AV_EXPORT __declspec(dllimport)
#endif  // defined(AV_IMPLEMENTATION)
#else
#define AV_EXPORT
#endif

#define DISALLOW_EVIL_DESTRUCTIONS(TypeName)    \
    protected:  \
        virtual ~TypeName() {}

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif

char* AVAPI_GetSDKVersion();
void* AVAPI_CreateContext(void* config);
void  AVAPI_DestroyContext(void* context);
void  AVAPI_EnableCrashReport(bool enable);

#ifdef __cplusplus
}
#endif

typedef char* (*PROC_AVAPI_GetSDKVersion)();
typedef void* (*PROC_AVAPI_CreateContext)(void* config);
typedef void (*PROC_AVAPI_DestroyContext)(void* context);
typedef void (*PROC_AVAPI_EnableCrashReport)(bool enable);

#endif


#endif // #ifndef AV_EXPORT_H_