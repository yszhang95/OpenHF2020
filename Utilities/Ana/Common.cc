#ifndef HelpClass_H
#include "Ana/Common.h"
#endif
#ifdef HelpClass_H
Particle::Particle(const Particle& p):
  _id(p._id), _selfConj(p._selfConj), _longLived(p._longLived),
  _flip(0), _treeIdx(USHRT_MAX)
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
    std::cerr << "No daughter found with pdgId " << _id << std::endl;
    return nullptr;
  }
}

Particle::ParticlePtr Particle::daughter(size_t i) const
{
  if (_daus.size()) {
    return _daus.at(i);
  }
  else {
    std::cerr << "No daughter found with pdgId " << _id << std::endl;
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

bool passKinematic (const float pT, const float eta, const KineCut& cut)
{
  if (pT < cut.pTMin || pT > cut.pTMax
      || std::abs(eta) < cut.etaMin
      || std::abs(eta) > cut.etaMax) return false;
  return true;
}

#endif
