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
config.JobType.psetName = 'pPbSkimAndTree2016_D0Both_NoMVA_ForLamC_cfg.py'

config.section_('Data')
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
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
for i in [1]:
  dataMap["PAMinimumBias%d_pPb" % i] = { "PD": "/PAMinimumBias%d/PARun2016C-PromptReco-v1/AOD" % i, "Units": 10, "Memory": 2000, "RunTime": 300, "LumiMask" : "Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt"}
  dataMap["PAMinimumBias%d_Pbp" % i] = { "PD": "/PAMinimumBias%d/PARun2016C-PromptReco-v1/AOD" % i, "Units": 10, "Memory": 2000, "RunTime": 300, "LumiMask" : "Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt"}
## Submit the PA PDs
for key, val in dataMap.items():
    config.General.requestName = 'D0_NoMVA_'+key + '_MB_20230205'
    config.Data.inputDataset = val["PD"]
    config.Data.lumiMask = val["LumiMask"]
    config.Data.unitsPerJob = val["Units"]
    config.JobType.maxMemoryMB = val["Memory"]
    config.JobType.maxJobRuntimeMin = val["RunTime"]
    config.Data.outputDatasetTag = config.General.requestName
    config.Data.outLFNDirBase = '/store/group/phys_heavyions/ec/yousen/OpenHF2020Storage/D0Trees/'
    print("Submitting CRAB job for: "+val["PD"])
    submit(config)
