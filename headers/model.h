#ifndef MODEL_H
#define MODEL_H

#include "../includes/glad/glad.h" 

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/assimp/Importer.hpp"
#include "../includes/assimp/scene.h"
#include "../includes/assimp/postprocess.h"

#include "../headers/stb_image.h"
#include "../headers/shader_s.h"
#include "../headers/mesh.h"

#include <windows.h>
#include <cstdlib>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model {
public:
	// model data
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;

	bool gammaCorrection;
	Model(std::string const &path, bool gamma = false);
	void Draw(Shader &shader);

private:
	const aiScene* scene = nullptr;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform = glm::mat4(1.0f));
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false, const aiScene* scene = nullptr);
};
#endif