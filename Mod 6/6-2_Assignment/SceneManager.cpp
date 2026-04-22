///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#include <glm/gtx/transform.hpp>

// declare the global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	// free up the allocated memory
	m_pShaderManager = NULL;
	if (NULL != m_basicMeshes)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}
	// clear the collection of defined materials
	m_objectMaterials.clear();
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformation()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	// matrix math is used to calculate the final model matrix
	modelView = translation * rotationZ * rotationY * rotationX * scale;
	if (NULL != m_pShaderManager)
	{
		// pass the model matrix into the shader
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		// pass the color values into the shader
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		// find the defined material that matches the tag
		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			// pass the material properties into the shader
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

 /***********************************************************
  *  DefineObjectMaterials()
  *
  *  This method is used for configuring the various material
  *  settings for all of the objects within the 3D scene.
  ***********************************************************/

void SceneManager::DefineObjectMaterials()
{
	/*** STUDENTS - add the code BELOW for defining object materials. ***/
	/*** There is no limit to the number of object materials that can ***/
	/*** be defined. Refer to the code in the OpenGL Sample for help  ***/

	// Material for the floor plane - matte warm surface
	OBJECT_MATERIAL floorMaterial;
	floorMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	floorMaterial.ambientStrength = 0.8f;
	floorMaterial.diffuseColor = glm::vec3(0.8f, 0.6f, 0.6f);
	floorMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	floorMaterial.shininess = 2.0f;
	floorMaterial.tag = "floor";
	m_objectMaterials.push_back(floorMaterial);

	// Material for the cylinder - smooth plastic look
	OBJECT_MATERIAL plasticMaterial;
	plasticMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	plasticMaterial.ambientStrength = 0.8f;
	plasticMaterial.diffuseColor = glm::vec3(0.7f, 0.5f, 0.5f);
	plasticMaterial.specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
	plasticMaterial.shininess = 32.0f;
	plasticMaterial.tag = "plastic";
	m_objectMaterials.push_back(plasticMaterial);

	// Material for the box shapes - wood-like low gloss
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	woodMaterial.ambientStrength = 0.8f;
	woodMaterial.diffuseColor = glm::vec3(0.6f, 0.4f, 0.3f);
	woodMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.shininess = 5.0f;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);

	// Material for the sphere - shiny metallic ball
	OBJECT_MATERIAL metalMaterial;
	metalMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	metalMaterial.ambientStrength = 0.8f;
	metalMaterial.diffuseColor = glm::vec3(0.7f, 0.3f, 0.3f);
	metalMaterial.specularColor = glm::vec3(0.9f, 0.9f, 0.9f);
	metalMaterial.shininess = 96.0f;
	metalMaterial.tag = "metal";
	m_objectMaterials.push_back(metalMaterial);

	// Material for the cone - soft ceramic surface
	OBJECT_MATERIAL ceramicMaterial;
	ceramicMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
	ceramicMaterial.ambientStrength = 0.8f;
	ceramicMaterial.diffuseColor = glm::vec3(0.85f, 0.75f, 0.75f);
	ceramicMaterial.specularColor = glm::vec3(0.3f, 0.3f, 0.3f);
	ceramicMaterial.shininess = 15.0f;
	ceramicMaterial.tag = "ceramic";
	m_objectMaterials.push_back(ceramicMaterial);
}


/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting, if no light sources have
	// been added then the display window will be black - to use the 
	// default OpenGL lighting then comment out the following line
	m_pShaderManager->setBoolValue(g_UseLightingName, true);

	/*** STUDENTS - add the code BELOW for setting up light sources ***/
	/*** Up to four light sources can be defined. Refer to the code ***/
	/*** in the OpenGL Sample for help                              ***/

	// Light source 0: Primary white light - upper left overhead position
	// High ambient ensures shapes are visible with the lighting model
	m_pShaderManager->setVec3Value("lightSources[0].position", -3.0f, 4.0f, 6.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 1.0f, 1.0f, 1.0f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 1.0f, 1.0f, 1.0f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.5f);

	// Light source 1: Colored pink/rose light - right side of scene
	// Adds warm color contrast from a second angle position
	m_pShaderManager->setVec3Value("lightSources[1].position", 3.0f, 4.0f, 6.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 1.0f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.9f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.8f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.5f);
}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// define the materials for objects in the scene
	DefineObjectMaterials();
	// add and define the light sources for the scene
	SetupSceneLights();

	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene - the following code loads
	// the basic 3D meshes into the graphics pipeline buffers

	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadSphereMesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/******************************************************************/
	// SHAPE 1: PLANE - floor/ground surface
	/******************************************************************/
	scaleXYZ = glm::vec3(20.0f, 1.0f, 10.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.8f, 0.6f, 0.6f, 1.0f);
	// Set floor material - warm matte surface
	m_pShaderManager->setVec3Value("material.ambientColor", 0.5f, 0.4f, 0.4f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.8f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("material.shininess", 2.0f);
	m_basicMeshes->DrawPlaneMesh();

	/******************************************************************/
	// SHAPE 2: CYLINDER - vertical pedestal
	/******************************************************************/
	scaleXYZ = glm::vec3(0.9f, 2.8f, 0.9f);
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;
	positionXYZ = glm::vec3(0.0f, 0.9f, 0.4f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.7f, 0.5f, 0.5f, 1.0f);
	// Set plastic material - smooth moderate shine
	m_pShaderManager->setVec3Value("material.ambientColor", 0.4f, 0.4f, 0.4f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.7f, 0.5f, 0.5f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setFloatValue("material.shininess", 32.0f);
	m_basicMeshes->DrawCylinderMesh();

	/******************************************************************/
	// SHAPE 3: BOX - long seesaw bar
	/******************************************************************/
	scaleXYZ = glm::vec3(1.0f, 9.0f, 1.3f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 95.0f;
	positionXYZ = glm::vec3(0.2f, 2.27f, 2.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.6f, 0.4f, 0.3f, 1.0f);
	// Set wood material - low gloss natural surface
	m_pShaderManager->setVec3Value("material.ambientColor", 0.4f, 0.3f, 0.2f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.6f, 0.4f, 0.3f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("material.shininess", 5.0f);
	m_basicMeshes->DrawBoxMesh();

	/******************************************************************/
	// SHAPE 4: BOX - small block on pedestal top
	/******************************************************************/
	scaleXYZ = glm::vec3(1.7f, 1.5f, 1.5f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 40.0f;
	ZrotationDegrees = 8.0f;
	positionXYZ = glm::vec3(3.3f, 3.85f, 2.19f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.6f, 0.4f, 0.3f, 1.0f);
	// Set wood material - matching the seesaw bar
	m_pShaderManager->setVec3Value("material.ambientColor", 0.4f, 0.3f, 0.2f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.6f, 0.4f, 0.3f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("material.shininess", 5.0f);
	m_basicMeshes->DrawBoxMesh();

	/******************************************************************/
	// SHAPE 5: SPHERE - shiny ball on right side of seesaw
	/******************************************************************/
	scaleXYZ = glm::vec3(1.0f, 1.0f, 1.0f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(3.2f, 5.6f, 2.5f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.7f, 0.3f, 0.3f, 1.0f);
	// Set metal material - high specular shiny ball
	m_pShaderManager->setVec3Value("material.ambientColor", 0.4f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.7f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.9f, 0.9f, 0.9f);
	m_pShaderManager->setFloatValue("material.shininess", 96.0f);
	m_basicMeshes->DrawSphereMesh();

	/******************************************************************/
	// SHAPE 6: CONE - upright cone on left side of seesaw
	/******************************************************************/
	scaleXYZ = glm::vec3(1.2f, 4.0f, 1.2f);
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 5.0f;
	positionXYZ = glm::vec3(-3.3f, 2.50f, 2.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.85f, 0.75f, 0.75f, 1.0f);
	// Set ceramic material - soft slightly glossy surface
	m_pShaderManager->setVec3Value("material.ambientColor", 0.4f, 0.4f, 0.4f);
	m_pShaderManager->setFloatValue("material.ambientStrength", 0.8f);
	m_pShaderManager->setVec3Value("material.diffuseColor", 0.85f, 0.75f, 0.75f);
	m_pShaderManager->setVec3Value("material.specularColor", 0.3f, 0.3f, 0.3f);
	m_pShaderManager->setFloatValue("material.shininess", 15.0f);
	m_basicMeshes->DrawConeMesh();
}