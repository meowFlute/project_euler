#!/bin/bash

# Script: add_problem.sh
# Author: Scott Christensen
# Description:
#   I want to automate the repetitive tasks included in adding a new problem
#   to my project_euler project. These steps include:
#	1) Edit main.c
#	    - add a function pointer to problem_func_ptrs array
#	2) Edit project_euler.h to include new header file
#	3) create new header file in problems/ folder
#	    - off the top of my head I could see this as two commands
#		a) cp problem_001.h problem_xxx.h
#		b) sed to find a replace 001 with xxx
#	4) create a new .c file that includes a basic skeleton
#	    - includes stdio.h and probem_xxx.h
#	    - function implementation with printf("Problem XXX: problem not yet solved\n");
#   Additional requirements:
#	- Portability:
#	    - I won't use the colorization functions I have in my runcom folder so this stays more portable
#	    - I'll just use common functions so this runs on any linux machine

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
    echo "Usage: ${BASEDIR}/add_problem.sh [-p problem_number]"
}

print_help() {
    print_usage
    echo
    echo "OPTION DESCRIPTIONS:"
    echo "	-p	(REQUIRED) Problem number. Make this 1 larger than HIGHEST_PROBLEM_COMPLETED in main.c"
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
    echo ERROR:file ${BASEDIR}/problems/${header_filename} already exists!
    echo aborting...
    exit 1
fi
cp "${BASEDIR}/problems/problem_xxx.h" "${BASEDIR}/problems/${header_filename}"
sed -i 's/xxx/'$standardized_problem_number'/g' "${BASEDIR}/problems/${header_filename}"

# Create problems/problem_xxx.c`
cfile_filename=$( printf "problem_%03d.c" $problem_number )
if [ -f "${BASEDIR}/problems/${cfile_filename}" ]
then
    echo ERROR:file ${BASEDIR}/problems/${cfile_filename} already exists!
    echo aborting...
    exit 1
fi
cp "${BASEDIR}/problems/problem_xxx.c" "${BASEDIR}/problems/${cfile_filename}"
sed -i 's/xxx/'$standardized_problem_number'/g' "${BASEDIR}/problems/${cfile_filename}"

# Add header to project_euler.h
one_less=$(($problem_number-1))
standardized_one_less=$( printf "%03d" $one_less )
sed -i -e '/'$standardized_one_less'/a\' -e '#include "problems/'$header_filename'"' "${BASEDIR}/project_euler.h"

# Finally, edit the main file
# We have to update HIGHEST_PROBLEM_COMPLETED because that determines the length of the function pointer array
sed -i 's/COMPLETED   '$one_less'/COMPLETED   '$problem_number'/' "${BASEDIR}/main.c"
# That means all we need to do is add a line for a function pointer similar to above
sed -i -e '/problem_'$standardized_one_less'/a\' -e '    problem_'$standardized_problem_number'' "${BASEDIR}/main.c"
# don't forget you have to add a comma to what was previously the last line of an array
sed -i 's/problem_'$standardized_one_less'/problem_'$standardized_one_less',/' "${BASEDIR}/main.c"
