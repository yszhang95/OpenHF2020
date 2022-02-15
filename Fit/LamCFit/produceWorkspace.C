#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"

#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TChain.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"


#include <memory>
#include <string>
#include <stdexcept>

// the following copied from
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
// It is licensed under CC0 1.0.
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

/**
   I need to think how to generate workspace
   1. I need one work space
   2. I need several RooDataSet hosted by one workspace
   3. I need to label each RooDataSet. They are labeled by several
      cuts.
      - pT
      - y
      - MVA
      - event filter
      - trigger
      - multiplicity
   4. I need to save serveral variables
      - mass
      - pT
      - y
      - MVA
   5. I need to think about how to deal with these variables. I temporarily
      do not save them because I label them in the name of dataset
      - Ntrkoffline
      - trigger bits
      - fitler bits
   6. I may save the following variables in the future
      - eta
      - phi
      - daughter related variables
      - other training variables
 */

using std::string;
struct Conf
{
  std::string _inFileList;
  std::string _wsName;
  std::string _wsTitle;
  std::string _dsName;
  std::string _dsTitle;
  std::string _outDir;
  std::string _ofile;
  bool _reweight;
};


struct NTupleVars
{
  Float_t _cand_pT;
  Float_t _cand_mass;
  Float_t _cand_mva;
  Float_t _cand_y;
  Int_t _trigBit_2;
  Int_t _trigBit_4;
  Int_t _filterBit_4;
  Int_t _filterBit_5;
  UShort_t _Ntrkoffline;


};

struct varCuts
{
  // member variables
  std::string _mvaName;
  Float_t _mvaCut;
  Float_t _pTMin;
  Float_t _pTMax;
  Float_t _yAbsMax;
  UShort_t _NtrkofflineMin;
  UShort_t _NtrkofflineMax;
  bool _usedz1p0;
  bool _usegplus;
  bool _useMB;
  bool _useHM;

  // constructor
  varCuts() :  _mvaCut(-1), _pTMin(0), _pTMax(100), _yAbsMax(2.4),
    _NtrkofflineMin(0), _NtrkofflineMax(UShort_t(-1)),
    _usedz1p0(0), _usegplus(0), _useMB(0), _useHM(0)
  {}

  varCuts(const FitParConfigs::CutConfigs cutConfigs):
    _usedz1p0(0), _usegplus(0), _useMB(0), _useHM(0)
  {
    _mvaName = cutConfigs.getMvaName();
    _mvaCut = cutConfigs.getFloatMin("mva");
    _pTMin = cutConfigs.getFloatMin("pT");
    _pTMax = cutConfigs.getFloatMax("pT");
    _yAbsMax = cutConfigs.getFloatMax("yAbs");
    try {
      int temp = cutConfigs.getIntMin("Ntrkoffline");
      if (temp<0) _NtrkofflineMin = 0;
      else _NtrkofflineMin = temp;
      if (temp<0) {
        std::cout << "[ERROR] The min of Ntrkoffline seems to be negative. "
          "Set it to be 0 instead\n";
      }
    } catch (std::out_of_range&) {
      _NtrkofflineMin = 0;
      std::cout << "[ERROR] The min of Ntrkoffline seems to be undefined. "
        "Set it to be 0 instead\n";
    }
    try {
      int temp = cutConfigs.getIntMax("Ntrkoffline");
      if (temp > UShort_t(-1)) _NtrkofflineMax = UShort_t(-1);
      else _NtrkofflineMax = temp;
      if (temp > UShort_t(-1))
        std::cout << "[ERROR] The max of Ntrkoffline is larger"
          " than maximum of unsigned short. "
          "Set it to be " << UShort_t(-1) << " instead\n";
    } catch (std::out_of_range&) {
      _NtrkofflineMax = UShort_t(-1);
      std::cout << "[ERROR] The max of Ntrkoffline seems to be undefined. "
        "Set it to be " << UShort_t(-1) << " instead\n";
    }

    try {
      _usedz1p0 = cutConfigs.getBool("usedz1p0");
    }
    catch (std::out_of_range& e) {
      std::cout << "[ERROR] Not found setup for useDz1p0. "
        "Set it to be false\n";
    }
    try {
      _usegplus = cutConfigs.getBool("usegplus");
    }
    catch (std::out_of_range& e) {
      std::cout << "[ERROR] Not found setup for useGplus. "
        "Set it to be false\n";
    }
    try {
      _useHM = cutConfigs.getBool("usehm");
    }
    catch (std::out_of_range& e) {
      std::cout << "[ERROR] Not found setup for useHM. "
        "Set it to be false\n";
    }
    try {
      _useMB = cutConfigs.getBool("usemb");
    }
    catch (std::out_of_range& e) {
      std::cout << "[ERROR] Not found setup for useMB. "
        "Set it to be false\n";
    }

    // check boundaries
    if (_pTMin > _pTMax || std::abs(_pTMax-_pTMin)<1e-5) {
      throw std::logic_error("[FATAL] pTMin is larger than or equal to pTMax\n");
    }
    if (_yAbsMax<0) {
      throw std::logic_error("[FATAL] Maximum of |y| cannot be negative\n");
    }
    if (_NtrkofflineMin > _NtrkofflineMax) {
      throw std::logic_error("[FATAL] NrkofflineMin is larger than NtrkofflineMax\n");
    }
  }

  // member functions
  // pileup filter dz1p0
  bool pass_dz1p0(const NTupleVars& ntupleVars) const
  { return ntupleVars._filterBit_4 > 0.5; }
  // pileup filter gplus
  bool pass_gplus(const NTupleVars& ntupleVars) const
  { return ntupleVars._filterBit_5 > 0.5; }
  // trigger bits, MB
  bool pass_MB(const NTupleVars& ntupleVars) const
  { return ntupleVars._trigBit_4 > 0.5; }
  bool pass_HM(const NTupleVars& ntupleVars) const
  { return ntupleVars._trigBit_2 > 0.5; }
  // multiplicity cuts
  bool pass_ntrkoffline(const NTupleVars& ntupleVars ) const
  {
    return ntupleVars._Ntrkoffline >= _NtrkofflineMin
      && ntupleVars._Ntrkoffline < _NtrkofflineMax;
  }
  // rapidity cuts
  bool pass_y (const NTupleVars& ntupleVars) const
  { return std::abs(ntupleVars._cand_y) < _yAbsMax; }
  // pT cuts
  bool pass_pT(const NTupleVars& ntupleVars) const
  { return ntupleVars._cand_pT > _pTMin && ntupleVars._cand_pT < _pTMax; }
  // MVA cuts;
  bool pass_mva(const NTupleVars& ntupleVars) const
  { return ntupleVars._cand_mva > _mvaCut; }

  // string labels
  string getEventFilter() const
  {
    string eventFilter;
    if (_usedz1p0) eventFilter += "dz1p0";
    if (_usegplus) {
      if (eventFilter.empty()) eventFilter += "gplus";
      else eventFilter += "_gplus";
    }
    if (_useMB) {
      if (eventFilter.empty()) eventFilter += "MB";
      else eventFilter += "_MB";
    }
    if (_useHM) {
      if (eventFilter.empty()) eventFilter += "HM";
      else eventFilter += "_HM";
    }
    return eventFilter;
  }

  string getNtrkoffline() const
  {
    string ntrkofflineStr;
    if ( _NtrkofflineMin == 0 && _NtrkofflineMax == UShort_t(-1) ) {
      return "NtrkAll";
    }
    if (ntrkofflineStr.empty()) {
      ntrkofflineStr += "Ntrk" + std::to_string(_NtrkofflineMin) + "to";
      if (_NtrkofflineMax == UShort_t(-1)) {
        ntrkofflineStr += "Inf";
      }
      else {
        ntrkofflineStr += std::to_string(_NtrkofflineMax);
      }
    }
    return ntrkofflineStr;
  }

  string getKinematics() const
  {
    auto kinematics = string_format("pT%gto%g_yAbs%g",
                                    _pTMin, _pTMax, _yAbsMax);
    std::replace( kinematics.begin(), kinematics.end(), '.', 'p');
    // alternative way to std::replace
    // for (auto pos = kinematics.find("."); pos!=string::npos; ) {
    //   kinematics.replace(pos, 1, "p");
    //   pos = kinematics.find(".");
    // }
    return kinematics;
  }

  string getMva() const
  {
    auto mvaCutStr = string_format("mva%g", _mvaCut);
    std::replace( mvaCutStr.begin(), mvaCutStr.end(), '.', 'p');
    // alternative way to std::replace
    // auto pos = mvaCutStr.find(".");
    // mvaCutStr.replace(pos, 1, "p");
    auto posNeg = mvaCutStr.find("-");
    if (posNeg != string::npos)
      mvaCutStr.replace(posNeg, 1, "Neg");
    auto posInf = mvaCutStr.find("inf");
    if (posInf != string::npos)
      mvaCutStr.replace(posInf, 3, "Inf");
    return mvaCutStr;
  }
};

void produceWorkspace(std::string configName)
{
  const double massLo = 2.09;
  const double massHi = 2.55;

  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const varCuts mycuts(cutConfigs);

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();

  const std::string wsName = "ws_" + kinStr + "_" + mvaStr + "_" + eventStr;
  const std::string dsName = "ds_" + kinStr + "_" + mvaStr + "_" + eventStr;
  const std::string ofileName = "ofile_" + wsName + ".root";

  TChain chain(inputConfigs.getName("Data").c_str(), "LambdaCKsP");
  const auto files = inputConfigs.getPaths("Data");
  for (const auto & file : files) {
    const auto it = file.find(".list");
    if (it != std::string::npos) {
      TFileCollection tf("tf", "", file.c_str());
      chain.AddFileInfoList(tf.GetList());
    } else {
      chain.Add(file.c_str());
    }
  }
  TTree* t = &chain;

  NTupleVars ntupleVars;
  t->SetBranchAddress("cand_pT", &ntupleVars._cand_pT);
  t->SetBranchAddress("cand_y", &ntupleVars._cand_y);
  t->SetBranchAddress(mvaName.c_str(), &ntupleVars._cand_mva);
  t->SetBranchAddress("cand_mass", &ntupleVars._cand_mass);
  t->SetBranchAddress("trigBit_2", &ntupleVars._trigBit_2);
  t->SetBranchAddress("trigBit_4", &ntupleVars._trigBit_4);
  t->SetBranchAddress("filterBit_4", &ntupleVars._filterBit_4);
  t->SetBranchAddress("filterBit_5", &ntupleVars._filterBit_5);
  t->SetBranchAddress("Ntrkoffline", &ntupleVars._Ntrkoffline);

  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("cand_pT", 1);
  t->SetBranchStatus("cand_y", 1);
  t->SetBranchStatus(mvaName.c_str(), 1);
  t->SetBranchStatus("cand_mass", 1);
  t->SetBranchStatus("trigBit_2", 1);
  t->SetBranchStatus("trigBit_4", 1);
  t->SetBranchStatus("filterBit_4", 1);
  t->SetBranchStatus("filterBit_5", 1);
  t->SetBranchStatus("Ntrkoffline", 1);

  RooRealVar cand_mass("cand_mass",
                       "mass of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                       massLo, massHi);
  // think about what to do here, regarding MVA name
  RooRealVar cand_mva("cand_mva",
                      "mass of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                      0, 1);
  RooRealVar cand_pT("cand_pT",
                     "p_{T} of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                     0, 50);
  RooRealVar cand_y("cand_y",
                     "rapidity of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                     0, 50);
  RooRealVar Ntrkoffline("Ntrkoffline", "# of offline tracks", -0.5, 800.);
  RooRealVar dz1p0("dz1p0", "pile-up filter dz1p0", -0.5, 1.5);
  RooRealVar gplus("gplus", "pile-up filter gplus", -0.5, 1.5);
  RooRealVar trigHM("trigHM", "High multiplicity trigger", -0.5, 1.5);
  RooRealVar trigMB("trigMB", "Minimum bias trigger", -0.5, 1.5);

  // not owned by default
  // either argSets.setRealValue("cand_mass", 2.29)
  // or cand_mass.setVal(2.29)
  const auto argSets = RooArgSet(cand_mass, cand_mva, cand_pT, cand_y);

  // I do not know to write using xroot is safe or not
  // I disable xrootd by creating an object instead of using pointer.
  TFile ofile(ofileName.c_str(), "recreate");
  RooWorkspace myws(wsName.c_str(), "");
  RooAbsData::setDefaultStorageType(RooAbsData::Tree);
  // I do not care weight yet
  RooDataSet mydata(dsName.c_str(), "", argSets, 0);

  Long64_t nentries = t->GetEntries();
  for (Long64_t ientry=0; ientry!=nentries; ++ientry) {
    t->GetEntry(ientry);
    // event level
    // pileup filter
    const bool pass_dz1p0 = ntupleVars._filterBit_4 > 0.5;
    // pileup filter
    const bool pass_gplus = ntupleVars._filterBit_5 > 0.5;
    // trigger bits, MB
    const bool pass_MB = ntupleVars._trigBit_4 > 0.5;
    // trigger bits, MB
    const bool pass_HM = ntupleVars._trigBit_2 > 0.5;

    // multiplicity cuts
    const bool pass_ntrkoffline =
      ntupleVars._Ntrkoffline >= mycuts._NtrkofflineMin &&
      ntupleVars._Ntrkoffline < mycuts._NtrkofflineMax;

    // candidate level
    // rapidity cuts
    const bool pass_y = std::abs(ntupleVars._cand_y) < mycuts._yAbsMax;
    const bool pass_pT = ntupleVars._cand_pT > mycuts._pTMin
      && ntupleVars._cand_pT < mycuts._pTMax;
    // MVA cuts;
    const bool pass_mva = ntupleVars._cand_mva > mycuts._mvaCut;

    // in all
    bool pass_cuts = pass_ntrkoffline && pass_y && pass_pT && pass_mva;
    if (mycuts._useHM) pass_cuts = pass_cuts && pass_HM;
    if (mycuts._useMB) pass_cuts = pass_cuts && pass_MB;
    if (mycuts._usedz1p0) pass_cuts = pass_cuts && pass_dz1p0;
    if (mycuts._usegplus) pass_cuts = pass_cuts && pass_gplus;

    if (!pass_cuts) continue;

    // let us set up values
    cand_mass.setVal(ntupleVars._cand_mass);
    cand_mva.setVal(ntupleVars._cand_mva);
    cand_pT.setVal(ntupleVars._cand_pT);
    cand_y.setVal(ntupleVars._cand_y);

    // add them to dataset
    // forget the weight temporarily
    // I do not know how to handle this
    // it seems I need to add weight in RooArgSet, and
    // I also need to add the name of weight in RooDataSet constructor
    // here, should I do
    // weight.setVal(1.1); mydata.add(argSets, weight.getVal());
    // or I only need to do
    // mydata.add(argSets, 1.1)
    // ?
    mydata.add(argSets);
  }
  // or you can myws.writeToFile("output.root", kTRUE)
  // the second arguments is for `recreate`
  // if you do
  // TFile ofile("name.root", "recreate"); TTree t("t", ""); t.Write();
  // t will flush parts of data to ofile once buffers are full
  // I do not know if writeToFile have similar results
  ofile.cd();
  myws.import(mydata);
  myws.Write();
}

void produceWorkspaceAlice()
{
  produceWorkspace("testws_pT8to10.txt");
}
