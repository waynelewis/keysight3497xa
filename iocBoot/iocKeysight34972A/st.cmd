#!../../bin/linux-x86_64/Keysight3497xA

## You may have to change Keysight3497xA to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/Keysight3497xAApp/Db")
epicsEnvSet("DEV_IP_ADDRESS", "192.168.1.41")
epicsEnvSet("DEV_ASYN_PORT", "MUX1")

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/Keysight3497xA.dbd"
Keysight3497xA_registerRecordDeviceDriver pdbbase

## Establish the connection to the 34972A
drvAsynIPPortConfigure("$(DEV_ASYN_PORT)",  "$(DEV_IP_ADDRESS):5025", 0, 0, 0)

## Load record instances
dbLoadRecords("db/Keysight3497xA.db","P=MMT:,R=MUX1:,PORT=$(DEV_ASYN_PORT)")

#asynSetTraceMask("$(DEV_ASYN_PORT)", -1, 0xFF)
#asynSetTraceIOMask("$(DEV_ASYN_PORT)", -1, 0xFF)

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=wlewis"
