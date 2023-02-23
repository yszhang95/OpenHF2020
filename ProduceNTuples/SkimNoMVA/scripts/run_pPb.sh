#!/bin/bash

# A simple locator for the top directory
here=`pwd`

echo "<<<<<<<<<< Starting the script >>>>>>>>>>"
echo "<<<<<<<<<< Checking input sandbox >>>>>>>>>>"
tar ztf input_files.tar.gz

# I must create a file containing the paths to input files.
# I do not know how to make use of CRAB utilities
echo "<<<<<<<<<< Generating list of inputs >>>>>>>>>>"
python3 -c "import PSet; print ('\n'.join(list(PSet.process.source.fileNames)))" > input_files.list
inputfiles=${here}/input_files.list
echo

echo "<<<<<<<<<< Extracting scripts >>>>>>>>>>"
tar zxf OpenHF2020.tar.gz
mv OpenHF2020 ${CMSSW_BASE}/src/
cd ${CMSSW_BASE}/src/OpenHF2020

echo "Now the directory is ${PWD}"

echo "<<<<<<<<<< Compiling >>>>>>>>>>"
source setup.sh
make tree-reader

OLD_LD=${LD_LIBRARY_PATH}
OLD_INC=${ROOT_INCLUDE_PATH}
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:${OPENHF2020TOP}:`HepMC3-config --includedir`

echo "Executing skimTree.py"
cat << _EOF_ > branches.list
cand_mass
cand_pT
cand_y
cand_phi
cand_massDau0
cand_etaDau1
cand_phiDau1
cand_pTDau1
trk_dau1_dEdxRes
cand_angle3D
cand_dau0_angle3D
cand_dau0_decayLength3D
cand_dau0_decayLengthError3D
cand_dau0_dca
cand_Ntrkoffline
trigBit_2
trigBit_4
filterBit_4
filterBit_5
_EOF_
${OPENHF2020TOP}/Training/Macros/skimTree.py -i ${inputfiles} \
                --pTMin 1.9 --wantAbort \
                --selectDeDx \
                --branches=branches.list
mycode=$?
LD_LIBRARY_PATH=${OLD_LD}
ROOT_INCLUDE_PATH=${OLD_INC}

echo "Executed skimTree.py and exit code is ${mycode}"

echo "<<<<<<<<<< Move the output to top directory >>>>>>>>>>"
outputFile=input_files_lambdacAna_AllEntries_pT1p9to20p0_yAbs1p0_absMassDiff20p000_LamCKsP.root
mv $outputFile $here/
mycode2=$?
echo "Exit code of mv is ${mycode2}"

# Move back
cd $here
echo "<<<<<<<<<< Moved backed to the top directory >>>>>>>>>>"
echo "<<<<<<<<<< Listing the top directory >>>>>>>>>>"
ls
echo "Ended Listing the directory"

echo "<<<<<<<<<< Futher skim dz1p0 and Ks, drop branches >>>>>>>>>>"
root -q skimNTuple.C\(\"${outputFile}\"\)
mycode3=$?

# At the end of the script modify the FJR

# Error code
exitCode=0
exitMessage=
errorType=
if [[ $mycode -ne 0 ]]; then
    exitCode=${mycode}
    exitMessage="skimTree.py has something wrong"
    errorType="My arbitrary error type"
elif [[ $mycode2 -ne 0 ]]; then
    exitCode=${mycode2}
    exitMessage="mv has something with ${outputFile}"
    errorType="My arbitrary error type"
elif [[ $mycode3 -ne 0 ]]; then
    exitCode=${mycode3}
    exitMessage="skimNTuple.C has something with ${outputFile}"
    errorType="My arbitrary error type"
fi

# Dump FrameworkJobReport.xml
echo "<<<<<<<<<< Content of FrameworkJobReport.xml >>>>>>>>>>"
cat FrameworkJobReport.xml

if [[ ${exitCode} -eq 0 ]]; then
    exit 0
fi

if [ -e FrameworkJobReport.xml ]
then
    cat << EOF > FrameworkJobReport.xml.tmp
<FrameworkJobReport>
<FrameworkError ExitStatus="$exitCode" Type="$errorType" >
$exitMessage
</FrameworkError>
EOF
    tail -n+2 FrameworkJobReport.xml >> FrameworkJobReport.xml.tmp
    mv FrameworkJobReport.xml.tmp FrameworkJobReport.xml
else
    cat << EOF > FrameworkJobReport.xml
<FrameworkJobReport>
<FrameworkError ExitStatus="$exitCode" Type="$errorType" >
$exitMessage
</FrameworkError>
</FrameworkJobReport>
EOF
fi

# Dump FrameworkJobReport.xml
echo "<<<<<<<<<< New FrameworkJobReport.xml >>>>>>>>>>"
cat FrameworkJobReport.xml
