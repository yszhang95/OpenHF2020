#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
from itertools import repeat

def getArrs(arr):
    output = {}
    x = np.array([(x1-x2, x1+x2) for x1, x2 in zip(arr[:,0], arr[:, 1])])
    output["x"] = x.flatten()

    y1 = arr[:, 2] - arr[:, 3]
    y2 = arr[:, 2] + arr[:, 4]

    output["y1"] = [y for item in y1 for y in repeat(item, 2)]
    output["y2"] = [y for item in y2 for y in repeat(item, 2)]

    return output

yvalues = {}
yvalues_dict = {}

yvalues[r'$-2.46<y_{cm}<-1.46$'] = np.loadtxt('output_pp_8p16TeV/pTspectra_y-2p46to-1p46.txt', delimiter=' ')
# yvalues[r'$-1.46<y_{cm}<0.54$'] = np.loadtxt('output_pp_8p16TeV/pTspectra_y-1p46to0p54.txt', delimiter=' ')
yvalues[r'$-1.46<y_{cm}<-0.46$'] = np.loadtxt('output_pp_8p16TeV/pTspectra_y-1p46to-0p46.txt', delimiter=' ')
yvalues[r'$-0.46<y_{cm}<0.54$'] = np.loadtxt('output_pp_8p16TeV/pTspectra_y-0p46to0p54.txt', delimiter=' ')
yvalues[r'$0.54<y_{cm}<1.54$'] = np.loadtxt('output_pp_8p16TeV/pTspectra_y0p54to1p54.txt', delimiter=' ')

for k, v in yvalues.items():
    yvalues_dict[k] = getArrs(v)

fig, ax = plt.subplots()
for k, v in yvalues_dict.items():
    ax.fill_between(v['x'], v['y1'], v['y2'], label=k, alpha=0.35)

ax.set_title(r"FONLL calculations for b$\rightarrow$B")
ax.set_xlabel(r'$p_{T} (GeV)$')
ax.set_ylabel(r'd$\sigma$/d$p_{T}$ (pb/GeV)')
ax.set_yscale('log')

ax.legend()
fig.savefig('fonll.pdf')
