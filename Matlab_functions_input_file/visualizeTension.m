%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Generate plots to visualize the tension distribution across cells
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function visualizeTension(inputFileName)

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

%% =========================
% USER INPUT
%% =========================
  % <-- your snapshot file


%% =========================
% READ FILE
%% =========================
fid = fopen(inputDir, 'r');
if fid == -1
    error('Could not open file.');
end

tensions = [];

readingEdges = false;

i = 1;
while ~feof(fid)
    line = strtrim(fgetl(fid));

    % Start vertices block
    if strcmp(line, 'Edges')
        readingEdges = true;
        fgetl(fid); % skip header line
        continue;
    end

    % Stop at edges
    if strcmp(line, 'Cells')
        break;
    end

    if readingEdges && ~isempty(line)

        % Parse numeric columns directly
        data = textscan(line, ...
            '%f %f %f %f %f %f %f %f %f %f', ...
            'Delimiter', ' ', ...
            'MultipleDelimsAsOne', true);

        if isempty(data{1})
            continue;
        end

        % Opening angles alpha
        if data{6} ~= 0
            tensions(i) = data{6};
            i = i + 1;
        end
        
    end
end

fig = figure;
histogram(tensions, 50);
xlabel('Edge Tension');
ylabel('Count');
title('Tension Distribution Across All Edges');

outputDir = fullfile(projectRoot,"visualization_files",folderOutputName,"visualizeTension"); 
saveas(fig,outputDir,'png'); 

