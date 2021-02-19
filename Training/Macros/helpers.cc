#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/GenVector/VectorUtil.h"
#include "TMath.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"

#include "TreeReader/ParticleTreeMC.hxx"

namespace MCMatch{
  class MatchCriterion {
    public:
      template <typename T>
        bool match(const T& reco, const T& gen);

      MatchCriterion (const float dR, const float dRelPt) :
        _deltaR(dR), _deltaRelPt(dRelPt) { };
      ~MatchCriterion() {}
      void SetDeltaRelPt(const float dRelPt) { _deltaRelPt = dRelPt; }
      void SetDeltaR(const float dR) { _deltaR = dR; }

    private:
      float _deltaR;
      float _deltaRelPt;
  };

  template <typename T>
    bool MatchCriterion::match(const T& reco, const T& gen)
    {
      const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
      const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
      return dR < _deltaR && dRelPt < _deltaRelPt;
    }

  void genMultipleMatch(const TString& inputList)
  {
    using std::cout;
    using std::endl;
    using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<float>;
    PtEtaPhiM_t reco, gen;
    TFileCollection tf("tf", "", inputList);
    TChain t("lambdacAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);
    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << p.GetEntries() << " entries." << endl;;
    return;
  }
};
