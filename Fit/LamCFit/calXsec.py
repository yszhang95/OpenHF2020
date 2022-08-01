#!/usr/bin/env python3
import numpy as np
import pandas as pd
Lc8TeV = pd.read_csv("Lc_8TeV_AliceCut.txt", delimiter=" ")
print(Lc8TeV)

BR = 0.0159 * 0.67
dY = 1
Lc8TeV["Lumi"] = Lc8TeV["Nevt"]/ 8.1E9 * 4217 #ub^-1
# Y/eff / B.R. / dPt /dY / Lumi / 2, 2 is for (Lc+Anti-Lc)/2
Lc8TeV["dSigma/dPt/dY"] = Lc8TeV["Y"] / Lc8TeV["MCReco"] * Lc8TeV["MCGen"] * Lc8TeV["Frac"] / BR / (Lc8TeV["pTMax"] - Lc8TeV["pTMin"]) / dY / Lc8TeV["Lumi"] / 2
Lc8TeV["dSigma/dPt/dYErr"] = Lc8TeV["Yerr"] / Lc8TeV["MCReco"] * Lc8TeV["MCGen"] * Lc8TeV["Frac"] / BR / (Lc8TeV["pTMax"] - Lc8TeV["pTMin"]) / dY / Lc8TeV["Lumi"] / 2
print(Lc8TeV)
