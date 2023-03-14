from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

# We want to put all the CRAB project directories from the tasks we submit here into one common directory.
# That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
from CRABClient.UserUtilities import config, getUsername
config = config()

config.section_("General")
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pPbSkimAndTree2016_D0Both_BDT_MC_ForLamC_cfg.py'

config.section_('Data')
#config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.publication = False
config.JobType.allowUndistributedCMSSW = True

config.section_('Site')
config.Data.ignoreLocality = False
# config.Site.whitelist = ['T2_US_*','T2_FR_*','T2_CH_CERN', 'T2_IT_*', 'T2_DE_*']
config.Site.storageSite = 'T2_CH_CERN'

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print "Failed submitting task: %s" % (hte.headers)
    except ClientException as cle:
        print "Failed submitting task: %s" % (cle)

#############################################################################################
## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
#############################################################################################

dataMap = {}
dataMap["PromptD0KPi_pPb"] = { "PD" : "/PromptD0_D0pT-1p2_pPb-EmbEPOS_8p16_Pythia8/pPb816Summer16DR-pPbEmb_80X_mcRun2_pA_v4-v1/AODSIM", "Units": 1, "Memory" : 2000, "RunTime" : 360, "dbs" : "global" }
dataMap["NonPromptD0KPi_pPb"] = { "PD" : "/NonPromptD0_D0pT-1p2_pPb-EmbEPOS_8p16_Pythia8/pPb816Summer16DR-pPbEmb_80X_mcRun2_pA_v4-v1/AODSIM", "Units": 1, "Memory" : 2000, "RunTime" : 360, "dbs" : "global" }
for key, val in dataMap.items():
    config.General.requestName = "MC_BDT_" +key + "_20230304"
    config.Data.inputDataset = val["PD"]
    config.Data.unitsPerJob = val["Units"]
    config.JobType.maxMemoryMB = val["Memory"]
    config.JobType.maxJobRuntimeMin = val["RunTime"]
    config.Data.outputDatasetTag = config.General.requestName
    config.Data.outLFNDirBase = '/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/D0Trees/MC/'
    config.Data.inputDBS = val["dbs"]
    print("Submitting CRAB job for: "+val["PD"])
    submit(config)
