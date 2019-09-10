TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# Build the library
LIBRARY_IOC += KS3497xA

DBD += KS3497xASupport.dbd

KS3497xA_SRCS += KS3497xA.cpp

KS3497xA_LIBS += asyn
KS3497xA_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = KS3497xAApp
# KS3497xAApp.dbd will be created and installed
DBD += KS3497xAApp.dbd

# KS3497xAApp.dbd will be made up from these files:
KS3497xAApp_DBD += base.dbd

# Include dbd files from all support applications:
KS3497xAApp_DBD += asyn.dbd
KS3497xAApp_DBD += drvAsynIPPort.dbd
KS3497xAApp_DBD += devEpics.dbd

# Include DBD file for this device
KS3497xAApp_DBD += KS3497xASupport.dbd

# Add all the support libraries needed by this IOC
KS3497xAApp_LIBS += asyn

# Include the library for the KS3497xA
KS3497xAApp_LIBS += KS3497xA

# KS3497xAApp_registerRecordDeviceDriver.cpp derives from KS3497xAApp.dbd
KS3497xAApp_SRCS += KS3497xAApp_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
KS3497xAApp_SRCS_DEFAULT += KS3497xAAppMain.cpp
#KS3497xAApp_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#KS3497xAApp_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
KS3497xAApp_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

