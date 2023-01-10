#!/usr/bin/env python3
# 1. count the number of Lambda_c from b-hadrons
# 2. count the number of Lambda_c from c-quary which is from B mesons

import ROOT
from ROOT import TChain, TFile, TFileCollection

ROOT.gErrorIgnoreLevel = ROOT.kFatal

tf = TFileCollection("tf", "", "NonPrompt_pT1p9_pPb_20220831.list")

chain = TChain("lambdacAna_mc/ParticleTree")

chain.AddFileInfoList(tf.GetList())

tree = ROOT.ParticleTreeMC(chain)

nentries = tree.GetEntriesFast()

particle = ROOT.Particle(4122)
Ks = ROOT.Particle(310)
Ks.selfConj(True)
Ks.longLived(True)
particle.addDaughter(Ks)
proton = ROOT.Particle (2212)
particle.addDaughter(proton)

nevt = 0
nlc = 0
nlb2lc = 0
nbd2lc = 0
nbu2lc = 0
nb2lc = 0
nb = 0
nc = 0

for ientry in range(nentries):
    # if ientry > 999:
    #     break
    jentry =  tree.LoadTree(ientry);
    if jentry < 0:
        break
    nevt = nevt + 1
    tree.GetEntry(ientry)
    # count the # of inclusive b->lambda_c
    for pdgId, momIdxs in zip(tree.gen_pdgId(), tree.gen_momIdx()):
        momId = None
        niter = 0
        if abs(pdgId) == 4122:
            nlc = nlc + 1
            # while not momIdxs.empty():
            while momIdxs:
                pdgId = momId
                momId = tree.gen_pdgId().at(momIdxs[0])
                momIdxs = tree.gen_momIdx().at(momIdxs[0])
                niter += 1
            if momId:
                momIdStr = "{}".format(abs(momId))
                # bottom
                if momIdStr[0] == "5":
                    nb = nb + 1
                # charm
                elif momIdStr[0] == "4":
                   nc = nc + 1
                # other partons
                elif pdgId:
                   pdgIdStr = "{}".format(abs(pdgId))
                   # other partons to b hadrons
                   if pdgIdStr[0] == "5":
                      nb = nb + 1
                      # print(momId, pdgId)
    # count the # of lambda_b -> lambda_c
    for pdgId, momIdxs in zip(tree.gen_pdgId(), tree.gen_momIdx()):
        momId = None
        niter = 0
        if abs(pdgId) == 4122:
            while momIdxs:
                momId = tree.gen_pdgId().at(momIdxs[0])
                momIdxs = tree.gen_momIdx().at(momIdxs[0])
                if abs(momId) == 5122:
                    nlb2lc = nlb2lc + 1
                    break
                elif abs(momId) == 511:
                    nbd2lc = nbd2lc + 1
                    break
                elif abs(momId) == 521:
                    nbu2lc = nbu2lc + 1
                    break
                elif abs(momId) == 5:
                    nb2lc = nb2lc + 1


print("nevt", nevt)
print("nlc", nlc)
print("nlb2lc", nlb2lc)
print("nbu2lc", nbu2lc)
print("nbd2lc", nbd2lc)
print("nb2lc", nb2lc)
print("nb", nb)
print("nc", nc)
print("nlb2lc + nbu2lc + nbd2lc + nb2lc", nlb2lc + nbu2lc + nbd2lc + nb2lc)
