#!/bin/sh
#######################################################################
## - unit-test-b.sh
## - Assignment 2 - Metronome
## - @reference - item b) on page 7 of assignment specification
## - @author Gerald.Hurdle@AlgonquinCollege.com
#######################################################################


echo "********************************************"
echo "Unit Test [B] \n./metronome 120 2 4 &"
echo "Expected: metronome runs at [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]"
./metronome 120 2 4 &
sleep 15

#//TODO - refactor to Unit Test X
echo "********************************************"
echo quit > /dev/local/metronome
exit 0