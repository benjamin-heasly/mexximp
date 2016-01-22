%% Make Mexximp scene from scratch.
%
% This is intended as a well-known input or "fixture" to be used by the
% accompanying tests.  It's not inteded to be a general-purpose utility.
%
% It's also an explicit and long-winded deomonstration of how to construct
% a valid scene.  Being explicit and long-winded seems good to do once.
% Surely we will want utilities to make this easier going forward.
%
% BSH
function scene = makeTestScene()

% mexximpConstants gives us template structs to fill in
scene = mexximpConstants('scene');

%% Camera.
camera = mexximpConstants('camera');
camera.name = 'camera';
camera.position = [0 0 0];
camera.lookAtDirection = [0 0 1];
camera.upDirection = [0 1 0];
camera.aspectRatio = [1 1 1];
camera.horizontalFov = pi()/4;
camera.clipPlaneFar = 1000;
camera.clipPlaneNear = 0.1;
scene.cameras = camera;

%% Lights.
yellowLight = mexximpConstants('light');
yellowLight.name = 'yellowLight';
yellowLight.position = [0 0 0];
yellowLight.type = 'spot';
yellowLight.lookAtDirection = [0 0 1];
yellowLight.innerConeAngle = pi()/6;
yellowLight.outerConeAngle = pi()/6;
yellowLight.constantAttenuation = 1;
yellowLight.linearAttenuation = 0;
yellowLight.quadraticAttenuation = 1;
yellowLight.ambientColor = [0 0 0];
yellowLight.diffuseColor = [1 1 0];
yellowLight.specularColor = [1 1 0];

redLight = yellowLight;
redLight.name = 'redLight';
redLight.diffuseColor = [1 0 0];
redLight.specularColor = [1 0 0];

greenLight = yellowLight;
greenLight.name = 'greenLight';
greenLight.diffuseColor = [0 1 0];
greenLight.specularColor = [0 1 0];

distantLight = mexximpConstants('light');
distantLight.name = 'distantLight';
distantLight.position = [0 0 0];
distantLight.type = 'directional';
distantLight.lookAtDirection = [0 0 1];
distantLight.innerConeAngle = 0;
distantLight.outerConeAngle = 0;
distantLight.constantAttenuation = 1;
distantLight.linearAttenuation = 0;
distantLight.quadraticAttenuation = 0;
distantLight.ambientColor = [0 0 0];
distantLight.diffuseColor = .001*[1 1 1];
distantLight.specularColor = .001*[1 1 1];

scene.lights = [yellowLight redLight greenLight distantLight];

%% Materials.
whiteShiny = mexximpConstants('material');
whiteShiny.properties = makeUberProperties('whiteShiny', ...
    [0 0 0 0], ...
    [1 1 1 1], ...
    [1 1 1 1], ...
    [0 0 0 0], ...
    [0 0 0 0], ...
    0, ...
    1);

whiteMatte = mexximpConstants('material');
uberProps = makeUberProperties('whiteMatte', ...
    [0 0 0 0], ...
    [1 1 1 1], ...
    [0 0 0 0], ...
    [0 0 0 0], ...
    [0 0 0 0], ...
    0, ...
    1);
fileName = which('stone_wall.exr');
textureProps = makeUVTextureProps(fileName, 'diffuse', 0);
whiteMatte.properties = [uberProps textureProps];

scene.materials = [whiteShiny, whiteMatte];

%% Meshes.
plane = makeMesh('plane', 1);
plane.vertices = [ ...
    -1 -1 0;
    -1 +1 0;
    +1 -1 0;
    +1 +1 0]';

% by coincidence: uv coordinates same as vertices
plane.textureCoordinates0 = plane.vertices;

% normals perpendicular to plane, towards camera
plane.normals = -[ ...
    0 0 -1;
    0 0 -1;
    0 0 -1;
    0 0 -1]';
plane.faces(1) = makeFace([0 2 1]);
plane.faces(2) = makeFace([1 2 3]);
plane.primitiveTypes = mexximpConstants('meshPrimitive');
plane.primitiveTypes.triangle = true;

icomesh = icosphere(3);
ball = makeMesh('ball', 0);
ball.vertices = icomesh.Vertices';
ball.normals = icomesh.VertexNormals';
facesCell = cell(1, size(icomesh.Faces, 1));
for ii = 1:size(icomesh.Faces, 1)
    facesCell{ii} = makeFace(icomesh.Faces(ii,:)-1);
end
ball.faces = [facesCell{:}];
ball.primitiveTypes = mexximpConstants('meshPrimitive');
ball.primitiveTypes.triangle = true;

scene.meshes = [plane, ball];

%% Embedded Textures.
% we don't need embedded textures

%% Node Hierarchy.
rootNode = mexximpConstants('node');
rootNode.name = 'root';
rootNode.transformation = eye(4);

% node with same name as camera will contain the camera
cameraNode = mexximpConstants('node');
cameraNode.name = camera.name;
cameraNode.transformation = mexximpLookAt([3 5 10], [0 0 -5], [0 1 0]);

% node with same name as a light will contain the light
xLightNode = mexximpConstants('node');
xLightNode.name = redLight.name;
xLightNode.transformation = mexximpRotate([0 1 0], pi()/2) * mexximpTranslate([25 0 0]);

yLightNode = mexximpConstants('node');
yLightNode.name = yellowLight.name;
yLightNode.transformation = mexximpRotate([1 0 0], -pi()/2) * mexximpTranslate([0 25 0]);

zLightNode = mexximpConstants('node');
zLightNode.name = greenLight.name;
zLightNode.transformation = mexximpTranslate([0 0 25]);

distantLightNode = mexximpConstants('node');
distantLightNode.name = distantLight.name;
distantLightNode.transformation = eye(4);

% nodes instantiante meshes using indexes into scene.meshes
backdropNode = mexximpConstants('node');
backdropNode.name = 'backdrop';
backdropNode.transformation = mexximpScale([20 20 1]) * mexximpTranslate([0 0 -10]);
backdropNode.meshIndices = uint32(0);

originNode = mexximpConstants('node');
originNode.name = 'origin';
originNode.transformation = mexximpRotate([1 0 -1], pi()/30);
originNode.meshIndices = uint32(1);

rootNode.children = [cameraNode, ...
    xLightNode, ...
    yLightNode, ...
    zLightNode, ...
    distantLightNode, ...
    backdropNode, ...
    originNode];
scene.rootNode = rootNode;

%% Make a face struct with the given vertex indices.
function face = makeFace(indices)
face = mexximpConstants('face');
face.nIndices = numel(indices);
face.indices = uint32(indices);

%% Pack up a mesh including some defaults.
function mesh = makeMesh(name, materialIndex)
mesh = mexximpConstants('mesh');
mesh.name = name;
mesh.materialIndex = uint32(materialIndex);

mesh.tangents = zeros(3,0);
mesh.bitangents = zeros(3,0);

mesh.colors0 = zeros(4,0);
mesh.colors1 = zeros(4,0);
mesh.colors2 = zeros(4,0);
mesh.colors3 = zeros(4,0);
mesh.colors4 = zeros(4,0);
mesh.colors5 = zeros(4,0);
mesh.colors6 = zeros(4,0);
mesh.colors7 = zeros(4,0);

mesh.textureCoordinates0 = zeros(3,0);
mesh.textureCoordinates1 = zeros(3,0);
mesh.textureCoordinates2 = zeros(3,0);
mesh.textureCoordinates3 = zeros(3,0);
mesh.textureCoordinates4 = zeros(3,0);
mesh.textureCoordinates5 = zeros(3,0);
mesh.textureCoordinates6 = zeros(3,0);
mesh.textureCoordinates7 = zeros(3,0);

%% Pack up properties for an "uber" material.
function properties = makeUberProperties(name, ambient, diffuse, specular, emissive, reflective, shininess, refractIndex)
% a material is a flexible collection of "properties"
properties(1) = struct( ...
    'key', 'name', ...
    'dataType', 'string', ...
    'data', name, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(2) = struct( ...
    'key', 'shading_model', ...
    'dataType', 'integer', ...
    'data', 2, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(3) = struct( ...
    'key', 'two_sided', ...
    'dataType', 'integer', ...
    'data', 1, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(4) = struct( ...
    'key', 'enable_wireframe', ...
    'dataType', 'integer', ...
    'data', 0, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(5) = struct( ...
    'key', 'ambient', ...
    'dataType', 'float', ...
    'data', ambient, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(6) = struct( ...
    'key', 'diffuse', ...
    'dataType', 'float', ...
    'data', diffuse, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(7) = struct( ...
    'key', 'specular', ...
    'dataType', 'float', ...
    'data', specular, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(8) = struct( ...
    'key', 'emissive', ...
    'dataType', 'float', ...
    'data', emissive, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(9) = struct( ...
    'key', 'reflective', ...
    'dataType', 'float', ...
    'data', reflective, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(10) = struct( ...
    'key', 'shininess', ...
    'dataType', 'float', ...
    'data', shininess, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(11) = struct( ...
    'key', 'reflectivity', ...
    'dataType', 'float', ...
    'data', 0, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);
properties(12) = struct( ...
    'key', 'refract_i', ...
    'dataType', 'float', ...
    'data', refractIndex, ...
    'textureSemantic', 'none', ...
    'textureIndex', 0);

%% Pack up properties for a diffuse reflectance texture.
function properties = makeUVTextureProps(fileName, semantic, index)
% a material is a flexible collection of "properties"
properties(1) = struct( ...
    'key', 'texture', ...
    'dataType', 'string', ...
    'data', fileName, ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
properties(2) = struct( ...
    'key', 'mapping_u', ...
    'dataType', 'integer', ...
    'data', 0, ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
properties(3) = struct( ...
    'key', 'mapping_v', ...
    'dataType', 'integer', ...
    'data', 0, ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
properties(4) = struct( ...
    'key', 'uv_transform', ...
    'dataType', 'float', ...
    'data', [0 0 1 1 0], ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
properties(5) = struct( ...
    'key', 'texture_blend', ...
    'dataType', 'float', ...
    'data', 1, ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
properties(6) = struct( ...
    'key', 'uvw_source', ...
    'dataType', 'integer', ...
    'data', 0, ...
    'textureSemantic', semantic, ...
    'textureIndex', index);
