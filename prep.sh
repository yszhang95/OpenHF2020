#!/usr/bin/bash
Options=""
if [[ "${1}" = "-f" ]]; then
  echo "Force"
  Options=${1}
fi
TargetDir=""
if [[ -z ${2} ]]; then
  TargetDir="root://eoscms.cern.ch///store/group/phys_heavyions/yousen/OpenHF2020Storage/Package"
else
  TargetDir=${1}
fi
TopDir=${PWD}
git clone https://github.com/yszhang95/OpenHF2020.git
cd $TopDir
tar zcf OpenHF2020.tar.gz OpenHF2020
rm -rf OpenHF2020
xrdcp ${Options} OpenHF2020.tar.gz ${TargetDir}/OpenHF2020.tar.gz
rm OpenHF2020.tar.gz
