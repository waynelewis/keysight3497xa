TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Keysight3497xA
# Keysight3497xA.dbd will be created and installed
DBD += Keysight3497xA.dbd

# Keysight3497xA.dbd will be made up from these files:
Keysight3497xA_DBD += base.dbd

# Include dbd files from all support applications:
#Keysight3497xA_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#Keysight3497xA_LIBS += xxx

# Keysight3497xA_registerRecordDeviceDriver.cpp derives from Keysight3497xA.dbd
Keysight3497xA_SRCS += Keysight3497xA_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Keysight3497xA_SRCS_DEFAULT += Keysight3497xAMain.cpp
Keysight3497xA_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Keysight3497xA_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Keysight3497xA_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

