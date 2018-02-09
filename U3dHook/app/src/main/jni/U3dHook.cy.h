//
// Created by Administrator on 2017/4/24 0024.
//

#ifndef U3DHOOK_U3DHOOK_CY_H
#define U3DHOOK_U3DHOOK_CY_H

#include <android/log.h>
#include "substrate.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>

using namespace std;

#define LOG_TAG "GameFuck"
#define MAX 1024 * 1024 * 10

#define HOOK_SYMBOL(handle, func) hook_function(handle, #func, (void*) new_##func, (void**) &orig_##func)
#define HOOK_DEF(ret, func, ...) \
  ret (*orig_##func)(__VA_ARGS__); \
  ret new_##func(__VA_ARGS__)

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/*
 * Basic data types
 */
typedef int            gboolean;
typedef int            gint;
typedef unsigned int   guint;
typedef short          gshort;
typedef unsigned short gushort;
typedef long           glong;
typedef unsigned long  gulong;
typedef void *         gpointer;
typedef const void *   gconstpointer;
typedef char           gchar;
typedef unsigned char  guchar;

#if !G_TYPES_DEFINED
#ifdef _MSC_VER
typedef __int8				gint8;
typedef unsigned __int8		guint8;
typedef __int16				gint16;
typedef unsigned __int16	guint16;
typedef __int32				gint32;
typedef unsigned __int32	guint32;
typedef __int64				gint64;
typedef unsigned __int64	guint64;
typedef float				gfloat;
typedef double				gdouble;
typedef unsigned __int16	gunichar2;
#else
/* Types defined in terms of the stdint.h */
typedef int8_t         gint8;
typedef uint8_t        guint8;
typedef int16_t        gint16;
typedef uint16_t       guint16;
typedef int32_t        gint32;
typedef uint32_t       guint32;
typedef int64_t        gint64;
typedef uint64_t       guint64;
typedef float          gfloat;
typedef double         gdouble;
typedef uint16_t       gunichar2;
#endif
#endif

#define MONO_PUBLIC_KEY_TOKEN_LENGTH	17


typedef struct {
    const char *name;
    const char *culture;
    const char *hash_value;
    const guint8* public_key;
    // string of 16 hex chars + 1 NULL
    guchar public_key_token [MONO_PUBLIC_KEY_TOKEN_LENGTH];
    guint32 hash_alg;
    guint32 hash_len;
    guint32 flags;
    guint16 major, minor, build, revision;
} MonoAssemblyName;

struct _MonoAssembly {
    /*
     * The number of appdomains which have this assembly loaded plus the number of
     * assemblies referencing this assembly through an entry in their image->references
     * arrays. The later is needed because entries in the image->references array
     * might point to assemblies which are only loaded in some appdomains, and without
     * the additional reference, they can be freed at any time.
     * The ref_count is initially 0.
     */
    int ref_count; /* use atomic operations only */
    char *basedir;
    MonoAssemblyName aname;
    guint8 friend_assembly_names_inited;
    guint8 in_gac;
    guint8 dynamic;
    guint8 corlib_internal;
    gboolean ref_only;
    /* security manager flags (one bit is for lazy initialization) */
    guint32 ecma:2;		/* Has the ECMA key */
    guint32 aptc:2;		/* Has the [AllowPartiallyTrustedCallers] attributes */
    guint32 fulltrust:2;	/* Has FullTrust permission */
    guint32 unmanaged:2;	/* Has SecurityPermissionFlag.UnmanagedCode permission */
    guint32 skipverification:2;	/* Has SecurityPermissionFlag.SkipVerification permission */
};

struct _MonoImage {
    /*
     * The number of assemblies which reference this MonoImage though their 'image'
     * field plus the number of images which reference this MonoImage through their
     * 'modules' field, plus the number of threads holding temporary references to
     * this image between calls of mono_image_open () and mono_image_close ().
     */
    int   ref_count;
    void *raw_data_handle;
    char *raw_data;
    int raw_data_len;

    char *name;
    const char *assembly_name;
    const char *module_name;

    _MonoAssembly *assembly;
};

typedef enum {
    MONO_IMAGE_OK,
    MONO_IMAGE_ERROR_ERRNO,
    MONO_IMAGE_MISSING_ASSEMBLYREF,
    MONO_IMAGE_IMAGE_INVALID
} MonoImageOpenStatus;

struct _MonoClass{
    _MonoImage *image;
    const char *name;

};

typedef int gboolean;

void* get_module_base(int pid, const char* module_name);
void* get_remote_addr(int target_pid, const char* module_name, void* local_addr);
bool saveFile(const void* addr, int len, const char *outFileName);
bool saveFile(const char *addr, int len, const char *outFileName);
string getNextFilePath(const char *fileExt);

void u3dHook(void *handle);
string getFilePath(const char *fileExt, const char *name);
bool saveDllFile(char *offset, int32_t data_len, const char *outFileName);


#endif //U3DHOOK_U3DHOOK_CY_H
