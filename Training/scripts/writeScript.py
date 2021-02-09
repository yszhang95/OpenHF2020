#!/usr/bin/env python3
xmlFiles = [
    'topo4_CutsGA_BDT4.xml',
    'topo4_CutsOnKs_BDTG.xml',
    'topo4_dau3trans_BDTG.xml',
    'topo4_dau4_BDTG.xml',
    'topo8_BDTG.xml',
    'topo8_dau3trans_BDTG.xml,'
]

configSetup ='''universe = vanilla
Executable = run.sh
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
Output = run_$(Cluster)_$(Process).stdout
Error = run_$(Cluster)_$(Process).stderr
Log = run_$(Cluster)_$(Process).log
'''
for f in xmlFiles:
    configSetup+='''
Arguments = %s
Initialdir = %s
Queue
''' % (f, f.replace(".xml", "_dir"))

with open("submit.jdl", "w") as ofile:
    ofile.write(configSetup)
    ofile.close()
