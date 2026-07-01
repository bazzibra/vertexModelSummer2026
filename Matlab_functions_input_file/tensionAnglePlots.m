function tensionAnglePlots(filenamme)

scriptDir = fileparts(mfilename('fullpath'));
projectRoot = fileparts(scriptDir);
inputDir = fullfile(projectRoot,"input_files",filenamme);

underScoreIndexing = strfind(filenamme,'_');
relevantFilePortion = filenamme(1:underScoreIndexing(9)); 
folderOutputName = relevantFilePortion + "visualization";

outputDir = fullfile(projectRoot,'visualization_files',folderOutputName);
fileName = fullfile(outputDir,'tension_angle_evolution.mp4'); 

%filenamme = 'RFB_NbPts_1000_rng_2000_pot_ani_lim_320_relaxed_out.txt';
[edgeArray,Y,NUCLEATIONTHRESHOLD, EpsilonMax] = readEdgeData(inputDir);

time = edgeArray(:, 1);
tension = edgeArray(:, 5);
restlength = edgeArray(:, 7);
length = edgeArray(:, 6);
angle = edgeArray(:, 9);

%Number of edges above threshold and over max tension
uniqueTimes = unique(time, 'stable');

countAboveNucleation = zeros(numel(uniqueTimes), 1);
countAboveEpsilonMax = zeros(numel(uniqueTimes), 1);

for k = 1:numel(uniqueTimes)
    frameMask = (time == uniqueTimes(k));
    frameTension = tension(frameMask);

    countAboveNucleation(k) = sum(frameTension > NUCLEATIONTHRESHOLD);
    countAboveEpsilonMax(k) = sum(frameTension > EpsilonMax * Y);
end

fig = figure(1);
plot(uniqueTimes, countAboveNucleation, 'r-', 'LineWidth', 2);
hold on;
plot(uniqueTimes, countAboveEpsilonMax, 'b-', 'LineWidth', 2);
hold off;
grid on;
xlabel('Time');
ylabel('Number of Edges');
title('Edges Above Tension Thresholds Over Time');
legend('Tension > NUCLEATIONTHRESHOLD', 'Tension > EpsilonMax * Y', 'Location', 'best');

outputDir = fullfile(projectRoot,"visualization_files",folderOutputName,'TensionEvolutionGraph'); 
saveas(fig,outputDir,'png'); 

close all

% Build a video showing both histograms over all time steps.
xedges = linspace(0, pi, 101);
yedges1 = linspace(0, EpsilonMax * Y, 51);
yedges2 = linspace(EpsilonMax * Y, 1.5 * EpsilonMax * Y, 11);
yedges = [yedges1, yedges2(2:end)];

videoName = 'tension_angle_evolution.mp4';
v = VideoWriter(fileName, 'MPEG-4');
v.FrameRate = 12;
open(v);

fig = figure(2);
set(fig, 'Color', 'w');
fig.Position(3) = 1000;
tiledlayout(1, 2, 'Padding', 'compact', 'TileSpacing', 'compact');

for k = 1:numel(uniqueTimes)
    currentTime = uniqueTimes(k);
    frameMask = (time == currentTime);

    frameAngle = angle(frameMask);
    frameTension = tension(frameMask);

    thresholdMask = frameTension > NUCLEATIONTHRESHOLD;
    frameAngleAboveThreshold = frameAngle(thresholdMask);

    nexttile(1);
    cla;
    histogram(frameAngleAboveThreshold, 'BinLimits', [0 pi], 'NumBins', 100);
    xlim([0 pi]);
    xticks([0, pi/4, pi/2, 3*pi/4, pi]);
    xticklabels({'$0$', '$\pi/4$', '$\pi/2$', '$3\pi/4$', '$\pi$'});
    set(gca, 'TickLabelInterpreter', 'latex');
    xlabel('Angle');
    ylabel('Count');
    title(sprintf('Edges Above Threshold, t = %.6g', currentTime));

    nexttile(2);
    cla;
    histogram2(frameAngle, frameTension, ...
        'XBinEdges', xedges, ...
        'YBinEdges', yedges, ...
        'DisplayStyle', 'tile', ...
        'ShowEmptyBins', 'off');
    hold on;
    yline(NUCLEATIONTHRESHOLD, 'r--', 'LineWidth', 2);
    yline(EpsilonMax * Y, 'b--', 'LineWidth', 2);
    hold off;
    xlim([0 pi]);
    ylim([yedges(1), yedges(end)]);
    xticks([0, pi/4, pi/2, 3*pi/4, pi]);
    xticklabels({'$0$', '$\pi/4$', '$\pi/2$', '$3\pi/4$', '$\pi$'});
    set(gca, 'TickLabelInterpreter', 'latex');
    xlabel('Angle');
    ylabel('Tension');
    title(sprintf('Tension vs Orientation, t = %.6g', currentTime));
    cb = colorbar;
    cb.Label.String = 'Count';

    drawnow;
    writeVideo(v, getframe(fig));
end

close(v);
fprintf('Saved video: %s\n', videoName);