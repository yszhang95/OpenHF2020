#include <array>
#include <vector>
#include <memory>
#include <climits>

#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector4Dfwd.h"

#ifndef HelpClass_H
#define HelpClass_H

enum PdgId : int { Proton=2212, Ks=310, LambdaC=4122, Pion=211, Kaon=321};

class Particle {
public:
  using ParticlePtr = std::shared_ptr<Particle>;
  Particle(int id): _id(id), _selfConj(false), _flip(0), _longLived(false), _treeIdx(USHRT_MAX) {};
  Particle(const Particle& p);
  //~Particle() { cout << "Deleted" << endl; }

  bool isStable()  const    { return _daus.size(); }
  bool selfConj()  const    { return _selfConj;    }
  void selfConj(bool conj)  { _selfConj = conj;    }
  bool longLived() const    { return _longLived;   }
  void longLived(bool isLL) { _longLived = isLL;   }
  int  id()        const    { return _id;          }
  bool isNull()    const    { return _id == 0;     }
  std::vector<ParticlePtr> daughters() const { return _daus; }
  void addDaughter(Particle& dau) { _daus.push_back(std::make_shared<Particle>(dau)); }
  ParticlePtr daughter(size_t i);
  ParticlePtr daughter(size_t i) const;
  void setTreeIdx(unsigned short treeIdx) { _treeIdx = treeIdx; }
  int flip() { return _flip; }
  unsigned short treeIdx() const { return _treeIdx; }
  void flipFlavor();
protected:
  int _id;
  bool _selfConj;
  bool _longLived;
  int _flip;
  unsigned short _treeIdx;
  std::vector<ParticlePtr> _daus;
};

struct KineCut {
  float pTMin;
  float pTMax;
  float etaMin;
  float etaMax;
  KineCut(float pTMin, float pTMax, float etaMin, float etaMax) :
    pTMin(pTMin), pTMax(pTMax), etaMin(etaMin), etaMax(etaMax) {}
};

bool passKinematic (const float, const float, const KineCut&);
#endif
