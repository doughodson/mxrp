
#ifndef __mxrp_base_util_platform_api_H__
#define __mxrp_base_util_platform_api_H__

//------------------------------------------------------------------------------
// Operating system, compiler and window manager specific includes
//------------------------------------------------------------------------------

#if defined(WIN32)
  #if defined(__MINGW32__)
    // MinGW
    #include "mxrp/base/util/platform/platform_api_mingw.hpp"
  #else
    // Visual Studio
    #include "mxrp/base/util/platform/platform_api_msvc.hpp"
  #endif
#else
  // Linux GCC
  #include "mxrp/base/util/platform/platform_api_linux.hpp"
#endif

#endif

