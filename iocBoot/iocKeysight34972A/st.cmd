#!../../bin/linux-x86_64/Keysight3497xA

## You may have to change Keysight3497xA to something else
## everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/Keysight3497xA.dbd"
Keysight3497xA_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=wlewis")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=wlewis"
