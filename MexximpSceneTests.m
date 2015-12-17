classdef MexximpSceneTests < matlab.unittest.TestCase
    
    properties (Constant)
        emptyScene = struct( ...
            'cameras', [], ...
            'lights', [], ...
            'materials', [], ...
            'meshes', [], ...
            'embeddedTextures', [], ...
            'rootNode', []);
        lightTypes = {'undefined', 'directional', 'point', 'spot'};
        dataTypes = {'float', 'string', 'integer', 'buffer'};
        textureSemantics = {'none', 'diffuse', 'specular', 'ambient', ...
            'emissive', 'height', 'normals', 'shininess', 'opacity', ...
            'displacement', 'light_map', 'reflection', 'unknown'};
        materialPropertyKeys = {'name', 'two_sided', 'shading_model', ...
            'enable_wireframe', 'blend_func', 'opacity', 'bump_scaling', ...
            'shininess', 'reflectivity', 'shininess_strength', 'refract_i', ...
            'diffuse', 'ambient', 'specular', 'emissive', 'transparent', ...
            'reflective', 'global_background_image', 'texture', ...
            'uvw_source', 'texture_op', 'mapping', 'texture_blend', ...
            'mapping_u', 'mapping_v', 'texture_map_axis', 'uv_transform', ...
            'texture_flags'};
        floatTolerance = 1e-6;
        itemSize = 1:10;
    end
    
    methods (Test)
        
        function testEmptySceneRoundTrip(testCase)
            testCase.doSceneRoundTrip(testCase.emptyScene);
        end
        
        function testCamerasRoundTrip(testCase)
            scene = testCase.emptyScene;
            for s = testCase.itemSize
                scene.cameras = struct( ...
                    'name', MexximpSceneTests.randomString(s), ...
                    'position', rand(3, 1), ...
                    'lookAtDirection', rand(3, 1), ...
                    'upDirection', rand(3, 1), ...
                    'aspectRatio', num2cell(rand(1, s)), ...
                    'horizontalFov', num2cell(rand(1, s)), ...
                    'clipPlaneFar', num2cell(rand(1, s)), ...
                    'clipPlaneNear', num2cell(rand(1, s)));
                testCase.doSceneRoundTrip(scene);
            end
        end
        
        function testLightsRoundTrip(testCase)
            scene = testCase.emptyScene;
            for s = testCase.itemSize
                scene.lights = struct( ...
                    'name', MexximpSceneTests.randomString(s), ...
                    'position', rand(3, 1), ...
                    'type', MexximpSceneTests.randomElements(s, testCase.lightTypes), ...
                    'lookAtDirection', rand(3, 1), ...
                    'innerConeAngle', num2cell(rand(1, s)), ...
                    'outerConeAngle', num2cell(rand(1, s)), ...
                    'constantAttenuation', num2cell(rand(1, s)), ...
                    'linearAttenuation', num2cell(rand(1, s)), ...
                    'quadraticAttenuation', num2cell(rand(1, s)), ...
                    'ambientColor', rand(3, 1), ...
                    'diffuseColor', rand(3, 1), ...
                    'specularColor', rand(3, 1));
                testCase.doSceneRoundTrip(scene);
            end
        end
        
        function testMaterialsRoundTrip(testCase)
            scene = testCase.emptyScene;
            for s = testCase.itemSize
                keys = MexximpSceneTests.randomElements(s, testCase.materialPropertyKeys);
                types = MexximpSceneTests.randomElements(s, testCase.dataTypes);
                datas = MexximpSceneTests.randomDatas(s, types);
                semantics = MexximpSceneTests.randomElements(s, testCase.textureSemantics);
                properties = struct( ...
                    'key', keys, ...
                    'dataType',  types, ...
                    'data', datas, ...
                    'textureSemantic', semantics, ...
                    'textureIndex', num2cell(randi([0 s], [1 s])));
                
                scene.materials = struct( ...
                    'properties', properties);
                testCase.doSceneRoundTrip(scene);
            end
        end
        function testMeshesRoundTrip(testCase)
            scene = testCase.emptyScene;
            for s = testCase.itemSize
                
                primitives = struct( ...
                    'point', rand() > .5, ...
                    'line', rand() > .5, ...
                    'triangle', rand() > .5, ...
                    'polygon', rand() > .5);
                
                scene.meshes = struct( ...
                    'name', MexximpSceneTests.randomString(s), ...
                    'materialIndex', num2cell(randi(s, 1, s)), ...
                    'primitiveTypes', primitives, ...
                    'vertices', rand(3, s), ...
                    'faces', [], ...
                    'colors0', rand(4, s), ...
                    'colors1', rand(4, s), ...
                    'colors2', rand(4, s), ...
                    'colors3', rand(4, s), ...
                    'colors4', rand(4, s), ...
                    'colors5', rand(4, s), ...
                    'colors6', rand(4, s), ...
                    'colors7', rand(4, s), ...
                    'normals', rand(3, s), ...
                    'tangents', rand(3, s), ...
                    'bitangents', rand(3, s), ...
                    'textureCoordinates0', rand(3, s), ...
                    'textureCoordinates1', rand(3, s), ...
                    'textureCoordinates2', rand(3, s), ...
                    'textureCoordinates3', rand(3, s), ...
                    'textureCoordinates4', rand(3, s), ...
                    'textureCoordinates5', rand(3, s), ...
                    'textureCoordinates6', rand(3, s), ...
                    'textureCoordinates7', rand(3, s));
                testCase.doSceneRoundTrip(scene);
            end
        end
        
    end
    
    methods (Access = private)
        function doSceneRoundTrip(testCase, scene)
            scenePrime = mexximpTest('scene', scene);
            testCase.assertEqual(scenePrime, scene, ...
                'AbsTol', testCase.floatTolerance);
        end
    end
    
    methods (Static)
        function string = randomString(stringSize)
            alphabet = '0':'z';
            string = alphabet(randi(numel(alphabet), [1, stringSize]));
        end
        
        function types = randomElements(nElements, allElements)
            types = allElements(randi(numel(allElements), [1, nElements]));
        end
        
        function datas = randomDatas(nElements, types)
            datas = cell(1, nElements);
            for ii = 1:nElements
                switch(types{ii})
                    case 'float'
                        datas{ii} = rand(1, nElements);
                    case 'string'
                        datas{ii} = MexximpSceneTests.randomString(nElements);
                    case 'integer'
                        datas{ii} = randi([-nElements nElements], 1, nElements, 'int32');
                    case 'buffer'
                        datas{ii} = randi(255, 1, nElements, 'uint8');
                    otherwise
                        datas{ii} = [];
                end
            end
        end
    end
end