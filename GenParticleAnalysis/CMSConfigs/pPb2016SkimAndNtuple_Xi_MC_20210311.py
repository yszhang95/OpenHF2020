# Config for producing ntuple for training studies
import FWCore.ParameterSet.Config as cms
process = cms.Process("LamCKsTraining")

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
process.GlobalTag.globaltag = '80X_mcRun2_pA_v4'

# Define the input source
process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
       #'root://cmsxrootd.fnal.gov///store/himc/pPb816Summer16DR/ReggeGribovPartonMC_EposLHC_pPb_4080_4080_DataBS/AODSIM/MB_80X_mcRun2_pA_v4-v2/120000/003FC5B1-2009-E711-8AD2-0025904C6414.root',
       'root://cmsxrootd.fnal.gov///store/himc/pPb816Summer16DR/ReggeGribovPartonMC_EposLHC_pPb_4080_4080_DataBS/AODSIM/MB_80X_mcRun2_pA_v4-v2/120000/06A08BA8-4F10-E711-B5C0-00266CF3E3C4.root',
                                     ),
   inputCommands=cms.untracked.vstring('keep *')
)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Offline selections
# See https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/hfCoincFilter_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/python/collisionEventSelection_cff.py
# and https://github.com/davidlw/VertexCompositeAnalysis/blob/8_0_X/VertexCompositeProducer/test/pPbSkimAndTree2016_D0Both_BDT_NoPtY_cfg.py
# VertexCompositeAnalysis package is needed
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping)

# Event filter, used by end path and reconstruction path
process.eventFilter = cms.Sequence(
    process.colEvtSel
)
process.eventFilterStep = cms.Path(process.eventFilter)

# Candidate production
from VertexCompositeAnalysis.VertexCompositeProducer.generalParticles_cff import generalParticles
process.Lambda = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(3122),
    charge = cms.int32(0),
    doSwap = cms.bool(True),
    width = cms.double(0.07),

    fitAlgo = cms.vuint32([3]),

    preSelection = cms.string(""),
    pocaSelection = cms.string("pt >= 0.0 && abs(rapidity) < 2.4 && userFloat('dca') < 0.5"),
    postSelection = cms.string("userFloat('normChi2')<7"),
    preMassSelection = cms.string(""),
    finalSelection = cms.string("cos(userFloat('angle3D'))>0.999 && abs(userFloat('lVtxSig'))>5.0"),

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
                 finalSelection = cms.string(
                     "abs(userFloat('dxySig'))>4"
                     "&& abs(userFloat('dzSig'))>4")
                 ),
        cms.PSet(pdgId = cms.int32(2212), charge = cms.int32(+1),
                 selection = cms.string(
                     #"pt>0.4 && abs(eta)<2.4"
                     "pt>=0."
                     "&& quality('loose')"
                     "&& normalizedChi2<7.0"
                     "&& numberOfValidHits >=4"),
                 finalSelection = cms.string(
                     "abs(userFloat('dxySig'))>3"
                     "&& abs(userFloat('dzSig'))>3")
                 ),
    ]),
)

process.Xi = generalParticles.clone(
    vtxSortByTrkSize = cms.bool(False),
    pdgId = cms.int32(3312),
    charge = cms.int32(0),
    doSwap = cms.bool(False),
    width = cms.double(0.01),

    preSelection = cms.string("abs(charge)==1"),
    pocaSelection = cms.string("pt >= 0.0 && abs(rapidity) < 2.4"),
    postSelection = cms.string("userFloat('normChi2')<7"),
    preMassSelection = cms.string(""),
    finalSelection = cms.string("abs(userFloat('lVtxSig'))>3.0"),

    dEdxInputs = cms.vstring('dedxHarmonic2', 'dedxPixelHarmonic2'),

    # daughter information
    daughterInfo = cms.VPSet([
        cms.PSet(pdgId = cms.int32(3122), source = cms.InputTag('Lambda'), selection = cms.string("abs(userFloat('lVtxSig'))>12")),
        cms.PSet(pdgId = cms.int32(211),
                 selection = cms.string(
                     "pt>=0."
                     "&& quality('loose')"
                     "&& normalizedChi2<7.0"
                     "&& numberOfValidHits >=4"),
                 finalSelection = cms.string(
                     "abs(userFloat('dxySig'))>5"
                     "&& abs(userFloat('dzSig'))>5")
                 ),
    ]),
)

# Define the analysis steps
process.rereco_step = cms.Path(process.eventFilter * process.Lambda * process.Xi)

# Add the VertexComposite tree
from VertexCompositeAnalysis.VertexCompositeAnalyzer.particle_tree_cff import particleAna_mc
process.LambdaAna_mc = particleAna_mc.clone(
    recoParticles = cms.InputTag("Lambda"),
    genParticles = cms.untracked.InputTag("genParticles"),
    selectEvents = cms.string("eventFilterStep"),
    addSource    = cms.untracked.bool(False),
    genPdgId     = cms.untracked.vuint32([3122]),
    nTracksVMap = cms.untracked.InputTag(""),
    saveTree = cms.untracked.bool(True),
)
process.XiAna_mc = particleAna_mc.clone(
    recoParticles = cms.InputTag("Xi"),
    genParticles = cms.untracked.InputTag("genParticles"),
    selectEvents = cms.string("eventFilterStep"),
    addSource    = cms.untracked.bool(False),
    genPdgId     = cms.untracked.vuint32([3312]),
    nTracksVMap = cms.untracked.InputTag(""),
    saveTree = cms.untracked.bool(True),
)

# Define the output
process.TFileService = cms.Service("TFileService", fileName = cms.string('Xi_mc.root'))
process.pLambda = cms.EndPath(process.LambdaAna_mc)
process.pXi = cms.EndPath(process.XiAna_mc)

# Define the process schedule
process.schedule = cms.Schedule(
        process.eventFilterStep,
        process.rereco_step,
        process.pLambda,
        process.pXi,
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

process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
    ignoreTotal = cms.untracked.int32(1),
    )
