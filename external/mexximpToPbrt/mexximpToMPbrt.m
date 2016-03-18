function pbrtScene = mexximpToMPbrt(mexximpScene, varargin)
%% Convert a mexximp scene struct to an mPbrtScene object.
%
% pbrtScene = mexximpToMPbrt(mexximpScene) converts the given
% mexximpScene struct to an MPbrtScene object suitable for modifying,
% writing to file, rendering, etc.
%
% This function forwards any named parameters to various helper functions,
% including:
%   - mexximpCameraToMPbrt()
%   - mexximpLightToMPbrt()
%   - mexximpMaterialToMPbrt()
%   - mexximpMeshToMPbrt()
%   - mexximpNodeToMPbrt()
% Please see these functions documentation about what parameters they
% accept.  (Sorry not to reproduce all of this this parameter documentation
% here. It would be handy for a while, but it would probably go out of
% date.)
%
% Returns an MPbrtScene object based on the given mexximpScene struct.
%
% pbrtScene = mexximpToMPbrt(mexximpScene, varargin)
%
% Copyright (c) 2016 mexximp Team

parser = inputParser();
parser.addRequired('mexximpScene', @isstruct);
parser.parse(mexximpScene);
mexximpScene = parser.Results.scene;

%% Fresh scene to add to.
pbrtScene = MPbrtScene();

%% Camera and POV transformations.
elementTypes = {elements.type};
cameraInds = find(strcmp('cameras', elementTypes));
for cc = cameraInds
    pbrtNode = mexximpCameraToMPbrt(mexximpScene, elements(cc), varargin{:});
    pbrtScene.overall.append(pbrtNode);
end

%% MakeNamedMaterial for each material.
%   Invoked with NamedMaterial by nodes below.
materialInds = find(strcmp('materials', elementTypes));
for mm = materialInds
    pbrtNode = mexximpMaterialToMPbrt(mexximpScene, elements(mm), varargin{:});
    pbrtScene.world.append(pbrtNode);
end

%% Lights and world transformations with AttributeBegin/End.
lightInds = find(strcmp('lights', elementTypes));
for ll = lightInds
    pbrtNode = mexximpLightToMPbrt(mexximpScene, elements(ll), varargin{:});
    pbrtScene.world.append(pbrtNode);
end

%% Named ObjectBegin/End for each mesh.
%   Invoked with ObjectInstance by nodes below.
meshInds = find(strcmp('meshes', elementTypes));
for mm = meshInds
    pbrtNode = mexximpMeshToMPbrt(mexximpScene, elements(mm), varargin{:});
    pbrtScene.world.append(pbrtNode);
end

%% Objects and world transformations with AttributeBegin/End.
nodeInds = find(strcmp('nodes', elementTypes));
for nn = nodeInds
    [pbrtNode, nObjects] = mexximpNodeToMPbrt(mexximpScene, elements(nn), varargin{:});
    
    % skip nodes that don't invoke any mesh objects
    if 0 < nObjects
        pbrtScene.world.append(pbrtNode);
    end
end
