%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Generate grey plots for TCJ opening angle and bisector orientation
% Panels C, D, E
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

%% =========================
% USER INPUT
%% =========================
filename = 'RFB_NbPts_0400_rng_2031_pot_ani_lim_320_relaxed_out_lasttimestep.txt';   % <-- your snapshot file

nbins_alpha = 20;
nbins_theta = 20;

alpha_min = 0;
alpha_max = pi;

theta_min = 0;
theta_max = pi/2;

rare_threshold = 1e-4;

%% =========================
% READ FILE
%% =========================
fid = fopen(filename, 'r');
if fid == -1
    error('Could not open file.');
end

alpha_all = [];
theta_all = [];

alpha_nuc = [];
theta_nuc = [];

readingVertices = false;

while ~feof(fid)
    line = strtrim(fgetl(fid));

    % Start vertices block
    if strcmp(line, 'Vertices')
        readingVertices = true;
        fgetl(fid); % skip header line
        continue;
    end

    % Stop at edges
    if strcmp(line, 'Edges')
        break;
    end

    if readingVertices && ~isempty(line)

        % Parse numeric columns directly
        data = textscan(line, ...
            '%f %f %f %f %f %f %f %f %f %f %f %f', ...
            'Delimiter', ' ', ...
            'MultipleDelimsAsOne', true);

        if isempty(data{1})
            continue;
        end

        % Opening angles alpha
        alpha = [data{4}, data{5}, data{6}];
        
        % Bisector orientations theta
        theta = [data{7}, data{8}, data{9}];
        
        % Nucleation flags
        nuc = [data{10}, data{11}, data{12}];
        
        % Store all TCJs
        alpha_all = [alpha_all; alpha(:)];
        theta_all = [theta_all; theta(:)];
        
        % Store only nucleation events
        for k = 1:3
            if nuc(k) == 1      % or == 1, depending on your file format
                alpha_nuc(end+1,1) = alpha(k);
                theta_nuc(end+1,1) = theta(k);
            end
        end
    end
end

fclose(fid);

%% =========================
% BIN DEFINITIONS
%% =========================
alpha_edges = linspace(alpha_min, alpha_max, nbins_alpha + 1);
theta_edges = linspace(theta_min, theta_max, nbins_theta + 1);

alpha_centers = 0.5 * (alpha_edges(1:end-1) + alpha_edges(2:end));
theta_centers = 0.5 * (theta_edges(1:end-1) + theta_edges(2:end));

%% =========================
% PANEL C: Fraction of TCJs
%% =========================
H_C = histcounts2(theta_all, alpha_all, theta_edges, alpha_edges);
H_Cnorm = H_C / sum(H_C(:));

figure;
imagesc(theta_centers, alpha_centers, H_Cnorm');
axis xy;
pi_axis_ticks();
colormap(gray);
colorbar;
xlabel('Bisector Orientation \theta (rad)');
ylabel('Opening Angle \alpha (rad)');
title(sprintf('C: Fraction of TCJs, n = %d', sum(H_C(:))));


%% =========================
% PANEL D: Fraction of nucleation events
% (placeholder: identical to C for now)
%% =========================
H_D = histcounts2(theta_nuc, alpha_nuc,theta_edges, alpha_edges);

H_Dnorm = H_D / sum(H_D(:));

figure;
imagesc(theta_centers, alpha_centers, H_Dnorm');
axis xy;
pi_axis_ticks();
colormap(gray);
colorbar;
xlabel('Bisector Orientation \theta (rad)');
ylabel('Opening Angle \alpha (rad)');
title(sprintf('D: Fraction of aSF Nucleation Events, n = %d', sum(H_D(:))));

%% =========================
% PANEL E: Relative nucleation rate
%% =========================
H_E = H_D ./ H_C;

% Mask rarely observed TCJs
rare_mask = H_C < rare_threshold;
H_E(rare_mask) = NaN;

figure;
imagesc(theta_centers, alpha_centers, H_E');
axis xy;
pi_axis_ticks();
colormap(gray);
colorbar;
xlabel('Bisector Orientation \theta (rad)');
ylabel('Opening Angle \alpha (rad)');
title('E: Relative Nucleation Probability');

hold on;
contour(theta_centers, alpha_centers, rare_mask', ...
        [1 1], 'k--', 'LineWidth', 1);
hold off;

%% =========================
% DONE
%% =========================
disp('Grey plots generated successfully.');



function pi_axis_ticks()

    xticks([0 pi/8 pi/4 3*pi/8 pi/2]);
    xticklabels({'0','$\pi/8$','$\pi/4$','$3\pi/8$','$\pi/2$'});

    yticks([0 pi/4 pi/2 3*pi/4 pi]);
    yticklabels({'0','$\pi/4$','$\pi/2$','$3\pi/4$','$\pi$'});

    set(gca,'TickLabelInterpreter','latex')

end

