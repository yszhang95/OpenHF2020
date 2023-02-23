# Import CMS python class definitions such as Process, Source, and EDProducer
import FWCore.ParameterSet.Config as cms

# Import contents of a file

# Set up a process, named SkimTree in this case
process = cms.Process("SkimTree")

# Configure the object that reads the input file
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring()
            )
