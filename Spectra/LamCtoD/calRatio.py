#!/usr/bin/env python
import numpy as np
import pandas as pd

def getData(Ntrk="Ntrk0to35", particle="Ks"):
    yields = pd.read_csv("{p}_{n}.csv".format(p=particle, n=Ntrk), comment="#");
    yields["deltaPt"] = yields["PT [GEV] HIGH"] - yields["PT [GEV] LOW"]
    yields["yields"] = yields["(1/Nev)*(1/(2*PI*PT))*D2(N)/DPT/DYRAP [GEV**-2]"] * yields["deltaPt"]
    yields["stat_err"] = yields["stat +"] * yields["deltaPt"]
    yields["sys_err"] = yields["sys +"] * yields["deltaPt"]

    ok = np.logical_and(yields["PT [GEV]"]>3.,  yields["PT [GEV]"]<5.)
    return yields[["yields", "stat_err", "sys_err"]][ok]

def getYields(Ntrk="Ntrk0to35", particle="Ks"):
    yields = getData(Ntrk, particle)
    yields_sum = np.sum(yields["yields"]);
    stat_sum = np.sqrt(np.sum(yields["stat_err"]**2));
    sys_sum = np.sqrt(np.sum(yields["sys_err"]**2));
    print(particle, yields_sum)
    return [yields_sum, stat_sum, sys_sum]

def getArr(particle="Ks"):

    output_stat = []
    output_syst = []
    for NtrkMin, NtrkMax in zip([0, 35, 60, 90, 120, 150, 185, 220], [35, 60, 90, 120, 150, 185, 220, 260]):
        yields = getYields("Ntrk{NtrkMin}to{NtrkMax}".format(NtrkMin=NtrkMin, NtrkMax=NtrkMax), particle)
        output_stat.append([(NtrkMin+NtrkMax)/2, yields[0], (NtrkMax-NtrkMin)/2, yields[1]])
        output_syst.append([(NtrkMin+NtrkMax)/2, yields[0], (NtrkMax-NtrkMin)/2, yields[2]])

    ar_output_stat = np.array(output_stat)
    np.savetxt("{}_pT3to5_stat.txt".format(particle), ar_output_stat, delimiter=" ")

    ar_output_syst = np.array(output_syst)
    np.savetxt("{}_pT3to5_syst.txt".format(particle), ar_output_syst, delimiter=" ")
    return {"stat": ar_output_stat, "syst": ar_output_syst}

output_Ks = getArr("Ks")
output_Lam = getArr("Lam")
print(output_Lam["stat"][:,1])
print(output_Ks["stat"][:,1])
output = output_Lam["stat"][:,1]/output_Ks["stat"][:,1]/2
output_stat_err = output * np.sqrt((output_Lam["stat"][:,3]/output_Lam["stat"][:,1])**2 +
                                   (output_Ks["stat"][:,3]/output_Ks["stat"][:,1])**2)
output_syst_err = output * np.sqrt((output_Lam["syst"][:,3]/output_Lam["syst"][:,1])**2 +
                                   (output_Ks["syst"][:,3]/output_Ks["syst"][:,1])**2)
output_stat = np.array([output_Lam["stat"][:,0], output, output_Lam["stat"][:,2], output_stat_err]).transpose()
output_syst = np.array([output_Lam["syst"][:,0], output, output_Lam["syst"][:,2], output_syst_err]).transpose()
np.savetxt("ratio_LamToKs_pT3to5_stat.txt", output_stat, delimiter=" ")
np.savetxt("ratio_LamToKs_pT3to5_syst.txt", output_syst, delimiter=" ")
