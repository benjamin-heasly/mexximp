
#include "mexximp_scene.h"
#include "mexximp_util.h"
#include "mexximp_constants.h"

#include <mex.h>
#include <matrix.h>

namespace mexximp {
    
    // scene (top-level)
    
    // caller must pass in a newed aiScene
    unsigned to_assimp_scene(const mxArray* matlab_scene, aiScene* assimp_scene) {
        if (!matlab_scene || !assimp_scene || !mxIsStruct(matlab_scene)) {
            return 0;
        }
        
        if (!assimp_scene) {
            return 0;
        }
        
        mxArray* matlab_cameras = mxGetField(matlab_scene, 0, "cameras");
        assimp_scene->mNumCameras = to_assimp_cameras(matlab_cameras, &assimp_scene->mCameras);
        
        mxArray* matlab_lights = mxGetField(matlab_scene, 0, "lights");
        assimp_scene->mNumLights = to_assimp_lights(matlab_lights, &assimp_scene->mLights);
        
        mxArray* matlab_materials = mxGetField(matlab_scene, 0, "materials");
        assimp_scene->mNumMaterials = to_assimp_materials(matlab_materials, &assimp_scene->mMaterials);
        
        mxArray* matlab_meshes = mxGetField(matlab_scene, 0, "meshes");
        assimp_scene->mNumMeshes = to_assimp_meshes(matlab_meshes, &assimp_scene->mMeshes);
        
        mxArray* matlab_node = mxGetField(matlab_scene, 0, "rootNode");
        to_assimp_nodes(matlab_node, 0, &assimp_scene->mRootNode, 0);
        
        mxArray* matlab_textures = mxGetField(matlab_scene, 0, "embeddedTextures");
        assimp_scene->mNumTextures = to_assimp_textures(matlab_textures, &assimp_scene->mTextures);
        
        return 1;
    }
    
    unsigned to_matlab_scene(const aiScene* assimp_scene, mxArray** matlab_scene) {
        if (!matlab_scene) {
            return 0;
        }
        
        *matlab_scene = mxCreateStructMatrix(
                1,
                1,
                COUNT(scene_field_names),
                &scene_field_names[0]);
        
        if (!assimp_scene) {
            return 0;
        }
        
        mxArray* matlab_cameras;
        to_matlab_cameras(assimp_scene->mCameras, &matlab_cameras, assimp_scene->mNumCameras);
        mxSetField(*matlab_scene, 0, "cameras", matlab_cameras);
        
        mxArray* matlab_lights;
        to_matlab_lights(assimp_scene->mLights, &matlab_lights, assimp_scene->mNumLights);
        mxSetField(*matlab_scene, 0, "lights", matlab_lights);
        
        mxArray* matlab_materials;
        to_matlab_materials(assimp_scene->mMaterials, &matlab_materials, assimp_scene->mNumMaterials);
        mxSetField(*matlab_scene, 0, "materials", matlab_materials);
        
        mxArray* matlab_meshes;
        to_matlab_meshes(assimp_scene->mMeshes, &matlab_meshes, assimp_scene->mNumMeshes);
        mxSetField(*matlab_scene, 0, "meshes", matlab_meshes);
        
        mxArray* matlab_node = mxCreateStructMatrix(
                1,
                1,
                COUNT(node_field_names),
                &node_field_names[0]);
        to_matlab_nodes(assimp_scene->mRootNode, &matlab_node, 0);
        mxSetField(*matlab_scene, 0, "rootNode", matlab_node);
        
        mxArray* matlab_textures;
        to_matlab_textures(assimp_scene->mTextures, &matlab_textures, assimp_scene->mNumTextures);
        mxSetField(*matlab_scene, 0, "embeddedTextures", matlab_textures);
        
        return 1;
    }
    
    // cameras
    
    unsigned to_assimp_cameras(const mxArray* matlab_cameras, aiCamera*** assimp_cameras) {
        if (!matlab_cameras || !mxIsStruct(matlab_cameras)) {
            return 0;
        }
        
        unsigned num_cameras = mxGetNumberOfElements(matlab_cameras);
        *assimp_cameras = new aiCamera*[num_cameras];
        if (!*assimp_cameras) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_cameras; i++) {
            (*assimp_cameras)[i] = new aiCamera();
            
            get_xyz_in_place(matlab_cameras, i, "position", &(*assimp_cameras)[i]->mPosition);
            get_xyz_in_place(matlab_cameras, i, "lookAtDirection", &(*assimp_cameras)[i]->mLookAt);
            get_xyz_in_place(matlab_cameras, i, "upDirection", &(*assimp_cameras)[i]->mUp);
            get_string(matlab_cameras, i, "name", &(*assimp_cameras)[i]->mName, "camera");
            (*assimp_cameras)[i]->mAspect = get_scalar(matlab_cameras, i, "aspectRatio", 1.0);
            (*assimp_cameras)[i]->mClipPlaneNear = get_scalar(matlab_cameras, i, "clipPlaneNear", 0.1);
            (*assimp_cameras)[i]->mClipPlaneFar = get_scalar(matlab_cameras, i, "clipPlaneFar", 1000);
            (*assimp_cameras)[i]->mHorizontalFOV = get_scalar(matlab_cameras, i, "horizontalFov", 3.14159/4.0);
        }
        
        return num_cameras;
    }
    
    unsigned to_matlab_cameras(aiCamera** assimp_cameras, mxArray** matlab_cameras, unsigned num_cameras) {
        if (!matlab_cameras) {
            return 0;
        }
        
        if (!assimp_cameras || 0 == num_cameras) {
            *matlab_cameras = emptyDouble();
            return 0;
        }
        
        *matlab_cameras = mxCreateStructMatrix(
                1,
                num_cameras,
                COUNT(camera_field_names),
                &camera_field_names[0]);
        
        for (unsigned i = 0; i < num_cameras; i++) {
            set_xyz(*matlab_cameras, i, "position", &assimp_cameras[i]->mPosition, 1);
            set_xyz(*matlab_cameras, i, "lookAtDirection", &assimp_cameras[i]->mLookAt, 1);
            set_xyz(*matlab_cameras, i, "upDirection", &assimp_cameras[i]->mUp, 1);
            set_string(*matlab_cameras, i, "name", &assimp_cameras[i]->mName);
            set_scalar(*matlab_cameras, i, "aspectRatio", assimp_cameras[i]->mAspect);
            set_scalar(*matlab_cameras, i, "clipPlaneNear", assimp_cameras[i]->mClipPlaneNear);
            set_scalar(*matlab_cameras, i, "clipPlaneFar", assimp_cameras[i]->mClipPlaneFar);
            set_scalar(*matlab_cameras, i, "horizontalFov", assimp_cameras[i]->mHorizontalFOV);
        }
        
        return num_cameras;
    }
    
    // lights
    
    unsigned to_assimp_lights(const mxArray* matlab_lights, aiLight*** assimp_lights) {
        if (!matlab_lights || !assimp_lights || !mxIsStruct(matlab_lights)) {
            return 0;
        }
        
        unsigned num_lights = mxGetNumberOfElements(matlab_lights);
        *assimp_lights = new aiLight*[num_lights];
        if (!*assimp_lights) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_lights; i++) {
            (*assimp_lights)[i] = new aiLight();
            
            get_rgb_in_place(matlab_lights, i, "ambientColor", &(*assimp_lights)[i]->mColorAmbient);
            get_rgb_in_place(matlab_lights, i, "diffuseColor", &(*assimp_lights)[i]->mColorDiffuse);
            get_rgb_in_place(matlab_lights, i, "specularColor", &(*assimp_lights)[i]->mColorSpecular);
            get_xyz_in_place(matlab_lights, i, "position", &(*assimp_lights)[i]->mPosition);
            get_xyz_in_place(matlab_lights, i, "lookAtDirection", &(*assimp_lights)[i]->mDirection);
            get_string(matlab_lights, i, "name", &(*assimp_lights)[i]->mName, "light");
            (*assimp_lights)[i]->mType = light_type_code(get_c_string(matlab_lights, i, "type", "undefined"));
            (*assimp_lights)[i]->mAngleInnerCone = get_scalar(matlab_lights, i, "innerConeAngle", 2*3.14159);
            (*assimp_lights)[i]->mAngleOuterCone = get_scalar(matlab_lights, i, "outerConeAngle", 2*3.14159);
            (*assimp_lights)[i]->mAttenuationConstant = get_scalar(matlab_lights, i, "constantAttenuation", 1);
            (*assimp_lights)[i]->mAttenuationLinear = get_scalar(matlab_lights, i, "linearAttenuation", 0);
            (*assimp_lights)[i]->mAttenuationQuadratic = get_scalar(matlab_lights, i, "quadraticAttenuation", 0);
        }
        
        return num_lights;
    }
    
    unsigned to_matlab_lights(aiLight** assimp_lights, mxArray** matlab_lights, unsigned num_lights) {
        if (!matlab_lights) {
            return 0;
        }
        
        if (!assimp_lights || 0 == num_lights) {
            *matlab_lights = emptyDouble();
            return 0;
        }
        
        *matlab_lights = mxCreateStructMatrix(
                1,
                num_lights,
                COUNT(light_field_names),
                &light_field_names[0]);
        
        for (unsigned i = 0; i < num_lights; i++) {
            set_rgb(*matlab_lights, i, "ambientColor", &assimp_lights[i]->mColorAmbient, 1);
            set_rgb(*matlab_lights, i, "diffuseColor", &assimp_lights[i]->mColorDiffuse, 1);
            set_rgb(*matlab_lights, i, "specularColor", &assimp_lights[i]->mColorSpecular, 1);
            set_xyz(*matlab_lights, i, "position", &assimp_lights[i]->mPosition, 1);
            set_xyz(*matlab_lights, i, "lookAtDirection", &assimp_lights[i]->mDirection, 1);
            set_string(*matlab_lights, i, "name", &assimp_lights[i]->mName);
            set_c_string(*matlab_lights, i, "type", light_type_string(assimp_lights[i]->mType));
            set_scalar(*matlab_lights, i, "innerConeAngle", assimp_lights[i]->mAngleInnerCone);
            set_scalar(*matlab_lights, i, "outerConeAngle", assimp_lights[i]->mAngleOuterCone);
            set_scalar(*matlab_lights, i, "constantAttenuation", assimp_lights[i]->mAttenuationConstant);
            set_scalar(*matlab_lights, i, "linearAttenuation", assimp_lights[i]->mAttenuationLinear);
            set_scalar(*matlab_lights, i, "quadraticAttenuation", assimp_lights[i]->mAttenuationQuadratic);
        }
        
        return num_lights;
    }
    
    // materials
    
    unsigned to_assimp_materials(const mxArray* matlab_materials, aiMaterial*** assimp_materials) {
        if (!matlab_materials || !assimp_materials || !mxIsStruct(matlab_materials)) {
            return 0;
        }
        
        unsigned num_materials = mxGetNumberOfElements(matlab_materials);
        *assimp_materials = new aiMaterial*[num_materials];
        if (!*assimp_materials) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_materials; i++) {
            (*assimp_materials)[i] = new aiMaterial();
            
            // get 5 properties by default, we will allocate our own below
            (*assimp_materials)[i]->Clear();
            delete[] (*assimp_materials)[i]->mProperties;
            
            mxArray* matlab_properties = mxGetField(matlab_materials, i, "properties");
            unsigned num_properties = to_assimp_material_properties(
                    matlab_properties,
                    &(*assimp_materials)[i]->mProperties);
            (*assimp_materials)[i]->mNumAllocated = num_properties;
            (*assimp_materials)[i]->mNumProperties = num_properties;
        }
        
        return num_materials;
    }
    
    unsigned to_matlab_materials(aiMaterial** assimp_materials, mxArray** matlab_materials, unsigned num_materials) {
        if (!matlab_materials) {
            return 0;
        }
        
        if (!assimp_materials || 0 == num_materials) {
            *matlab_materials = emptyDouble();
            return 0;
        }
        
        *matlab_materials = mxCreateStructMatrix(
                1,
                num_materials,
                COUNT(material_field_names),
                &material_field_names[0]);
        
        for (unsigned i = 0; i < num_materials; i++) {
            mxArray* matlab_properties;
            to_matlab_material_properties(assimp_materials[i]->mProperties,
                    &matlab_properties,
                    assimp_materials[i]->mNumProperties);
            if (matlab_properties) {
                mxSetField(*matlab_materials, i, "properties", matlab_properties);
            }
        }
        
        return num_materials;
    }
    
    // material properties
    
    unsigned to_assimp_material_properties(const mxArray* matlab_properties, aiMaterialProperty*** assimp_properties) {
        if (!matlab_properties || !assimp_properties || !mxIsStruct(matlab_properties)) {
            return 0;
        }
        
        unsigned num_properties = mxGetNumberOfElements(matlab_properties);
        if (!num_properties) {
            return 0;
        }
        
        *assimp_properties = new aiMaterialProperty*[num_properties];
        if (!*assimp_properties) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_properties; i++) {
            (*assimp_properties)[i] = new aiMaterialProperty();
            
            (*assimp_properties)[i]->mKey.Set(ugly_key(get_c_string(matlab_properties, i, "key", "property")));
            
            (*assimp_properties)[i]->mIndex = get_scalar(matlab_properties, i, "textureIndex", 0);
            (*assimp_properties)[i]->mSemantic = texture_type_code(get_c_string(matlab_properties, i, "textureSemantic", "unknown"));
            
            aiPropertyTypeInfo type_code = material_property_type_code(get_c_string(matlab_properties, i, "dataType", "buffer"));
            (*assimp_properties)[i]->mType = type_code;
            
            unsigned num_bytes;
            (*assimp_properties)[i]->mData = get_property_data(matlab_properties, i, "data", type_code, &num_bytes);
            (*assimp_properties)[i]->mDataLength = num_bytes;
        }
        
        return num_properties;
    }
    
    unsigned to_matlab_material_properties(aiMaterialProperty** assimp_properties, mxArray** matlab_properties, unsigned num_properties) {
        if (!matlab_properties) {
            return 0;
        }
        
        if (!assimp_properties || 0 == num_properties) {
            *matlab_properties = emptyDouble();
            return 0;
        }
        
        *matlab_properties = mxCreateStructMatrix(
                1,
                num_properties,
                COUNT(material_property_field_names),
                &material_property_field_names[0]);
        
        for (unsigned i = 0; i < num_properties; i++) {
            set_c_string(*matlab_properties, i, "key", nice_key(assimp_properties[i]->mKey.C_Str()));
            set_scalar(*matlab_properties, i, "textureIndex", assimp_properties[i]->mIndex);
            set_c_string(*matlab_properties, i, "textureSemantic", texture_type_string((aiTextureType)assimp_properties[i]->mSemantic));
            
            aiPropertyTypeInfo type_code = assimp_properties[i]->mType;
            set_c_string(*matlab_properties, i, "dataType", material_property_type_string(type_code));
            set_property_data(*matlab_properties, i, "data", assimp_properties[i]->mData,  type_code, assimp_properties[i]->mDataLength);
        }
        
        return num_properties;
    }
    
    // meshes
    
    unsigned to_assimp_meshes(const mxArray* matlab_meshes, aiMesh*** assimp_meshes) {
        if (!matlab_meshes || !assimp_meshes || !mxIsStruct(matlab_meshes)) {
            return 0;
        }
        
        unsigned num_meshes = mxGetNumberOfElements(matlab_meshes);
        *assimp_meshes = new aiMesh*[num_meshes];
        if (!*assimp_meshes) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_meshes; i++) {
            (*assimp_meshes)[i] = new aiMesh();
            
            get_string(matlab_meshes, i, "name", &(*assimp_meshes)[i]->mName, "mesh");
            (*assimp_meshes)[i]->mMaterialIndex = get_scalar(matlab_meshes, i, "materialIndex", 0);
            (*assimp_meshes)[i]->mVertices = get_xyz(matlab_meshes, i, "vertices", &(*assimp_meshes)[i]->mNumVertices);
            (*assimp_meshes)[i]->mBitangents = get_xyz(matlab_meshes, i, "bitangents", 0);
            (*assimp_meshes)[i]->mNormals = get_xyz(matlab_meshes, i, "normals", 0);
            (*assimp_meshes)[i]->mTangents = get_xyz(matlab_meshes, i, "tangents", 0);
            (*assimp_meshes)[i]->mPrimitiveTypes = mesh_primitive_codes(mxGetField(matlab_meshes, i, "primitiveTypes"));
            
            (*assimp_meshes)[i]->mColors[0] = get_rgba(matlab_meshes, i, "colors0", 0);
            (*assimp_meshes)[i]->mColors[1] = get_rgba(matlab_meshes, i, "colors1", 0);
            (*assimp_meshes)[i]->mColors[2] = get_rgba(matlab_meshes, i, "colors2", 0);
            (*assimp_meshes)[i]->mColors[3] = get_rgba(matlab_meshes, i, "colors3", 0);
            (*assimp_meshes)[i]->mColors[4] = get_rgba(matlab_meshes, i, "colors4", 0);
            (*assimp_meshes)[i]->mColors[5] = get_rgba(matlab_meshes, i, "colors5", 0);
            (*assimp_meshes)[i]->mColors[6] = get_rgba(matlab_meshes, i, "colors6", 0);
            (*assimp_meshes)[i]->mColors[7] = get_rgba(matlab_meshes, i, "colors7", 0);
            
            (*assimp_meshes)[i]->mTextureCoords[0] = get_xyz(matlab_meshes, i, "textureCoordinates0", 0);
            (*assimp_meshes)[i]->mTextureCoords[1] = get_xyz(matlab_meshes, i, "textureCoordinates1", 0);
            (*assimp_meshes)[i]->mTextureCoords[2] = get_xyz(matlab_meshes, i, "textureCoordinates2", 0);
            (*assimp_meshes)[i]->mTextureCoords[3] = get_xyz(matlab_meshes, i, "textureCoordinates3", 0);
            (*assimp_meshes)[i]->mTextureCoords[4] = get_xyz(matlab_meshes, i, "textureCoordinates4", 0);
            (*assimp_meshes)[i]->mTextureCoords[5] = get_xyz(matlab_meshes, i, "textureCoordinates5", 0);
            (*assimp_meshes)[i]->mTextureCoords[6] = get_xyz(matlab_meshes, i, "textureCoordinates6", 0);
            (*assimp_meshes)[i]->mTextureCoords[7] = get_xyz(matlab_meshes, i, "textureCoordinates7", 0);
            
            mxArray* matlab_faces = mxGetField(matlab_meshes, i, "faces");
            (*assimp_meshes)[i]->mNumFaces = to_assimp_faces(matlab_faces, &(*assimp_meshes)[i]->mFaces);
        }
        
        return num_meshes;
    }
    
    unsigned to_matlab_meshes(aiMesh** assimp_meshes, mxArray** matlab_meshes, unsigned num_meshes) {
        if (!matlab_meshes) {
            return 0;
        }
        
        if (!assimp_meshes || 0 == num_meshes) {
            *matlab_meshes = emptyDouble();
            return 0;
        }
        
        *matlab_meshes = mxCreateStructMatrix(
                1,
                num_meshes,
                COUNT(mesh_field_names),
                &mesh_field_names[0]);
        
        for (unsigned i = 0; i < num_meshes; i++) {
            set_string(*matlab_meshes, i, "name", &assimp_meshes[i]->mName);
            set_scalar(*matlab_meshes, i, "materialIndex", assimp_meshes[i]->mMaterialIndex);
            set_xyz(*matlab_meshes, i, "vertices", assimp_meshes[i]->mVertices, assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "bitangents", assimp_meshes[i]->mBitangents, assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "normals", assimp_meshes[i]->mNormals, assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "tangents", assimp_meshes[i]->mTangents, assimp_meshes[i]->mNumVertices);
            mxSetField(*matlab_meshes, i, "primitiveTypes", mesh_primitive_struct(assimp_meshes[i]->mPrimitiveTypes));
            
            set_rgba(*matlab_meshes, i, "colors0", assimp_meshes[i]->mColors[0], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors1", assimp_meshes[i]->mColors[1], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors2", assimp_meshes[i]->mColors[2], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors3", assimp_meshes[i]->mColors[3], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors4", assimp_meshes[i]->mColors[4], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors5", assimp_meshes[i]->mColors[5], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors6", assimp_meshes[i]->mColors[6], assimp_meshes[i]->mNumVertices);
            set_rgba(*matlab_meshes, i, "colors7", assimp_meshes[i]->mColors[7], assimp_meshes[i]->mNumVertices);
            
            set_xyz(*matlab_meshes, i, "textureCoordinates0", assimp_meshes[i]->mTextureCoords[0], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates1", assimp_meshes[i]->mTextureCoords[1], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates2", assimp_meshes[i]->mTextureCoords[2], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates3", assimp_meshes[i]->mTextureCoords[3], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates4", assimp_meshes[i]->mTextureCoords[4], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates5", assimp_meshes[i]->mTextureCoords[5], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates6", assimp_meshes[i]->mTextureCoords[6], assimp_meshes[i]->mNumVertices);
            set_xyz(*matlab_meshes, i, "textureCoordinates7", assimp_meshes[i]->mTextureCoords[7], assimp_meshes[i]->mNumVertices);
            
            mxArray* matlab_faces;
            to_matlab_faces(assimp_meshes[i]->mFaces, &matlab_faces, assimp_meshes[i]->mNumFaces);
            if (matlab_faces) {
                mxSetField(*matlab_meshes, i, "faces", matlab_faces);
            }
        }
        
        return num_meshes;
    }
    
    // mesh faces
    
    unsigned to_assimp_faces(const mxArray* matlab_faces, aiFace** assimp_faces) {
        if (!matlab_faces || !assimp_faces || !mxIsStruct(matlab_faces)) {
            return 0;
        }
        
        unsigned num_faces = mxGetNumberOfElements(matlab_faces);
        *assimp_faces = new aiFace[num_faces];
        if (!*assimp_faces) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_faces; i++) {
            // ignore the nIndices passed from matlab -- it's just a convenience
            (*assimp_faces)[i].mIndices = get_indices(matlab_faces, i, "indices", &(*assimp_faces)[i].mNumIndices);
        }
        
        return num_faces;
    }
    
    unsigned to_matlab_faces(aiFace* assimp_faces, mxArray** matlab_faces, unsigned num_faces) {
        if (!matlab_faces) {
            return 0;
        }
        
        if (!assimp_faces || 0 == num_faces) {
            *matlab_faces = emptyDouble();
            return 0;
        }
        
        *matlab_faces = mxCreateStructMatrix(
                1,
                num_faces,
                COUNT(face_field_names),
                &face_field_names[0]);
        
        for (unsigned i = 0; i < num_faces; i++) {
            // pass nIndices explicitly to matlab as a convenience
            set_scalar(*matlab_faces, i, "nIndices", assimp_faces[i].mNumIndices);
            set_indices(*matlab_faces, i, "indices", assimp_faces[i].mIndices, assimp_faces[i].mNumIndices);
        }
        
        return num_faces;
    }
    
    // node hierarchy
    
    unsigned to_assimp_nodes(const mxArray* matlab_node, unsigned index, aiNode** assimp_node, aiNode* assimp_parent) {
        if (!matlab_node || !assimp_node || !mxIsStruct(matlab_node)) {
            return 0;
        }
        
        *assimp_node = new aiNode();
        if (!*assimp_node) {
            return 0;
        }
        
        (*assimp_node)->mParent = assimp_parent;
        get_string(matlab_node, index, "name", &(*assimp_node)->mName, "node");
        get_4x4_in_place(matlab_node, index, "transformation", &(*assimp_node)->mTransformation);
        (*assimp_node)->mMeshes = get_indices(matlab_node, index, "meshIndices", &(*assimp_node)->mNumMeshes);
        
        mxArray* matlab_children = mxGetField(matlab_node, index, "children");
        if (!matlab_children) {
            return 0;
        }
        
        unsigned num_children = mxGetNumberOfElements(matlab_children);
        (*assimp_node)->mNumChildren = num_children;
        if (!num_children) {
            return 0;
        }
        
        // child array
        aiNode** child_array = new aiNode*[num_children];
        if (!child_array) {
            return 0;
        }
        (*assimp_node)->mChildren = child_array;
        
        unsigned num_descendants = num_children;
        for (int i = 0; i<num_children; i++) {
            num_descendants += to_assimp_nodes(matlab_children, i, &child_array[i], *assimp_node);
        }
        
        return num_descendants;
    }
    
    unsigned to_matlab_nodes(aiNode* assimp_node, mxArray** matlab_node, unsigned index) {
        if (!matlab_node) {
            return 0;
        }
        
        if (!assimp_node) {
            *matlab_node = emptyDouble();
            return 0;
        }
        
        set_string(*matlab_node, index, "name", &assimp_node->mName);
        set_indices(*matlab_node, index, "meshIndices", assimp_node->mMeshes, assimp_node->mNumMeshes);
        set_4x4(*matlab_node, index, "transformation", &assimp_node->mTransformation, 1);
        
        unsigned num_children = assimp_node->mNumChildren;
        if (!num_children) {
            return 0;
        }
        
        mxArray* children = mxCreateStructMatrix(
                1,
                num_children,
                COUNT(node_field_names),
                &node_field_names[0]);
        if (!children) {
            return 0;
        }
        
        mxSetField(*matlab_node, index, "children", children);
        
        unsigned num_descendants = num_children;
        for (int i = 0; i<num_children; i++) {
            num_descendants += to_matlab_nodes(assimp_node->mChildren[i], &children, i);
        }
        
        return num_descendants;
    }
    
    // embedded textures
    
    unsigned to_assimp_textures(const mxArray* matlab_textures, aiTexture*** assimp_textures) {
        if (!matlab_textures || !assimp_textures || !mxIsStruct(matlab_textures)) {
            return 0;
        }
        
        unsigned num_textures = mxGetNumberOfElements(matlab_textures);
        *assimp_textures = new aiTexture*[num_textures];
        if (!*assimp_textures) {
            return 0;
        }
        
        for (unsigned i = 0; i < num_textures; i++) {
            (*assimp_textures)[i] = new aiTexture();
            
            mxArray* matlab_texels = mxGetField(matlab_textures, i, "image");
            if (!mxIsUint8(matlab_texels)) {
                continue;
            }
            
            unsigned num_dims = mxGetNumberOfDimensions(matlab_texels);
            const mwSize* dims = mxGetDimensions(matlab_texels);
            (*assimp_textures)[i]->mHeight = 0;
            if (4 == dims[0]) {
                if (2 == num_dims) {
                    (*assimp_textures)[i]->mWidth = dims[1];
                    (*assimp_textures)[i]->mHeight = 1;
                } else if (3 == num_dims) {
                    (*assimp_textures)[i]->mWidth = dims[1];
                    (*assimp_textures)[i]->mHeight = dims[2];
                }
            }
            
            if (0 == (*assimp_textures)[i]->mHeight) {
                // raw bytes of compressed texture
                (*assimp_textures)[i]->pcData = (aiTexel*) get_bytes(matlab_textures, i, "image", &(*assimp_textures)[i]->mWidth);
                
                // take up to 3 chars of format string, plus null terminator
                mxArray* matlab_format = mxGetField(matlab_textures, i, "format");
                mxGetString(matlab_format, (*assimp_textures)[i]->achFormatHint, 4);
                
            } else {
                // rgba8888 texels of uncompressed texture
                (*assimp_textures)[i]->pcData = get_texel(matlab_textures, i, "image", 0);
                // let format string remain zeroed
            }
        }
        
        return num_textures;
    }
    
    unsigned to_matlab_textures(aiTexture** assimp_textures, mxArray** matlab_textures, unsigned num_textures) {
        if (!matlab_textures) {
            return 0;
        }
        
        if (!assimp_textures || 0 == num_textures) {
            *matlab_textures = emptyDouble();
            return 0;
        }
        
        *matlab_textures = mxCreateStructMatrix(
                1,
                num_textures,
                COUNT(texture_field_names),
                &texture_field_names[0]);
        
        for (unsigned i = 0; i < num_textures; i++) {
            if (0 == assimp_textures[i]->mHeight) {
                // raw bytes of compressed texture
                set_bytes(*matlab_textures, i, "image", (char*)assimp_textures[i]->pcData, assimp_textures[i]->mWidth);
                set_c_string(*matlab_textures, i, "format", assimp_textures[i]->achFormatHint);
                
            } else {
                // rgba8888 texels of uncompressed texture
                set_texel(*matlab_textures, i, "image", assimp_textures[i]->pcData, assimp_textures[i]->mWidth, assimp_textures[i]->mHeight);
                set_c_string(*matlab_textures, i, "format", "");
            }
        }
        
        return num_textures;
    }
    
}
