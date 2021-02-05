#include "TChain.h"
#include "TFile.h"
#include "THashList.h"
#include "TH1D.h"
#include "TFileCollection.h"
#include "TString.h"

//#include "../../Utilities/TreeReader/ParticleTree.hh"
#include "ParticleTree.hh"

#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/Vector4Dfwd.h"
#include "TMath.h"

#include <iostream>
#include <memory>
#include <cmath>

// decayL3D/2D for lambda C and Ks
// alpha3D/2D for lambda C and Ks
// track xy and z DCA
// missing DCA between two daus

const float massLambda = 1.115683;
const float massPion = 0.13957;
const float massProton = 0.93827;
const float massLamC = 2.2865;

template <typename T>
int findBin(const T& val, const std::vector<T>& min, const std::vector<T>& max)
{
  if(min.size() != max.size()) return -1;
  for(unsigned int ibin=0; ibin<min.size(); ibin++){
    if(val < max[ibin] && val >= min[ibin]) return ibin;
  }
  return -1;
}

void fillVarHists(const std::string& inputFileList, const std::vector<float>& ptBinLowEdges, const std::vector<float>& ptBinHighEdges, const double absYMax)
{
  TH1::SetDefaultSumw2(kTRUE);
  const int nPt = ptBinHighEdges.size() == ptBinLowEdges.size() ? ptBinLowEdges.size() : -1;

  if(nPt < 0) {
    std::cerr << "bin number of low edges is uneven to high edges" << std::endl;
    return;
  }

  std::map<std::string, int> particleType;
  particleType["lamc"] = 4122;
  particleType["ks"] = 310; 
  particleType["proton"] = 2212;

  typedef ROOT::Math::PtEtaPhiM4D<float> LorentzPolarVectorF;

  typedef std::map<std::string, std::unique_ptr<TH1D>> MapStrTH1DPtr;
  typedef std::unique_ptr<TH1D> TH1DPtr;
  typedef std::map<int, std::string> MapIntStr;
  typedef std::vector<std::string> VecStr;
  typedef std::vector<MapStrTH1DPtr> VecPtrs;

  VecPtrs lamcptrs(nPt), ksptrs(nPt);

  const VecStr composite_vars = {"decayL3D", "decayL2D", "alpha3D", "alph2D", "vtxProb", "decayL3DSig", "decayL2DSig"};
  const VecStr trk_vars = {"xyDCASig", "zDCASig", "dedx"};

  const VecStr types = {"signal", "unmatch"};

  // histograms
  for(int ipt=0; ipt<nPt; ipt++){
    lamcptrs.at(ipt)["decayL3Dgen"] = TH1DPtr(new TH1D(Form("hlamcdecayL3Dgen_pt%d", ipt), 
          Form("decayL3Dgen_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
    lamcptrs.at(ipt)["decayL2Dgen"] = TH1DPtr(new TH1D(Form("hlamcdecayL2Dgen_pt%d", ipt), 
          Form("decayL2Dgen_lamcpt%.1fto%.1f;distance (cm);Events/0.01(cm)", ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));

    lamcptrs.at(ipt)["alpha3Dgen"] = TH1DPtr(new TH1D(Form("hlamcalpha3Dgen_pt%d", ipt), 
          Form("alpha3Dgen_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
    lamcptrs.at(ipt)["alpha2Dgen"] = TH1DPtr(new TH1D(Form("hlamcalpha2Dgen_pt%d", ipt), 
          Form("alpha2Dgen_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));

    /*
    lamcptrs.at(ipt)["decayL3Dsignal"] = TH1DPtr(new TH1D(Form("hlamcdecayL3Dsignal_pt%d", ipt), 
          Form("decayL3Dsignal_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
    lamcptrs.at(ipt)["decayL2Dsignal"] = TH1DPtr(new TH1D(Form("hlamcdecayL2Dsignal_pt%d", ipt), 
          Form("decayL2Dsignal_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));

    lamcptrs.at(ipt)["decayL3DSigsignal"] = TH1DPtr(new TH1D(Form("hlamcdecayL3DSigsignal_pt%d", ipt), 
          Form("decayL3DSigsignal_lamcpt%.1fto%.1f;significance;Events/0.1", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));
    lamcptrs.at(ipt)["decayL2DSigsignal"] = TH1DPtr(new TH1D(Form("hlamcdecayL2DSigsignal_pt%d", ipt), 
          Form("decayL2DSigsignal_lamcpt%.1fto%.1f;significance;Events/0.1", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));

    lamcptrs.at(ipt)["alpha3Dsignal"] = TH1DPtr(new TH1D(Form("hlamcalpha3Dsignal_pt%d", ipt), 
          Form("alpha3Dsignal_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
    lamcptrs.at(ipt)["alpha2Dsignal"] = TH1DPtr(new TH1D(Form("hlamcalpha2Dsignal_pt%d", ipt), 
          Form("alpha2Dsignal_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));

    lamcptrs.at(ipt)["vtxProbsignal"] = TH1DPtr(new TH1D(Form("hlamcvtxProbsignal_pt%d", ipt), 
          Form("vtxProb_lamcpt%.1fto%.1f;vtxProb;Events/0.025",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, 0, 1.));

    lamcptrs.at(ipt)["decayL3Dunmatch"] = TH1DPtr(new TH1D(Form("hlamcdecayL3Dunmatch_pt%d", ipt), 
          Form("decayL3Dunmatch_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
    lamcptrs.at(ipt)["decayL2Dunmatch"] = TH1DPtr(new TH1D(Form("hlamcdecayL2Dunmatch_pt%d", ipt), 
          Form("decayL2Dunmatch_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));

    lamcptrs.at(ipt)["decayL3DSigunmatch"] = TH1DPtr(new TH1D(Form("hlamcdecayL3DSigunmatch_pt%d", ipt), 
          Form("decayL3DSigunmatch_lamcpt%.1fto%.1f;significance;Events/0.1", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));
    lamcptrs.at(ipt)["decayL2DSigunmatch"] = TH1DPtr(new TH1D(Form("hlamcdecayL2DSigunmatch_pt%d", ipt), 
          Form("decayL2DSigunmatch_lamcpt%.1fto%.1f;significance;Events/0.1", 
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));

    lamcptrs.at(ipt)["alpha3Dunmatch"] = TH1DPtr(new TH1D(Form("hlamcalpha3Dunmatch_pt%d", ipt), 
          Form("alpha3Dunmatch_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
    lamcptrs.at(ipt)["alpha2Dunmatch"] = TH1DPtr(new TH1D(Form("hlamcalpha2Dunmatch_pt%d", ipt), 
          Form("alpha2Dunmatch_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));

    lamcptrs.at(ipt)["vtxProbunmatch"] = TH1DPtr(new TH1D(Form("hlamcvtxProbunmatch_pt%d", ipt), 
          Form("vtxProb_lamcpt%.1fto%.1f;vtxProb;Events/0.025",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, 0, 1.));
            */

    for(const auto& type : types){
      lamcptrs.at(ipt)["decayL3D"+type] = TH1DPtr(new TH1D(Form("hlamcdecayL3D%s_pt%d", type.c_str(), ipt), 
            Form("decayL3D_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
      lamcptrs.at(ipt)["decayL2D"+type] = TH1DPtr(new TH1D(Form("hlamcdecayL2D%s_pt%d", type.c_str(), ipt), 
            Form("decayL2D_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));
  
      lamcptrs.at(ipt)["decayL3DSig"+type] = TH1DPtr(new TH1D(Form("hlamcdecayL3DSig%s_pt%d", type.c_str(), ipt), 
            Form("decayL3DSig_lamcpt%.1fto%.1f;significance;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));
      lamcptrs.at(ipt)["decayL2DSig"+type] = TH1DPtr(new TH1D(Form("hlamcdecayL2DSig%s_pt%d", type.c_str(), ipt), 
            Form("decayL2DSig_lamcpt%.1fto%.1f;significance;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 8));
  
      lamcptrs.at(ipt)["alpha3D"+type] = TH1DPtr(new TH1D(Form("hlamcalpha3D%s_pt%d", type.c_str(), ipt), 
            Form("alpha3D_lamcpt%.1fto%.1f;pointing angle;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
      lamcptrs.at(ipt)["alpha2D"+type] = TH1DPtr(new TH1D(Form("hlamcalpha2D%s_pt%d", type.c_str(), ipt), 
            Form("alpha2D_lamcpt%.1fto%.1f;pointing angle;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
  
      lamcptrs.at(ipt)["vtxProb"+type] = TH1DPtr(new TH1D(Form("hlamcvtxProb%s_pt%d", type.c_str(), ipt), 
            Form("vtxProb_lamcpt%.1fto%.1f;vtxProb;Events/0.025", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, 0, 1.));

      lamcptrs.at(ipt)["pseudoDL"+type] = TH1DPtr(new TH1D(Form("hlamcpseudoDL%s_pt%d", type.c_str(), ipt), 
            Form("pseudoDL_lamcpt%.1fto%.1f;pseudo proper decay length (cm);Events/0.03cm", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, -0.8, 1.6));
      lamcptrs.at(ipt)["dcaToPV"+type] = TH1DPtr(new TH1D(Form("hlamcdcaToPV%s_pt%d", type.c_str(), ipt), 
            Form("dcaToPV_lamcpt%.1fto%.1f;DCA (cm);Events/0.002cm", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 60, 0, 0.12));

      lamcptrs.at(ipt)["y"+type] = TH1DPtr(new TH1D(Form("hlamcy%s_pt%d", type.c_str(), ipt), 
            Form("y_lamcpt%.1fto%.1f;y;Events/0.05", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 48, -1.2, 1.2));
    }

    ksptrs.at(ipt)["decayL3Dgen"] = TH1DPtr(new TH1D(Form("hksdecayL3Dgen_pt%d", ipt), 
          Form("decayL3Dgen_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
    ksptrs.at(ipt)["decayL2Dgen"] = TH1DPtr(new TH1D(Form("hksdecayL2Dgen_pt%d", ipt), 
          Form("decayL2Dgen_lamcpt%.1fto%.1f;distance (cm);Events/0.01(cm)", ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));

    ksptrs.at(ipt)["alpha3Dgen"] = TH1DPtr(new TH1D(Form("hksalpha3Dgen_pt%d", ipt), 
          Form("alpha3Dgen_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
    ksptrs.at(ipt)["alpha2Dgen"] = TH1DPtr(new TH1D(Form("hksalpha2Dgen_pt%d", ipt), 
          Form("alpha2Dgen_lamcpt%.1fto%.1f;pointing angle;Events/0.1",
            ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));

    for(const auto& type : types){
      ksptrs.at(ipt)["decayL3D"+type] = TH1DPtr(new TH1D(Form("hksdecayL3D%s_pt%d", type.c_str(), ipt), 
            Form("decayL3D_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 1.6));
      ksptrs.at(ipt)["decayL2D"+type] = TH1DPtr(new TH1D(Form("hksdecayL2D%s_pt%d", type.c_str(), ipt), 
            Form("decayL2D_lamcpt%.1fto%.1f;distance (cm);Events/0.02(cm)", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 80, 0, 0.8));
  
      ksptrs.at(ipt)["decayL3DSig"+type] = TH1DPtr(new TH1D(Form("hksdecayL3DSig%s_pt%d", type.c_str(), ipt), 
            Form("decayL3DSig_lamcpt%.1fto%.1f;significance;Events/0.1667", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 90, 0, 15));
      ksptrs.at(ipt)["decayL2DSig"+type] = TH1DPtr(new TH1D(Form("hksdecayL2DSig%s_pt%d", type.c_str(), ipt), 
            Form("decayL2DSig_lamcpt%.1fto%.1f;significance;Events/0.1667", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 90, 0, 15));
  
      ksptrs.at(ipt)["alpha3D"+type] = TH1DPtr(new TH1D(Form("hksalpha3D%s_pt%d", type.c_str(), ipt), 
            Form("alpha3D_lamcpt%.1fto%.1f;pointing angle;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
      ksptrs.at(ipt)["alpha2D"+type] = TH1DPtr(new TH1D(Form("hksalpha2D%s_pt%d", type.c_str(), ipt), 
            Form("alpha2D_lamcpt%.1fto%.1f;pointing angle;Events/0.1", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, -0.2, 3.8));
  
      ksptrs.at(ipt)["vtxProb"+type] = TH1DPtr(new TH1D(Form("hksvtxProb%s_pt%d", type.c_str(), ipt), 
            Form("vtxProb_lamcpt%.1fto%.1f;vtxProb;Events/0.025", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, 0, 1.));

      ksptrs.at(ipt)["dca"+type] = TH1DPtr(new TH1D(Form("hksdca%s_pt%d", type.c_str(), ipt), 
            Form("dca_lamcpt%.1fto%.1f;dca;Events/0.025", 
              ptBinLowEdges.at(ipt), ptBinHighEdges.at(ipt)), 40, 0, 1.));
    }
  } // pt end

  MapStrTH1DPtr trackptrs;
  trackptrs["dxy_pi_in_ks_lamc_signal"] = TH1DPtr(new TH1D(Form("hdxy_pi_in_ks_lamc_signal"), Form("dxy_pi_in_ks_lamc;dxy;Events/0.1"), 100, -5, 5));
  trackptrs["dz_pi_in_ks_lamc_signal"] = TH1DPtr(new TH1D(Form("hdz_pi_in_ks_lamc_signal"), Form("dz_pi_in_ks_lamc;dz;Events/0.1"), 100, -5, 5));
  trackptrs["dxy_p_in_lamc_signal"] = TH1DPtr(new TH1D(Form("hdxy_p_in_lamc_signal"), Form("dxy_p_in_lamc;dxy;Events/0.1"), 100, -5, 5));
  trackptrs["dz_p_in_lamc_signal"] = TH1DPtr(new TH1D(Form("hdz_p_in_lamc_signal"), Form("dz_p_in_lamc;dz;Events/0.1"), 100, -5, 5));
  //trackptrs["dedx_p_in_lamc_signal"] = TH1DPtr(new TH1D(Form("hdedx_p_in_lamc_signal"), Form("dxy_p_in_lamc;dxy;Events/0.1"), 50, 0, 5));
  //
  trackptrs["dxy_pi_in_ks_lamc_unmatch"] = TH1DPtr(new TH1D(Form("hdxy_pi_in_ks_lamc_unmatch"), Form("dxy_pi_in_ks_lamc;dxy;Events/0.1"), 100, -5, 5));
  trackptrs["dz_pi_in_ks_lamc_unmatch"] = TH1DPtr(new TH1D(Form("hdz_pi_in_ks_lamc_unmatch"), Form("dz_pi_in_ks_lamc;dz;Events/0.1"), 100, -5, 5));
  trackptrs["dxy_p_in_lamc_unmatch"] = TH1DPtr(new TH1D(Form("hdxy_p_in_lamc_unmatch"), Form("dxy_p_in_lamc;dxy;Events/0.1"), 100, -5, 5));
  trackptrs["dz_p_in_lamc_unmatch"] = TH1DPtr(new TH1D(Form("hdz_p_in_lamc_unmatch"), Form("dz_p_in_lamc;dz;Events/0.1"), 100, -5, 5));



  std::cout << "initialized ptrs" << std::endl;
  
  TChain lamc("lambdacAna_mc/ParticleTree");
  TFileCollection fc ("fc", "the list of input files", inputFileList.c_str());
  lamc.AddFileInfoList(fc.GetList()); 

  ParticleTree tree(&lamc);
  //std::cout << tree.GetEntriesFast() << std::endl;

  std::cout << "start to read files" << std::endl;

  Long64_t nentries = tree.GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;

  int ifile=0;
  for (Long64_t jentry=0; jentry<nentries; jentry++) {
    Long64_t ientry = tree.LoadTree(jentry);
    if (ientry < 0) break;
    nb = tree.GetEntry(jentry);   nbytes += nb;

    for(size_t i=0; i < (tree.cand_mass->size()); i++){
      // lamc candidates
      if(abs((*tree.cand_pdgId)[i]) == particleType.at("lamc") && static_cast<unsigned short>((*tree.cand_status)[i]) == 3){
        std::unique_ptr<ROOT::Math::PtEtaPhiMVector> pi1p4;
        std::unique_ptr<ROOT::Math::PtEtaPhiMVector> pi2p4;
        size_t nDau = (*tree.cand_dauIdx).size();

        int ksIdx = -1;
        int pIdx = -1;
        char ksDauCharge[2];
        int grandDauIdx1 = -1;
        int grandDauIdx2 = -1;

        // reject lambdas mislabelled as kss
        for(unsigned int idau=0; idau<nDau; ++idau){
          auto dauIdx = (*tree.cand_dauIdx)[i][idau];
          if(std::abs((*tree.cand_pdgId)[dauIdx]) == 310){
            ksIdx = dauIdx;
            pIdx = (dauIdx+1) % 2;
  
            grandDauIdx1 = (*tree.cand_dauIdx)[dauIdx][0];
            grandDauIdx2 = (*tree.cand_dauIdx)[dauIdx][1];
  
            const float pt1 = (*tree.cand_pT)[grandDauIdx1];
            const float eta1 = (*tree.cand_eta)[grandDauIdx1];
            const float phi1 = (*tree.cand_phi)[grandDauIdx1];
            ksDauCharge[0] = (*tree.cand_charge)[grandDauIdx1];
  
            const float pt2 = (*tree.cand_pT)[grandDauIdx2];
            const float eta2 = (*tree.cand_eta)[grandDauIdx2];
            const float phi2 = (*tree.cand_phi)[grandDauIdx2];
            ksDauCharge[1] = (*tree.cand_charge)[grandDauIdx2];
  
            pi1p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt1, eta1, phi1, massPion));
            pi2p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt2, eta2, phi2, massPion));
  
            break;
          }
        }

        //std::cout << (*pi1p4 + *pi2p4).M()<< "  " << (*tree.cand_mass)[ksIdx] << std::endl;
        
        if(pi1p4->P() > pi2p4->P()) {
          pi1p4->SetM(massProton);
        } else {
          pi2p4->SetM(massProton);
        }
  
        double newCandM = (*pi1p4 + *pi2p4).M();
        if( std::abs(newCandM-massLambda) < 0.02) continue;

        if(ksIdx < 0) continue;
        if(pIdx < 0) continue;
        if(grandDauIdx1 < 0) continue;
        if(grandDauIdx2 < 0) continue;

        const float pt = (*tree.cand_pT)[i];
        auto ipt = findBin(pt, ptBinLowEdges, ptBinHighEdges);
        if(ipt < 0) continue;

        const float y = (*tree.cand_y)[i];
        if(abs(y) > absYMax) continue;

        const double dcaToPV = (*tree.cand_decayLength3D)[i] * std::sin((*tree.cand_angle3D)[i]);
        const double pseudoDL = (*tree.cand_decayLength3D)[i] * std::cos((*tree.cand_angle3D)[i]) * massLamC/ ( 
            (*tree.cand_pT)[i]* std::cosh((*tree.cand_eta)[i]));

        // match
        if((*tree.cand_matchGEN)[i] && !((*tree.cand_isSwap)[i]) ){
          /*
          std::cout << "mass " << (*tree.cand_mass)[i] << std::endl;
          std::cout << "matchGEN " << (*tree.cand_matchGEN)[i] << std::endl;
          std::cout << "isSwap " << (*tree.cand_isSwap)[i] << std::endl;
          std::cout << "pdgId " << (*tree.cand_pdgId)[i] << std::endl;
          std::cout << "status " << static_cast<unsigned short>((*tree.cand_status)[i]) << std::endl;
          */

          std::string type = "signal";
          lamcptrs.at(ipt)["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[i]);
          lamcptrs.at(ipt)["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[i]);
          lamcptrs.at(ipt)["decayL3DSig"+type]->Fill((*tree.cand_decayLength3D)[i]/(*tree.cand_decayLengthError3D)[i]);
          lamcptrs.at(ipt)["decayL2DSig"+type]->Fill((*tree.cand_decayLength2D)[i]/(*tree.cand_decayLengthError2D)[i]);
          lamcptrs.at(ipt)["alpha3D"+type]->Fill((*tree.cand_angle3D)[i]);
          lamcptrs.at(ipt)["alpha2D"+type]->Fill((*tree.cand_angle2D)[i]);
          lamcptrs.at(ipt)["vtxProb"+type]->Fill((*tree.cand_vtxProb)[i]);

          lamcptrs.at(ipt)["dcaToPV"+type]->Fill(dcaToPV);
          lamcptrs.at(ipt)["pseudoDL"+type]->Fill(pseudoDL);

          lamcptrs.at(ipt)["y"+type]->Fill(y);

          auto gen_idx = (*tree.cand_genIdx)[i];
          lamcptrs.at(ipt)["decayL3Dgen"]->Fill( (*tree.gen_decayLength3D)[gen_idx] );
          lamcptrs.at(ipt)["alpha3Dgen"]->Fill( (*tree.gen_angle3D)[gen_idx] );
          lamcptrs.at(ipt)["decayL2Dgen"]->Fill( (*tree.gen_decayLength2D)[gen_idx] );
          lamcptrs.at(ipt)["alpha2Dgen"]->Fill( (*tree.gen_angle2D)[gen_idx] );

          ksptrs.at(ipt)["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[ksIdx]);
          ksptrs.at(ipt)["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[ksIdx]);
          ksptrs.at(ipt)["decayL3DSig"+type]->Fill((*tree.cand_decayLength3D)[ksIdx]/(*tree.cand_decayLengthError3D)[ksIdx]);
          ksptrs.at(ipt)["decayL2DSig"+type]->Fill((*tree.cand_decayLength2D)[ksIdx]/(*tree.cand_decayLengthError2D)[ksIdx]);
          ksptrs.at(ipt)["alpha3D"+type]->Fill((*tree.cand_angle3D)[ksIdx]);
          ksptrs.at(ipt)["alpha2D"+type]->Fill((*tree.cand_angle2D)[ksIdx]);
          ksptrs.at(ipt)["vtxProb"+type]->Fill((*tree.cand_vtxProb)[ksIdx]);
          ksptrs.at(ipt)["dca"+type]->Fill((*tree.cand_dca)[ksIdx]);

          auto genks_idx = (*tree.cand_genIdx)[ksIdx];
          ksptrs.at(ipt)["decayL3Dgen"]->Fill( (*tree.gen_decayLength3D)[genks_idx] );
          ksptrs.at(ipt)["alpha3Dgen"]->Fill( (*tree.gen_angle3D)[genks_idx] );
          ksptrs.at(ipt)["decayL2Dgen"]->Fill( (*tree.gen_decayLength2D)[genks_idx] );
          ksptrs.at(ipt)["alpha2Dgen"]->Fill( (*tree.gen_angle2D)[genks_idx] );

          auto grandDauTrkIdx1 =  (*tree.cand_trkIdx)[grandDauIdx1];
          auto grandDauTrkIdx2 =  (*tree.cand_trkIdx)[grandDauIdx2];
          trackptrs["dxy_pi_in_ks_lamc_"+type]->Fill((*tree.trk_xyDCASignificance)[grandDauTrkIdx1]);
          trackptrs["dxy_pi_in_ks_lamc_"+type]->Fill((*tree.trk_xyDCASignificance)[grandDauTrkIdx2]);
          trackptrs["dz_pi_in_ks_lamc_"+type]->Fill((*tree.trk_zDCASignificance)[grandDauTrkIdx1]);
          trackptrs["dz_pi_in_ks_lamc_"+type]->Fill((*tree.trk_zDCASignificance)[grandDauTrkIdx2]);
        } // unmatch
        else {
          std::string type = "unmatch";
          lamcptrs.at(ipt)["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[i]);
          lamcptrs.at(ipt)["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[i]);
          lamcptrs.at(ipt)["decayL3DSig"+type]->Fill((*tree.cand_decayLength3D)[i]/(*tree.cand_decayLengthError3D)[i]);
          lamcptrs.at(ipt)["decayL2DSig"+type]->Fill((*tree.cand_decayLength2D)[i]/(*tree.cand_decayLengthError2D)[i]);
          lamcptrs.at(ipt)["alpha3D"+type]->Fill((*tree.cand_angle3D)[i]);
          lamcptrs.at(ipt)["alpha2D"+type]->Fill((*tree.cand_angle2D)[i]);
          lamcptrs.at(ipt)["vtxProb"+type]->Fill((*tree.cand_vtxProb)[i]);

          lamcptrs.at(ipt)["dcaToPV"+type]->Fill(dcaToPV);
          lamcptrs.at(ipt)["pseudoDL"+type]->Fill(pseudoDL);

          lamcptrs.at(ipt)["y"+type]->Fill(y);

          if(!(*tree.cand_matchGEN)[ksIdx]){

          ksptrs.at(ipt)["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[ksIdx]);
          ksptrs.at(ipt)["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[ksIdx]);
          ksptrs.at(ipt)["decayL3DSig"+type]->Fill((*tree.cand_decayLength3D)[ksIdx]/(*tree.cand_decayLengthError3D)[ksIdx]);
          ksptrs.at(ipt)["decayL2DSig"+type]->Fill((*tree.cand_decayLength2D)[ksIdx]/(*tree.cand_decayLengthError2D)[ksIdx]);
          ksptrs.at(ipt)["alpha3D"+type]->Fill((*tree.cand_angle3D)[ksIdx]);
          ksptrs.at(ipt)["alpha2D"+type]->Fill((*tree.cand_angle2D)[ksIdx]);
          ksptrs.at(ipt)["vtxProb"+type]->Fill((*tree.cand_vtxProb)[ksIdx]);
          ksptrs.at(ipt)["dca"+type]->Fill((*tree.cand_dca)[ksIdx]);

          //if(!(*tree.cand_matchGEN)[ksIdx]){
            auto grandDauTrkIdx1 =  (*tree.cand_trkIdx)[grandDauIdx1];
            auto grandDauTrkIdx2 =  (*tree.cand_trkIdx)[grandDauIdx2];
            trackptrs["dxy_pi_in_ks_lamc_"+type]->Fill((*tree.trk_xyDCASignificance)[grandDauTrkIdx1]);
            trackptrs["dxy_pi_in_ks_lamc_"+type]->Fill((*tree.trk_xyDCASignificance)[grandDauTrkIdx2]);
            trackptrs["dz_pi_in_ks_lamc_"+type]->Fill((*tree.trk_zDCASignificance)[grandDauTrkIdx1]);
            trackptrs["dz_pi_in_ks_lamc_"+type]->Fill((*tree.trk_zDCASignificance)[grandDauTrkIdx2]);
          //}
          }
        }
      }
      // ks candidates
      if(abs((*tree.cand_pdgId)[i]) == particleType.at("ks") && static_cast<unsigned short>((*tree.cand_status)[i]) == 2){
        const auto& momid_vec = (*tree.cand_momIdx)[i];
        //auto it = std::find(momid_vec.begin, momid_vec.end(), particleType.at("lamc"));
        auto mom_idx = (*tree.cand_momIdx)[i][0];
        if(abs((*tree.cand_pdgId)[mom_idx]) != particleType.at("lamc")) continue;
        // match
        if((*tree.cand_matchGEN)[i] && !((*tree.cand_isSwap)[i]) ){
          //std::cout << (*tree.cand_momIdx)[i].size() << std::endl;
          //std::cout << (*tree.cand_dauIdx)[i].size() << std::endl;
          //if((*tree.cand_matchGEN)[mom_idx] && !((*tree.cand_isSwap)[mom_idx]) ) std::cout << "found ks candidates" << std::endl;
          //if((*tree.cand_matchGEN)[mom_idx] ) std::cout << "found ks candidates" << std::endl;
          //std::cout << "found ks candidates" << std::endl;
          /*
          std::cout << "mass " << (*tree.cand_mass)[i] << std::endl;
          std::cout << "matchGEN " << (*tree.cand_matchGEN)[i] << std::endl;
          std::cout << "isSwap " << (*tree.cand_isSwap)[i] << std::endl;
          std::cout << "pdgId " << (*tree.cand_pdgId)[i] << std::endl;
          std::cout << "status " << static_cast<unsigned short>((*tree.cand_status)[i]) << std::endl;
          */

          std::string type = "signal";

          /*
          ksptrs["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[i]);
          ksptrs["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[i]);
          ksptrs["alpha3D"+type]->Fill((*tree.cand_angle3D)[i]);
          ksptrs["alpha2D"+type]->Fill((*tree.cand_angle2D)[i]);
          ksptrs["vtxProb"+type]->Fill((*tree.cand_vtxProb)[i]);
          */

          /*
          auto gen_idx = (*tree.cand_genIdx)[i];
          ksptrs["decayL3Dgen"]->Fill( (*tree.cand_decayLength3D)[gen_idx] );
          ksptrs["alpha3Dgen"]->Fill( (*tree.cand_angle3D)[gen_idx] );
          ksptrs["decayL2Dgen"]->Fill( (*tree.cand_decayLength2D)[gen_idx] );
          ksptrs["alpha2Dgen"]->Fill( (*tree.cand_angle2D)[gen_idx] );
          */
        } // unmatch
        else {
          std::string type = "unmatch";
          /*
          ksptrs["decayL3D"+type]->Fill((*tree.cand_decayLength3D)[i]);
          ksptrs["decayL2D"+type]->Fill((*tree.cand_decayLength2D)[i]);
          ksptrs["alpha3D"+type]->Fill((*tree.cand_angle3D)[i]);
          ksptrs["alpha2D"+type]->Fill((*tree.cand_angle2D)[i]);
          ksptrs["vtxProb"+type]->Fill((*tree.cand_vtxProb)[i]);
          */
        }
      }

      // trackptrs
    } // cand end

    if(!tree.cand_mass) std::cout << "nullptr cand_mass" << std::endl;
    if(!tree.cand_matchGEN) std::cout << "nullptr cand_matchGEN" << std::endl;
    if(!tree.cand_isSwap) std::cout << "nullptr cand_isSwap" << std::endl;
    if(!tree.cand_pdgId) std::cout << "nullptr cand_pdgId" << std::endl;
    if(!tree.cand_status) std::cout << "nullptr cand_status" << std::endl;

    //if(ientry > 5) break;
    if( ifile == tree.fCurrent) {
      std::cout << "reading " << ifile++ << "th file" << std::endl;
    }
  }

  TFile ofile("hist.root", "recreate");
  for(const auto& ptrs : lamcptrs){
    for(const auto& ptr : ptrs){
      ptr.second->Write();
    }
  } 

  for(const auto& ptrs : ksptrs){
    for(const auto& ptr : ptrs){
      ptr.second->Write();
    }
  } 
  for(const auto& ptr : trackptrs){
    ptr.second->Write();
  }

  //gDirectory->Print();
  //gDirectory->Clear();
}
