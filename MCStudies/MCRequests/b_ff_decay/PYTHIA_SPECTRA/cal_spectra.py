#!/usr/env/bin python
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle


from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')
options.register ('outputRecord',
                  'record.txt',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "Name of output text containing counting information")
options.parseArguments()

flog_name = options.inputFiles[0]
flog_name = flog_name.replace(".root", ".log")

import re
total_xsec = 0
pat = re.compile(r"After filter: final cross section = (\d+.*) \+- (\d+.*) pb")
with open(flog_name) as flog:
    for l in flog.readlines():
        m = pat.match(l)
        if m:
            total_xsec = float(m.group(1))
            print("Total cross section =", total_xsec, "+/-", m.group(2), "pb")
            break


events = Events (options)
# create handle outside of loop
handle  = Handle ('std::vector<reco::GenParticle>')
# a label is just a tuple of strings that is initialized just
# like and edm::InputTag
label = ("genParticles")
ROOT.gROOT.SetBatch()        # don't pop up canvases

# loop over events
# print(events.size())
import array
ofile = ROOT.TFile(flog_name.replace(".log", "_spectra.root"), "recreate")
ymins = [-3.46, -2.46, -1.46, -0.46, 0.54, 1.54]
ymaxs = [-2.46, -1.46, -0.46, 0.54, 1.54, 2.54]
pts = array.array('d', [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 18, 20, 22, 24, 26, 28, 30])

hspectra = []
iy=0
for iy in range(len(ymins)):
    hspectra.append(ROOT.TH1D("spectra_y{:d}".format(iy),
      ";pT;dsigma/dpT (pb)", len(pts)-1, pts))

iev=0

for event in events:
    # if iev > 10:
    #     break
    iev += 1
    # use getByLabel, just like in cmsRun
    event.getByLabel (label, handle)
    # get the product
    genParticles = handle.product()
    pdgId = 511 if "Bd" in flog_name else 521
    for genParticle in genParticles:
        if abs(genParticle.pdgId()) == pdgId:
            for iy in range(len(ymins)):
                ymin = ymins[iy]
                ymax = ymaxs[iy]
                if genParticle.rapidity() < ymax and genParticle.rapidity() > ymin:
                    hspectra[iy].Fill(genParticle.pt())

entries = [h.GetEntries() for h in hspectra]
total_entries = sum(entries)
for h in hspectra:
    ofile.cd()
    n = h.GetEntries()
    h.Scale(1./h.Integral(0, 10000), "width")
    h.Scale(total_xsec/total_entries * n)
    h.Write()
