#!/usr/bin/bash
# set up variables for locating files

export OPENHF2020TOP=`git rev-parse --show-toplevel`
echo
echo '$OPENHF2020TOP is set to' ${OPENHF2020TOP}
if [[ -z $1 ]]; then
  echo
  echo "If you want to use compiled libraries under" '${OPENHF2020TOP}/Utilities/lib'
  echo "inside ROOT prompt, you may want to modifiy the value of" '${LD_LIBRARY_PATH}.'
  echo "To make it in effect, run"
  echo "    source setup.sh --enable-lib"
  echo "See more in Utilities/README.org"
fi
echo

if [[ $1 == "--enable-lib" ]]; then
  export LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities/lib:${LD_LIBRARY_PATH}
  export LD_LIBRARY_PATH=${OPENHF2020TOP}/Utilities:${LD_LIBRARY_PATH}

  echo "Prepend ${OPENHF2020TOP}/Utilities/lib to" '$LD_LIBRARY_PATH, for root_v6.22'
  echo "Prepend ${OPENHF2020TOP}/Utilities/to" '$LD_LIBRARY_PATH, for root_v6.06'
  echo
  echo "Warning: NEVER run any cmsRun scripts after sourcing this script"
  echo '         ${LD_LIBRARY_PATH}' "has been contaminated"
  echo

fi
