function [V_pbc,E_pbc] = MakePBC(V,E,BoxSize)

%For edges that cross boundaries, do the following:
%   1. Create vertices at crossing points (use negative id to distinguish
%   them from 'real' vertices)
%   2. For ease of drawing, split the crossing edges into segments defined
%   by the 2 original vertices and the vertices created above. Give the
%   edge segments the same id as the original edge, then delete the
%   original edge.
%   3. Split cells that cross boundaries into multiple cells with the same
%   id (but bounded by different edges and vertices). (TO BE IMPLEMENTED)


V_pbc = V;
E_pbc = E;
vcount = -1;     %counter for newly created vertex id

%% PBC for edges

%For each edges that cross a boundary, find 2 crossing points on the
%boundary and split the edge into 2 segments: vertex1 to the 1st crossing
%point, and the 2nd crossing point to vertex2.
%Add these edge segments to the list (with the same id, length, etc except
%for vertex and flag data), then delete the original crossing edge.

%Also, keep a 'cell' list of ordered vertices like:
%   NewEdgeList{} = [edgeId], [vertexId1], [vboundaryId1, vboundaryId2  , ..., vboundaryIdN]
%                   [edgeId], [vertexId2], [vboundaryIdN, vboundaryIdN-1, ..., vboundaryId1] 
%useful for doing PBC on cells later.


N = size(E.id, 1);
edelete = [];               %for keeping track crossing edges to be deleted later
NewEdgeList{1,3} = [];    %useful for doing PBC on cells later
count = 1;

for i = 1:N     %go through all edges
    vertex = [E.vertex1(i), E.vertex2(i)];
    ind1 = find(V.id == vertex(1));
    ind2 = find(V.id == vertex(2));
    x = [V.x(ind1), V.x(ind2)];
    y = [V.y(ind1), V.y(ind2)];
    
    if (E.xflag(i)==0 && E.yflag(i)==0)         %If edge does not cross any boundary
        continue
        
    elseif (E.xflag(i)==1 && E.yflag(i)==0)     %If edge crosses vertical boundary
        [xsort,isort] = sort(x, 'ascend');      %sort x ascending
        vstart = vertex(isort(2));
        vend = vertex(isort(1));              %starting and ending vertices
        slope = (y(isort(1))-y(isort(2))) / (xsort(1)+BoxSize(1)-xsort(2));
        xboundary = zeros(2,1);
        yboundary = zeros(2,1);
        xboundary(1) = BoxSize(1);
        yboundary(1) = y(isort(2)) + (BoxSize(1) - xsort(2))*slope;  %point right before crossing boundary
        xboundary(2) = 0;
        yboundary(2) = yboundary(1);                                 %point right after crossing boundary
        
    elseif (E.xflag(i)==0 && E.yflag(i)==1)      %If edge crosses horizontal boundary
        [ysort,isort] = sort(y, 'ascend');       %sort y ascending
        vstart = vertex(isort(2));
        vend = vertex(isort(1));              %starting and ending vertices
        slope = (x(isort(1))-x(isort(2))) / (ysort(1)+BoxSize(2)-ysort(2));
        xboundary = zeros(2,1);
        yboundary = zeros(2,1);
        xboundary(1)= x(isort(2)) + (BoxSize(2) - ysort(2))*slope;
        yboundary(1) = BoxSize(2);                                   %point right before crossing boundary
        xboundary(2) = xboundary(1);
        yboundary(2) = 0;                                            %point right after crossing boundary
        
     elseif (E.xflag(i)==1 && E.yflag(i)==1)      %If edge crosses both boundaries
        [xsort,isortx] = sort(x, 'ascend');
        [ysort,isorty] = sort(y, 'ascend');
        xboundary = zeros(4,1);
        yboundary = zeros(4,1);
        
        if (isortx(1)==isorty(1))
            %crossing from top right to bottom left
            vstart = vertex(isortx(2));
            vend = vertex(isortx(1));              %starting and ending vertices
            slope = ((ysort(1)+BoxSize(2))-ysort(2)) / ((xsort(1)+BoxSize(1))-xsort(2));
            ycross = ysort(2) + (BoxSize(1)-xsort(2))*slope;   %y when crossing vertical boundary
            xcross = xsort(2) + (BoxSize(2)-ysort(2))/slope;   %x when crossing horizontal boundary
            
            if (xcross<=BoxSize(1) && ycross>=BoxSize(2))
                %crossing horizontal axis first, then vertical axis (or
                %crossing through a corner)
                xboundary(1) = xcross;
                yboundary(1) = BoxSize(2);                        %point right before horizontal axis crossing
                xboundary(2) = xboundary(1);
                yboundary(2) = 0;                                 %point right after horizontal axis crossing
                xboundary(3) = BoxSize(1);
                yboundary(3) = ycross - BoxSize(2);               %point right before vertical axis crossing
                xboundary(4) = 0;
                yboundary(4) = yboundary(3);                      %point right after vertical axis crossing
            elseif (xcross>BoxSize(1) && ycross<BoxSize(2))
                %crossing vertical axis first, then horizontal axis
                xboundary(1) = BoxSize(1);
                yboundary(1) = ycross;                       %point right before vertical axis crossing
                xboundary(2) = 0;
                yboundary(2) = yboundary(1);                   %point right after vertical axis crossing
                xboundary(3) = xcross - BoxSize(1);
                yboundary(3) = BoxSize(2);                   %point right before horizontal axis crossing
                xboundary(4) = xboundary(3);
                yboundary(4) = 0;                            %point right after horizontal axis crossing
            else
                error('??!');
            end
            
            
        elseif (isortx(1)==isorty(2))
            %crossing from bottom right to top left
            vstart = vertex(isortx(2));
            vend = vertex(isortx(1));              %starting and ending vertices
            slope = ((ysort(2)-BoxSize(2))-ysort(1)) / ((xsort(1)+BoxSize(1))-xsort(2));
            ycross = ysort(1) + (BoxSize(1)-xsort(2))*slope;   %y when crossing vertical boundary
            xcross = xsort(2) - ysort(1)/slope;                %x when crossing horizontal boundary
            
            if (xcross<BoxSize(1) && ycross<0)
                %crossing horizontal axis first, then vertical axis
                xboundary(1) = xcross;
                yboundary(1) = 0;                                 %point right before horizontal axis crossing
                xboundary(2) = xboundary(1);
                yboundary(2) = BoxSize(2);                        %point right after horizontal axis crossing
                xboundary(3) = BoxSize(1);
                yboundary(3) = BoxSize(2) + ycross;               %point right before vertical axis crossing
                xboundary(4) = 0;
                yboundary(4) = yboundary(3);                      %point right after vertical axis crossing
            elseif (xcross>BoxSize(1) && ycross>0)
                %crossing vertical axis first, then horizontal axis
                xboundary(1) = BoxSize(1);
                yboundary(1) = ycross;                            %point right before vertical axis crossing
                xboundary(2) = 0;
                yboundary(2) = yboundary(1);                        %point right after vertical axis crossing
                xboundary(3) = xcross - BoxSize(1);
                yboundary(3) = 0;                                 %point right before horizontal axis crossing
                xboundary(4) = xboundary(3);
                yboundary(4) = BoxSize(2);                        %point right after horizontal axis crossing
            else
                error('??');
            end
            
        else
            error('?!?');
        end
        
    else
        error('Flag must be either 0 or 1!');
    end
    
    
    orderedBoundaryVerts = [];
    
    %Add boundary points to list of vertices
    n = length(xboundary);
    vnewidTemp = zeros(n,1);        %list of ID of the newly-created boundary vertices
    for j = 1:n
        V_pbc.id(end+1) = vcount;
        V_pbc.x(end+1) = xboundary(j);
        V_pbc.y(end+1) = yboundary(j);
        vnewidTemp(j) = vcount;
        vcount = vcount - 1;        %give boundary vertices negative IDs
    end
    
    %Add edge segments to list of edges
    vnewid = [vstart; vnewidTemp; vend];
    for j = 1:2:length(vnewid)-1        %stepsize is 2
        E_pbc.id(end+1) = E.id(i);
        E_pbc.vertex1(end+1) = vnewid(j);
        E_pbc.vertex2(end+1) = vnewid(j+1);
        E_pbc.xflag(end+1) = 0;
        E_pbc.yflag(end+1) = 0;
        E_pbc.tension(end+1) = E.tension(i);
        E_pbc.length(end+1) = E.length(i);
    end
    
    %Record index for deleting original (crossing) edges later
    edelete = [edelete, i];
    
    
    %Create entry of crossedgelist
    NewEdgeList{count,1} = E.id(i);
    NewEdgeList{count,2} = vstart;
    NewEdgeList{count,3} = vnewidTemp;
    
    NewEdgeList{count+1, 1} = E.id(i);
    NewEdgeList{count+1, 2} = vend;
    NewEdgeList{count+1, 3} = vnewidTemp(end:-1:1);   %flip the order
    
    count = count + 2;   
end


%Remove the original edge from the list
E_pbc.id(edelete) = [];
E_pbc.vertex1(edelete) = [];
E_pbc.vertex2(edelete) = [];
E_pbc.xflag(edelete) = [];
E_pbc.yflag(edelete) = [];
E_pbc.tension(edelete) = [];
E_pbc.length(edelete) = [];
