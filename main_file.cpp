/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <windows.h>

float kamera;
int turn = 0;
float speedf = 0;  //speed forward
float speedr = 0; //speed right
float rotate = 0;//[radiany/s]
float turnWheel = 0; //do obracania kolami podczas skretu
float angleWheel = 0; // aby sie kola krencily
float Wheelturn = 0;  //kumuluj sie
float moving = 0;   //sprawdzanie w którą stronę porusza sie samochod
GLuint tex;
GLuint texROAD;
GLuint texBAR;
GLuint texWheel;

std::vector<glm::vec4> verts;      //do modelu
std::vector<glm::vec4> norms;
std::vector<glm::vec2> texcoords;
std::vector<unsigned int>indices;

std::vector<glm::vec4> vertsROAD;      //do modelu
std::vector<glm::vec4> normsROAD;
std::vector<glm::vec2> texcoordsROAD;
std::vector<unsigned int>indicesROAD;

std::vector<glm::vec4> vertsBAR;      //do modelu
std::vector<glm::vec4> normsBAR;
std::vector<glm::vec2> texcoordsBAR;
std::vector<unsigned int>indicesBAR;

std::vector<glm::vec4> vertsWheel;      //do modelu
std::vector<glm::vec4> normsWheel;
std::vector<glm::vec2> texcoordsWheel;
std::vector<unsigned int>indicesWheel;

void LoadModel(std::string plik)    //ladowanie modelu
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	auto mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i]; //aiVector3D podobny do glm ::vec3

		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		aiVector3D normal = mesh->mNormals[i]; //Wektory znormalizowane

		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		//std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;

		aiVector3D texcoord = mesh->mTextureCoords[0][i];
		//x, y, z wykorzystywane jako u, v, w . 0 jeżeli tekstura ma mniej wymiarów

		texcoords.push_back(glm::vec2(texcoord.x, texcoord.y));

		//std::cout << texcoord.x << " " << texcoord.y << std::endl;
	}

	//dla każdego wielokąta składowego
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i]; //face to jeden z wielokątów siatki
		//dla każdego indeksu-> wierzchołka tworzącego wielokąt
			//dla aiProcess_Triangulate to zawsze będzie 3
		for (int j = 0; j < face.mNumIndices; j++)
		{
			//std::cout << face.mIndices[j] << " ";
			indices.push_back(face.mIndices[j]);
		}
		//std::cout << std::endl;
	}
}

void LoadModelROAD (std::string plik)    //ladowanie modelu
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	auto mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i]; //aiVector3D podobny do glm ::vec3

		vertsROAD.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		aiVector3D normal = mesh->mNormals[i]; //Wektory znormalizowane

		normsROAD.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		//std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;

		aiVector3D texcoordROAD = mesh->mTextureCoords[0][i];
		//x, y, z wykorzystywane jako u, v, w . 0 jeżeli tekstura ma mniej wymiarów

		texcoordsROAD.push_back(glm::vec2(texcoordROAD.x, texcoordROAD.y));

		//std::cout << texcoord.x << " " << texcoord.y << std::endl;
	}

	//dla każdego wielokąta składowego
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i]; //face to jeden z wielokątów siatki
		//dla każdego indeksu-> wierzchołka tworzącego wielokąt
			//dla aiProcess_Triangulate to zawsze będzie 3
		for (int j = 0; j < face.mNumIndices; j++)
		{
			//std::cout << face.mIndices[j] << " ";
			indicesROAD.push_back(face.mIndices[j]);
		}
		//std::cout << std::endl;
	}
}

void LoadModelBAR (std::string plik)    //ladowanie modelu
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	auto mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i]; //aiVector3D podobny do glm ::vec3

		vertsBAR.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		aiVector3D normal = mesh->mNormals[i]; //Wektory znormalizowane

		normsBAR.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		//std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;

		aiVector3D texcoordBAR = mesh->mTextureCoords[0][i];
		//x, y, z wykorzystywane jako u, v, w . 0 jeżeli tekstura ma mniej wymiarów

		texcoordsBAR.push_back(glm::vec2(texcoordBAR.x, texcoordBAR.y));

		//std::cout << texcoord.x << " " << texcoord.y << std::endl;
	}

	//dla każdego wielokąta składowego
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i]; //face to jeden z wielokątów siatki
		//dla każdego indeksu-> wierzchołka tworzącego wielokąt
			//dla aiProcess_Triangulate to zawsze będzie 3
		for (int j = 0; j < face.mNumIndices; j++)
		{
			//std::cout << face.mIndices[j] << " ";
			indicesBAR.push_back(face.mIndices[j]);
		}
		//std::cout << std::endl;
	}
}

void LoadModelWheel (std::string plik)    //ladowanie modelu
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(plik,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	auto mesh = scene->mMeshes[0];
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i]; //aiVector3D podobny do glm ::vec3

		vertsWheel.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
		aiVector3D normal = mesh->mNormals[i]; //Wektory znormalizowane

		normsWheel.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		//std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;

		aiVector3D texcoordWheel = mesh->mTextureCoords[0][i];
		//x, y, z wykorzystywane jako u, v, w . 0 jeżeli tekstura ma mniej wymiarów

		texcoordsWheel.push_back(glm::vec2(texcoordWheel.x, texcoordWheel.y));

		//std::cout << texcoord.x << " " << texcoord.y << std::endl;
	}

	//dla każdego wielokąta składowego
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i]; //face to jeden z wielokątów siatki
		//dla każdego indeksu-> wierzchołka tworzącego wielokąt
			//dla aiProcess_Triangulate to zawsze będzie 3
		for (int j = 0; j < face.mNumIndices; j++)
		{
			//std::cout << face.mIndices[j] << " ";
			indicesWheel.push_back(face.mIndices[j]);
		}
		//std::cout << std::endl;
	}
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

GLuint readTextureROAD(const char* filename) {
	GLuint texROAD;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &texROAD); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, texROAD); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texROAD;
}

GLuint readTextureBAR(const char* filename) {
	GLuint texBAR;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &texBAR); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, texBAR); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texBAR;
}

GLuint readTextureWheel(const char* filename) {
	GLuint texWheel;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &texWheel); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, texWheel); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texWheel;
}

void enviroment()     //tu znajduje sie cale srodowisko tj.ziemia, trasa i barierki
{
	/*float vertsROAD[] =
	{
		//
		12,0,-4,1,
		12,0,4,1,
		-12,0,-4,1,

		-12,0,-4,1,
		12,0,4,1,
		-12,0,4,1,
		//one side
		//
		12,0,-4,1,
		12,0,4,1,
		24,0,4,1,

		24,0,4,1,
		12,0,4,1,
		18,0,8,1,

		24,0,4,1,
		24,0,12,1,
		18,0,8,1,

		18,0,8,1,
		12,0,16,1,
		12,0,24,1,

		12,0,24,1,
		24,0,12,1,
		18,0,8,1,
		//Die Kurve 1
		//
		12,0,16,1,
		12,0,24,1,
		-12,0,16,1,

		-12,0,16,1,
		12,0,24,1,
		-12,0,24,1,
		//sec side
		//
		-12,0,-4,1,
		-12,0,4,1,
		-24,0,4,1,

		-24,0,4,1,
		-12,0,4,1,
		-18,0,8,1,

		-24,0,4,1,
		-24,0,12,1,
		-18,0,8,1,

		-18,0,8,1,
		-12,0,16,1,
		-12,0,24,1,

		-12,0,24,1,
		-24,0,12,1,
		-18,0,8,1,
		//die Kurve 2
		//
	};
	float colorsROAD[] =
	{
		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
		//one side

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
		//Die Kurve 1

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
		//sec side

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,

		1,0,0,1,
		1,0,0,1,
		1,0,0,1,
		//die Kurve 2
		//
	};
	vertexCount = 42;*/
	//Wczytanie i import obrazka – w initOpenGLProgram
	glm::mat4 RoadPosition = glm::mat4(1.0f);
	RoadPosition = glm::scale(RoadPosition, glm::vec3(80.0f, 0.5f, 50.0f));
	RoadPosition = glm::rotate(RoadPosition, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	RoadPosition = glm::translate(RoadPosition, glm::vec3(1.0f, 0.0f, 0.0f));
	spLambert->use(); //Aktywuj program cieniujący
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(RoadPosition));

	glEnableVertexAttribArray(spLambert->a("vertex"));
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, vertsROAD.data()); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spLambert->a("texCoord"));
	glVertexAttribPointer(spLambert->a("texCoord"), 2, GL_FLOAT, false, 0, texcoordsROAD.data()); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

	glEnableVertexAttribArray(spLambert->a("normal"));
	glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, normsROAD.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texROAD);
	glUniform1i(spLambert->u("texROAD"), 0);

	glDrawElements(GL_TRIANGLES, indicesROAD.size(), GL_UNSIGNED_INT, indicesROAD.data());

	glDisableVertexAttribArray(spLambert->a("vertex"));
	glDisableVertexAttribArray(spLambert->a("normal"));
	glDisableVertexAttribArray(spLambert->a("color"));


	spLambert->use(); //Aktywuj program cieniujący
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(RoadPosition));

	glEnableVertexAttribArray(spLambert->a("vertex"));
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, vertsBAR.data()); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spLambert->a("texCoord"));
	glVertexAttribPointer(spLambert->a("texCoord"), 2, GL_FLOAT, false, 0, texcoordsBAR.data()); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

	glEnableVertexAttribArray(spLambert->a("normal"));
	glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, normsBAR.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBAR);
	glUniform1i(spLambert->u("texBAR"), 0);

	glDrawElements(GL_TRIANGLES, indicesBAR.size(), GL_UNSIGNED_INT, indicesBAR.data());

	glDisableVertexAttribArray(spLambert->a("vertex"));
	glDisableVertexAttribArray(spLambert->a("normal"));
	glDisableVertexAttribArray(spLambert->a("color"));

	float vertsGround[] =
	{
		0, 0, 0, 1,
		0, 1, 0, 1,
		1, 0, 0, 1,

		1, 1, 0, 1,
		0, 1, 0, 1,
		1, 0, 0, 1,
	};

	float coordsGround[] = {
	1.0f, 0.0f, //A
	0.0f, 1.0f, //B
	0.0f, 0.0f, //C

	1.0f, 0.0f, //A
	1.0f, 1.0f, //D
	0.0f, 1.0f, //B
	};

	float normalsGround[] =
	{
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
	};
	int vertexCount = 6;

	glm::mat4 GroundPosition = glm::mat4(1.0f);
	GroundPosition = glm::scale(GroundPosition, glm::vec3(200.0f, 1.0f, 150.0f));
	GroundPosition = glm::rotate(GroundPosition, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	GroundPosition = glm::translate(GroundPosition, glm::vec3(0.0f, -0.5f, 0.01f));
	spLambert->use(); //Aktywuj program cieniujący
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(GroundPosition));

	glEnableVertexAttribArray(spLambert->a("vertex"));
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, vertsGround); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spLambert->a("texCoord"));
	glVertexAttribPointer(spLambert->a("texCoord"), 2, GL_FLOAT, false, 0, coordsGround); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

	glEnableVertexAttribArray(spLambert->a("normal"));
	glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, normalsGround);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texBAR);
	glUniform1i(spLambert->u("texBAR"), 0);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glDisableVertexAttribArray(spLambert->a("vertex"));
	glDisableVertexAttribArray(spLambert->a("normal"));
	glDisableVertexAttribArray(spLambert->a("color"));

	/*float vertsBARRIER[] =
	{
		-12,0,-4,1,
	12,0,-4,1,
	12,1,-4,1,

	-12,0,-4,1,
	-12,1,-4,1,
	12,1,-4,1,

	-12,0,4,1,
	12,0,4,1,
	12,1,4,1,

	-12,0,4,1,
	-12,1,4,1,
	12,1,4,1,

	12,1,-4,1,
	12,0,-4,1,
	24,1,4,1,

	12, 0, -4, 1,
	24, 1, 4, 1,
	24, 0, 4, 1,

	12,0,4,1,
	12,1,4,1,
	18,0,8,1,

	12, 1, 4, 1,
	18, 1, 8, 1,
	18, 0, 8, 1,

	24, 0, 4, 1,
	24, 1, 12, 1,
	24, 1, 4, 1,

	24, 0, 4, 1,
	24, 0, 12, 1,
	24, 1, 12, 1,

	18, 0, 8, 1,
	12, 0, 16, 1,
	18, 1, 8, 1,

	18, 1, 8, 1,
	12, 0, 16, 1,
	12, 1, 16, 1,

	12, 0, 24, 1,
	24, 0, 12, 1,
	12, 1, 24, 1,

	12, 1, 24, 1,
	24, 0, 12, 1,
	24, 1, 12, 1,

	12, 0, 16, 1,
	12, 1, 16, 1,
	-12, 0, 16, 1,

	12, 1, 16, 1,
	-12, 1, 16, 1,
	-12, 0, 16, 1,

	12, 0, 24, 1,
	12, 1, 24, 1,
	-12, 0, 24, 1,

	12, 1, 24, 1,
	-12, 1, 24, 1,
	-12, 0, 24, 1,

	-18, 0, 8, 1,
	-12, 0, 16, 1,
	-18, 1, 8, 1,

	-18, 1, 8, 1,
	-12, 0, 16, 1,
	-12, 1, 16, 1,

	-18, 0, 8, 1,
	-18, 1, 8, 1,
	-12, 0, 4, 1,

	-12, 0, 4, 1,
	-18, 1, 8, 1,
	-12, 1, 4, 1,

	-24, 0, 4, 1,
	-24, 0, 12, 1,
	-24, 1, 4, 1,

	-24, 1, 4, 1,
	-24, 0, 12, 1,
	-24, 1, 12, 1,

	-12, 0, -4, 1,
	-24, 1, 4, 1,
	-24, 0, 4, 1,

	-12, 1, -4, 1,
	-24, 1, 4, 1,
	-12, 0, -4, 1,

	-12, 0, 24, 1,
	-24, 0, 12, 1,
	-12, 1, 24, 1,

	-24, 0, 12, 1,
	-24, 1, 12, 1,
	-12, 1, 24, 1,
	//barriers
	};
	float colorsBARRIER[] =
	{
		0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,

	0, 0, 1, 1,
	0, 0, 1, 1,
	0, 0, 1, 1,
	//barriers
	};
	int vertexCount = 87;
	spLambert->use();
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(RoadPosition));

	glEnableVertexAttribArray(spLambert->a("vertex"));
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, vertsBARRIER);

	glEnableVertexAttribArray(spLambert->a("color"));
	glVertexAttribPointer(spLambert->a("color"), 4, GL_FLOAT, false, 0, colorsBARRIER);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDisableVertexAttribArray(spLambert->a("vertex"));
	glDisableVertexAttribArray(spLambert->a("color"));*/
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			rotate = 4;
			turn = -4;
			turnWheel = 1;
		}
		if (key == GLFW_KEY_RIGHT) {
			rotate = -4;
			turn = 4;
			turnWheel = 2;
		}
		if (key == GLFW_KEY_UP) {
			speedf = 0.1;
			moving = 1;
			angleWheel = 0.1;
		}
		if (key == GLFW_KEY_DOWN) {
			speedf = -0.1;
			moving = -1;
			angleWheel = -0.1;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			rotate = 0;
			speedr = 0;
			turn = 0;
			turnWheel = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speedf = 0;
			moving = 0;
			angleWheel = 0;
		}
	}
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.597, 0.915, 1, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glEnable(GL_LIGHTING); //swiatelko
	glEnable(GL_LIGHT0);   //LIGHT 1
	glEnable(GL_LIGHT1);  //LIGHT 2
	glfwSetKeyCallback(window, key_callback);
	tex = readTexture("Car.png");
	texROAD = readTextureROAD("ROAD.png");
	texBAR = readTextureBAR("stone-wall.png");
	texWheel = readTextureWheel("Wheel.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float forward, float right, float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	glm::vec3 CamPos;

	if (kamera > -0.009999 and kamera < 0.0099999)
	{
		kamera = kamera + turn * glfwGetTime();
	}

	if (turn == 4)     //animacja boczków
	{
		CamPos = glm::vec3(forward, 8.0f, right) + glm::vec3(-20 * cos(angle_y + kamera * 20), 0, 20 * sin(angle_y + kamera * 20));
	}
	else if (turn == -4)
	{
		CamPos = glm::vec3(forward, 8.0f, right) + glm::vec3(-20 * cos(angle_y + kamera * 20), 0, 20 * sin(angle_y + kamera * 20));
	}
	else
	{
		CamPos = glm::vec3(forward, 8.0f, right) + glm::vec3(-20 * cos(angle_y), 0, 20 * sin(angle_y));
		kamera = 0;
	}
	//glm::vec3 CamPos = glm::vec3(forward, 8.0f, right) + glm::vec3(-20 * cos(angle_y), 0, 20 * sin(angle_y));
	//std::cout << kamera << std::endl;

	glm::mat4 V = glm::lookAt(CamPos, glm::vec3(forward, 0.0f, right), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania
	//kamera

	/*//-----------
	//Tablica współrzędnych wierzchołków samochodu
	float vertsCAR[] = {
	4, 0,2,1,
	-4,0,2,1,
	4,0,-2,1,

	-4, 0,2,1,
	-4,0,-2,1,
	4,0,-2,1,
	//PODWOZIE

	4, 0,2,1,
	4,1,2,1,
	4,0,-2,1,

	4, 1,2,1,
	4,0,-2,1,
	4,1,-2,1,
	//
	-4, 0,-2,1,
	-4,1,-2,1,
	4,0,-2,1,

	-4,1,-2,1,
	4,0,-2,1,
	4,1,-2,1,
	//
	-4, 0,2,1,
	-4,1,2,1,
	-4,0,-2,1,

	-4, 1,2,1,
	-4,0,-2,1,
	-4,1,-2,1,
	//
	-4, 0,2,1,
	-4,1,2,1,
	4,0,2,1,

	-4,1,2,1,
	4,0,2,1,
	4,1,2,1,
	//SCIANY

	4, 1,2,1,
	4,1,-2,1,
	3,1,2,1,

	3, 1,2,1,
	4,1,-2,1,
	3,1,-2,1,
	//MASKA

	3, 1,2,1,
	1,3,-2,1,
	3,1,-2,1,

	1, 3,2,1,
	1,3,-2,1,
	3,1,2,1,
	//SZYBA

	1, 3,2,1,
	1,3,-2,1,
	-2,3,-2,1,

	-2,3,-2,1,
	1, 3,2,1,
	-2,3,2,1,
	//DACH

	-2, 3,2,1,
	-2,3,-2,1,
	-2,1,-2,1,

	-2,1,-2,1,
	-2, 3,2,1,
	-2,1,2,1,
	//SZYBA TYL

	-2, 1,2,1,
	-2,1,-2,1,
	-4,1,-2,1,

	-4,1,-2,1,
	-4, 1,2,1,
	-2,1,2,1,
	//TYLNA KLAPA

	-2,1,-2,1,
	-2,3,-2,1,
	1,1,-2,1,

	1,1,-2,1,
	1,3,-2,1,
	-2,3,-2,1,

	-2,1,2,1,
	-2,3,2,1,
	1,1,2,1,

	1,1,2,1,
	1,3,2,1,
	-2,3,2,1,

	1,1,2,1,
	1,3,2,1,
	3,1,2,1,

	1,1,-2,1,
	1,3,-2,1,
	3,1,-2,1,
	};  //wierzchołki
	//wierzcholki
	float colorsCAR[] = {
	1,0,0,1,
	1,0,0,1,
	1,0,0,1,

	1,0,0,1,
	1,0,0,1,
	1,0,0,1,
	//PODWOZIE

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	//
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	//
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	//
	0,1,0,1,
	0,1,0,1,
	0,1,0,1,

	0,1,0,1,
	0,1,0,1,
	0,1,0,1,
	//SCIANY

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	//MASKA

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	//SZYBA

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	//DACH

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	//SZYBA TYL

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,

	0,0,1,1,
	0,0,1,1,
	0,0,1,1,
	//TYLNA KLAPA

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,

	1,0,1,1,
	1,0,1,1,
	1,0,1,1,
	}; //kolorki
	//kolorki
	int vertexCount = 78;*/

	glm::mat4 M = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
	M = glm::translate(M, glm::vec3(forward, 2.0f, right));
	//M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi Y
	//M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));
	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi Y
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	/*spLambert->use();
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(spLambert->a("vertex"));
	//glVertexAttribPointer(spConstant->a("vertex"), 4, GL_FLOAT, false, 0, vertsCAR);
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, verts.data());

	glEnableVertexAttribArray(spLambert->a("TexCoord"));
	//glEnableVertexAttribArray(spLambert->a("color"));
	//glVertexAttribPointer(spConstant->a("color"), 4, GL_FLOAT, false, 0, colorsCAR);
	glVertexAttribPointer(spLambert->a("TexCoord"), 2, GL_FLOAT, false, 0, texcoords.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spLambert->u("tex"), 0);

	glEnableVertexAttribArray(spLambert->a("normal"));
	glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, norms.data());

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	glDisableVertexAttribArray(spLambert->a("vertex"));
	//glDisableVertexAttribArray(spLambert->a("color"));
	glDisableVertexAttribArray(spLambert->a("TexCoord"));*/
	//-----------

	spLambert->use(); //Aktywuj program cieniujący

	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M)); //Załaduj do programu cieniującego macierz modelu


	glEnableVertexAttribArray(spLambert->a("vertex"));
	glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spLambert->a("texCoord"));
	glVertexAttribPointer(spLambert->a("texCoord"), 2, GL_FLOAT, false, 0, texcoords.data()); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

	glEnableVertexAttribArray(spLambert->a("normal"));
	glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, norms.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(spLambert->u("tex"), 0);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	glDisableVertexAttribArray(spLambert->a("vertex"));
	glDisableVertexAttribArray(spLambert->a("normal"));
	glDisableVertexAttribArray(spLambert->a("color"));

	Wheelturn = Wheelturn + angleWheel;
	//std::cout << Wheelturn << std::endl;

	for (int i = 0; i < 4; i++)
	{
		glm::mat4 PosW = M;
		if (i == 0)
		{
			PosW = glm::translate(M, glm::vec3(3.0f, -1.0f, 1.0f));
			PosW = glm::scale(PosW, glm::vec3(0.75f, 0.75f, 0.5f));
			if (turnWheel == 1)
			{
				if (moving == 1)
				{
					PosW = glm::rotate(PosW, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else
				{
					PosW = glm::rotate(PosW, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			if (turnWheel == 2)
			{
				if (moving == 1)
				{
					PosW = glm::rotate(PosW, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else
				{
					PosW = glm::rotate(PosW, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			PosW = glm::rotate(PosW, Wheelturn, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (i == 1)
		{
			PosW = glm::translate(M, glm::vec3(3.0f, -1.0f, -1.0f));
			PosW = glm::scale(PosW, glm::vec3(0.75f, 0.75f, 0.5f));
			if (turnWheel == 1)
			{
				if (moving == 1)
				{
					PosW = glm::rotate(PosW, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else
				{
					PosW = glm::rotate(PosW, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			if (turnWheel == 2)
			{
				if (moving == 1)
				{
					PosW = glm::rotate(PosW, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else
				{
					PosW = glm::rotate(PosW, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
			PosW = glm::rotate(PosW, Wheelturn, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else if (i == 2)
		{
			PosW = glm::translate(M, glm::vec3(-1.0f, -1.0f, -1.0f));
			PosW = glm::scale(PosW, glm::vec3(0.75f, 0.75f, 0.5f));
			PosW = glm::rotate(PosW, Wheelturn, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else
		{
			PosW = glm::translate(M, glm::vec3(-1.0f, -1.0f, 1.0f));
			PosW = glm::scale(PosW, glm::vec3(0.75f, 0.75f, 0.5f));
			PosW = glm::rotate(PosW, Wheelturn, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(PosW)); //Załaduj do programu cieniującego macierz modelu

		glEnableVertexAttribArray(spLambert->a("vertex"));
		glVertexAttribPointer(spLambert->a("vertex"), 4, GL_FLOAT, false, 0, vertsWheel.data()); //Współrzędne wierzchołków bierz z tablicy myCubeVertices

		glEnableVertexAttribArray(spLambert->a("texCoord"));
		glVertexAttribPointer(spLambert->a("texCoord"), 2, GL_FLOAT, false, 0, texcoordsWheel.data()); //Współrzędne teksturowania bierz z tablicy myCubeTexCoords

		glEnableVertexAttribArray(spLambert->a("normal"));
		glVertexAttribPointer(spLambert->a("normal"), 4, GL_FLOAT, false, 0, normsWheel.data());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texWheel);
		glUniform1i(spLambert->u("texWheel"), 0);

		glDrawElements(GL_TRIANGLES, indicesWheel.size(), GL_UNSIGNED_INT, indicesWheel.data());

		glDisableVertexAttribArray(spLambert->a("vertex"));
		glDisableVertexAttribArray(spLambert->a("normal"));
		glDisableVertexAttribArray(spLambert->a("color"));
	}
	enviroment();

	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	PlaySound(TEXT("doge.wav"), NULL, SND_ASYNC | SND_LOOP);
	initOpenGLProgram(window); //Operacje inicjujące
	LoadModel(std::string("Car.fbx"));
	LoadModelROAD(std::string("ROAD.fbx"));
	LoadModelBAR(std::string("Barriers.fbx"));
	LoadModelWheel(std::string("wheel.fbx"));
	//Główna pętla
	float distancef = 0;
	float distancer = 0;
	float angle_y = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_y += rotate * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		distancef = (distancef + (speedf * cos(angle_y)));
		distancer = (distancer + (speedf * -sin(angle_y)));
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, distancef, distancer, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
