%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Movie + Saved Frames of Tension Histogram Evolution
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

filename = '6_out.txt';

fid = fopen(filename,'r');
if fid == -1
    error('Could not open file');
end

%% Create output folder
output_folder = 'tension_frames';
if ~exist(output_folder,'dir')
    mkdir(output_folder);
end

%% Create movie
video = VideoWriter('TensionEvolution.mp4','MPEG-4');
video.FrameRate = 10;
open(video);

%% Variables
readingEdges = false;
tension_all = [];
current_time = 0;
frame_id = 1;

figure;

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

    % Start edges block
    if strcmp(line,'Edges')
        readingEdges = true;
        fgetl(fid); % skip header
        tension_all = [];
        continue
    end

    % End of timestep
    if strcmp(line,'Cells')

        % Plot histogram
        histogram(tension_all,30,'FaceColor',[0.2 0.4 0.7],'EdgeColor','k')

        xlabel('Edge Tension')
        ylabel('Count')
        title(['Tension Distribution | t = ',num2str(current_time)])

        xlim([0 max(tension_all)*1.1])
        grid on

        drawnow

        %% Capture frame
        frame = getframe(gcf);

        %% Write to movie
        writeVideo(video,frame);

        %% Save frame as image
        frame_name = sprintf('frame_%04d.png',frame_id);
        imwrite(frame.cdata, fullfile(output_folder,frame_name));

        frame_id = frame_id + 1;

        readingEdges = false;
        continue
    end

    % Read edge tensions
    if readingEdges

        data = textscan(line,...
            '%f %f %f %f %f %f %f %f %f %f',...
            'Delimiter',' \t',...
            'MultipleDelimsAsOne',true);

        if isempty(data{1})
            continue
        end

        tension = data{6};
        tension_all = [tension_all; tension];

    end

end

fclose(fid);
close(video);

disp('Movie created: TensionEvolution.mp4');
disp('Frames saved in folder: tension_frames');

