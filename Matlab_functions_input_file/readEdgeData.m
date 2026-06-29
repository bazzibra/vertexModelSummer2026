function [edgeArray, currentY,currentNUCLEATIONTHRESHOLD, currentEpsilonMax] = readEdgeData(filePath)
%READEDGEDATA Read edge-level data from simulation output text file.
%   edgeTable = readEdgeData(filePath)
%   Returns one row per edge per timestep with:
%   - time
%   - edge_id
%   - vertex1, vertex2
%   - tension
%   - length
%   - restlength (if present, otherwise inferred when possible)
%   - angle_deg, angle_rad (computed from vertex positions)

if nargin < 1 || isempty(filePath)
	error('readEdgeData:MissingInput', 'Please provide a valid output .txt file path.');
end

fid = fopen(filePath, 'r');
if fid == -1
	error('readEdgeData:FileOpenFailed', 'Could not open file: %s', filePath);
end

cleanupObj = onCleanup(@() fclose(fid));

% Growable buffers
time_all = [];
edge_id_all = [];
v1_all = [];
v2_all = [];
tension_all = [];
length_all = [];
restlength_all = [];
angle_deg_all = [];
angle_rad_all = [];

% Timestep-level state
currentTime = NaN;
currentLx = NaN;
currentLy = NaN;
currentY = NaN;
currentNUCLEATIONTHRESHOLD = NaN;
currentEpsilonMax = NaN;

vertexMap = containers.Map('KeyType', 'double', 'ValueType', 'any');
processedTimeKeys = containers.Map('KeyType', 'char', 'ValueType', 'logical');

line = fgetl(fid);
while ischar(line)
	line = strtrim(line);

	% Track global scalars needed for optional restlength inference and PBC angle.
	if startsWith(line, 'CURRENTTIME', 'IgnoreCase', true)
		vals = sscanf(line, 'CURRENTTIME %f');
		if ~isempty(vals)
			candidateTime = vals(1);
		else
			toks = split(line);
			if numel(toks) >= 2
				candidateTime = str2double(toks{2});
			else
				candidateTime = NaN;
			end
		end

		if ~isnan(candidateTime)
			timeKey = sprintf('%.15g', candidateTime);
			if isKey(processedTimeKeys, timeKey)
				break
			end
		end

		currentTime = candidateTime;
		line = fgetl(fid);
		continue
	elseif startsWith(line, 'LX', 'IgnoreCase', true)
		vals = sscanf(line, 'LX %f');
		if ~isempty(vals)
			currentLx = vals(1);
		end
		line = fgetl(fid);
		continue
	elseif startsWith(line, 'LY', 'IgnoreCase', true)
		vals = sscanf(line, 'LY %f');
		if ~isempty(vals)
			currentLy = vals(1);
		end
		line = fgetl(fid);
		continue
	elseif startsWith(line, 'Y', 'IgnoreCase', true)
		% Spring constant (Dynamic runs): tension = Y * (L - L0) / L0
		vals = sscanf(line, 'Y %f');
		if ~isempty(vals)
			currentY = vals(1);
		end
		line = fgetl(fid);
		continue
    elseif startsWith(line, 'NUCLEATIONTHRESHOLD', 'IgnoreCase', true)
		vals = sscanf(line, 'NUCLEATIONTHRESHOLD %f');
		if ~isempty(vals)
			currentNUCLEATIONTHRESHOLD = vals(1);
		end
		line = fgetl(fid);
		continue
    elseif startsWith(line, 'EpsilonMax', 'IgnoreCase', true)
		% Spring constant (Dynamic runs): tension = Y * (L - L0) / L0
		vals = sscanf(line, 'EpsilonMax %f');
		if ~isempty(vals)
			currentEpsilonMax = vals(1);
		end
		line = fgetl(fid);
		continue
	end

	if strcmpi(line, 'Vertices')
		[vertexMap, line] = readVerticesBlock(fid);
		continue
	end

	if strcmpi(line, 'Edges')
		[blockData, line] = readEdgesBlock(fid);
		if ~isempty(blockData)
			nRows = size(blockData.raw, 1);

			edge_id = blockData.raw(:, blockData.idx.id);
			v1 = blockData.raw(:, blockData.idx.vertex1);
			v2 = blockData.raw(:, blockData.idx.vertex2);
			tns = blockData.raw(:, blockData.idx.tension);
			len = blockData.raw(:, blockData.idx.length);

			if ~isnan(blockData.idx.restlength)
				rlen = blockData.raw(:, blockData.idx.restlength);
			else
				rlen = nan(nRows, 1);
				if ~isnan(currentY)
					denom = currentY + tns;
					valid = denom ~= 0;
					rlen(valid) = (currentY .* len(valid)) ./ denom(valid);
				end
			end

			angDeg = nan(nRows,1);
			angRad = nan(nRows,1);

			for i = 1:nRows
				[ok1, p1] = getVertex(vertexMap, v1(i));
				[ok2, p2] = getVertex(vertexMap, v2(i));
				if ~(ok1 && ok2)
					continue
				end

				dx = p2(1) - p1(1);
				dy = p2(2) - p1(2);

				% Minimum-image adjustment for periodic domains when box size is known.
				if ~isnan(currentLx) && currentLx > 0
					dx = dx - currentLx * round(dx / currentLx);
				end
				if ~isnan(currentLy) && currentLy > 0
					dy = dy - currentLy * round(dy / currentLy);
				end

				angRad(i) = mod(atan2(dy, dx), pi);
				angDeg(i) = mod(rad2deg(angRad(i)), 360.0);
			end

			time_all = [time_all; repmat(currentTime, nRows, 1)];
			edge_id_all = [edge_id_all; edge_id];
			v1_all = [v1_all; v1];
			v2_all = [v2_all; v2];
			tension_all = [tension_all; tns];
			length_all = [length_all; len];
			restlength_all = [restlength_all; rlen];
			angle_deg_all = [angle_deg_all; angDeg];
			angle_rad_all = [angle_rad_all; angRad];

			if ~isnan(currentTime) && nRows > 0
				processedTimeKeys(sprintf('%.15g', currentTime)) = true;
			end
		end
		continue
	end

	line = fgetl(fid);
end

edgeTable = table(time_all, edge_id_all, v1_all, v2_all, tension_all, ...
				  length_all, restlength_all, angle_deg_all, angle_rad_all, ...
	'VariableNames', {'time','edge_id','vertex1','vertex2','tension','length', ...
					  'restlength','angle_deg','angle_rad'});
edgeArray = table2array(edgeTable);
clear cleanupObj

end


function [vertexMap, nextLine] = readVerticesBlock(fid)
% Read header line
header = fgetl(fid);
if ~ischar(header)
	vertexMap = containers.Map('KeyType', 'double', 'ValueType', 'any');
	nextLine = header;
	return
end

fields = split(strtrim(header));
idx_id = find(strcmp(fields, 'id'), 1);
idx_x  = find(strcmp(fields, 'x'), 1);
idx_y  = find(strcmp(fields, 'y'), 1);

vertexMap = containers.Map('KeyType', 'double', 'ValueType', 'any');

line = fgetl(fid);
while ischar(line)
	tline = strtrim(line);
	if strcmpi(tline, 'Edges') || strcmpi(tline, 'Cells') || strcmpi(tline, 'End')
		break
	end

	vals = sscanf(tline, '%f')';
	if numel(vals) >= max([idx_id, idx_x, idx_y])
		vid = vals(idx_id);
		vertexMap(vid) = [vals(idx_x), vals(idx_y)];
	end
	line = fgetl(fid);
end

nextLine = line;
end


function [blockData, nextLine] = readEdgesBlock(fid)
header = fgetl(fid);
blockData = struct('raw', [], 'idx', struct('id', NaN, 'vertex1', NaN, 'vertex2', NaN, ...
	'tension', NaN, 'length', NaN, 'restlength', NaN));

if ~ischar(header)
	nextLine = header;
	return
end

fields = split(strtrim(header));

blockData.idx.id = find(strcmp(fields, 'id'), 1);
blockData.idx.vertex1 = find(strcmp(fields, 'vertex1'), 1);
blockData.idx.vertex2 = find(strcmp(fields, 'vertex2'), 1);
blockData.idx.tension = find(strcmp(fields, 'tension'), 1);
blockData.idx.length = find(strcmp(fields, 'length'), 1);
blockData.idx.restlength = find(strcmp(fields, 'restlength'), 1);

required = [blockData.idx.id, blockData.idx.vertex1, blockData.idx.vertex2, ...
			blockData.idx.tension, blockData.idx.length];
if any(isnan(required))
	error('readEdgeData:MissingEdgeFields', ...
		'Edge header must include id, vertex1, vertex2, tension, and length.');
end

raw = [];
line = fgetl(fid);
while ischar(line)
	tline = strtrim(line);
	if strcmpi(tline, 'Cells') || strcmpi(tline, 'End') || strcmpi(tline, 'Vertices')
		break
	end

	vals = sscanf(tline, '%f')';
	if ~isempty(vals)
		raw = [raw; vals];
	end
	line = fgetl(fid);
end

blockData.raw = raw;
nextLine = line;


end


function [ok, p] = getVertex(vertexMap, vid)
ok = isKey(vertexMap, vid);
if ok
	p = vertexMap(vid);
else
	p = [NaN, NaN];
end
end
