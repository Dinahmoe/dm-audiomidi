# author: Alessandro Saccoia <alessandro@dinahmoe.com>
# Copyright 2014 Dinahmoe

# usage: every target that uses this library should add the right link time dependencies:
# SET_TARGET_PROPERTIES(blablabla PROPERTIES LINK_FLAGS "${DM_AUDIOMIDI_LINK_FLAGS}")

# 1/19/2015 Refactored

CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT("dm-audiomidi")

MESSAGE(STATUS "Configuring dm-audiomidi")

SET(DM_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dm-cmake)
INCLUDE(SetTargetOptions)
INCLUDE(SetPlatformDefines)

OPTION(APPLE_DONT_USE_QUICKTIME "Excludes core audio and uses the rest on apple" OFF)

SET_PLATFORM_DEFINES()
SET(compile_options "")

IF(DEFINED USE_SAMPLERATE)
  INCLUDE(FindSampleRate)
  MESSAGE(STATUS "****************** Using LibSampleRate -> GPL LICENSE! *******************")
  SET(compile_options "${compile_options} -DDM_AUDIOMIDI_USE_SAMPLERATE=1 ")
ENDIF()

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include/")                         
FILE(GLOB include_f "${CMAKE_CURRENT_SOURCE_DIR}/include/*.*")

SET(DM_AUDIOMIDI_LINK_FLAGS "" CACHE INTERNAL "Linker flags for dm_audiomidi" )

### CoreAudio ### <- Quicktime, Coreaudio formats
IF (APPLE AND NOT APPLE_DONT_USE_QUICKTIME)
  MESSAGE(STATUS "dm-audiomidi: Using Apple quicktime libraries")
  LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_CoreAudio.hpp)
  LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_CoreAudio.cpp)
  SET(DM_AUDIOMIDI_LINK_FLAGS "${DM_AUDIOMIDI_LINK_FLAGS} -framework AudioToolbox -framework CoreFoundation" CACHE INTERNAL "Linker flags for dm_audiomidi" )
  SET(compile_options "${compile_options} -DDM_AUDIOMIDI_USE_COREAUDIO=1 ")
ENDIF(APPLE AND NOT APPLE_DONT_USE_QUICKTIME)

### OGG ### <- always added
MESSAGE(STATUS "dm-audiomidi: Using Ogg Decoder -> PUBLIC DOMAIN LICENSE")
LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.cpp)
LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.hpp)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_ogg.cpp PROPERTIES COMPILE_FLAGS "-Wno-unused-value -Wno-sign-compare -Wno-all")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/stb_vorbis.c PROPERTIES COMPILE_FLAGS "-Wno-unused-value -Wno-sign-compare -Wno-all")
SET(compile_options "${compile_options} -DDM_AUDIOMIDI_USE_OGG=1 ")

### AAC ###
IF ((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME)
  MESSAGE(STATUS "dm-audiomidi: Using FDK AAc -> MIT-STYLE LICENSE")
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libAACdec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libAACenc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libFDK/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libMpegTPDec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libMpegTPEnc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libPCMutils/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSBRdec/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSBRenc/include)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/libSYS/include)
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fdk-aac/ "fdk-aac")
  LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_aac.cpp)
  LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_aac.hpp)
  SET(compile_options "${compile_options} -DDM_AUDIOMIDI_USE_AAC=1 ")
  SET(DM_AUDIOMIDI_EXTRA_LIBS ${DM_LIBFDK_AAC_LIBS} CACHE INTERNAL "Libraries for dm-audiomidi")
ENDIF ((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME)

### SNDFILE ###
IF ((NOT ANDROID) AND((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME))
  INCLUDE(FindSndFile)
  IF (SNDFILE_FOUND)
    MESSAGE(STATUS "dm-audiomidi: Using Libsndfile -> LGPL License")
    INCLUDE_DIRECTORIES(${SNDFILE_INCLUDE_DIRS})
    IF (WIN32)
      SET(DM_AUDIOMIDI_LINK_FLAGS "${DM_AUDIOMIDI_LINK_FLAGS} -lsndfile" CACHE INTERNAL "Linker flags for dm_audiomidi" )
    ELSE(WIN32)
      SET(DM_AUDIOMIDI_LINK_FLAGS "${DM_AUDIOMIDI_LINK_FLAGS} ${SNDFILE_LIBRARIES}" CACHE INTERNAL "Linker flags for dm_audiomidi" )
    ENDIF(WIN32)
    LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_sndfile.cpp)
    LIST(APPEND DM_AUDIOMIDI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormat_sndfile.hpp)
    SET(compile_options "${compile_options} -DDM_AUDIOMIDI_USE_SNDFILE=1 ")
    MESSAGE(${DM_AUDIOMIDI_LINK_FLAGS})
  ELSE(SNDFILE_FOUND)
    MESSAGE(FATAL_ERROR "Libsoundfile not found, don't use FORCE_SOUNDFILE")
  ENDIF(SNDFILE_FOUND)
ENDIF ((NOT ANDROID) AND((NOT APPLE) OR APPLE_DONT_USE_QUICKTIME))

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../dm-utils/include)

MESSAGE(STATUS ${compile_options})

ADD_LIBRARY(dm-audiomidi
    ${include_f}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AudioFormatsManager.cpp
    ${DM_AUDIOMIDI_SRCS})
# TARGET_LINK_LIBRARIES(dm-audiomidi ${DM_AUDIOMIDI_EXTRA_LIBS})
FSET_TARGET_OPTIONS(dm-audiomidi ${compile_options})

ADD_SUBDIRECTORY(test EXCLUDE_FROM_ALL)
