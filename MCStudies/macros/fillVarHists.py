#!/usr/bin/env python
import ROOT as r
r.gROOT.LoadMacro('fillVarHists.C+')
print "Loaded fillVarHists.C"

#ptmins = r.std.vector('float')( (6., 7., 8., 9., 10., 12., 16., 20., 40)) # works in ROOT Version: 6.22/00, python3
#ptmaxs = r.std.vector('float')( (7., 8., 9., 10., 12., 16., 20., 40., 60))# works in ROOT Version: 6.22/00, python3

ptmin_tuple = (6., 7., 8., 9., 10., 12., 16., 20., 40)
ptmax_tuple = (7., 8., 9., 10., 12., 16., 20., 40., 60)

ptmins = r.std.vector('float')()
ptmaxs = r.std.vector('float')()

for ptmin, ptmax in zip(ptmin_tuple, ptmax_tuple):
  ptmins.push_back(ptmin)
  ptmaxs.push_back(ptmax)

#print list(ptmins) # print(ptmins) in python3
#print list(ptmaxs) # print(ptmins) in python3

inputFiles = "../files/MCLambdaCChain20200720.txt"
#inputFiles = "../files/test.txt"
absYMin = 1.0
r.fillVarHists(inputFiles, ptmins, ptmaxs, absYMin)
