#include "optimizer.cc"
void runOptimizer()
{
  // 2 to 3 MB
  PARS mypars(0.0002, 0.0016, 0.0001);
  mypars.dataFileName = "TMVA_MB_LamCKsP2to3App.root";
  mypars.mcFileName = "Merged_MC_LambdaCKsP2to3App.root";
  mypars.dataHistName = "hMassPtMVA_MLP2to3MBNp2N_noDR_y0_Total";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP2to3MBNp2N_noDR_y0";
  mypars.mvaName = "MLP2to3MBNp2N_noDR";
  mypars.pTMin = 2.0;
  mypars.pTMax = 3.0;
  optimizer(mypars);

  /*
  PARS mypars(0.0002, 0.008, 0.0001);
  mypars.dataFileName = "dataMB_pPb_3to4.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP3to4App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP3to4MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP3to4MBNp2N_noDR_y0";
  mypars.mvaName = "MLP3to4MBNp2N_noDR";
  mypars.pTMin = 3.0;
  mypars.pTMax = 4.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.0002, 0.006, 0.0001);
  mypars.dataFileName = "dataMB_pPb_4to6.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP4to6App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mvaName = "MLP4to6MBNp2N_noDR";
  mypars.pTMin = 4.0;
  mypars.pTMax = 5.0;
  optimizer(mypars);
  */
  /*
  PARS mypars(0.0002, 0.006, 0.0001);
  mypars.dataFileName = "dataMB_pPb_4to6.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP4to6App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mvaName = "MLP4to6MBNp2N_noDR";
  mypars.pTMin = 5.0;
  mypars.pTMax = 6.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.00, 0.02, 0.002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP6to8MBApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP6to8MBApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP6to8MBNp2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP6to8MBNp2N_noDR_y0;1";
  mypars.mvaName = "MLP6to8MBNp2N_noDR_y0";
  mypars.pTMin = 6.0;
  mypars.pTMax = 7.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.00, 0.024, 0.002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP6to8MBApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP6to8MBApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP6to8MBNp2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP6to8MBNp2N_noDR_y0;1";
  mypars.mvaName = "MLP6to8MBNp2N_noDR_y0";
  mypars.pTMin = 7.0;
  mypars.pTMax = 8.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.00, 0.030, 0.002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP8to10MBApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP8to10MBApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP8to10MBNp2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP8to10MBNp2N_noDR_y0;1";
  mypars.mvaName = "MLP8to10MBNp2N_noDR_y0";
  mypars.pTMin = 8.0;
  mypars.pTMax = 9.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.00, 0.030, 0.002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP8to10MBApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP8to10MBApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP8to10MBNp2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP8to10MBNp2N_noDR_y0;1";
  mypars.mvaName = "MLP8to10MBNp2N_noDR_y0";
  mypars.pTMin = 9.0;
  mypars.pTMax = 10.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.0008, 0.0032, 0.0002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP4to6HMApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP4to6HMApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6Np2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6Np2N_noDR_y0;1";
  mypars.mvaName = "MLP4to6Np2N_noDR_y0";
  mypars.nsigma = 2.6;
  mypars.pTMin = 4.0;
  mypars.pTMax = 5.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.0008, 0.0026, 0.0002);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP4to6HMApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP4to6HMApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6Np2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6Np2N_noDR_y0;1";
  mypars.mvaName = "MLP4to6Np2N_noDR_y0";
  mypars.nsigma = 2.6;
  mypars.pTMin = 5.0;
  mypars.pTMax = 6.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.003, 0.008, 0.0005);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP6to8HMApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP6to8HMApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP6to8Np2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP6to8Np2N_noDR_y0;1";
  mypars.mvaName = "MLP6to8Np2N_noDR_y0";
  mypars.nsigma = 2.6;
  mypars.pTMin = 6.0;
  mypars.pTMax = 7.0;
  optimizer(mypars);
  */

  /*
  PARS mypars(0.0014, 0.0028, 0.0001);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP6to8HMApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP6to8HMApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP6to8Np2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP6to8Np2N_noDR_y0;1";
  mypars.mvaName = "MLP6to8Np2N_noDR_y0";
  mypars.nsigma = 2.6;
  mypars.pTMin = 7.0;
  mypars.pTMax = 8.0;
  optimizer(mypars);
  */

  /*
  //PARS mypars(0.0015, 0.0045, 0.0002);
  PARS mypars(0.005, 0.015, 0.0005);
  mypars.dataFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_pPb_LambdaCKsP8to10HMApp_NTuple_Ntuple.root";
  mypars.mcFileName = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/TMVA_MC_pPb_LambdaCKsP8to10HMApp_NTuple_Ntuple.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP8to10Np2N_noDR_y0;1";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP8to10Np2N_noDR_y0;1";
  mypars.mvaName = "MLP8to10Np2N_noDR_y0";
  mypars.nsigma = 2.6;
  mypars.pTMin = 8.0;
  mypars.pTMax = 10.0;
  optimizer(mypars);
  */
}
