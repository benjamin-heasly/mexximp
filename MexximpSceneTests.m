classdef MexximpSceneTests < matlab.unittest.TestCase
    
    properties (Constant)
        floatTolerance = 1e-6;
    end
    
    methods (Test)
        
        function testEmptySceneRoundTrip(testCase)
            scene = struct( ...
                'cameras', [], ...
                'lights', [], ...
                'materials', [], ...
                'meshes', [], ...
                'embeddedTextures', [], ...
                'rootNode', []);
            scenePrime = mexximpTest('scene', scene);
            testCase.assertEqual(scenePrime, scene, ...
                'AbsTol', testCase.floatTolerance);
        end
    end
end