# set up variables for locating files
export OPENHF2020TOP=`git rev-parse --show-toplevel`
export LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities/:${LD_LIBRARY_PATH}

echo
echo '$OPENHF2020TOP is set to' ${OPENHF2020TOP}
echo "Prepend ${OPENHF2020TOP}/Utilities/ to" '$LD_LIBRARY_PATH'
echo
echo "Warning: NEVER run any cmsRun scripts after sourcing this script"
echo '         ${LD_LIBRARY_PATH}' "has been contaminated"
echo
