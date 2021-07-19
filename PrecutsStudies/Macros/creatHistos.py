#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description=
'''
This is a script in step 1.
It aims to create histograms used in step 2.
Requirements: RDataFrame and PyROOT on Python3
Test platform: CentOS7
               Python 3.6.8
               GCC 4.8.5 20150623 (Red Hat 4.8.5-44)
               ROOT 6.22/06
''', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('sList', metavar='sList', type=str, nargs='?',
                        help='list of MC signal files')
parser.add_argument('bList', metavar='bList', type=str, nargs='?',
                        help='list of data files used as background')

args = parser.parse_args()

sFList = open(args.sList)
sFs = ( f.strip('\n') for f in  sFList)
bFList = open(args.bList)
bFs = ( f.strip('\n') for f in  bFList)

import ROOT as r
pTCut ={
    "LamC pT:3-4GeV |y|<1" : "cand_pT<4 && cand_pT>3",
    "LamC pT:4-5GeV |y|<1" : "cand_pT<5 && cand_pT>4",
    }
dfSig = r.RDataFrame("lambdacAna_mc/ParticleNTuple", list(sFs)).Filter("cand_matchGEN")
dfBkg = r.RDataFrame("lambdacAna/ParticleNTuple", list(bFs))

#print (dfSig.GetColumnNames())
hists = {"s" : {}, "b" : {}}

ipt = 0
for (k, v) in pTCut.items():
  common = v + "&& cand_dau0_decayLengthError3D>0 && cand_decayLengthError3D>0"
  sCut = common
  bCut = common
  hists["s"][k+"piplus_dxySig"] = dfSig.Filter(sCut).Define("gdau1_dxySig", 'abs(trk_gdau1_xyDCASignificance)')\
      .Histo1D(("hSPt%d_piplus_dxySig" % ipt, "%s;Pi+ dxySig;Counts" % k, 50, 0, 5), "gdau1_dxySig")
  hists["s"][k+"piminus_dxySig"] = dfSig.Filter(sCut).Define("gdau0_dxySig", 'abs(trk_gdau0_xyDCASignificance)')\
      .Histo1D(("hSPt%d_piminus_dxySig" % ipt, "%s;Pi- dxySig;Counts" % k, 50, 0, 5), "gdau0_dxySig")
  hists["s"][k+"piplus_dzSig"] = dfSig.Filter(sCut).Define("gdau1_dzSig", 'abs(trk_gdau1_zDCASignificance)')\
      .Histo1D(("hSPt%d_piplus_dzSig" % ipt, "%s;Pi+ dzSig;Counts" % k, 50, 0, 5), "gdau1_dzSig")
  hists["s"][k+"piminus_dzSig"] = dfSig.Filter(sCut).Define("gdau0_dzSig", 'abs(trk_gdau0_zDCASignificance)')\
      .Histo1D(("hSPt%d_piminus_dzSig" % ipt, "%s;Pi- dzSig;Counts" % k, 50, 0, 5), "gdau0_dzSig")

  hists["s"][k+"dca_pipi"] = dfSig.Filter(sCut).Histo1D(
      ("hSPt%d_dca_pipi" % ipt, "%s;DCA between Pi-Pi+ (cm);Counts" % k, 50, 0, 2), "cand_dau0_dca")

  hists["s"][k+"vtxChi2_Ks"] = dfSig.Filter(sCut).Histo1D(
      ("hSPt%d_vtxChi2_Ks" % ipt, "%s;Ks Chi2/Ndf;Counts" % k, 50, 0, 10), "cand_dau0_vtxChi2")

  hists["s"][k+"dlSigKs"] = dfSig.Filter(sCut)\
      .Define("DLSig3DKs", "cand_dau0_decayLength3D/cand_dau0_decayLengthError3D")\
      .Histo1D(
      ("hSPt%d_dlSig_Ks" % ipt, "%s, Ks DL w.r.t. PV;3D Decay length significance;Counts" % k, 50, 0, 10),
      "DLSig3DKs")

  hists["s"][k+"angle3D_Ks"] = dfSig.Filter(sCut)\
      .Define("cosAlphaKs", "cos(cand_dau0_angle3D)")\
      .Histo1D(
      ("hSPt%d_cosAlpha3D_Ks" % ipt, "%s, Ks angle w.r.t. PV;cosine of 3D pointing angle;Counts" % k, 50, -1, 1),
      "cosAlphaKs")

  hists["s"][k+"mass_Ks"] = dfSig.Filter(sCut)\
      .Define("massDiff_Ks", "abs(cand_dau0_mass - 0.497614)")\
      .Histo1D(
      ("hSPt%d_massDiff_Ks" % ipt, "%s, Ks mass w.r.t. PDG Value;Mass_{Ks} (GeV);Counts" % k, 50, 0., 0.03),
      "massDiff_Ks")

  hists["b"][k+"piplus_dxySig"] = dfBkg.Filter(sCut).Define("gdau1_dxySig", 'abs(trk_gdau1_xyDCASignificance)')\
      .Histo1D(("hBPt%d_piplus_dxySig" % ipt, "%s;Pi+ dxySig;Counts" % k, 50, 0, 5), "gdau1_dxySig")
  hists["b"][k+"piminus_dxySig"] = dfBkg.Filter(sCut).Define("gdau0_dxySig", 'abs(trk_gdau0_xyDCASignificance)')\
      .Histo1D(("hBPt%d_piminus_dxySig" % ipt, "%s;Pi0 dxySig;Counts" % k, 50, 0, 5), "gdau0_dxySig")
  hists["b"][k+"piplus_dzSig"] = dfBkg.Filter(sCut).Define("gdau1_dzSig", 'abs(trk_gdau1_zDCASignificance)')\
      .Histo1D(("hBPt%d_piplus_dzSig" % ipt, "%s;Pi+ dzSig;Counts" % k, 50, 0, 5), "gdau1_dzSig")
  hists["b"][k+"pimius_dzSig"] = dfBkg.Filter(sCut).Define("gdau0_dzSig", 'abs(trk_gdau0_zDCASignificance)')\
      .Histo1D(("hBPt%d_piminus_dzSig" % ipt, "%s;Pi0 dzSig;Counts" % k, 50, 0, 5), "gdau0_dzSig")

  hists["b"][k+"dca_pipi"] = dfBkg.Filter(sCut).Histo1D(
      ("hBPt%d_dca_pipi" % ipt, "%s;DCA between Pi-Pi+ (cm);Counts" % k, 50, 0, 2), "cand_dau0_dca")

  hists["b"][k+"vtxChi2_Ks"] = dfBkg.Filter(sCut).Histo1D(
      ("hBPt%d_vtxChi2_Ks" % ipt, "%s;Ks Chi2/Ndf;Counts" % k, 50, 0, 10), "cand_dau0_vtxChi2")

  hists["b"][k+"dlSigKs"] = dfBkg.Filter(sCut)\
      .Define("DLSig3DKs", "cand_dau0_decayLength3D/cand_dau0_decayLengthError3D")\
      .Histo1D(
      ("hBPt%d_dlSig_Ks" % ipt, "%s, Ks DL w.r.t. PV;3D Decay length significance;Counts" % k, 50, 0, 10),
      "DLSig3DKs")

  hists["b"][k+"angle3D_Ks"] = dfBkg.Filter(sCut)\
      .Define("cosAlphaKs", "cos(cand_dau0_angle3D)")\
      .Histo1D(
      ("hBPt%d_cosAlpha3D_Ks" % ipt, "%s, Ks angle w.r.t. PV;cosine of 3D pointing angle;Counts" % k, 50, -1, 1),
      "cosAlphaKs")

  hists["b"][k+"mass_Ks"] = dfBkg.Filter(sCut)\
      .Define("massDiff_Ks", "abs(cand_dau0_mass - 0.497614)")\
      .Histo1D(
      ("hBPt%d_massDiff_Ks" % ipt, "%s, Ks mass w.r.t. PDG Value;Mass_{Ks} (GeV);Counts" % k, 50, 0., 0.03),
      "massDiff_Ks")
  ipt = ipt+1

fout = r.TFile.Open("outputHists.root", "recreate")
for (k,v) in hists["s"].items():
  v.Write()
for (k,v) in hists["b"].items():
  v.Write()
