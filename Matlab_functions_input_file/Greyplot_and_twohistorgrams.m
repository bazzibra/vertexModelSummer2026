%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Generate grey plots for TCJ opening angle and bisector orientation
% + Histogram of edge tensions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Greyplot_and_twohistorgrams(inputFileName)

clc; 



%% =========================
% USER INPUT
%% =========================

scriptDir = fileparts(mfilename('fullpath'));
projectRoot = fileparts(scriptDir);
filename = fullfile(projectRoot,'input_files',inputFileName); 

nbins_alpha = 20;
nbins_theta = 20;

alpha_min = 0;
alpha_max = pi;

theta_min = 0;
theta_max = pi/2;

rare_threshold = 1e-4;

%% =========================
% OPEN FILE
%% =========================
fid = fopen(filename,'r');

if fid == -1
    error('Could not open file.');
end

alpha_all = [];
theta_all = [];
tension_all = [];

readingVertices = false;
readingEdges = false;

%% =========================
% READ FILE
%% =========================
while ~feof(fid)

    line = strtrim(fgetl(fid));

    if isempty(line)
        continue
    end

    % Detect vertex header
    if contains(line,'Anglestheta1')
        readingVertices = true;
        readingEdges = false;
        continue
    end

    % Detect edges section
    if strcmp(line,'Edges')
        readingVertices = false;
        readingEdges = true;
        fgetl(fid); % skip edge header
        continue
    end

    % Stop at cells section
    if strcmp(line,'Cells')
        break
    end

    %% =====================
    % READ VERTEX DATA
    %% =====================
    if readingVertices

        data = textscan(line,'%f');

        if length(data{1}) < 9
            continue
        end

        values = data{1};

        % Opening angles
        alpha = values(4:6);

        % Bisector orientations
        theta = values(7:9);

        alpha_all = [alpha_all; alpha(:)];
        theta_all = [theta_all; theta(:)];

    end

    %% =====================
    % READ EDGE DATA
    %% =====================
    if readingEdges

        data = textscan(line,'%f');

        if length(data{1}) < 6
            continue
        end

        values = data{1};

        tension_val = values(6);

        tension_all = [tension_all; tension_val];

    end

end

fclose(fid);

%% =========================
% CHECK DATA
%% =========================
disp(['Total alpha values: ', num2str(length(alpha_all))])
disp(['Total theta values: ', num2str(length(theta_all))])
disp(['Total tensions: ', num2str(length(tension_all))])

%% =========================
% BIN DEFINITIONS
%% =========================
alpha_edges = linspace(alpha_min, alpha_max, nbins_alpha+1);
theta_edges = linspace(theta_min, theta_max, nbins_theta+1);

alpha_centers = 0.5*(alpha_edges(1:end-1) + alpha_edges(2:end));
theta_centers = 0.5*(theta_edges(1:end-1) + theta_edges(2:end));

%% =========================
% PANEL C : TCJ Distribution
%% =========================
H_C = histcounts2(theta_all,alpha_all,theta_edges,alpha_edges);

if sum(H_C(:)) > 0
    H_C = H_C / sum(H_C(:));
end

figure
imagesc(theta_centers,alpha_centers,H_C')
axis xy
colormap(gray)
colorbar

xlabel('Bisector Orientation \theta (rad)')
ylabel('Opening Angle \alpha (rad)')
title('C: Fraction of TCJs')

%% =========================
% PANEL D : Nucleation Events
% (Placeholder for now)
%% =========================
H_D = H_C;

figure
imagesc(theta_centers,alpha_centers,H_D')
axis xy
colormap(gray)
colorbar

xlabel('Bisector Orientation \theta (rad)')
ylabel('Opening Angle \alpha (rad)')
title('D: Fraction of aSF Nucleation Events')

%% =========================
% PANEL E : Relative Rate
%% =========================
H_E = H_D ./ H_C;

rare_mask = H_C < rare_threshold;

H_E(rare_mask) = NaN;

figure
imagesc(theta_centers,alpha_centers,H_E')
axis xy
colormap(gray)
colorbar

xlabel('Bisector Orientation \theta (rad)')
ylabel('Opening Angle \alpha (rad)')
title('E: Relative aSF Nucleation Rate')

hold on
contour(theta_centers,alpha_centers,rare_mask',[1 1],'k--','LineWidth',1)
hold off

%% =========================
% HISTOGRAM OF TENSIONS
%% =========================
figure

histogram(tension_all,30)

xlabel('Edge Tension')
ylabel('Frequency')

title('Histogram of Edge Tensions')

grid on

%% =========================
% DONE
%% =========================
disp('All plots generated successfully.')

end