#!/usr/bin/env python
import pandas as pd
import numpy as np
"PT [GEV],PT [GEV] LOW,PT [GEV] HIGH,SIG(Q=LAM)/2*SIG(Q=K0S),stat +,stat -,sys +,sys -"
def getData(Ntrk="Ntrk0to35"):
    yields = pd.read_csv("ratios_LamToKs_{n}.csv".format(n=Ntrk), comment="#");
    yields["pTError"] = (yields["PT [GEV] HIGH"] - yields["PT [GEV] LOW"])/2
    return yields[["PT [GEV]", "pTError", "SIG(Q=LAM)/2*SIG(Q=K0S)", "stat +", "sys +"]]
def getArr(yields, err="stat"):
    return np.array([yields["PT [GEV]"], yields["SIG(Q=LAM)/2*SIG(Q=K0S)"], yields["pTError"], yields["{} +".format(err)]]).transpose()

Ntrk0to35_stat = getArr(getData("Ntrk0to35"), "stat");
np.savetxt("ratio_LamToKs_Ntrk0to35_stat.txt", Ntrk0to35_stat, delimiter=" ");
Ntrk0to35_syst = getArr(getData("Ntrk0to35"), "sys");
np.savetxt("ratio_LamToKs_Ntrk0to35_syst.txt", Ntrk0to35_syst, delimiter=" ");
Ntrk220to260_stat = getArr(getData("Ntrk220to260"), "stat");
np.savetxt("ratio_LamToKs_Ntrk220to260_stat.txt", Ntrk220to260_stat, delimiter=" ");
Ntrk220to260_syst = getArr(getData("Ntrk220to260"), "sys");
np.savetxt("ratio_LamToKs_Ntrk220to260_syst.txt", Ntrk220to260_syst, delimiter=" ");
