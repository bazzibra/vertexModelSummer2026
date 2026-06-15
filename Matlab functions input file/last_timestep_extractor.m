%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Extract SECOND-TO-LAST Timestep from Large Simulation Output
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc;

%% =========================
% USER INPUT
%% =========================
inputFile  = '12_out.txt';
outputFile = '12_lasttimestep.txt';

%% =========================
% READ FILE
%% =========================
disp('Scanning massive file...');

fidIn = fopen(inputFile,'r');
if fidIn == -1
    error('Cannot open input file.');
end

% Rolling buffers
previousTimestepBuffer = {};
currentTimestepBuffer  = {};

while ~feof(fidIn)

    line = fgetl(fidIn);

    % Detect start of a timestep
    if startsWith(line,'INTEGRATORNAME')

        % Shift buffers forward
        previousTimestepBuffer = currentTimestepBuffer;
        currentTimestepBuffer = {};

    end

    % Store line in current timestep
    currentTimestepBuffer{end+1,1} = line;

end

fclose(fidIn);

%% =========================
% SAFETY CHECK
%% =========================
if isempty(previousTimestepBuffer)
    error('File contains only one timestep.');
end

%% =========================
% WRITE OUTPUT
%% =========================
disp('Writing second-to-last timestep...')

fidOut = fopen(outputFile,'w');
if fidOut == -1
    error('Cannot create output file.');
end

for i = 1:length(previousTimestepBuffer)
    fprintf(fidOut,'%s\n',previousTimestepBuffer{i});
end

fclose(fidOut);

disp('--- Extraction Complete ---')
disp(['Saved: ', outputFile])
