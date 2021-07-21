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
config.JobType.psetName = 'pPb2016SkimAndTree_LamCKsP_Data.py'
config.JobType.inputFiles = ['lumiData.csv', 'lumiDataHM.csv', 'lumiDataMB.csv']

config.section_('Data')
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
#config.Data.runRange = '285479-286496'
config.Data.publication = False
config.JobType.allowUndistributedCMSSW = True

config.section_('Site')
config.Data.ignoreLocality = True
config.Site.whitelist = ['T2_US_*','T2_FR_*','T2_CH_CERN', 'T2_IT_*', 'T2_DE_*']
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
#for i in range(1, 7):
for i in range(1, 2):
  #dataMap["PAHighMultiplicity%d_pPb" % i] = { "PD": "/PAHighMultiplicity%d/PARun2016C-PromptReco-v1/AOD" % i, "Units": 10, "Memory": 2000, "RunTime": 400, "LumiMask" : "Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt"}

  dataMap["PAHighMultiplicity%d_Pbp" % i] = { "PD": "/PAHighMultiplicity%d/PARun2016C-PromptReco-v1/AOD" % i, "Units": 10, "Memory": 2000, "RunTime": 400, "LumiMask" : "Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt"}

## Submit the PA PDs
for key, val in dataMap.items():
    config.General.requestName = 'ParticleTree_'+key+'_PALambdaC_KsP_20210706'
    config.Data.inputDataset = val["PD"]
    config.Data.lumiMask = val["LumiMask"]
    config.Data.unitsPerJob = val["Units"]
    config.JobType.maxMemoryMB = val["Memory"]
    config.JobType.maxJobRuntimeMin = val["RunTime"]
    config.Data.outputDatasetTag = config.General.requestName
    config.Data.outLFNDirBase = '/store/group/phys_heavyions/flowcorr/'
    print("Submitting CRAB job for: "+val["PD"])
    submit(config)
