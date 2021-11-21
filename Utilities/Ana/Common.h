#include <array>
#include <initializer_list>
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
  Particle(int id): _id(id), _selfConj(false), _longLived(false), _flip(0), _treeIdx(USHRT_MAX) {};
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
  float absEtaMin;
  float absEtaMax;
  float absYMin;
  float absYMax;
  float massPDG;
  float absMassLw;
  float absMassUp;
  KineCut(float pTMin_, float pTMax_,
          float absEtaMin_, float absEtaMax_,
          float absYMin_, float absYMax_,
          float massPDG_, float absMassLw_, float absMassUp_) :
    pTMin(pTMin_), pTMax(pTMax_),
    absEtaMin(absEtaMin_), absEtaMax(absEtaMax_),
    absYMin(absYMin_), absYMax(absYMax_),
    massPDG(massPDG_), absMassLw(absMassLw_), absMassUp(absMassUp_)
  {}
};

bool passKinematic (const float, const float,
                    const float, const float mass,
                    const KineCut&);

/**
   A struct for selecting protons using dE/dx.  The profile of dE/dx
   is represented via a function inside operator(). It is a function
   with two pieces. The end point of the first piece is turn1, and the
   end of the second piece is turn2. The candidates pass the selection
   if the dE/dx is within [lower, upper] * profile (or the
   mean). Details could be checked inside override operator().
 */
struct DeDxSelection
{
public:
  DeDxSelection(const float turn1, const float turn2,
             const float lower, const float upper,
             std::initializer_list<float> in);
  DeDxSelection(const float turn1, const float turn2,
             const float lower, const float upper);
  DeDxSelection(const float turn1, const float turn2);
  DeDxSelection();
  bool operator() (const float p, const float dedx);
  double getMean (const float p);
private:
  float _turn1;
  float _turn2;
  float _upper;
  float _lower;
  std::vector<float> _pars;
};

template<typename T>
class EfficiencyTable
{
public:
  explicit EfficiencyTable(T*) = delete;
  double getEfficiency(double) = delete;
  double getWeight(double) = delete;
  double getEfficiency(int) = delete;
  double getWeight(int) = delete;
};

class TGraph;
template<>
class EfficiencyTable<TGraph>
{
public:
  explicit EfficiencyTable(TGraph*);
  double getEfficiency(double);
  double getWeight(double);
  double getEfficiency(int);
  double getWeight(int);

private:
  std::vector<double> x;
  std::vector<double> xErrHigh;
  std::vector<double> xErrLow;
  std::vector<double> y;
  std::vector<double> yErrHigh;
  std::vector<double> yErrLow;
  int n;
};
#endif
