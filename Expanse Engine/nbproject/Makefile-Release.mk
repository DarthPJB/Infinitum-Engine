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
	${OBJECTDIR}/Engine.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-ldl
CXXFLAGS=-ldl

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../${CND_CONF} ../LibExpance_Common/../Release/libexpance_common.a -Wl,-rpath ../Plugin_Loader/../Release -L../Plugin_Loader/../Release -lplugin_loader

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../${CND_CONF}/expanse_engine

../${CND_CONF}/expanse_engine: ../LibExpance_Common/../Release/libexpance_common.a

../${CND_CONF}/expanse_engine: ../Plugin_Loader/../Release/libplugin_loader.so

../${CND_CONF}/expanse_engine: ${OBJECTFILES}
	${MKDIR} -p ../${CND_CONF}
	${LINK.cc} -o ../${CND_CONF}/expanse_engine ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Engine.o: Engine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../SharedInclude -MMD -MP -MF $@.d -o ${OBJECTDIR}/Engine.o Engine.cpp

# Subprojects
.build-subprojects:
	cd ../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release
	cd ../Plugin_Loader && ${MAKE}  -f Makefile CONF=Release
	cd ../Plugin_Loader && ${MAKE}  -f Makefile CONF=Release
	cd ../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release
	cd ../Test_Plugin/linux_gl_30 && ${MAKE}  -f Makefile CONF=Release
	cd ../Test_Plugin/Error_System && ${MAKE}  -f Makefile CONF=Release
	cd ../Test_Plugin/OBJ_model && ${MAKE}  -f Makefile CONF=Release

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../${CND_CONF}/expanse_engine

# Subprojects
.clean-subprojects:
	cd ../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release clean
	cd ../Plugin_Loader && ${MAKE}  -f Makefile CONF=Release clean
	cd ../Plugin_Loader && ${MAKE}  -f Makefile CONF=Release clean
	cd ../LibExpance_Common && ${MAKE}  -f Makefile CONF=Release clean
	cd ../Test_Plugin/linux_gl_30 && ${MAKE}  -f Makefile CONF=Release clean
	cd ../Test_Plugin/Error_System && ${MAKE}  -f Makefile CONF=Release clean
	cd ../Test_Plugin/OBJ_model && ${MAKE}  -f Makefile CONF=Release clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
