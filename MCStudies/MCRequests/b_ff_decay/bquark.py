import FWCore.ParameterSet.Config as cms

from Configuration.Generator.Pythia8CommonSettings_cfi import *
from Configuration.Generator.MCTunes2017.PythiaCP5Settings_cfi import *
from GeneratorInterface.EvtGenInterface.EvtGenSetting_cff import *

generator = cms.EDFilter("Pythia8GeneratorFilter",
        pythiaHepMCVerbosity = cms.untracked.bool(False),
        maxEventsToPrint = cms.untracked.int32(0),
        pythiaPylistVerbosity = cms.untracked.int32(0),
        comEnergy = cms.double(13000.0),

        ExternalDecays = cms.PSet(
            EvtGen130 = cms.untracked.PSet(
                operates_on_particles = cms.vint32(), # 0 (zero) means default list (hardcoded)
                decay_table = cms.string('GeneratorInterface/EvtGenInterface/data/DECAY_2014_NOLONGLIFE.DEC'),
                particle_property_file = cms.FileInPath('GeneratorInterface/EvtGenInterface/data/evtLbJpsipK_2014.pdl'),
                #user_decay_file =  cms.vstring('GeneratorInterface/EvtGenInterface/data/LambdaB_JPsipK.dec'),
                convertPythiaCodes = cms.untracked.bool(False),
                list_forced_decays = cms.vstring(),
                ),
            parameterSets = cms.vstring('EvtGen130')
            ),

        PythiaParameters = cms.PSet(pythia8CommonSettingsBlock,
            pythia8CP5SettingsBlock,
            processParameters = cms.vstring(
                                            '443:onMode = off',
                                            '443:onIfAny = -13 13',
                                            'SoftQCD:nonDiffractive = on',
                                            'PTFilter:filter = on', # this turn on the filter
                                            'PTFilter:quarkToFilter = 5', # PDG id of q quark (can be any other)
                                            'PTFilter:scaleToFilter = 1.0'),
            parameterSets = cms.vstring('pythia8CommonSettings',
                'pythia8CP5Settings',
                'processParameters',
                )

            )
         )

generator.PythiaParameters.processParameters.extend(EvtGenExtraParticles)

# Filters

bfilter = cms.EDFilter(
        "PythiaFilter",
        MaxEta = cms.untracked.double(9999.),
        MinEta = cms.untracked.double(-9999.),
        ParticleID = cms.untracked.int32(5)
        )

ProductionFilterSequence = cms.Sequence(generator*bfilter)
