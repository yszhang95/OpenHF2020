#include <array>
#include <vector>

#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector4Dfwd.h"

#ifndef __HelpClass__
#define __HelpClass__

class Particle {
  public:
    using ParticlePtr = shared_ptr<Particle>;
    Particle(int id): _id(id), _selfConj(false), _flip(0) ,_treeIdx(USHRT_MAX) {};
    Particle(const Particle& p);
    //~Particle() { cout << "Deleted" << endl; }

    bool isStable() const { return _daus.size(); }
    bool selfConj() const { return _selfConj;    }
    void selfConj(bool conj) { _selfConj = conj; }
    int  id()       const { return _id;          }
    bool isNull() const   { return _id == 0;     }
    vector<ParticlePtr> daughters() const { return _daus; }
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
    int _flip;
    unsigned short _treeIdx;
    vector<ParticlePtr> _daus;
};

Particle::Particle(const Particle& p):
  _id(p._id), _selfConj(p._selfConj), _flip(0), _treeIdx(USHRT_MAX)
{
  for (const auto& d : p._daus) {
    _daus.push_back(std::make_shared<Particle>(*d));
  }
}

Particle::ParticlePtr Particle::daughter(size_t i)
{
  if (_daus.size()) {
    return _daus[i];
  }
  else {
    cerr << "No daughter found with pdgId " << _id << endl;
    return nullptr;
  }
}

Particle::ParticlePtr Particle::daughter(size_t i) const
{
  if (_daus.size()) {
    return _daus.at(i);
  }
  else {
    cerr << "No daughter found with pdgId " << _id << endl;
    return nullptr;
  }
}

void Particle::flipFlavor()
{
  if(!_selfConj) {
    _id = -_id;
    for(auto& d : _daus) d->flipFlavor();
  }
  _flip++;
}

#endif
