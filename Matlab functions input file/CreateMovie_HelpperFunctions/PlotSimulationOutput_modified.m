function PlotSimulationOutput_modified( ...
    V,E,BoxSize,xMax,yMax,TENSION_THRESHOLD)

hold on;

%% ======================================
% COLORS (EDIT FREELY)
%% ======================================
HIGH_COLOR = [1 0 0];      % red  -> above threshold
LOW_COLOR  = [0 0 1];      % blue -> below threshold

HIGH_WIDTH = 2.5;
LOW_WIDTH  = 1.0;

%% ======================================
% PLOT EDGES
%% ======================================
N = size(E.id,1);

for i = 1:N

    vind1 = find(V.id == E.vertex1(i));
    vind2 = find(V.id == E.vertex2(i));

    if isempty(vind1) || isempty(vind2)
        continue
    end

    tension = E.tension(i);

    %% ---- THRESHOLD TEST ----
    if tension >= TENSION_THRESHOLD
        edge_color = HIGH_COLOR;
        lw = HIGH_WIDTH;
    else
        edge_color = LOW_COLOR;
        lw = LOW_WIDTH;
    end

    line([V.x(vind1),V.x(vind2)], ...
         [V.y(vind1),V.y(vind2)], ...
         'Color', edge_color, ...
         'LineWidth', lw);
end

%% ======================================
% DRAW BOX
%% ======================================
LX = BoxSize(1);
LY = BoxSize(2);

line([0 LX],[0 0],'Color','k','LineWidth',1);
line([0 LX],[LY LY],'Color','k','LineWidth',1);
line([0 0],[0 LY],'Color','k','LineWidth',1);
line([LX LX],[0 LY],'Color','k','LineWidth',1);

%% ======================================
% AXIS SETTINGS
%% ======================================
ha = gca;

set(gca,'Color',[128 128 128]/256);

set(ha,'XLim',[-5 xMax], ...
       'YLim',[-5 yMax], ...
       'DataAspectRatio',[1 1 1]);

set(ha,'XTick',[]);
set(ha,'YTick',[]);

%% ======================================
% LEGEND (optional but VERY useful)
%% ======================================
hold on
plot(nan,nan,'-','Color',HIGH_COLOR,'LineWidth',2.5,...
    'DisplayName','T > Threshold');
plot(nan,nan,'-','Color',LOW_COLOR,'LineWidth',1,...
    'DisplayName','T < Threshold');
legend(TextColor','w','Location','southoutside')

hold off
end