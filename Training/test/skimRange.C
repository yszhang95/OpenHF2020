using namespace ROOT;

float dEdxRes (const float p, const float dedx)
{
  const float turn1 = 0.7;
  const float turn2 = 20;
  const float pars[] = {3.27790e+01, -2.27040e+00, 5.71871e-01, 2.29089e+01, -2.48301e+00};
  float mean;
  if (p<turn1) {
    mean = (pars[0]*exp(pars[1]*p)+pars[2]);
  }
  else {
    mean =
      pars[3]*exp(pars[4]*p)+pars[0]*exp(pars[1]*turn1)+pars[2]-pars[3]*exp(pars[4]*turn1);
  }
  return dedx-mean;
}


void skimRange(const std::string tree, const std::string file,
               const unsigned int begin, const unsigned int end,
               const unsigned int stride=1)
{

  RDataFrame df(tree, file);
  auto skimdf = df.Range(begin, end, stride)
    .Filter("cos(cand_dau0_angle3D) > 0.999 && cand_dau0_decayLength3D/cand_dau0_decayLengthError3D > 5. && cand_dau0_dca < 0.5")
    .Define("cand_dau1_p", "cand_pTDau1*cosh(cand_etaDau1)").Define("dau1_dEDxRes", dEdxRes, {"cand_dau1_p", "trk_dau1_dEdx_dedxHarmonic2"})
    .Define("cand_dau0_p", "cand_pTDau0*cosh(cand_etaDau0)");
  const auto extension = file.find(".root");
  string outfile = file;
  string suffix = "_slimmed_range" + std::to_string(begin) + "to" + std::to_string(end) + ".root";
  if (extension != std::string::npos) outfile.replace(extension, 5, suffix);
  //std::cout << outfile << std::endl;
  skimdf.Snapshot(tree, outfile,
      {"cand_mass", "cand_pT", "cand_y", "cand_eta", "cand_phi", "cand_charge",
      "cand_dauCosOpenAngle3D", "cand_vtxChi2",
      "trk_dau1_nStripHit", "trk_dau1_dEdx_dedxHarmonic2", "dau1_dEDxRes",
      "trk_dau1_zDCASignificance", "trk_dau1_xyDCASignificance",
      "cand_angle2D", "cand_angle3D",
      "cand_decayLength2D", "cand_decayLength3D",
      "cand_decayLengthError2D", "cand_decayLengthError3D",
      "cand_pTDau0", "cand_etaDau0", "cand_phiDau0", "cand_dau0_mass", "cand_dau0_p",
      "cand_pTDau1", "cand_etaDau1", "cand_phiDau1", "cand_dau1_mass", "cand_dau1_p"
      });
}

void skimRange()
{
  std::string trees[] = {
    "lambdacAna/ParticleNTuple",
    "lambdacAna/ParticleNTuple"
  };

  std::string files[] = {
    "dataHM1_pPb_training_3to4.root",
    "dataHM2_pPb_training_3to4.root"
  };

  for (size_t i=0; i<2; i++) {
    skimRange(trees[i], files[i], 1, 1000000);
    std::cout << "Done " << files[i] << std::endl;
  }
}

void skim(const std::string tree, const std::string file)
{

  RDataFrame df(tree, file);
  auto skimdf = df.Filter("cos(cand_dau0_angle3D) > 0.999 && cand_dau0_decayLength3D/cand_dau0_decayLengthError3D > 5. && cand_dau0_dca < 0.5")
    .Define("cand_dau1_p", "cand_pTDau1*cosh(cand_etaDau1)").Define("dau1_dEDxRes", dEdxRes, {"cand_dau1_p", "trk_dau1_dEdx_dedxHarmonic2"})
    .Define("cand_dau0_p", "cand_pTDau0*cosh(cand_etaDau0)");
  string outfile = gSystem->BaseName(file.c_str());
  const auto extension = outfile.find(".root");
  string suffix = "_slimmed.root";
  if (extension != std::string::npos) outfile.replace(extension, 5, suffix);
  //std::cout << outfile << std::endl;
  skimdf.Snapshot(tree, outfile,
      {"cand_mass", "cand_pT", "cand_y", "cand_eta", "cand_phi", "cand_charge",
      "cand_dauCosOpenAngle3D", "cand_vtxChi2",
      "trk_dau1_nStripHit", "trk_dau1_dEdx_dedxHarmonic2", "dau1_dEDxRes",
      "trk_dau1_zDCASignificance", "trk_dau1_xyDCASignificance",
      "cand_angle2D", "cand_angle3D",
      "cand_decayLength2D", "cand_decayLength3D",
      "cand_decayLengthError2D", "cand_decayLengthError3D",
      "cand_pTDau0", "cand_etaDau0", "cand_phiDau0", "cand_dau0_mass", "cand_dau0_p",
      "cand_pTDau1", "cand_etaDau1", "cand_phiDau1", "cand_dau1_mass", "cand_dau1_p"
      });
}

void skim()
{
  std::string trees[] = {
    "lambdacAna_mc/ParticleNTuple",
    "lambdacAna_mc/ParticleNTuple"
  };

  std::string files[] = {
    "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP.root",
    "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/MC_Pbp_lambdacAna_mc_AllEntries_LamCKsP.root"
  };

  for (size_t i=0; i<2; i++) {
    skim(trees[i], files[i]);
    std::cout << "Done " << files[i] << std::endl;
  }
}
