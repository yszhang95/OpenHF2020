# Config for producing ntuple for training studies
import FWCore.ParameterSet.Config as cms
process = cms.Process("LamCKsPAna")

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
'root://cmsxrootd.fnal.gov///store/hidata/PARun2016C/PAHighMultiplicity1/AOD/PromptReco-v1/000/286/496/00000/4638778D-9ABD-E611-BBB8-FA163E114111.root'
                                     ),
   inputCommands=cms.untracked.vstring('keep *')
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))

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


# Offline selections
# See https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/hfCoincFilter_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/collisionEventSelection_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/test/pPbSkimAndTree2016_D0Both_BDT_NoPtY_cfg.py
# VertexCompositeAnalysis package is needed
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
#process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping * process.olvFilter_pPb8TeV_dz1p0)
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping)

# Event filter, used by end path and reconstruction path
process.eventFilter = cms.Sequence(
    process.hltFilter *
    process.colEvtSel
)

# Luminosity producer
process.lumiInfo = cms.EDProducer('LumiProducerFromBrilcalc',
                                  lumiFile = cms.string("./lumiData.csv"),
                                  throwIfNotFound = cms.bool(False),
                                  doBunchByBunch = cms.bool(False))
process.lumiInfoMB = process.lumiInfo.clone(lumiFile = cms.string("./lumiDataMB.csv"), isTrigger = cms.bool(True))
process.lumiInfoHM = process.lumiInfo.clone(lumiFile = cms.string("./lumiDataHM.csv"), isTrigger = cms.bool(True))
process.lumi_seq = cms.Sequence(process.lumiInfo + process.lumiInfoMB + process.lumiInfoHM)

process.eventFilterStep = cms.Path(process.eventFilter * process.lumi_seq)

# Candidate production
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles
process.kShort = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(310),
    charge = cms.int32(0),
    doSwap = cms.bool(False),
    width = cms.double(0.02),

    fitAlgo = cms.vuint32([3]),

    preSelection = cms.string(""),
    pocaSelection = cms.string("pt >= 0.0 && abs(rapidity) < 2.4 && userFloat('dca') < 1."),
    postSelection = cms.string("userFloat('normChi2')<7"),
    preMassSelection = cms.string(""),
    finalSelection = cms.string("cos(userFloat('angle3D'))>0.997 && abs(userFloat('lVtxSig'))>2.5"),

    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(-1),
                 selection = cms.string(
                     #"pt>0.4 && abs(eta)<2.4"
                     "pt>=0."
                     "&& quality('loose')"
                     "&& normalizedChi2<7.0"
                     "&& numberOfValidHits >=4"),
                 finalSelection = cms.string(""
                     "abs(userFloat('dxySig'))>1"
                     "&& abs(userFloat('dzSig'))>1"
                     )
                 ),
        cms.PSet(pdgId = cms.int32(211), charge = cms.int32(+1),
                 selection = cms.string(
                     #"pt>0.4 && abs(eta)<2.4"
                     "pt>=0."
                     "&& quality('loose')"
                     "&& normalizedChi2<7.0"
                     "&& numberOfValidHits >=4"),
                 finalSelection = cms.string(""
                     "abs(userFloat('dxySig'))>1"
                     "&& abs(userFloat('dzSig'))>1"
                     )
                 ),
    ]),
)
process.LambdaC = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(4122),
    doSwap = cms.bool(False),
    width = cms.double(0.22),
    preSelection = cms.string("abs(y)<2.4"),
    preMassSelection = cms.string("abs(charge)==1"),
    postSelection = cms.string("userFloat('vertexProb')>0.000001"),
    finalSelection = cms.string(''),

    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(310), source = cms.InputTag('kShort'), selection = cms.string('')),
        cms.PSet(pdgId = cms.int32(2212), #charge = cms.int32(+1),
          selection = cms.string("pt>0.3 && abs(eta)<2.4"
              "&& quality('highPurity') && ptError/pt<0.1"
              "&& normalizedChi2<7.0"
              "&& numberOfValidHits >=11"),
          finalSelection = cms.string("abs(userFloat('dzSig')) < 3 && abs(userFloat('dxySig')) < 3")),
    ]),
)

# Define the analysis steps
process.rereco_step = cms.Path(process.eventFilter * process.kShort * process.LambdaC)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna
process.lambdacAna = particleAna.clone(
    recoParticles = cms.InputTag("LambdaC"),
    selectEvents = cms.string("eventFilterStep"),
    addSource    = cms.untracked.bool(False),
    genPdgId     = cms.untracked.vuint32([4122, 310, 2212, 211]),
    nTracksVMap = cms.untracked.InputTag(""),
    saveTree = cms.untracked.bool(True),

    # trigger information
    triggerResults = cms.untracked.InputTag("TriggerResults::HLT"),
    triggerEvent   = cms.untracked.InputTag("hltTriggerSummaryAOD::HLT"),
    triggerInfo = cms.untracked.VPSet([
      cms.PSet(path = cms.string('HLT_PAFullTracks_Multiplicity120_v')), # High multiplicity
      cms.PSet(path = cms.string('HLT_PAFullTracks_Multiplicity150_v')), # High multiplicity
      cms.PSet(path = cms.string('HLT_PAFullTracks_Multiplicity185_part'), lumiInfo = cms.InputTag("lumiInfoHM", "brilcalc")), # High multiplicity
      cms.PSet(path = cms.string('HLT_PAFullTracks_Multiplicity250_v')), # High multiplicity
      cms.PSet(path = cms.string('HLT_PAL1MinimumBiasHF_OR_SinglePixelTrack_part'), lumiInfo = cms.InputTag("lumiInfoMB", "brilcalc")), # Minimum bias
      ]),

    #Filter info
	  eventFilterResults = cms.untracked.InputTag("TriggerResults"),
	  eventFilterNames = cms.untracked.vstring(
	      'Flag_colEvtSel',
	      'Flag_hfCoincFilter',
	      'Flag_primaryVertexFilterPA',
	      'Flag_NoScraping',
	      'Flag_pileupVertexFilterCut',
	      'Flag_pileupVertexFilterCutGplus'
	  ),

	  # centrality and event plane information
	  centralityBin = cms.untracked.InputTag("", ""),
	  centrality    = cms.untracked.InputTag(""),
	  eventPlane    = cms.untracked.InputTag("", ""),

	  # luminosity information
    lumiInfo    = cms.untracked.InputTag("lumiInfo", "brilcalc"),
    lumiScalers = cms.untracked.InputTag("scalersRawToDigi"),
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('lambdacana.root'))
process.p = cms.EndPath(process.lambdacAna)

# Define the process schedule
process.schedule = cms.Schedule(
        process.eventFilterStep,
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

#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#    ignoreTotal = cms.untracked.int32(1),
#    )
