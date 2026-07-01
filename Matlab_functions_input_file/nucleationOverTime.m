function nucleationOverTime(inputFileName)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOVING GREY PLOT: Relative Nucleation Probability (Panel E)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    underScoreIndexing = strfind(inputFileName,'_');
    relevantFilePortion = inputFileName(1:underScoreIndexing(9)); 
    folderOutputName = relevantFilePortion + "visualization";
    
    
    scriptDir = fileparts(mfilename('fullpath'));
    projectRoot = fileparts(scriptDir);
    
    inputDir = fullfile(projectRoot,'input_files',inputFileName);

    fid = fopen(inputDir,'r');
    if fid == -1
        error('Could not open file.');
    end
    

    
    
    %% VARIABLES
    readingVertices = false;
    
    time = [];
    totalnuc = [];
    nuc = 0;
   
    
    %% =========================
    % MAIN LOOP
    %% =========================
    while ~feof(fid)
    
        line = strtrim(fgetl(fid));
    
        if isempty(line)
            continue
        end
    
        %% Read time
        if contains(line,'CURRENTTIME')
    
            tokens = split(line);
            time = [time, str2double(tokens{2})];
            totalnuc = [totalnuc, nuc];
            nuc = 0;
    
        end
    
        %% Start of vertex block
        if contains(line,'Anglestheta1')
    
            readingVertices = true;
    
            continue
    
        end
    
        %% End of timestep -> make frame
        if strcmp(line,'Edges')
    
            readingVertices = false;
            continue
    
        end
    
        %% Read vertex data
        if readingVertices
    
            vals = sscanf(line,'%f');
    
            if numel(vals) < 12
                continue
            end
    
            %% Extract quantities
            nuc = nuc + sum(vals(10:12));
    
    
        end
    
    end

    
    fig = figure();
    plot(time, totalnuc, 'LineWidth', 1)
    
    xlim([0, max(time)])
    ylim([0, 1.1*max(totalnuc)])
    
    title('Number of Nucleations Over Time')
    xlabel('Time')
    ylabel('Number of Nucleations')
    
    grid on
    box on
    
    disp('Nucleation over time plot created successfully.');
    nameFig = 'nucleationOverTimeGraph';
    outputDir = fullfile(projectRoot,"visualization_files",folderOutputName,nameFig); 
    saveas(fig,outputDir,'png'); 


    
end