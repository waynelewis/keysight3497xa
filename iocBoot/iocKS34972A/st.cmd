#!../../bin/linux-x86_64/KS3497xAApp

## You may have to change KS3497xA to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/KS3497xAApp/Db")
epicsEnvSet("DEV_IP_ADDRESS", "192.168.1.41")
epicsEnvSet("DEV_ASYN_PORT", "KS3497XA")
epicsEnvSet("DEV_SUP_ASYN_PORT", "MUX1")

epicsEnvSet("PREFIX", "MMT:")
epicsEnvSet("DEVICE", "MUX1:")

epicsEnvSet("AUTOSAVE_PATH", "/usr/local/autosave/$(IOC)")

cd "$(TOP)"

## Register all support components
dbLoadDatabase "dbd/KS3497xAApp.dbd"
KS3497xAApp_registerRecordDeviceDriver pdbbase

## Establish the connection to the 34972A
drvAsynIPPortConfigure("$(DEV_ASYN_PORT)",  "$(DEV_IP_ADDRESS):5025", 0, 0, 0)

## Initialize the driver support code
initKS3497xA("$(DEV_SUP_ASYN_PORT)", "$(DEV_ASYN_PORT)", 1000)

## Load record instances
dbLoadRecords("db/KS3497xA.db","P=$(PREFIX),R=$(DEVICE),PORT=$(DEV_SUP_ASYN_PORT)")
dbLoadRecords("db/KS3497xA_channels.db","P=$(PREFIX),R=$(DEVICE),PORT=$(DEV_SUP_ASYN_PORT)")
dbLoadRecords("db/asynRecord.db","P=$(PREFIX),R=$(DEVICE)Asyn,PORT=$(DEV_ASYN_PORT),OMAX=80,IMAX=80,ADDR=0")

#asynSetTraceMask("$(DEV_ASYN_PORT)", -1, 0xFF)
#asynSetTraceMask("$(DEV_SUP_ASYN_PORT)", -1, 0x11)
asynSetTraceMask("$(DEV_SUP_ASYN_PORT)", -1, 0x01)
#asynSetTraceIOMask("$(DEV_ASYN_PORT)", -1, 0xFF)

# Autosave setup
save_restoreSet_Debug(0)
save_restoreSet_IncompleteSetsOk(1)
save_restoreSet_DatedBackupFiles(1)

set_savefile_path("$(AUTOSAVE_PATH)","/save")
set_requestfile_path("$(AUTOSAVE_PATH)","/req")

system("install -m 755 -d $(AUTOSAVE_PATH)/save")
system("install -m 755 -d $(AUTOSAVE_PATH)/req")

set_pass0_restoreFile("info_positions.sav")
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")

dbLoadRecords("$(AUTOSAVE)/asApp/Db/save_restoreStatus.db","P=$(PREFIX)")
save_restoreSet_status_prefix("$(PREFIX)")

iocInit()

# Autosave post-iocInit commands
## more autosave/restore machinery
cd $(AUTOSAVE_PATH)/req
makeAutosaveFiles()
create_monitor_set("info_positions.req", 5 , "")
create_monitor_set("info_settings.req", 15 , "")

cd $(TOP)
