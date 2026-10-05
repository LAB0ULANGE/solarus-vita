####
# By default, you will build a binary with the maximum of compatibility that your current version can provide.
# You may want to produce an optimized (or exotic) binary instead of a standard one.
# To do so, overload the following options :
#
# SOLARUS_ARCH represent the build (multi-)architecture.
# SOLARUS_SYSROOT represent the path to the macOS SDK.
# SOLARUS_DEPLOYMENT represent the oldest macOS version supported.
#
# Some options set to the cache also need the FORCE parameter for obscure reasons.
#
# Exportable to XCode.
####

# Build architectures.
if(NOT SOLARUS_ARCH)
  set(SOLARUS_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()
set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Build architecture" FORCE)

# Deployment version.
if(NOT SOLARUS_DEPLOYMENT)
  set(SOLARUS_DEPLOYMENT "14.0")
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

# LuaJIT workaround.
# According to the LuaJIT doc, additional linker flags are needed with a 64bit build.
if(SOLARUS_USE_LUAJIT AND SOLARUS_ARCH MATCHES "x86_64")
  if(XCODE)
    set_property(TARGET solarus PROPERTY
      "XCODE_ATTRIBUTE_LINKER_FLAGS[arch=x86_64]" "-pagezero_size 10000 -image_base 100000000"
    )
  elseif(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-pagezero_size 10000 -image_base 100000000")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "LuaJIT woraround" FORCE)
  endif()
endif()
