import CRABClient
from CRABClient.UserUtilities import config

config = config()

config.General.requestName = 'test_condor'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.JobType.pluginName = 'Analysis'
config.JobType.scriptExe = 'run.sh'
config.JobType.psetName = 'dummy.py'

config.Data.inputDBS = 'phys03'
config.JobType.inputFiles = ['FrameworkJobReport.xml', 'OpenHF2020.tar.gz']
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.JobType.maxJobRuntimeMin = 10
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/yousen/test'
config.JobType.outputFiles = ['simpleoutput.txt']
config.Data.userInputFiles = [
'root://osg-se.sprace.org.br//store/user/yousen/RiceHIN//OpenHF/LambdaCKsPTree/PAMinimumBias1/ParticleTree_PAMinimumBias1_pPb_PALambdaC_KsP_20210823/210823_221246/0000/lambdacana_1.root',
'root://osg-se.sprace.org.br//store/user/yousen/RiceHIN//OpenHF/LambdaCKsPTree/PAMinimumBias1/ParticleTree_PAMinimumBias1_pPb_PALambdaC_KsP_20210823/210823_221246/0000/lambdacana_10.root'
]
config.Data.outputDatasetTag = 'test_condor'

config.Data.totalUnits = 1

config.Site.whitelist = ['T2_BR_SPRACE']
config.Site.storageSite = 'T2_CH_CERN'
