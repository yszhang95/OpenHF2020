# MCStudies
Macros under the current path are used to study MC templates

## Fit gen-matched lambdaC candidates
- Read mass from trees in `files` with `convertTreeToRooDS.C`.
```
cd macros
root -b <<- _EOF_
.L convertTreeToRooDS.C+
convertTreeToRooDS("../file/test");
.q
_EOF_
```
- Read the `RooDataSet` from last step and fit it with `MCLamC.C`.
