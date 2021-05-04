#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Do a test')
parser.add_argument('-i', dest='inFile', type=str, nargs=1, help='input file name')
parser.add_argument('-o', dest='outDir', type=str, nargs=1, help='output directory')
args = parser.parse_args()

outDir = args.outDir[0]
if '/' != outDir[-1]:
    outDir = outDir+'/'

import ROOT as r
f = r.TFile(args.inFile[0], "READ")
t = f.Get("lambdacAna_mc/ParticleNTuple")
tp = t.GetPlayer()

tp.SetScanRedirect(r.kTRUE)
tp.SetScanFileName(outDir+"cand.txt")
t.Scan("cand_charge:cand_mass:cand_pT:cand_eta:cand_phi:cand_charge:cand_decayLength3D:cand_decayLength2D:cand_angle3D:cand_angle2D:cand_decayLengthError3D:cand_decayLengthError2D:cand_pseudoDecayLengthError3D:cand_pseudoDecayLengthError2D:cand_dca:cand_massDau0:cand_massDau1:cand_pTDau0:cand_pTDau1:cand_etaDau0:cand_etaDau1:cand_phiDau0:cand_phiDau1")
tp.SetScanFileName(outDir+"cand_dau0.txt")
t.Scan("cand_dau0_charge:cand_dau0_mass:cand_dau0_pT:cand_dau0_eta:cand_dau0_phi:cand_dau0_decayLength3D:cand_dau0_decayLength2D:cand_dau0_angle3D:cand_dau0_angle2D:cand_dau0_decayLengthError3D:cand_dau0_decayLengthError2D:cand_dau0_pseudoDecayLengthError3D:cand_dau0_pseudoDecayLengthError2D:cand_dau0_dca:cand_dau0_massDau0:cand_dau0_massDau1:cand_dau0_pTDau0:cand_dau0_pTDau1:cand_dau0_etaDau0:cand_dau0_etaDau1:cand_dau0_phiDau0:cand_dau0_phiDau1")
tp.SetScanFileName(outDir+"cand_dau1.txt")
t.Scan("cand_dau1_charge:cand_dau1_pT:cand_dau1_eta:cand_dau1_phi:trk_dau1_isHP:trk_dau1_nHit:trk_dau1_nChi2:trk_dau1_xyDCASignificance:trk_dau1_zDCASignificance:trk_dau1_dEdx_dedxHarmonic2:trk_dau1_dEdx_dedxPixelHarmonic2")
tp.SetScanFileName(outDir+"cand_gdau0.txt")
t.Scan("cand_gdau0_charge:cand_gdau0_pT:cand_gdau0_eta:cand_gdau0_phi:trk_gdau0_isHP:trk_gdau0_nHit:trk_gdau0_nChi2:trk_gdau0_xyDCASignificance:trk_gdau0_zDCASignificance:trk_gdau0_dEdx_dedxHarmonic2:trk_gdau0_dEdx_dedxPixelHarmonic2")
tp.SetScanFileName(outDir+"cand_gdau1.txt")
t.Scan("cand_gdau1_charge:cand_gdau1_pT:cand_gdau1_eta:cand_gdau1_phi:trk_gdau1_isHP:trk_gdau1_nHit:trk_gdau1_nChi2:trk_gdau1_xyDCASignificance:trk_gdau1_zDCASignificance:trk_gdau1_dEdx_dedxHarmonic2:trk_gdau1_dEdx_dedxPixelHarmonic2")
