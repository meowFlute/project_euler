#!/bin/bash

# Script: remove_problem.sh
# Author: Scott Christensen
# Description:
#   I want to automate the repetitive tasks included in removing a problem

# Let's locate the absolute path of this script first
# I got this from https://stackoverflow.com/questions/59895/how-do-i-get-the-directory-where-a-bash-script-is-located-from-within-the-script/246128#246128
SOURCE=${BASH_SOURCE[0]}
pushd . > /dev/null # stores off where we were before all that cd'ing, might not be necessary
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
BASEDIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
popd > /dev/null # This just gets us back to where we were before all that cd'ing
# so now $BASEDIR should be the directory this script is being run from no matter how it was invoked

print_usage() {
    echo "Usage: ${BASEDIR}/remove_problem.sh [-p problem_number]"
}

print_help() {
    print_usage
    echo
    echo "OPTION DESCRIPTIONS:"
    echo "	-p	(REQUIRED) Problem number. Make this equal to HIGHEST_PROBLEM_COMPLETED in main.c"
    echo "	-h	print this help"
}

while getopts ':p:' opt
do
    case "$opt" in
	p)  problem_number=$OPTARG
	    ;;
	h)  print_help
	    exit 1
	    ;;
	*)  print_help
	    exit 1
	    ;;
    esac
done

# verify option inputs
# is the argument for problems empty?
if [ -z "${problem_number}" ]
then
    echo "INPUT ERROR: option -p is required"
    print_help
    exit 1
fi
# is the problem number not a number?
[ -n "$problem_number" ] && [ "$problem_number" -eq "$problem_number" ] 2>/dev/null
if [ $? -ne 0 ]
then
   echo "INPUT ERROR: Please verify your inputs meet the following rules"
   print_help
   exit 1
fi

# Start with creating the files listed in steps 3 and 4 in the script description
# Create problems/problem_xxx.h 
header_filename=$( printf "problem_%03d.h" $problem_number )
standardized_problem_number=$( printf "%03d" $problem_number )
if [ -f "${BASEDIR}/problems/${header_filename}" ]
then
    echo removing ${BASEDIR}/problems/${header_filename}
    rm "${BASEDIR}/problems/${header_filename}"
else
    echo ERROR:file ${BASEDIR}/problems/${header_filename} does not exist!
    echo aborting...
    exit 1
fi

# Create problems/problem_xxx.c`
cfile_filename=$( printf "problem_%03d.c" $problem_number )
if [ -f "${BASEDIR}/problems/${cfile_filename}" ]
then
    echo removing ${BASEDIR}/problems/${cfile_filename}
    rm "${BASEDIR}/problems/${cfile_filename}"
else
    echo ERROR:file ${BASEDIR}/problems/${cfile_filename} does not exist!
    echo aborting...
    exit 1
fi

# Add header to project_euler.h
one_less=$(($problem_number-1))
standardized_one_less=$( printf "%03d" $one_less )
echo "${BASEDIR}/project_euler.h: removing reference to ${header_filename}"
sed -i "/${header_filename}/d" "${BASEDIR}/project_euler.h"
echo "${BASEDIR}/project_euler.h: changing HIGHEST_PROBLEM_COMPLETED to ${one_less} and SUB_VERSION to 0"
sed -i 's/COMPLETED   '$problem_number'/COMPLETED   '$one_less'/' "${BASEDIR}/project_euler.h"
sed -i 's/SUB_VERSION                 [0-9]\+/SUB_VERSION                 0/' "${BASEDIR}/project_euler.h"

# Finally, edit the main file
# That means all we need to do is add a line for a function pointer similar to above
echo "${BASEDIR}/main.c: removing function pointer 'problem_${standardized_problem_number}'"
sed -i '/problem_'$standardized_problem_number'/d' "${BASEDIR}/main.c"
# don't forget you have to remove a comma from what is now the last line of an array
sed -i 's/problem_'$standardized_one_less',/problem_'$standardized_one_less'/' "${BASEDIR}/main.c"
