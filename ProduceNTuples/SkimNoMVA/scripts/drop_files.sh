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

# Do my own business
root -b <<- _EOF_
.L dropEmptyFiles.C
dropEmptyFiles("$here/input_files.list")
.q
_EOF_

# rename output
echo "<<<<<<<<<< Renamed to good_input_files.list >>>>>>>>>>"
mv good/input_files.list good_input_files.list

echo "<<<<<<<<<< Renamed to bad_input_files.list >>>>>>>>>>"
mv bad/input_files.list bad_input_files.list

# List current directory
echo "<<<<<<<<<< Listing the current directory >>>>>>>>>>"
ls

# Give a few examples
echo "<<<<<<<<<<< Content of good inputs >>>>>>>>>>"
head good_input_files.list
echo "......"
tail good_input_files.list

echo "<<<<<<<<<<< Content of bad inputs >>>>>>>>>>"
head bad_input_files.list
echo "......"
tail bad_input_files.list

# All done
echo "<<<<<<<<<<< Done Everything >>>>>>>>>>"
