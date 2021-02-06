# Config for producing ntuple for precuts studies
import FWCore.ParameterSet.Config as cms
process = cms.Process("LamCKsPPrecuts")

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.MessageLogger.cerr.FwkReport.reportEvery = 200
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v15'

# Define the input source
process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
       'root://cmsxrootd.fnal.gov//store/hidata/PARun2016C/PAHighMultiplicity1/AOD/PromptReco-v1/000/285/480/00000/8C5BFB5F-20AF-E611-9E09-02163E0144B0.root',
       'root://cmsxrootd.fnal.gov//store/hidata/PARun2016C/PAHighMultiplicity1/AOD/PromptReco-v1/000/285/480/00000/0C73EB2B-22AF-E611-85F1-02163E013657.root',
       'root://cmsxrootd.fnal.gov//store/hidata/PARun2016C/PAHighMultiplicity1/AOD/PromptReco-v1/000/285/480/00000/4C0D189A-1BAF-E611-B5CA-FA163EAF1F45.root'
                                     ),
   inputCommands=cms.untracked.vstring('keep *')
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))

# Trigger Selection
# Comment out for the timing being assuming running on secondary
# dataset with trigger bit selected already
# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    'HLT_PAFullTracks_Multiplicity120_v*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity150_v*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity185_part*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity250_v*', # High multiplicity
    'HLT_PAL1MinimumBiasHF_OR_SinglePixelTrack_part*', # Minimum bias
    ]

# Offlin selections
# See https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/hfCoincFilter_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/collisionEventSelection_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/test/pPbSkimAndTree2016_D0Both_BDT_NoPtY_cfg.py
# VertexCompositeAnalysis package is needed
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping * process.olvFilter_pPb8TeV_dz1p0)

# Event filter, used by end path and reconstruction path
process.eventFilter = cms.Sequence(
    process.hltFilter *
    process.colEvtSel
)
process.eventFilterStep = cms.Path(process.eventFilter)

# Ntrkoffline
process.load('VertexCompositeAnalysis.VertexCompositeProducer.ntrkUtils_cff')

# Candidate production
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles
process.kShort = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(310),
    charge = cms.int32(0),
    doSwap = cms.bool(False),
    width = cms.double(0.03),

    preSelection = cms.string(""),
    pocaSelection = cms.string("pt >= 0.0 && abs(rapidity) < 2.4"),
    postSelection = cms.string(""),
    preMassSelection = cms.string(""),
    finalSelection = cms.string( ""),

    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(-1),
              selection = cms.string(
              "pt>0.4 && abs(eta)<2.4"
              "&& quality('loose')"
              "&& normalizedChi2<7.0"
              "&& numberOfValidHits >=4")
            ),
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(+1),
              selection = cms.string(
              "pt>0.4 && abs(eta)<2.4"
              "&& quality('loose')"
              "&& normalizedChi2<7.0"
              "&& numberOfValidHits >=4")
            ),
    ]),
)
process.LambdaC = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(4122),
    doSwap = cms.bool(False),
    width = cms.double(0.2),
    preSelection = cms.string("pt<5.1 && pt > 2.9 && abs(y)<1"),
    preMassSelection = cms.string("abs(charge)==1"),
    finalSelection = cms.string(''),

    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(310), source = cms.InputTag('kShort'), selection = cms.string('')),
        cms.PSet(pdgId = cms.int32(2212), #charge = cms.int32(+1),
          selection = cms.string("pt>0.4 && abs(eta)<2.4"
              "&& quality('highPurity') && ptError/pt<0.1"
              "&& normalizedChi2<7.0"
              "&& numberOfValidHits >=11"),
          finalSelection = cms.string("abs(userFloat('dzSig')) < 3 && abs(userFloat('dxySig')) < 3")),
    ]),
)

# Define the analysis steps
process.rereco_step = cms.Path(process.eventFilter * process.nTracks * process.kShort * process.LambdaC)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.lambdacAna = particleAna.clone(
  recoParticles = cms.InputTag("LambdaC"),
  selectEvents = cms.string("eventFilterStep"),
  nTracksVMap = cms.untracked.InputTag('nTracks'),
  addSource    = cms.untracked.bool(False),
  saveTree = cms.untracked.bool(False)
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('lambdacana.root'))
process.p = cms.EndPath(process.lambdacAna)

# Define the process schedule
process.schedule = cms.Schedule(
        process.rereco_step,
        process.p
)

# Add the event selection filters, will be used by tree producer
process.Flag_colEvtSel = cms.Path(process.colEvtSel)
process.Flag_hfCoincFilter = cms.Path(process.hfCoincFilter)
process.Flag_primaryVertexFilterPA = cms.Path(process.primaryVertexFilterPA)
process.Flag_NoScraping = cms.Path(process.NoScraping)
process.Flag_pileupVertexFilterCut = cms.Path(process.olvFilter_pPb8TeV_dz1p0)
process.Flag_pileupVertexFilterCutGplus = cms.Path(process.pileUpFilter_pPb8TeV_Gplus)
eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_hfCoincFilter , process.Flag_primaryVertexFilterPA , process.Flag_NoScraping , process.Flag_pileupVertexFilterCut , process.Flag_pileupVertexFilterCutGplus ]
for P in eventFilterPaths:
    process.schedule.insert(0, P)
