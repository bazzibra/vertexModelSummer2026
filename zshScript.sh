#!/bin/bash

# Aliases for quick directory traversal

alias matlab='/Applications/MATLAB_R2025b.app/bin/matlab -batch'
alias toFunctions='cd Matlab_functions_input_file'
alias inputfiles='cd input_files'


# Functions for readability and to reduce code duplications
extractTimeStep () {
    echo extracting last time step
    toFunctions
    matlab "last_timestep_extractor $1 $2" 
    cd .. 
    if [[ "$2" == 0 ]]; then
        findInput 2
    elif [[ "$2" == 1 ]]; then
        findInput 3
    fi
}

findInput () {
    echo Finding Initialization Input
    inputfiles
    case "$1" in    
        # For finding the initialization file
        0)
            currRecent=$(ls -t *.txt | head -n 1)
            ;;
        # For finding the first out file from running the base script
        1)
            currRecent=$(ls -t *_out.txt | head -n 1)
            ;;
        # For finding the relaxed output for the base run
        2) 
            currRecent=$(ls -t *relaxed.txt | head -n 1)
            ;;
        # For finding the most output of the dynamic file
        3) 
            currRecent=$(ls -t *relaxed_out.txt | head -n 1)
            ;;
        4) 
            currRecent=$(ls -t *lasttimestep.txt | head -n 1)
            ;;
    esac
    cd .. 
}

runVisualization(){
    matlab "CreateMovie_m $1 $2"
    matlab "generategreyplots $3"
    matlab "greyplotcompletemovie $3"
    matlab "tensionAnglePlots $1" 
    cd ..
}

fileRename(){
    # Extract filename and extension
    local ref = $1


    Relaxation = 'relaxed'

    echo Renaming Files
    inputfiles
    case "$2" in
        0) 
            local tempHolder="${1//out_lasttimestep/relaxedbrotato}"
            mv "$1" "$tempHolder"
            $1 = "$tempHolder"
            ;;
    esac
}

toFunctions

# Generate initialization file and capture output

matlab "nucleation_initalization_file"

cd .. 

findInput 0

# Compile and run the base model to relax the sample, again, capture output and grab the last time step

make executeBase <<< $currRecent

findInput 1

echo $currRecent

extractTimeStep $currRecent 0

# Run the dynamic model, and once again capture output and grab the last time step

make executeDynamic <<< $currRecent

findInput 3

dynamicOutput="$currRecent"

extractTimeStep $currRecent 1 

# Capture that last time step as well and run visualization files

findInput 4

finalStep="$currRecent"

toFunctions

runVisualization $dynamicOutput 0 $finalStep

pkill -f MathWorksServiceHost || killall -9 matlab

exit 0