#!/bin/bash

# This script runs unit tests for a quest.
# All maps in the maps/tests/ directory are considered tests.
# Usage: ./run_tests.sh quest_path

if [ $# != 1 ];
then
  echo "Usage: $0 quest_path"
  exit 1
fi

quest_path="$1"

find "${quest_path}/data/maps/tests" -type f -regex ".*/data/maps/tests/.*" -name "test_*.dat" -printf "%P\n" | \
while read -r map; do
  echo "-- Test: lua-map ${map%%.*} --"
  echo solarus-test-lua-map -no-audio -no-video -turbo=yes -lua-console=no -map="tests/${map%%.*}" "${quest_path}"
  solarus-test-lua-map -no-audio -no-video -turbo=yes -lua-console=no -map="tests/${map%%.*}" "${quest_path}"
  if [ $? -eq 0 ]; then
    echo "OK"
  else
   echo "FAIL"
   exit 1
  fi
done

