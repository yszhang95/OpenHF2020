#!/usr/bin/env python
import subprocess
pTMins = [3, 4, 6, 8]
pTMaxs = [4, 6, 8, 10]
for pTMin, pTMax in zip(pTMins, pTMaxs):
  for ds in range(11, 21):
  #for ds in range(10, 11):
    cmd = "./writeSub.py -i ~/private/x509up_u118775 --transferCert 1 --site lxplus "\
        "--exe run_pT%dto%d_MB.sh --pTMin %d --pTMax %d --boost Pbp --dataset dataMB%d" % (pTMin, pTMax, pTMin, pTMax, ds)
    jdl = "sub_dataMB%d_Pbp_run_pT%dto%d_MB.jdl" % (ds, pTMin, pTMax)
    #print(cmd)
    subprocess.run([cmd], shell=True)
    #print ('manipJobs.pl', 'submit', jdl)
    subprocess.run(['../../../Utilities/bin/manipJobs.pl submit %s'% jdl], shell=True)
