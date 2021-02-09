#!/usr/bin/env python
import ROOT as r
fileName = '/eos/cms/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/PA8TeV_pPb_LamCKsP0p9_LamCKsP_pT0p9to6_y1p0_MC_Training_20210207/Merged_PA8TeV_pPb_LamCKsP_pT0p9to6_y1p0_MC_Training_20210207.root'
infile = r.TFile.Open(fileName)
t = infile.Get("lambdacAna_mc/ParticleNTuple")
outfile = r.TFile.Open(fileName.replace('.root', '_slim.root'), "recreate")
outfile.mkdir("lambdacAna_mc")
outfile.cd("lambdacAna_mc")
tt = t.CopyTree("cand_matchGEN")
tt.Write()
