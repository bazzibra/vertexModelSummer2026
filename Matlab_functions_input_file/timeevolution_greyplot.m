%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% MOVING GREY PLOT: Alpha vs Theta over time (Movie + Frames)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

filename = 'RFB_NbPts_0200_rng_1300_pot_ani_lim_320_relaxed_out.txt';

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

%% BIN SETUP
alpha_edges = linspace(alpha_min, alpha_max, nbins_alpha+1);
theta_edges = linspace(theta_min, theta_max, nbins_theta+1);

alpha_centers = 0.5*(alpha_edges(1:end-1)+alpha_edges(2:end));
theta_centers = 0.5*(theta_edges(1:end-1)+theta_edges(2:end));

%% OUTPUT
output_folder = 'grey_frames';
if ~exist(output_folder,'dir')
mkdir(output_folder);
end

video = VideoWriter('GreyPlotEvolution.mp4','MPEG-4');
video.FrameRate = 10;
open(video);

%% VARIABLES
readingVertices = false;
alpha_all = [];
theta_all = [];

frame_id = 1;
current_time = 0;

figure;

%% =========================
% MAIN LOOP
%% =========================
while ~feof(fid)


line = strtrim(fgetl(fid));

if isempty(line)
    continue
end

% Detect time
if contains(line,'CURRENTTIME')
    tokens = split(line);
    current_time = str2double(tokens{2});
end

% Detect vertex block
if contains(line,'Anglestheta1')
    readingVertices = true;
    alpha_all = [];
    theta_all = [];
    continue
end

% End of timestep → plot
if strcmp(line,'Edges')

    if ~isempty(alpha_all)

        clf

        % Histogram
        H = histcounts2(theta_all, alpha_all, ...
            theta_edges, alpha_edges);

        if sum(H(:)) > 0
            H = H / sum(H(:));
        end

        imagesc(theta_centers, alpha_centers, H')
        axis xy
        colormap(gray)
        colorbar

        xlabel('\theta (Bisector Orientation)')
        ylabel('\alpha (Opening Angle)')
        title(['t = ', num2str(current_time)])

        drawnow

        % Save frame
        frame = getframe(gcf);
        writeVideo(video, frame);

        frame_name = sprintf('frame_%04d.png', frame_id);
        imwrite(frame.cdata, fullfile(output_folder,frame_name));

        frame_id = frame_id + 1;
    end

    readingVertices = false;
    continue
end

%% READ VERTICES
if readingVertices

    data = textscan(line,'%f');

    if length(data{1}) < 9
        continue
    end

    vals = data{1};

    alpha = vals(4:6);
    theta = vals(7:9);

    % IMPORTANT: filter bad values
    valid = (alpha >= 0 & alpha <= pi) & ...
            (theta >= 0 & theta <= pi/2);

    alpha_all = [alpha_all; alpha(valid)];
    theta_all = [theta_all; theta(valid)];
end


end

fclose(fid);
close(video);

disp('Movie + frames created successfully.');
