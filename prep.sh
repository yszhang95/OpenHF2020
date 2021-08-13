#!/usr/bin/bash
TopDir=${PWD}
git clone https://github.com/yszhang95/OpenHF2020.git
cd $TopDir
tar zcf OpenHF2020.tar.gz OpenHF2020
rm -rf OpenHF2020
xrdcp OpenHF2020.tar.gz ${1}/OpenHF2020.tar.gz
rm OpenHF2020.tar.gz
