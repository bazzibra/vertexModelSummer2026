%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOVING GREY PLOT: Relative Nucleation Probability (Panel E)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

filename = 'RFB_NbPts_1000_rng_2000_pot_ani_lim_320_relaxed_out.txt';

fid = fopen(filename,'r');
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

rare_threshold = 5;    % minimum TCJ count per bin

%% BIN SETUP
alpha_edges = linspace(alpha_min, alpha_max, nbins_alpha+1);
theta_edges = linspace(theta_min, theta_max, nbins_theta+1);

alpha_centers = 0.5*(alpha_edges(1:end-1)+alpha_edges(2:end));
theta_centers = 0.5*(theta_edges(1:end-1)+theta_edges(2:end));

%% OUTPUT
output_folder = 'relative_nucleation_frames';

if ~exist(output_folder,'dir')
    mkdir(output_folder);
end

video = VideoWriter('RelativeNucleationEvolution.mp4','MPEG-4');
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

            colormap(gray)
            colorbar

            xlabel('\theta (Bisector Orientation)')
            ylabel('\alpha (Opening Angle)')

            title(sprintf( ...
                'Number of Nucleation Sites, t = %.0f', ...
                current_time))

            drawnow

            %% Save frame to movie
            
            %frame = getframe(gcf);
            
            %

            fig = gcf; 
            
            imagewd = getframe(fig);
            
            writeVideo(video, imagewd);
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

disp('Relative nucleation movie + frames created successfully.');

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