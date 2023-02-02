#!/bin/bash


# All things will be collected in simpleoutput.txt
touch simpleoutput.txt
# First section
# I want to know the format of input files
# they are in format of LFN or PFN?
# I do not think I get anything from job_input_file_list_*.txt
# when I do not give any chance to run cmsRun
# I have feedback from PSet.process.source.fileNames.

echo "First section" >> simpleoutput.txt

echo "================= Dumping Input files from txt    ===================="
echo "================= Dumping Input files from txt    ====================" >> simpleoutput.txt
cat job_input_file_list_*.txt
cat job_input_file_list_*.txt >> simpleoutput.txt
echo >> simpleoutput.txt

echo "================= Dumping Input files from python ===================="
echo "================= Dumping Input files from python ====================" >> simpleoutput.txt
python3 -c "import PSet; print ('\n'.join(list(PSet.process.source.fileNames)))"
python3 -c "import PSet; print ('\n'.join(list(PSet.process.source.fileNames)))" >> simpleoutput.txt

echo >> simpleoutput.txt

# generate input list
# assume I use the correct redirector; I still use LFN not PFN
python3 -c "import PSet; print ('\n'.join(list(PSet.process.source.fileNames)))" >> input.list


# From the first section, it seems that process.source.fileNames gives
# files given by config.Data.userInputFiles or files automatically
# sorted by inputDataset.
# With userInputFiles, it seems that the files are just what I give,
# in LFN or PFN?
# I am not interested in the case of inputDataset.

# Second section, I would like to check the time of reading the file
lastfile=`python3 -c "import PSet; print ('\n'.join(list(PSet.process.source.fileNames)))" | tail -n -1`
{ time LD_LIBRARY_PATH='' PYTHONPATH='' gfal-ls ${lastfile}; } 2> yousen_tmp.txt
echo "The second section, timing for finding single file"
echo "The second section, timing for finding single file" \
     >> simpleoutput.txt
cat yousen_tmp.txt >> simpleoutput.txt
rm yousen_tmp.txt

# Third section, I would like to check the CMSSW ENV in this script.
echo "The third section, CMSSW ENV"
if [ -z ${CMSSW_BASE+x} ]; then
    echo "No CMSSW ENV" >> simpleoutput.txt
fi

# Four section, compilation and run ROOT
echo "Fourth section, this is based on CMSSW ENV"
# this line below breaks without any tarball
# it seems that OpenHF2020 is not shipped
# need to create a tarball for OpenHF2020
# cd ${CMSSW_BASE}/src/OpenHF2020
# make clean &> yousen_tmp.log
# cat yousen_tmp.log >> simpleoutput.txt
# rm yousen_tmp.log
tar zxf OpenHF2020.tar.gz
mv OpenHF2020 ${CMSSW_BASE}/src/
here=${PWD}
cd ${CMSSW_BASE}/src/OpenHF2020
cp $here/simpleoutput.txt .
cp $here/input.list .

source setup.sh &> yousen_tmp.log
cat yousen_tmp.log >> simpleoutput.txt
rm yousen_tmp.log

make tree-reader &> yousen_tmp.log
cat yousen_tmp.log >> simpleoutput.txt
rm yousen_tmp.log

# this will lead file name too long;
# split it into several lines
# LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib \
#                ROOT_INCLUDE_PATH=\
#                ${ROOT_INCLUDE_PATH}:`HepMC3-config --includedir` \
#                Training/Macros/skimTree.py -i input.list --selectDeDx \
#     &> yousen_tmp.log
OLD_LD=${LD_LIBRARY_PATH}
OLD_INC=${ROOT_INCLUDE_PATH}
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OPENHF2020TOP}/Utilities/lib
ROOT_INCLUDE_PATH=${ROOT_INCLUDE_PATH}:`HepMC3-config --includedir`
# must be in the directory Training/Macros because
# the line LoadMacro("skimTree.cc") in skimTree.py
cd Training/Macros/
cp $here/input.list .
cp ../../simpleoutput.txt .
./skimTree.py -i input.list --selectDeDx &> yousen_tmp.log
cat yousen_tmp.log >> simpleoutput.txt
rm yousen_tmp.log
# unset LD_LIBRARY_PATH # this line will crash CMSSW
# unset ROOT_INCLUDE_PATH # this line will crash CMSSW
LD_LIBRARY_PATH=${OLD_LD}
ROOT_INCLUDE_PATH=${OLD_INC}

cp simpleoutput.txt $here

cd $here
# Transfer simpleoutput.txt; not necessary for real crab job.
xrdcp -f simpleoutput.txt root://eoscms.cern.ch//store/user/yousen/test/

# As a test, I want to use crab submit --dryrun.
# This dryun option only supports the script with cmsRun
# Please remove the following lines in reality because
# the plain ROOT files will fail the cmsRun.

cmsRun -j FrameworkJobReport.xml -p PSet.py
