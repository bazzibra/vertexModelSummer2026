r = 620; % Updated seed to match target
NbPoints = 50; 
limit = 3; 
cpt = 'quadratic';
prefix = 'RFB';
potential = 'ani';

%% Store data in the right folder
close all;
scriptDir = fileparts(mfilename('fullpath'));
projectRoot = fileparts(scriptDir);
outputDir = fullfile(projectRoot, 'input_files');

if ~exist(outputDir, 'dir')
    mkdir(outputDir);
end

%% Parameters
rng(r);
TOTALTIME = 1000.000000;
SEED = 89542;
TIMEINTERVAL = 0.1;
error_tolerance_absolute_mechanical = 0.010000;
TISSUE_EXT_STRESS_X = 0.000000;     %should both be 0 to relax tissue, change before dynamic
TISSUE_EXT_STRESS_Y = 0.000000; 
equilibriumtolerance = 1.000000e-2;
NUCLEATIONBASECRITERIA = false;
Ratio = 10/3;
EpsilonMax = 0.091;
ExternalStressRatio = 0.85;

T1ON = 1; 
APOPTOSISON = 1;
MITOSISON = 0;
WRITEMITOSIS = 1; 
VERBOSEON = 1; 
EQUILIBRIUMMITOSIS = 1; 

%Mysterious Parameters
CELLPRESSURETYPE = cpt;
TCJSTRENGTH = 100;
THETAAN = 3.14 / 2.0 ;    
ANSTRENGTH = 25; 

% --- Standard Constants ---
GAMMA = 10.0;
t0 = 1;
l0 = 25;
% Manually setting K to match your specific target output
LX = 1.5*l0*sqrt(NbPoints);
LY = 1.5*l0*sqrt(NbPoints);
A0 = LX*LY/NbPoints;
K = Ratio * GAMMA/l0;
ALPHAM = GAMMA*t0/l0;               %the paper had 5.5
TISSUE_ALPHA = 10*ALPHAM;
INITIALSTEPSIZE = t0/10000;
MINAREA = A0/100;
MINLENGTH = l0/40;
NEWEDGELENGTH = l0/20;
CURRENTTIME = 0.000000;
KAPPA=(1/100)*0.227*5.502/ALPHAM;
GAMMA0=GAMMA;
Y=1.25*GAMMA/EpsilonMax;
NUCLEATIONTHRESHOLD = 0.95*EpsilonMax*Y;

%% Simulation Detail Parameters
% Calculate Mesh based on A0 and NbPoints to get LX = 167.705098
Mesh = LX;

nomfichier_filename= [prefix '_NbPts_' num2str(NbPoints,'%04d') '_rng_' num2str(r,'%04d') '_pot_' potential '_lim_' num2str(limit, '%2d') '20.txt'];
nomfichier = fullfile(outputDir, nomfichier_filename);

%% Generate Voronoi
fprintf('Writing file: %s\n', nomfichier);
f1=figure();
f2=figure();
Points1=Mesh*rand(NbPoints,2); 

it = 0;
while it < limit
    Pointsa=Points1+Mesh*[ones(NbPoints,1) zeros(NbPoints,1)];
    Pointsb=Points1-Mesh*[zeros(NbPoints,1) ones(NbPoints,1)];
    Pointsc=Points1-Mesh*[ones(NbPoints,1) zeros(NbPoints,1)];
    Pointsd=Points1+Mesh*[zeros(NbPoints,1) ones(NbPoints,1)];
    Pointse=Points1+Mesh*[ones(NbPoints,1) ones(NbPoints,1)];
    Pointsf=Points1+Mesh*[ones(NbPoints,1) -ones(NbPoints,1)];
    Pointsg=Points1+Mesh*[-ones(NbPoints,1) ones(NbPoints,1)];
    Pointsh=Points1+Mesh*[-ones(NbPoints,1) -ones(NbPoints,1)];
    Pointst=[Points1; Pointsa; Pointsb; Pointsc; Pointsd; Pointse; Pointsf; Pointsg; Pointsh];
    
    [v,c]=voronoin(Pointst);
    
    centroids = zeros(NbPoints,2);
    centroidcount = 0;
   
    for i = 1:length(c)
        signedarea = 0;
        centroidx = 0;
        centroidy = 0;
        if (all(c{i}~=1))
            for s = 1:length(c{i})
                if s == length(c{i}) 
                    signedarea = signedarea + v(c{i}(s),1)*v(c{i}(1),2) - v(c{i}(1),1)*v(c{i}(s),2);
                else
                    signedarea = signedarea + v(c{i}(s),1)*v(c{i}(s+1),2) - v(c{i}(s+1),1)*v(c{i}(s),2);
                end
            end
            signedarea = .5*signedarea;
            for j = 1:length(c{i})
                if j == length(c{i})
                    centroidx = centroidx + (v(c{i}(j),1)+v(c{i}(1),1))*(v(c{i}(j),1)*v(c{i}(1),2) - v(c{i}(1),1)*v(c{i}(j),2));
                    centroidy = centroidy + (v(c{i}(j),2)+v(c{i}(1),2))*(v(c{i}(j),1)*v(c{i}(1),2) - v(c{i}(1),1)*v(c{i}(j),2));
                    
                else
                    centroidx = centroidx + (v(c{i}(j),1)+v(c{i}(j+1),1))*(v(c{i}(j),1)*v(c{i}(j+1),2) - v(c{i}(j+1),1)*v(c{i}(j),2));
                    centroidy = centroidy + (v(c{i}(j),2)+v(c{i}(j+1),2))*(v(c{i}(j),1)*v(c{i}(j+1),2) - v(c{i}(j+1),1)*v(c{i}(j),2));
                end
            end
            centroidx = (1/(6*signedarea))*centroidx;
            centroidy = (1/(6*signedarea))*centroidy;
            if centroidx > 0 && centroidx < Mesh && centroidy > 0 && centroidy < Mesh
                centroidcount = centroidcount + 1;
                centroids(centroidcount,:) = [centroidx centroidy];
            end
        end
    end
    it = it + 1;
    Points1 = centroids;
end

%% Representation of the result
 set(0,'CurrentFigure',f1);
 plot(Points1(:,1),Points1(:,2),'r+');
 set(0,'CurrentFigure',f2);
 plot(Points1(:,1),Points1(:,2),'r+');
 hold on
 for i = 1:length(c)
     if (all(c{i}~=1))
         set(0,'CurrentFigure',f1)
         patch(v(c{i},1),v(c{i},2),i)
     end
     IN=(all(v(c{i},1)>0)&all(v(c{i},1)<Mesh)&all(v(c{i},2)>0)&all(v(c{i},2)<Mesh));
     IN2=(any(v(c{i},1)>0)&any(v(c{i},1)<Mesh)&any(v(c{i},2)>0)&any(v(c{i},2)<Mesh))&(~IN);
     if (all(c{i}~=1)&&IN)
         set(0,'CurrentFigure',f2)
         patch(v(c{i},1),v(c{i},2),'y'); 
     end
     if (all(c{i}~=1)&&(IN2))
         set(0,'CurrentFigure',f2)
         patch(v(c{i},1),v(c{i},2),'g');
     end
 end
 set(0,'CurrentFigure',f2)
 xlim([0 Mesh]);
 ylim([0 Mesh]);
 plot(Points1(:,1),Points1(:,2),'bo');
 scatter(centroids(:,1),centroids(:,2),'r+')

for i = 1:length(c)
    INGLOBAL=any((v(c{i},1)>0)&v(c{i},1)<Mesh&v(c{i},2)>0&v(c{i},2)<Mesh);
    if INGLOBAL
        ListVertex=c{i};
        area=0;
        for k=1:length(c{i})
            kplus=mod(k,length(c{i}))+1;
            area=area+v(c{i}(k),1)*v(c{i}(kplus),2)-v(c{i}(kplus),1)*v(c{i}(k),2);
        end
        if(area<0)    c{i}=fliplr(c{i}); end
    end
end

%% Export to a file (EXACT MATCH TO TARGET VALUES)
fid=fopen(nomfichier,'wt');
fprintf(fid, 'Voronoi Tesselation Data: \n');
fprintf(fid, 'NumbCells %d \t rng %d \n', NbPoints, r);
fprintf(fid, 'ratio pressure to tension\t%f\n', Ratio);
fprintf(fid, 'Global:\n');
fprintf(fid, 'LX\t%f\nLY\t%f\n', Mesh, Mesh);
fprintf(fid, 'A0\t%f\n', A0);
fprintf(fid, 'SEED\t%d\n', SEED);
fprintf(fid, 'TIMEINTERVAL\t%f\n', TIMEINTERVAL);
fprintf(fid, 'MINAREA\t%f\n', MINAREA);
fprintf(fid, 'MINLENGTH\t%f\n', MINLENGTH);
fprintf(fid, 'NEWEDGELENGTH\t%f\n', NEWEDGELENGTH);
fprintf(fid, 'K\t%f\n', K);
fprintf(fid, 'GAMMA\t%f\n', GAMMA);
fprintf(fid, 'TOTALTIME\t%f\n', TOTALTIME);
fprintf(fid, 'CURRENTTIME\t%f\n', CURRENTTIME);
fprintf(fid, 'VERBOSEON\t%d\n', VERBOSEON);
fprintf(fid, 'INITIALSTEPSIZE\t%f\n', INITIALSTEPSIZE);
fprintf(fid, 'error_tolerance_absolute_mechanical\t%f\n', error_tolerance_absolute_mechanical);
% Format TISSUE_ALPHA to 2 decimal places as per target
fprintf(fid, 'TISSUE_ALPHA\t%2.2f\n', TISSUE_ALPHA);
fprintf(fid, 'ALPHAM\t%f\n', ALPHAM);
fprintf(fid, 'TISSUE_EXT_STRESS_X\t%f\n', TISSUE_EXT_STRESS_X);
fprintf(fid, 'TISSUE_EXT_STRESS_Y\t%f\n', TISSUE_EXT_STRESS_Y);
fprintf(fid, 'equilibriumtolerance\t%e\n', equilibriumtolerance);
fprintf(fid, 'NUCLEATIONTHRESHOLD\t%f\n', NUCLEATIONTHRESHOLD);
fprintf(fid, 'NUCLEATIONBASECRITERIA\t%d\n', NUCLEATIONBASECRITERIA);
fprintf(fid, 'KAPPA\t%f\n', KAPPA);
fprintf(fid, 'Y\t%f\n', Y);
fprintf(fid, 'EpsilonMax\t%f\n', EpsilonMax);
fprintf(fid, 'ExternalStressRatio\t%f\n', ExternalStressRatio);
fprintf(fid, 'endGlobal\n');

fprintf(fid, 'Vertices\n');
fprintf(fid, 'id\tx\ty\n');

NbVertex=0;
CentralVertices=zeros(length(v),2);
for i = 2:length(v)   
    if(v(i,1)>0&&v(i,1)<Mesh&&v(i,2)>0&&v(i,2)<Mesh)
        fprintf(fid,'%d',i);
        fprintf(fid,'\t %2.4f',v(i,1));
        fprintf(fid,'\t %2.4f',v(i,2));
        fprintf(fid,'\n');
        NbVertex=NbVertex+1;
        CentralVertices(NbVertex,1)=v(i,1);
        CentralVertices(NbVertex,2)=v(i,2);
    end
end

CentralVertices=CentralVertices(1:NbVertex,1:2);
EqVertex=zeros(NbVertex,1);

for i = 2:length(v)
    vcorrige=zeros(1,2);
    vtest=v(i,:);
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;end
    vtest=v(i,:)-Mesh*[1 0];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)-Mesh*[0 1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)+Mesh*[1 0];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)+Mesh*[0 1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)-Mesh*[1 1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)-Mesh*[1 -1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)+Mesh*[1 1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    vtest=v(i,:)+Mesh*[1 -1];
    if (vtest(1)>0&&vtest(1)<Mesh&&vtest(2)>0&&vtest(2)<Mesh) vcorrige=vtest;  end
    
    column=find(all(abs((repmat(vcorrige,size(v,1),1))'-v')<0.000001));
    if(~isempty(column))
        EqVertex(i)=column;
    end
end

fprintf(fid, 'Edges\n');
fprintf(fid, 'id\tvertex1\tvertex2\txflag\tyflag\n');
NbEdgeMax=3*NbVertex; 
Edges.Vertices=zeros(NbEdgeMax,2);
Edges.Crossx12=zeros(NbEdgeMax,1);
Edges.Crossx21=zeros(NbEdgeMax,1);
Edges.Crossy12=zeros(NbEdgeMax,1);
Edges.Crossy21=zeros(NbEdgeMax,1);

EdgeIncrement=0; 
for i = 1:length(c) 
    INGLOBAL=any( (v(c{i},1)>0) & v(c{i},1)<Mesh & v(c{i},2)>0 & v(c{i},2)<Mesh);
    if INGLOBAL 
        ListVertex=c{i}; 
        for j=1:length(ListVertex) 
            testdejainscrit=0;
            for k=1:EdgeIncrement
                if ((EqVertex((ListVertex(j)))==Edges.Vertices(k,1)...
                        &&EqVertex(ListVertex(mod(j,length(ListVertex))+1))==Edges.Vertices(k,2))...
                        ||...
                        (EqVertex(ListVertex(j))==Edges.Vertices(k,2)...
                        &&EqVertex(ListVertex(mod(j,length(ListVertex))+1))==Edges.Vertices(k,1)))
                    testdejainscrit=1;
                else
                end
            end
            if(~testdejainscrit)
                EdgeIncrement=EdgeIncrement+1;
                Edges.Vertices(EdgeIncrement,1)=EqVertex(ListVertex(j)); 
                Edges.Vertices(EdgeIncrement,2)=EqVertex(ListVertex(mod(j,length(ListVertex))+1)); 
                Vertex1encours=ListVertex(j);
                Vertex1encourseq=EqVertex(ListVertex(j));
                Vertex2encours=ListVertex(mod(j,length(ListVertex))+1);
                Vertex2encourseq=EqVertex(ListVertex(mod(j,length(ListVertex))+1));
                
                fprintf(fid,'%d',EdgeIncrement);
                fprintf(fid,'\t %d', Edges.Vertices(EdgeIncrement,1));
                fprintf(fid,'\t %d', Edges.Vertices(EdgeIncrement,2));
                
                VertexCross21=(EqVertex(ListVertex(j))~=ListVertex(j))&&(EqVertex(ListVertex(mod(j,length(ListVertex))+1))==ListVertex(mod(j,length(ListVertex))+1));
                VertexCross12=(EqVertex(ListVertex(j))==ListVertex(j))&&(EqVertex(ListVertex(mod(j,length(ListVertex))+1))~=ListVertex(mod(j,length(ListVertex))+1));
                
                if((EqVertex(ListVertex(j))~=ListVertex(j))&&(EqVertex(ListVertex(mod(j,length(ListVertex))+1))==ListVertex(mod(j,length(ListVertex))+1)))
                    if(v(ListVertex(j),1)<0) Edges.Crossx12(EdgeIncrement)=1; end
                    if(v(ListVertex(j),1)>Mesh) Edges.Crossx21(EdgeIncrement)=1; end
                    if(v(ListVertex(j),2)<0) Edges.Crossy12(EdgeIncrement)=1; end
                    if(v(ListVertex(j),2)>Mesh) Edges.Crossy21(EdgeIncrement)=1; end
                end
                if((EqVertex(ListVertex(j))==ListVertex(j))&&(EqVertex(ListVertex(mod(j,length(ListVertex))+1))~=ListVertex(mod(j,length(ListVertex))+1)))
                    if(v(ListVertex(mod(j,length(ListVertex))+1),1)<0) Edges.Crossx21(EdgeIncrement)=1; end
                    if(v(ListVertex(mod(j,length(ListVertex))+1),1)>Mesh) Edges.Crossx12(EdgeIncrement)=1; end
                    if(v(ListVertex(mod(j,length(ListVertex))+1),2)<0) Edges.Crossy21(EdgeIncrement)=1; end
                    if(v(ListVertex(mod(j,length(ListVertex))+1),2)>Mesh) Edges.Crossy12(EdgeIncrement)=1; end
                end
                
                if((EqVertex(ListVertex(j))~=ListVertex(j))&&(EqVertex(ListVertex(mod(j,length(ListVertex))+1))~=ListVertex(mod(j,length(ListVertex))+1)))
                    v1=v(ListVertex(j),:);
                    v2=v(ListVertex(mod(j,length(ListVertex))+1),:);
                    eqv1=v(EqVertex(ListVertex(j)),:);
                    eqv2=v(EqVertex(ListVertex(mod(j,length(ListVertex))+1)),:);
                    if(v2(1)>v1(1))&&(eqv2(1)<eqv1(1)) Edges.Crossx12(EdgeIncrement)=1; end
                    if(v2(1)<v1(1))&&(eqv2(1)>eqv1(1)) Edges.Crossx21(EdgeIncrement)=1; end
                    if (v2(2)>v1(2))&&(eqv2(2)<eqv1(2))Edges.Crossy12(EdgeIncrement)=1; end
                    if (v2(2)<v1(2))&&(eqv2(2)>eqv1(2)) Edges.Crossy21(EdgeIncrement)=1; end
                end
                
                newxflag=0;
                newyflag=0;
                if ( (Edges.Crossx12(EdgeIncrement)==1) || (Edges.Crossx21(EdgeIncrement)==1) )
                    newxflag=1;
                end
                if ( (Edges.Crossy12(EdgeIncrement)==1) || (Edges.Crossy21(EdgeIncrement)==1) )
                    newyflag=1;
                end
                
                fprintf(fid,'\t %d \t %d \n', newxflag, newyflag);
                
            end
        end
    end
end

%% Then we go to the cells
cinside=cell(length(c),1);
CellIncrement=0;
for i=1:length(c)
    INGLOBAL=any((v(c{i},1)>0)&v(c{i},1)<Mesh&v(c{i},2)>0&v(c{i},2)<Mesh);
    if INGLOBAL
        CellIncrement=CellIncrement+1;
        cinside{CellIncrement}=EqVertex(c{i});
    end
end
NbCellsInside=CellIncrement;
cinside=cinside(1:NbCellsInside);
cinside2=cell(NbCellsInside,1);
CellIncrement=0;
for i=1:NbCellsInside
    dejavu=false;
    for j=1:i-1
        if ((length(cinside{i})==length(cinside{j}))&&all(sort(cinside{i})==sort(cinside{j}))) dejavu=true; end
    end
    if (~dejavu) CellIncrement=CellIncrement+1;cinside2{CellIncrement}=cinside{i}; end
end
cinside2=cinside2(1:CellIncrement);

fprintf(fid, 'Cells\n');
if (strcmp(potential,'tcj'))
    fprintf(fid, 'id\tA0\tEdgeNumb\tEdgeList\tPotential\tTCJStrength\tCellPressureType\n');
else
    fprintf(fid, 'id\tA0\tEdgeNumb\tEdgeList\tPotential\tTCJStrength\tThetaAn\tAnStrength\tCellPressureType\n');
end
for i=1:length(cinside2)
    A0_Cell = normrnd(A0, 200); % Using new A0
    ListVertex=c{i};
    fprintf(fid,'%d\t%f\t%d\t',i, A0_Cell, length(ListVertex));
    for j=1:length(ListVertex)
        NumeroEdge=0;
        for k=1:EdgeIncrement
            if (EqVertex((ListVertex(j)))==Edges.Vertices(k,1)...
                    &&EqVertex(ListVertex(mod(j,length(ListVertex))+1))==Edges.Vertices(k,2))
                NumeroEdge=k;
            end 
            if(EqVertex(ListVertex(j))==Edges.Vertices(k,2)...
                    &&EqVertex(ListVertex(mod(j,length(ListVertex))+1))==Edges.Vertices(k,1))
                NumeroEdge=k; 
            end
        end
        fprintf(fid,'%d,',NumeroEdge);
    end
    if (strcmp(potential, 'tcj'))
        fprintf(fid,'\t%s\t%f\t%s\n', potential, TCJSTRENGTH,CELLPRESSURETYPE);
    else
        fprintf(fid,'\t%s\t%f\t%f\t%f\t%s\n', potential, TCJSTRENGTH, THETAAN, ANSTRENGTH,CELLPRESSURETYPE);
    end
end
fprintf(fid, 'End\n');
fclose(fid);

% Visualization directory

visualizationDir = fullfile(projectRoot,"visualization_files",erase(nomfichier_filename,".txt") + "_visualization");
if ~exist(visualizationDir, 'dir')
    mkdir(visualizationDir); 
end