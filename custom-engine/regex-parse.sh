#!/bin/bash

pattern="(Bft =|KeyState =|TrigVals =|Player =)"
files="*.[hc]" # match all C header and code files

cat $files | grep -Eo "$pattern" #|| echo "Invalid file name(s)! Exiting..."

exit 0
