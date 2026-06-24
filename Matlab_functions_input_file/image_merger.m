%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Merge images with ZERO spacing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; clc; close all;

imgFolder = 'merge_images';

files = dir(fullfile(imgFolder,'*.png'));

if isempty(files)
    error('No images found — check folder name or extension');
end

%% numerical sorting
names = {files.name};
nums = zeros(length(names),1);

for i = 1:length(names)
    token = regexp(names{i},'(\d+)','tokens');
    nums(i) = str2double(token{1}{1});
end

[~,idx] = sort(nums);
files = files(idx);

N = length(files);

%% layout
ncols = 4;
nrows = ceil(N/ncols);

figure('Color','w','Position',[100 100 1600 800]);

tiledlayout(nrows,ncols,...
    'TileSpacing','none',...
    'Padding','none');

for i = 1:N
    ax = nexttile;
    img = imread(fullfile(imgFolder,files(i).name));
    imshow(img,'Border','tight');
    axis off
end

exportgraphics(gcf,'merged_packings.png','Resolution',300);

disp('✅ Perfect merge complete.');

