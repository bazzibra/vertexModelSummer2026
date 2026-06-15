function PlotSimulationOutput(V,E,BoxSize,xMax, yMax,min_length,max_length)
hold on;

% flag to plot length and tension!
% these are called things, but currently plotting tensions.
cmap_val = hot;
length_reg_vect = min_length:((max_length-min_length)/(size(cmap_val,1)-1)):max_length;

%% Plot Edges 
N = size(E.id,1);
for i = 1:N
    vind1 = find(V.id==E.vertex1(i));
    vind2 = find(V.id==E.vertex2(i));
    t=E.tension(i); 
    TagStr = ['Edge ', num2str(E.id(i))];
    lval = E.length(i);
    [~,min_I] = min(abs(length_reg_vect - t));
    % plot line with color based on tension
    line([V.x(vind1),V.x(vind2)], [V.y(vind1),V.y(vind2)], 'Color', cmap_val(min_I,:), 'LineWidth', 1, 'Tag', TagStr);
end


% Draw box at BoxSize
LX = BoxSize(1);
LY = BoxSize(2);
line([0, LX], [0, 0], 'HitTest', 'off', 'Color', 'k','LineWidth', 1);
line([0, LX], [LY, LY], 'HitTest', 'off', 'Color', 'k','LineWidth', 1);
line([0, 0], [0, LY], 'HitTest', 'off', 'Color', 'k','LineWidth', 1);
line([LX, LX], [0, LY], 'HitTest', 'off', 'Color', 'k','LineWidth', 1);

colormap hot;
clim([min_length, max_length])
h_cb = colorbar;
c.Label.String = 'edge tension';


%% Set axis properties
%If xMax and yMax are specified, set the maximum x and y axis to be xMax and yMax, respectively. Otherwise the axes limits are taken from BoxSize
ha = gca;
set(gca, 'Color', [128, 128, 128]/256); % Sets the background color to black
set(ha, 'XLim', [-5 xMax], 'YLim', [-5 yMax], 'DataAspectRatio', [1 1 1]);
set(ha,'Xtick',[],'Ytick',[])
hold off
end
