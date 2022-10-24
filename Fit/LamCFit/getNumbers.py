#!/usr/bin/env python3
import numpy as np
import pandas as pd
import re

pTMinEdges = (2.0, 3.0, 4.0, 5.0, 6.0, 8.0)
pTMaxEdges = (3.0, 4.0, 5.0, 6.0, 8.0, 10.)

NtrkMinEdges = (0, 35, 60, 90, 120)
NtrkMaxEdges = (35, 60, 90, 120, 185)

nsig = []
nsig_err = []
nsig_sig = []
for pTMinEdge, pTMaxEdge in zip(pTMinEdges, pTMaxEdges):
    nsig_pT = []
    nsig_err_pT = []
    nsig_sig_pT = []
    for NtrkMin, NtrkMax in zip(NtrkMinEdges, NtrkMaxEdges):
        fname = "CMS_BINNED/fix_scale/logs/LamC_CMS_pT%.0fto%.0f_Ntrk%dto%d.log" % (pTMinEdge, pTMaxEdge, NtrkMin, NtrkMax)
        #fname = "CMS_BINNED/LamC_CMS_pT%.0fto%.0f_Ntrk%dto%d.number" % (pTMinEdge, pTMaxEdge, NtrkMin, NtrkMax)
        with open(fname, 'r') as f:
            for line in f.readlines():
                #pass
                line = line.strip()
                ret = re.search("^Final nsig is (.*) \+ (.*) -", line)
                if ret:
                    #print(ret.group(1), ret.group(2), sep=",")
                    nsig_pT.append(float(ret.group(1)))
                    nsig_err_pT.append(float(ret.group(2)))
                    nsig_sig_pT.append(nsig_pT[-1]/nsig_err_pT[-1])
    nsig_sig.append(nsig_sig_pT)


Ntrks = [ "Ntrk%dto%d" % (NtrkMin, NtrkMax) for NtrkMin, NtrkMax in zip(NtrkMinEdges, NtrkMaxEdges)]
pTs = [ "pT%.0fto%.0f" % (pTMin, pTMax) for pTMin, pTMax in zip(pTMinEdges, pTMaxEdges)]
ds = pd.DataFrame(data=nsig_sig, columns=Ntrks, index=pTs)
#print(nsig_sig)
print(ds)
