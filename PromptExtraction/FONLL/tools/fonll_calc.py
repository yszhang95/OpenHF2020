#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import json
import requests

"""
A small class for configuration
"""

class fonll_calc(object):
    """
    A small class for configuration
    """

    COLLIDERs = {
      "RHIC (pp, 62.4 GeV) [ptmax < 12 GeV]" : 13,
      "pp, 68.6 GeV [ptmax < 12 GeV]" : 18,
      "RHIC (pp, 200 GeV) [ptmax < 30 GeV]" : 4,
      "RHIC (pp, 500 GeV) [ptmax < 50 GeV]" : 15,
      "Tevatron (ppbar, 900 GeV) [ptmax < 50 GeV]" : 10,
      "Tevatron Run 2 (ppbar, 1960 GeV) [ptmax < 100 GeV]" : 2,
      "LHC  (pp,   2.75 TeV) [ptmax < 300 GeV]" : 9,
      "LHC  (pp,   4+1.58 TeV) [ptmax < 300 GeV]" : 12,
      "LHC  (pp,   5.03 TeV) [ptmax < 300 GeV]" : 16,
      "LHC  (pp,   5.5 TeV) [ptmax < 300 GeV]" : 8,
      "LHC  (pp,   7 TeV) [ptmax < 300 GeV]" : 7,
      "LHC  (pp,   8 TeV) [ptmax < 300 GeV]" : 11,
      "LHC  (pp,   8.16 TeV) [ptmax < 300 GeV]" : 17,
      "LHC  (pp,  13 TeV) [ptmax < 300 GeV]" : 14,
      "LHC  (pp,  14 TeV) [ptmax < 300 GeV]" : 3
      }

    PDFs = {
        "CTEQ6.6": 10550,
        "NNPDF30nlo_as0118": 260000
        }

    ORDERs = {
        "FONLL": 1,
        "NLO": 2
        }

    # HEAVY_QUARKs = {
    #    "charm": "charm",
    #    "c": "charm", # added by me
    #    "bottom": "bottom",
    #    "beauty": "bottom", # added by me
    #    "b": "bottom" # added by me
    #    }

    # FINAL_STATEs = {
    #     "bare quark" : "bare quark",
    #     "B hadron" : "B hadron"
    #     }

    DECAYs = {
        "-" : 1,
        "B hadron to electron" : 2,
        "B to D to electron" : 3,
        "B to J/psi" : 4,
        "B to psi(2S)" : 7,
        "B to D" : 5,
        "B to D*" : 6
        }

    XSECT_TYPEs = {
        "Total cross section (within cuts)" : 1,
        "dsigma/dpt" : 2,
        "dsigma/dy" : 3,
        "dsigma/deta" : 5,
        "1/pi dsigma/dpt^2/dy|_y=0" : 4,
        "dsigma/dpt/dy" : 6,
        "dsigma/dpt/deta" : 7
        }

    BANDs = {
        "central prediction only" : 1,
        "uncertainty range from scales only": 2,
        "uncertainty range from scales and masses": 3
        }

    NPFFs = {
        "default" : 0,
        "Kartvelishvili" : 1,
        "Peterson" : 2,
        "BCFY for D*" : 4,
        "BCFY for D0" : 5,
        "BCFY for D+" : 8
        }

    # use_eta = False
    # nppars = "default"
    # n5moment = True

    BASIC_CONFIG = {
      "collider" : 14,
      "pdf" : 10550, # CTEQ6.6
      "order" : 1,
      "heavy_quark" : "bottom",
      "final_state" : "B hadron",
      "decay" : "-",
      "xsect_type" : 2, # dsigma/dpt
      "band" : 3,
      "ptmin" : 5,
      "ptmax" : 20,
      "ymin": -1,
      "ymax": 1,
      "npoints": 2,
      "allcurves" : True,
      "pdfunc" : True,
      "npff" : 0,
      "nppar" : "default",
      "n5moment" : True,
      "BRcD" : 1,
      "BRbB" : 1,
      "BRDl" : 0.103,
      "BRBl":  0.1086,
      "BRBDl" : 0.096,
      "BRBD" : 0.823,
      "BRBDs" : 0.173,
      "BRBJpsi" : 0.0116,
      "BRBpsi2S" : 0.00307
      }

    def __init__(self, json_name=None):
        self.config = self.BASIC_CONFIG.copy()
        load = {}
        if json_name:
            with open(json_name) as f:
                load = json.load(f)
        if "collider" in load.keys():
            self.config["collider"] = self.COLLIDERs[load["collider"]]
            del load["collider"]
        if "pdf" in load.keys():
            self.config["pdf"] = self.PDFs[load["pdf"]]
            del load["pdf"]
        if "order" in load.keys():
            self.config["order"] = self.ORDERs[load["order"]]
            del load["order"]
        if "decay" in load.keys():
            self.config["decay"] = self.DECAYs[load["decay"]]
            del load["decay"]
        if "xsect_type" in load.keys():
            self.config["xsect_type"] = self.XSECT_TYPEs[load["xsect_type"]]
            del load["xsect_type"]
        if "band" in load.keys():
            self.config["band"] = self.BANDs[load["band"]]
            del load["band"]
        if "npff" in load.keys():
            self.config["npff"] = self.NPFFs[load["npff"]]
            del load["npff"]
        for k, v in load.items():
            self.config[k] = load[k]

    @property
    def ptmin(self):
        return self.config["ptmin"]
    @ptmin.setter
    def ptmin(self, ptmin):
        self.config["ptmin"] = ptmin

    @property
    def ptmax(self):
        return self.config["ptmax"]
    @ptmax.setter
    def ptmax(self, ptmax):
        self.config["ptmax"] = ptmax

    @property
    def ymin(self):
        return self.config["ymin"]
    @ymin.setter
    def ymin(self, ymin):
        self.config["ymin"] = ymin

    @property
    def ymax(self):
        return self.config["ymax"]
    @ymax.setter
    def ymax(self, ymax):
        self.config["ymax"] = ymax

    def calc(self, path=None):
        print(self.config)
        url = 'https://www.lpthe.jussieu.fr/~cacciari/cgi-bin/call_band-13.pl'
        out = requests.post(url, data=self.config)

        if path:
            if os.path.exists(path):
                os.remove(path)
            dir_path = os.path.dirname(path)
            if not os.path.exists(dir_path):
                os.mkdir(dir_path)
            with open(path, 'w') as f:
                f.write(out.text)
            print("writen to", path)
        else:
            print(out.text)

if __name__ == '__main__':
    calc = fonll_calc("pp_8p16TeV.json")
    ymins = [-2.46, -1.46, -0.46, 0.54]
    ymaxs = [-1.46, -0.46, 0.54, 1.54]
    ptmins = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
              15, 16, 18, 20, 22, 24, 26, 28]
    ptmaxs = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
              15, 16, 18, 20, 22, 24, 26, 28, 30]
    for ymin, ymax in zip(ymins, ymaxs):
        foutstr = 'output_pp_8p16TeV/pTspectra_y{ymin}to{ymax}'.format(ymin=ymin, ymax=ymax)
        foutstr = foutstr.replace(".", 'p')
        foutstr = foutstr + ".txt"
        fout = open(foutstr, 'w')
        for ptmin, ptmax in zip(ptmins, ptmaxs):
            calc.ymin  = ymin
            calc.ymax  = ymax
            calc.ptmin = ptmin
            calc.ptmax = ptmax
            outstr = "output_pp_8p16TeV/pT{ptmin}to{ptmax}_y{ymin}to{ymax}".format(
                      ptmin=ptmin, ptmax=ptmax, ymin=ymin, ymax=ymax)
            outstr = outstr.replace(".", "p")
            outstr = outstr + ".txt"
            calc.calc(outstr)
            with open(outstr) as f:
                lines = []
                for l in f.readlines():
                    if not l[0] == '#':
                        lines.append(l.strip())
                # print(lines)
                nums_min = [ float(l) for l in lines[0].split(' ')]
                nums_max = [ float(l) for l in lines[1].split(' ')]
                # first pt, second central, third min, fourth max
                pt = (nums_min[0] + nums_max[0]) / 2
                pt_err = (nums_max[0] - nums_min[0]) / 2
                xsec = (nums_min[1] + nums_max[1]) / 2
                xsec_lo = (nums_min[2] + nums_max[2]) / 2
                xsec_hi = (nums_min[3] + nums_max[3]) / 2
                outnums = "{pt} {pt_err} {xsec} {lo} {hi}\n".format(pt=pt, pt_err=pt_err, xsec=xsec, lo=xsec-xsec_lo, hi=xsec_hi-xsec)
                fout.write(outnums)
