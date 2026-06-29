% T1 Plot 

function T1Plot(inputFileName)

clc; close all;

%% =========================
% USER INPUT
%% =========================   % <-- your snapshot file

% 

underScoreIndexing = strfind(inputFileName,'_');
relevantFilePortion = inputFileName(1:underScoreIndexing(9)); 
folderOutputName = relevantFilePortion + "visualization";


scriptDir = fileparts(mfilename('fullpath'));
projectRoot = fileparts(scriptDir);

inputDir = fullfile(projectRoot,'input_files',inputFileName);
