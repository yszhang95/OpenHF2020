#include "functions.h"

#ifdef __MyFunctions__

#include "TClonesArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TObjString.h"

#include "TXMLEngine.h"

using std::map;
using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::istringstream;

map<string, vector<string>> readConfigs(std::string inputXML="test.xml")
{
  if (DEBUG) { cout << "\nStart readConfigs" << endl; }
  map<string, vector<string>> configs;

  TXMLEngine xml;
  XMLDocPointer_t xmldoc =  xml.ParseFile(inputXML.c_str());
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);
  XMLNodePointer_t child = xml.GetChild(mainnode);

  istringstream iStr;

  while(child != 0) {
    const char* childname = xml.GetNodeName(child);
    const char* content = xml.GetNodeContent(child);
    child = xml.GetNext(child);

    iStr.str(string(content));
    const auto& has = configs.insert(
        map<string, vector<string>>::value_type(
          childname, vector<string>()));
    for(string temp; getline(iStr, temp); ){
      trim(temp);
      if(!temp.size()) continue;
      if(temp.at(0)=='#') continue;
      configs.at(childname).push_back(temp);
      temp.clear();
    }
    iStr.clear();
  }
  if (DEBUG) {
    for (const auto& e : configs) {
      cout << e.first << endl;
      for (const auto& ee : e.second) {
        cout << ee << endl;
      }
    }
  }
  xml.FreeDoc(xmldoc);
  if (DEBUG) { cout << "End readConfigs\n" << endl; }
  return configs;
}


map<string, TMVA::Types::EMVA> setupMethodCollection()
{
  map<string, TMVA::Types::EMVA> MethodCollection;
  MethodCollection["Variable"       ] = TMVA::Types::kVariable       ;
  MethodCollection["Cuts"           ] = TMVA::Types::kCuts           ;
  MethodCollection["Likelihood"     ] = TMVA::Types::kLikelihood     ;
  MethodCollection["PDERS"          ] = TMVA::Types::kPDERS          ;
  MethodCollection["HMatrix"        ] = TMVA::Types::kHMatrix        ;
  MethodCollection["Fisher"         ] = TMVA::Types::kFisher         ;
  MethodCollection["KNN"            ] = TMVA::Types::kKNN            ;
  MethodCollection["CFMlpANN"       ] = TMVA::Types::kCFMlpANN       ;
  MethodCollection["TMlpANN"        ] = TMVA::Types::kTMlpANN        ;
  MethodCollection["BDT"            ] = TMVA::Types::kBDT            ;
  MethodCollection["DT"             ] = TMVA::Types::kDT             ;
  MethodCollection["RuleFit"        ] = TMVA::Types::kRuleFit        ;
  MethodCollection["SVM"            ] = TMVA::Types::kSVM            ;
  MethodCollection["MLP"            ] = TMVA::Types::kMLP            ;
  MethodCollection["BayesClassifier"]=  TMVA::Types::kBayesClassifier;
  MethodCollection["FDA"            ] = TMVA::Types::kFDA            ;
  MethodCollection["Boost"          ] = TMVA::Types::kBoost          ;
  MethodCollection["PDEFoam"        ] = TMVA::Types::kPDEFoam        ;
  MethodCollection["LD"             ] = TMVA::Types::kLD             ;
  MethodCollection["Plugins"        ] = TMVA::Types::kPlugins        ;
  MethodCollection["Category"       ] = TMVA::Types::kCategory       ;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  MethodCollection["DNN"            ] = TMVA::Types::kDNN            ;
  MethodCollection["DL"             ] = TMVA::Types::kDL             ;
  MethodCollection["PyKeras"        ] = TMVA::Types::kPyKeras        ;
  MethodCollection["CrossValidation"] = TMVA::Types::kCrossValidation;
#endif
  MethodCollection["PyRandomForest" ] = TMVA::Types::kPyRandomForest ;
  MethodCollection["PyAdaBoost"     ] = TMVA::Types::kPyAdaBoost     ;
  MethodCollection["PyGTB"          ] = TMVA::Types::kPyGTB          ;
  MethodCollection["C50"            ] = TMVA::Types::kC50            ;
  MethodCollection["RSNNS"          ] = TMVA::Types::kRSNNS          ;
  MethodCollection["RSVM"           ] = TMVA::Types::kRSVM           ;
  MethodCollection["RXGB"           ] = TMVA::Types::kRXGB           ;
  MethodCollection["MaxMethod"      ] = TMVA::Types::kMaxMethod      ;
  return MethodCollection;
}

map<string, TString> getTrainPars(const TString& in, const std::string& type)
{
  if (DEBUG) { cout << "\nStart getPars" << endl; }
  map<string, TString> output;
  if (type == "methods") {
    TPRegexp r("(.*)\\? *(.*)\\? *(.*)");
    TObjArray* subStrL = r.MatchS(in);
    output["fullStr"] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
    output["method"] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
    output["name"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
    output["config"] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  } else
    if (type.find("worange") != string::npos) {
      TPRegexp r("(.*)\\? *(.*)\\? *(.*)\\? *(.)");
      TObjArray* subStrL = r.MatchS(in);
      output["fullStr"]     = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
      output["variable"]    = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
      output["description"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
      output["unit"]        = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
      output["type"]        = dynamic_cast<TObjString*>(subStrL->At(4))->GetString();
    }
  if (type.find("wrange") != string::npos) {
    TPRegexp r("(.*)\\? *(.*)\\? *(.*)\\? *(.*)\\? *(.*)\\? *(.*)");
    TObjArray* subStrL = r.MatchS(in);
    output["fullStr"    ] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
    output["variable"   ] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
    output["description"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
    output["unit"       ] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
    output["type"       ] = dynamic_cast<TObjString*>(subStrL->At(4))->GetString();

    auto minStr = dynamic_cast<TObjString*>(subStrL->At(5))->GetString();
    auto maxStr = dynamic_cast<TObjString*>(subStrL->At(6))->GetString();

    TPRegexp num("\\s*(-?\\d+\\.?\\d*)\\s*");

    TObjArray* minNum = num.MatchS(minStr);
    if (minNum->GetLast()<0) {
      output["minStr"] = "";
      output["min"   ] = "";
    } else {
      output["minStr"] = dynamic_cast<TObjString*>(minNum->At(0))->GetString();
      output["min"   ] = dynamic_cast<TObjString*>(minNum->At(1))->GetString();
    }

    TObjArray* maxNum = num.MatchS(maxStr);
    if (maxNum->GetLast()<0) {
      output["maxStr"] = "";
      output["max"   ] = "";
    } else {
      output["maxStr"] = dynamic_cast<TObjString*>(maxNum->At(0))->GetString();
      output["max"   ] = dynamic_cast<TObjString*>(maxNum->At(1))->GetString();
    }
  }
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  if (DEBUG) {for (const auto& e: output) { cout << e.second << endl; }}
  if (DEBUG) { cout << "End getPars\n" << endl; }

  return output;
}

#endif
