#!/usr/bin/env python3

pTMinFileLabels = (2.0, 3.0, 4.0, 4.0, 6.0, 8.0)
pTMaxFileLabels = (3.0, 4.0, 6.0, 6.0, 8.0, 10.)
pTMinEdges = (2.0, 3.0, 4.0, 5.0, 6.0, 8.0)
pTMaxEdges = (3.0, 4.0, 5.0, 6.0, 8.0, 10.)
mvaCuts = (0.0008, 0.0041, 0.0029, 0.0029, 0.009, 0.016)

NtrkMinEdges = (0, 35, 60, 90, 120)
NtrkMaxEdges = (35, 60, 90, 120, 185)

yAbsMax = 1.0

for pTMinEdge, pTMaxEdge, pTMinFileLabel, pTMaxFileLabel, mvaCut in zip(pTMinEdges, pTMaxEdges, pTMinFileLabels, pTMaxFileLabels, mvaCuts):
    pTFileLabel = "%.0fto%.0f" % (pTMinFileLabel, pTMaxFileLabel)
    pTLabel = "%.0fto%.0f" % (pTMinEdge, pTMaxEdge)

    for NtrkMin, NtrkMax in zip(NtrkMinEdges, NtrkMaxEdges):
        with open("configs/pT%.0fto%.0f_Ntrk%dto%d.conf" %
              (pTMinEdge, pTMaxEdge, NtrkMin, NtrkMax), 'w') as f:
            # print(f.name)
            # input configs
            config = "[BEG InputConfigs]\n"
            config += "MC, tree, /storage1/users/yz144/LambdaC_KsProton/SkimTrees/oldMCs/TMVA_MCLambdaCKsP%sMBApp.root, lambdacAna_mc/ParticleNTuple\n" % pTFileLabel
            for i in range(1, 9):
                config += "Data, tree, /storage1/users/yz144/LambdaC_KsProton/SkimTrees/pT%s/dataMB%i_pPb_pT%s.root, lambdacAna/ParticleNTuple\n" % (pTFileLabel, i, pTFileLabel)
            for i in range(1, 21):
                if i == 19 and (abs(pTMinEdge-3.0)<1E-3 or abs(pTMinEdge-6)<1E-3 ):
                    config += "#Data, tree, /storage1/users/yz144/LambdaC_KsProton/SkimTrees/pT%s/dataMB%i_Pbp_pT%s.root, lambdacAna/ParticleNTuple\n" % (pTFileLabel, i, pTFileLabel)
                else:
                    config += "Data, tree, /storage1/users/yz144/LambdaC_KsProton/SkimTrees/pT%s/dataMB%i_Pbp_pT%s.root, lambdacAna/ParticleNTuple\n" % (pTFileLabel, i, pTFileLabel)
            config += "[END InputConfigs]\n"
            f.write(config)
            # output config
            config = "\n[BEG OutputConfigs]\n"
            config += "log_path, txt, logs/LamC_CMS_pT%s_Ntrk%dto%d.log\n" % (pTLabel, NtrkMin, NtrkMax)
            config += "fig_path, txt, figs/LamC_CMS_pT%s_Ntrk%dto%d\n" % (pTLabel, NtrkMin, NtrkMax)
            config += "[END OutputConfigs]\n"
            f.write(config)
            # cut configs
            config = "\n[BEG CutConfigs]\n"
            config += "pT, %.2f, %.2f, float\n" % (pTMinEdge, pTMaxEdge)
            config += "yAbs, -inf, %.2f, float\n" % yAbsMax
            config += "Ntrkoffline, %d, %d, int\n" % (NtrkMin, NtrkMax)
            config += "MLP%sMBNp2N_noDR, %g, 1, mva\n" % (pTFileLabel, mvaCut)
            config += "useGplus, false, bool\n"
            config += "useDz1p0, true, bool\n"
            config += "useMB, true, bool\n"
            config += "useHM, false, bool\n"
            config += "[END CutConfigs]\n"
            f.write(config)
            # parameter configurations
            # temporarily empty
            config = "\n[BEG ParConfigs]\n"
            config += "dummy, 0, -1, 1\n"
            config += "[END ParConfigs]\n"
            f.write(config)
