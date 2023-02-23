#!/bin/bash
sys_info=$(cat /etc/system-release)
if [[ "$sys_info" != "CentOS Stream release 9" ]]; then
  echo "System release is not CentOS Stream release 9. ctags may not be proper one."
fi
files=$(find Utilities \( -name *.h -o -name *.hxx -o -name *.cpp -o -name *.cc -o -name *.cxx \) ! -name Linkdef.h)
files+=" "$(find Training/Macros/skimTree.cc -type f)
files+=" "$(find Training/include -type f)
files+=" "$(find Training/src -type f)
ctags -e ${files[@]}
echo "Made TAGS for "${files[@]}
