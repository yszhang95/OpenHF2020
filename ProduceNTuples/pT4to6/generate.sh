#!/usr/bin/bash
#../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT4to6_MB.sh --boost Pbp --dataset dataMB11 --site fnal --pTMin 4 --pTMax 6 --transferCert 1
for i in {1..8}
do
  ../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT4to6_MB.sh --boost pPb --dataset dataMB${i} --site fnal --pTMin 4 --pTMax 6 --transferCert 1
  ../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT4to6_MB.sh --boost Pbp --dataset dataMB${i} --site fnal --pTMin 4 --pTMax 6 --transferCert 1
done
for i in {9..20}
do
  ../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT4to6_MB.sh --boost Pbp --dataset dataMB${i} --site fnal --pTMin 4 --pTMax 6 --transferCert 1
done
