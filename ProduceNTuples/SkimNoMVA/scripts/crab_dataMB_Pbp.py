from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

import CRABClient
from CRABClient.UserUtilities import config, getUsername

config = config()

# do it later
# config.General.requestName = 'data_MB1_pPb'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.scriptExe = 'run_Pbp.sh'
config.JobType.psetName = 'dummy.py'
config.JobType.inputFiles = ['FrameworkJobReport.xml', 'OpenHF2020.tar.gz', 'skimNTuple.C']
config.JobType.maxJobRuntimeMin = 60

config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 200
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/yousen/LamC_pPb2016'
config.JobType.outputFiles = ['output.root']
# do it later
# config.Data.userInputFiles = [ f.strip() for f in open("lists/dataMB1_pPb.list").readlines() ]
config.Data.outputDatasetTag = 'condor_dataMB_LamC'

config.Site.whitelist = ['T2_BR_SPRACE']
config.Site.storageSite = 'T2_CH_CERN'

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print ("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print ("Failed submitting task: %s" % (cle))

#############################################################################################
## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
#############################################################################################

dataMap = {}
#for i in range(1, 9):
for i in range(1, 21):
    dataMap["dataMB{}_Pbp".format(i)] = {
            "files" : [ l.strip() for l in open("lists/dataMB{}_Pbp.list".format(i)).readlines() ]
            }
## Submit the PA PDs
for key, val in dataMap.items():
    config.General.requestName = key
    config.Data.userInputFiles = val["files"]
    print("Submitting CRAB job for: {}".format(key))
    submit(config)
