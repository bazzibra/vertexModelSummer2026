%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Aggregate Greyplots for Multiple Packings
%
% FINAL-TIMESTEP NUCLEATION VERSION
%
% Panel C : all TCJ geometry (all timesteps)
% Panel D : nucleation events (FINAL timestep tensions)
% Panel E : relative nucleation rate
%
% x-axis  : psi   (bisector orientation)
% y-axis  : theta (opening angle)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

%% ==========================================================
% USER INPUT
%% ==========================================================
filelist = {
'1_out.txt'
'3_out.txt'
'10_out.txt'
'11_out.txt'
'12_out.txt'
};

nbins_psi   = 20;
nbins_theta = 20;

psi_min   = 0;
psi_max   = pi/2;

theta_min = 0;
theta_max = pi;

rare_threshold = 1e-4;

%% ==========================================================
% NUCLEATION THRESHOLD
%% ==========================================================
Y_param    = 5.66;
EpsilonMax = 0.6;

T_star = 0.97 * Y_param * EpsilonMax;

fprintf('\n====================================\n');
fprintf('Using nucleation threshold T* = %.6f\n',T_star);
fprintf('====================================\n\n');

%% ==========================================================
% MASTER ARRAYS
%% ==========================================================
theta_all = [];
psi_all   = [];

theta_nuc = [];
psi_nuc   = [];

max_nodes = 50000;

%% ==========================================================
% FUNCTION — Collect ALL TCJs (Panel C)
%% ==========================================================
function [theta_all,psi_all] = ...
    collect_all_TCJs(v_theta,v_psi,v_edge_count,...
                     theta_all,psi_all)

N = size(v_theta,1);

for i = 1:N

    if v_edge_count(i) ~= 3
        continue
    end

    theta_vals = v_theta(i,:);
    psi_vals   = v_psi(i,:);

    if any(isnan(theta_vals)) || any(isnan(psi_vals))
        continue
    end

    theta_all = [theta_all; theta_vals(:)];
    psi_all   = [psi_all;   psi_vals(:)];
end
end

%% ==========================================================
% FUNCTION — FINAL TIMESTEP NUCLEATION (Panels D,E)
%% ==========================================================
function [theta_nuc,psi_nuc] = ...
    compute_final_nucleation(v_theta,v_psi,...
    v_edge_tensions,v_edge_count,...
    theta_nuc,psi_nuc,T_star)

N = size(v_theta,1);

for i = 1:N

    if v_edge_count(i) ~= 3
        continue
    end

    theta_vals = v_theta(i,:);
    psi_vals   = v_psi(i,:);
    tensions   = v_edge_tensions(i,:);

    if any(isnan(theta_vals)) || any(isnan(psi_vals))
        continue
    end

    above = find(tensions > T_star);

    if length(above) >= 2

        e1 = above(1);
        e2 = above(2);

        % angle BETWEEN high-tension edges
        angle_index = setdiff(1:3,[e1 e2]);

        if length(angle_index)==1
            theta_nuc = [theta_nuc;
                         theta_vals(angle_index)];

            psi_nuc   = [psi_nuc;
                         psi_vals(angle_index)];
        end
    end
end
end

%% ==========================================================
% LOOP OVER FILES
%% ==========================================================
for fidx = 1:length(filelist)

    filename = filelist{fidx};
    fprintf('Processing file: %s\n', filename);

    fid = fopen(filename,'r');
    if fid == -1
        warning('Could not open file');
        continue
    end

    v_theta = nan(max_nodes,3);
    v_psi   = nan(max_nodes,3);
    v_edge_tensions = nan(max_nodes,3);
    v_edge_count = zeros(max_nodes,1);

    readingVertices = false;
    readingEdges    = false;

    while ~feof(fid)

        line = strtrim(fgetl(fid));
        if isempty(line); continue; end

        %% -------- NEW TIMESTEP --------
        if startsWith(line,'CURRENTTIME')

            % collect ALL TCJ geometry
            [theta_all,psi_all] = ...
                collect_all_TCJs(v_theta,v_psi,...
                                 v_edge_count,...
                                 theta_all,psi_all);

            % reset for next timestep
            v_theta = nan(max_nodes,3);
            v_psi   = nan(max_nodes,3);
            v_edge_tensions = nan(max_nodes,3);
            v_edge_count = zeros(max_nodes,1);

            readingVertices = false;
            readingEdges    = false;
            continue
        end

        %% -------- SECTION FLAGS --------
        if contains(line,'Anglestheta1')
            readingVertices = true;
            readingEdges    = false;
            continue
        end

        if strcmp(line,'Edges')
            readingVertices = false;
            readingEdges = true;
            fgetl(fid); % skip header
            continue
        end

        if strcmp(line,'Cells')
            readingVertices = false;
            readingEdges = false;
            continue
        end

        %% -------- READ VERTEX DATA --------
        if readingVertices

            vals = textscan(line,'%f');
            vals = vals{1};

            if length(vals) < 9
                continue
            end

            vid = round(vals(1));

            if vid>0 && vid<=max_nodes
                v_theta(vid,:) = vals(4:6);
                v_psi(vid,:)   = vals(7:9);
            end
        end

        %% -------- READ EDGE DATA --------
        if readingEdges

            vals = textscan(line,'%f');
            vals = vals{1};

            if length(vals) < 6
                continue
            end

            v1 = round(vals(2));
            v2 = round(vals(3));
            tension_val = vals(6);

            if v1>0 && v1<=max_nodes
                idx = v_edge_count(v1)+1;
                if idx<=3
                    v_edge_tensions(v1,idx)=tension_val;
                    v_edge_count(v1)=idx;
                end
            end

            if v2>0 && v2<=max_nodes
                idx = v_edge_count(v2)+1;
                if idx<=3
                    v_edge_tensions(v2,idx)=tension_val;
                    v_edge_count(v2)=idx;
                end
            end
        end
    end

    %% ===== FINAL TIMESTEP NUCLEATION ONLY =====
    [theta_nuc,psi_nuc] = ...
        compute_final_nucleation(v_theta,v_psi,...
        v_edge_tensions,v_edge_count,...
        theta_nuc,psi_nuc,T_star);

    fclose(fid);
end

%% ==========================================================
% BINNING
%% ==========================================================
psi_edges   = linspace(psi_min,psi_max,nbins_psi+1);
theta_edges = linspace(theta_min,theta_max,nbins_theta+1);

psi_centers   = 0.5*(psi_edges(1:end-1)+psi_edges(2:end));
theta_centers = 0.5*(theta_edges(1:end-1)+theta_edges(2:end));

%% ==========================================================
% PANEL C — ALL TCJs
%% ==========================================================
H_C = histcounts2(psi_all,theta_all,psi_edges,theta_edges);
H_C = H_C / sum(H_C(:));

figure
imagesc(psi_centers,theta_centers,H_C')
axis xy
colormap(gray)
colorbar
xlabel('\psi (bisector orientation)')
ylabel('\theta (opening angle)')
title('Panel C — All TCJs')

%% ==========================================================
% PANEL D — FINAL TIMESTEP NUCLEATION EVENTS
%% ==========================================================
H_D = histcounts2(psi_nuc,theta_nuc,psi_edges,theta_edges);
H_D = H_D / sum(H_D(:));

figure
imagesc(psi_centers,theta_centers,H_D')
axis xy
colormap(gray)
colorbar
xlabel('\psi (bisector orientation)')
ylabel('\theta (opening angle)')
title('Panel D — Nucleating TCJs (Final Timestep)')

%% ==========================================================
% PANEL E — RELATIVE NUCLEATION RATE
%% ==========================================================
H_E = H_D ./ H_C;
H_E(H_C < rare_threshold) = NaN;

figure
imagesc(psi_centers,theta_centers,H_E')
axis xy
colormap(gray)
colorbar
xlabel('\psi (bisector orientation)')
ylabel('\theta (opening angle)')
title('Panel E — Relative Nucleation Rate')

fprintf('\n✅ Analysis complete.\n');