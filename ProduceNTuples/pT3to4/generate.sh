#!/usr/bin/bash
#./writeSub.py -i ~/private/x509_u118775 -dir pT3to4/ --exe pT3to4/run_pT3to4_MB.sh --boost Pbp --dataset dataMB10 --site fnal --pTMin 3 --pTMax 4 --transferCert 1
#../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT3to4_MB.sh --boost Pbp --dataset dataMB11 --site fnal --pTMin 3 --pTMax 4 --transferCert 1
#for i in {1..8}
#do
  #../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT3to4_MB.sh --boost Pbp --dataset dataMB${i} --site fnal --pTMin 3 --pTMax 4 --transferCert 1
  #../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT3to4_MB.sh --boost Pbp --dataset dataMB${i} --site fnal --pTMin 3 --pTMax 4 --transferCert 1
  #../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT3to4_MB.sh --boost pPb --dataset dataMB${i} --site fnal --pTMin 3 --pTMax 4 --transferCert 1
#done
for i in {10..20}
do
  ../writeSub.py -i ~/private/x509_u118775 -dir . --exe run_pT3to4_MB.sh --boost Pbp --dataset dataMB${i} --site fnal --pTMin 3 --pTMax 4 --transferCert 1
done
