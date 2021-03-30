#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/soil/stb_image_aug.o \
	${OBJECTDIR}/soil/image_helper.o \
	${OBJECTDIR}/soil/SOIL.o \
	${OBJECTDIR}/soil/image_DXT.o \
	${OBJECTDIR}/opengl_30.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fvisibility=hidden
CXXFLAGS=-fvisibility=hidden

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../../LibExpance_Common/../Release/libexpance_common.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../../${CND_CONF}/libgl_30.so

../../${CND_CONF}/libgl_30.so: ../../LibExpance_Common/../Release/libexpance_common.a

../../${CND_CONF}/libgl_30.so: ${OBJECTFILES}
	${MKDIR} -p ../../${CND_CONF}
	${LINK.cc} -lGL -lglu -shared -o ../../${CND_CONF}/libgl_30.so -fPIC ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/soil/stb_image_aug.o: soil/stb_image_aug.c 
	${MKDIR} -p ${OBJECTDIR}/soil
	${RM} $@.d
	$(COMPILE.c) -O2 -I. -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/soil/stb_image_aug.o soil/stb_image_aug.c

${OBJECTDIR}/soil/image_helper.o: soil/image_helper.c 
	${MKDIR} -p ${OBJECTDIR}/soil
	${RM} $@.d
	$(COMPILE.c) -O2 -I. -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/soil/image_helper.o soil/image_helper.c

${OBJECTDIR}/soil/SOIL.o: soil/SOIL.c 
	${MKDIR} -p ${OBJECTDIR}/soil
	${RM} $@.d
	$(COMPILE.c) -O2 -I. -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/soil/SOIL.o soil/SOIL.c

${OBJECTDIR}/soil/image_DXT.o: soil/image_DXT.c 
	${MKDIR} -p ${OBJECTDIR}/soil
	${RM} $@.d
	$(COMPILE.c) -O2 -I. -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/soil/image_DXT.o soil/image_DXT.c

${OBJECTDIR}/opengl_30.o: opengl_30.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../SharedInclude -I. -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/opengl_30.o opengl_30.cpp

# Subprojects
.build-subprojects:
	cd ../../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../../${CND_CONF}/libgl_30.so

# Subprojects
.clean-subprojects:
	cd ../../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
