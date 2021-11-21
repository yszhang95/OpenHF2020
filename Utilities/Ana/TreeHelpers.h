#ifndef ParticleTreeMC_hxx
#include "TreeReader/ParticleTreeMC.hxx"
#endif

#ifndef HelpClass_H
#include "Ana/Common.h"
#endif

#ifndef TreeHelpers_H
#define TreeHelpers_H

#include <array>
#include <vector>
#include <tuple>
#include <set>

#include <algorithm>
#include <climits>
#include <numeric>

#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/LorentzVectorfwd.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector3D.h"

// typedefs
//using PtEtaPhiM_t = ROOT::Math::PtEtaPhiMVector;
typedef ROOT::Math::PtEtaPhiMVector PtEtaPhiM_t;
typedef ROOT::Math::RhoEtaPhiVector PtEtaPhi_t;

// functions
// bool checkDecayChain(Particle&, unsigned short,
//                      const ParticleTreeMC&, const bool isFirstGen=true);

bool checkDecayChain(Particle&, unsigned short,
                     const ParticleTree&, const bool isFirstGen=true);

PtEtaPhiM_t getRecoP4(size_t, const ParticleTree&);

PtEtaPhiM_t getRecoDauP4(size_t, size_t, const ParticleTree&);

PtEtaPhi_t getRecoDauP3(size_t, size_t, const ParticleTree&);

// PtEtaPhiM_t getGenP4(size_t, const ParticleTreeMC&);
PtEtaPhiM_t getGenP4(size_t, const ParticleTree&);

bool passEvent(const ParticleTree&, const int pu=4, const int hlt=2);

// classes and structs

class MatchCriterion {
  public:
    template <typename T>
      bool match (const T& reco, const T& gen)
      {
        const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
        const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
        return dR < _deltaR && dRelPt < _deltaRelPt;
      }

    MatchCriterion (const float dR, const float dRelPt) :
      _deltaR(dR), _deltaRelPt(dRelPt) { };
    ~MatchCriterion() {}
    void SetDeltaRelPt(const float dRelPt) { _deltaRelPt = dRelPt; }
    void SetDeltaR(const float dR) { _deltaR = dR; }

  private:
    float _deltaR;
    float _deltaRelPt;
};

#endif
