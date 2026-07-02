%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOVING GREY PLOT: Relative Nucleation Probability (Panel E)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function greyplotcompletemovie(inputFileName)

    close all;

    scriptDir = fileparts(mfilename('fullpath'));
    projectRoot = fileparts(scriptDir);
    inputDir = fullfile(projectRoot,"input_files",inputFileName);

    underScoreIndexing = strfind(inputFileName,'_');
    relevantFilePortion = inputFileName(1:underScoreIndexing(9)); 
    folderOutputName = relevantFilePortion + "visualization";

    fid = fopen(inputDir,'r');
    if fid == -1
        error('Could not open file.');
    end
    
    %% PARAMETERS
    nbins_alpha = 20;
    nbins_theta = 20;
    
    alpha_min = 0;
    alpha_max = pi;
    
    theta_min = 0;
    theta_max = pi/2;
    
    %% BIN SETUP
    alpha_edges = linspace(alpha_min, alpha_max, nbins_alpha+1);
    theta_edges = linspace(theta_min, theta_max, nbins_theta+1);
    
    alpha_centers = 0.5*(alpha_edges(1:end-1)+alpha_edges(2:end));
    theta_centers = 0.5*(theta_edges(1:end-1)+theta_edges(2:end));

    theta_horizontal = [];
    theta_vertical = [];
    time = [];

    
    %% OUTPUT
    outputDir = fullfile(projectRoot,'visualization_files',folderOutputName);
    fileName = fullfile(outputDir,"RelativeNucleationEvolution.mp4"); 
    if ~exist(outputDir,'dir')
        mkdir(outputDir);
    end
    
    video = VideoWriter(fileName,'MPEG-4');
    video.FrameRate = 10;
    open(video);
    
    %% VARIABLES
    readingVertices = false;
    
    alpha_all = [];
    theta_all = [];
    
    alpha_nuc = [];
    theta_nuc = [];
    
    frame_id = 1;
    current_time = 0;
    
    fig = figure('Color','w');
    fig.Position = [100 100 800 600]; 
    
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
            current_time = str2double(tokens{2});
    
        end
    
        %% Start of vertex block
        if contains(line,'Anglestheta1')
    
            readingVertices = true;
    
            alpha_all = [];
            theta_all = [];
    
            alpha_nuc = [];
            theta_nuc = [];
    
            continue
    
        end
    
        %% End of timestep -> make frame
        if strcmp(line,'Edges')
    
            if ~isempty(alpha_all)
    
                clf
    
                %% Panel C histogram
                H_C = histcounts2( ...
                    theta_all, alpha_all, ...
                    theta_edges, alpha_edges);
    
                %% Panel D histogram
                H_D = histcounts2( ...
                    theta_nuc, alpha_nuc, ...
                    theta_edges, alpha_edges);
    
                %% Panel E
                H_E = nan(size(H_C));
    
                valid_bins = H_C > 0;
    
                H_E(valid_bins) = ...
                    H_D(valid_bins) ./ H_C(valid_bins);
    
                %% Mask poorly sampled bins
                %rare_mask = H_C < rare_threshold;
                %H_E(rare_mask) = NaN;
    
                %% Plot
                imagesc(theta_centers, alpha_centers, H_D')
    
                axis xy
                pi_axis_ticks()
    
                colormap(gray)
                colorbar
    
                xlabel('\theta (Bisector Orientation)')
                ylabel('\alpha (Opening Angle)')
    
                title(sprintf( ...
                    'Number of Nucleation Sites, t = %.0f', ...
                    current_time))
    
                drawnow
    
                %% Save frame to movie
                frame = getframe(gcf);
                writeVideo(video, frame);

                valid = theta_nuc <= (theta_max-theta_min)/2;

                theta_horizontal = [theta_horizontal, length(theta_nuc) - sum(valid)];
                theta_vertical = [theta_vertical, sum(valid)];
                time = [time, current_time];
    
                frame_id = frame_id + 1;
    
            end
    
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
            alpha = vals(4:6);
            theta = vals(7:9);
            nuc   = vals(10:12);
    
            %% Keep only valid values
            valid = ...
                (alpha >= 0 & alpha <= pi) & ...
                (theta >= 0 & theta <= pi/2);
    
            %% Store all TCJs
            alpha_all = [alpha_all; alpha(valid)];
            theta_all = [theta_all; theta(valid)];
    
            %% Store nucleation events only
            for k = 1:3
    
                if valid(k) && nuc(k)==1
    
                    alpha_nuc(end+1,1) = alpha(k);
                    theta_nuc(end+1,1) = theta(k);
    
                end
    
            end
    
        end
    
    end
    
    %% Cleanup
    fclose(fid);
    close(video);
    
    fig = figure();
    
    plot(time, theta_vertical, 'b-', 'LineWidth', 1.5)
    hold on
    plot(time, theta_horizontal, 'r--', 'LineWidth', 1.5)
    hold off
    
    xlabel('Time (s)')
    ylabel('Count')   % or degrees if appropriate
    title('Total Nucleated Vertices Split by \theta Over Time')
    legend('\theta <= pi/2', '\theta > pi/2', 'Location', 'best')
    grid on

    nameFig = 'NucleationSplitByAngleOverTime';
    outputDir = fullfile(projectRoot,"visualization_files",folderOutputName,nameFig); 
    saveas(fig,outputDir,'png'); 
    
    disp('Relative nucleation movie + frames + split nucleation plot created successfully.');

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % AXIS TICKS
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    function pi_axis_ticks()
    
        xticks([0 pi/8 pi/4 3*pi/8 pi/2]);
        xticklabels({'0','$\pi/8$','$\pi/4$','$3\pi/8$','$\pi/2$'});
    
        yticks([0 pi/4 pi/2 3*pi/4 pi]);
        yticklabels({'0','$\pi/4$','$\pi/2$','$3\pi/4$','$\pi$'});
    
        set(gca,'TickLabelInterpreter','latex')
    
    end
end