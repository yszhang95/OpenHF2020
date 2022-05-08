#!/usr/bin/bash
#for i in {2..5}
#do
#  split -d -l1 -a4 /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/FileLists/dataHM${i}_pPb.list lists/dataHM${i}/dataHM${i}_pPb.list.
#done
#for i in 6
#do
#  split -d -l1 -a4 /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/FileLists/dataHM${i}pre_pPb.list lists/dataHM${i}/dataHM${i}_pPb.list.
#done

#for i in {2..3}
#do
#  split -d -l1 -a4 /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/FileLists/dataHM${i}_Pbp.list lists/dataHM${i}/dataHM${i}_Pbp.list.
#done
#for i in {5..6}
#do
#  split -d -l1 -a4 /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/FileLists/dataHM${i}_Pbp.list lists/dataHM${i}/dataHM${i}_Pbp.list.
#done
#for i in 4
#do
#  split -d -l1 -a4 /eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/FileLists/dataHM${i}pre_Pbp.list lists/dataHM${i}/dataHM${i}_Pbp.list.
#done

#for i in {1..6}; do
#  sed -i "s/root:/davs:/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/root:/davs:/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#  sed -i "s/edu/edu:1094:/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/edu/edu:1094:/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#done

#for i in {1..6}; do
#  head lists/dataHM${i}/dataHM${i}_pPb.list.0000
#  head lists/dataHM${i}/dataHM${i}_Pbp.list.0000
#done

#for i in {1..6}; do
#  head lists/dataHM${i}/dataHM${i}_pPb.list.0000
#  head lists/dataHM${i}/dataHM${i}_Pbp.list.0000
#done

#for i in {1..6}; do
#  sed -i "s/davs:/root:/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/davs:/root:/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#  sed -i "s/edu:1094:/edu/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/edu:1094:/edu/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#done

#for i in {1..6}; do
#  sed -i "s/xrootd.cmsaf.mit.edu/cmsxrootd.fnal.gov/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/xrootd.cmsaf.mit.edu/cmsxrootd.fnal.gov/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#done

#for i in {1..6}; do
#  sed -i "s/root:\/\/cmsxrootd.fnal.gov/davs:\/\/xrootd.cmsaf.mit.edu:1094/" lists/dataHM${i}/dataHM${i}_pPb.list.*
#  sed -i "s/root:\/\/cmsxrootd.fnal.gov/davs:\/\/xrootd.cmsaf.mit.edu:1094/" lists/dataHM${i}/dataHM${i}_Pbp.list.*
#done
#for i in {2..6}; do
#  ../writeSub.py -i x509_u118775 -dir . --exe run_pT3to4_HM.sh --boost pPb --dataset dataHM${i} --pTMin 3 --pTMax 4 --transferCert 1 --site fnal
#done

for i in {1..6}; do
  ../writeSub.py -i x509_u118775 -dir . --exe run_pT3to4_HM.sh --boost Pbp --dataset dataHM${i} --pTMin 3 --pTMax 4 --transferCert 1 --site fnal
done
