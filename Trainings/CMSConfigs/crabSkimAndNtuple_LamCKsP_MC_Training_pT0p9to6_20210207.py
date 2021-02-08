import CRABClient
from WMCore.Configuration import Configuration
config = Configuration()

config.section_("General")
config.General.requestName = 'PA8TeV_pPb_LamCKsP0p9_LamCKsP_pT0p9to6_y1p0_MC_Training_20210207'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pPb2016SkimAndNtuple_LamCKsP_MC_Training_pT0p9to6_20210207.py'
config.JobType.allowUndistributedCMSSW = True

config.section_("Data")
config.Data.inputDataset = '/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/pPb816Summer16DR-pPbEmb_80X_mcRun2_pA_v4-v1/AODSIM'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.outputDatasetTag = config.General.requestName
config.Data.outLFNDirBase = '/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/'
config.Data.ignoreLocality = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
config.Site.whitelist = ['T1_US_*','T2_US_*','T1_FR_*','T2_FR_*','T2_CH_CERN']
