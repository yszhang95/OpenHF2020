#!/usr/bin/env python
import subprocess
pTMins = [3, 4, 6, 8]
pTMaxs = [4, 6, 8, 10]
for pTMin, pTMax in zip(pTMins, pTMaxs):
  for ds in range(11, 21):
  #for ds in range(10, 11):
    jdllog = "sub_dataMB%d_Pbp_run_pT%dto%d_MB.log" % (ds, pTMin, pTMax)
    #print ('manipJobs.pl', 'submit', jdl)
    subprocess.run(['../../../Utilities/bin/manipJobs.pl resubmit %s --submit' % jdllog], shell=True)
