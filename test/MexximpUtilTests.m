classdef MexximpUtilTests < matlab.unittest.TestCase
    
    properties (Constant)
        floatTolerance = 1e-6;
        itemSize = [0:9 10:10:1023];
    end
    
    methods (Test)
        
        function testXyzRoundTrips(testCase)
            for ii = 1:numel(testCase.itemSize)
                vectors = rand(3, testCase.itemSize(ii));
                vectorsPrime = mexximpTest('xyz', vectors);
                testCase.assertEqual(vectorsPrime, vectors, ...
                    'AbsTol', testCase.floatTolerance);
            end
        end
        
        function testStringRoundTrips(testCase)
            alphabet = '0':'z';
            for ii = 1:numel(testCase.itemSize)
                string = alphabet(randi(numel(alphabet), [1, testCase.itemSize(ii)]));
                stringPrime = mexximpTest('string', string);
                testCase.assertEqual(stringPrime, string);
            end
        end
        
        function testRgbRoundTrips(testCase)
            for ii = 1:numel(testCase.itemSize)
                rgbs = rand(3, testCase.itemSize(ii));
                rgbsPrime = mexximpTest('rgb', rgbs);
                testCase.assertEqual(rgbsPrime, rgbs, ...
                    'AbsTol', testCase.floatTolerance);
            end
        end
        
        function testRgbaRoundTrips(testCase)
            for ii = 1:numel(testCase.itemSize)
                rgbas = rand(4, testCase.itemSize(ii));
                rgbasPrime = mexximpTest('rgba', rgbas);
                testCase.assertEqual(rgbasPrime, rgbas, ...
                    'AbsTol', testCase.floatTolerance);
            end
        end
        
        function testTexelRoundTrips(testCase)
            for ii = 1:numel(testCase.itemSize)
                texels = randi(255, [4, testCase.itemSize(ii)], 'uint8');
                texelsPrime = mexximpTest('texel', texels);
                testCase.assertEqual(texelsPrime, texels);
            end
        end
        
        function test4x4RoundTrips(testCase)
            for ii = 1:numel(testCase.itemSize)
                matrices = rand(4, 4, testCase.itemSize(ii));
                matricesPrime = mexximpTest('4x4', matrices);
                testCase.assertEqual(matricesPrime, matrices, ...
                    'AbsTol', testCase.floatTolerance);
            end
        end
        
    end
end
