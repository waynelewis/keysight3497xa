TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = Keysight34972A
# Keysight34972A.dbd will be created and installed
DBD += Keysight34972A.dbd

# Keysight34972A.dbd will be made up from these files:
Keysight34972A_DBD += base.dbd

# Include dbd files from all support applications:
#Keysight34972A_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#Keysight34972A_LIBS += xxx

# Keysight34972A_registerRecordDeviceDriver.cpp derives from Keysight34972A.dbd
Keysight34972A_SRCS += Keysight34972A_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
Keysight34972A_SRCS_DEFAULT += Keysight34972AMain.cpp
Keysight34972A_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Keysight34972A_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Keysight34972A_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

