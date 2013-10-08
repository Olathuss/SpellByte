#ifndef DOT_SCENELOADER_H
#define DOT_SCENELOADER_H

// Includes
#include <Ogre/OgreString.h>
#include <Ogre/OgreVector3.h>
#include <Ogre/OgreQuaternion.h>
#include <vector>
#include "./utilities/tinyxml2.h"

using namespace tinyxml2;
// Forward declarations

namespace Ogre
{
	// Forward declarations
	class SceneManager;
	class SceneNode;

	class nodeProperty
	{
	public:
		String nodeName;
		String propertyNm;
		String valueName;
		String typeName;

		nodeProperty(const String &node, const String &propertyName, const String &value, const String &type)
			: nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
	};

	class DotSceneLoader
	{
	public:
		DotSceneLoader() : mSceneMgr(0) {}
		virtual ~DotSceneLoader() {}

		void parseDotScene(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
		String getProperty(const String &ndNm, const String &prop);

		std::vector<nodeProperty> nodeProperties;
		std::vector<String> staticObjects;
		std::vector<String> dynamicObjects;

	protected:
		void processScene(XMLElement *XMLRoot);

		void processNodes(XMLElement *XMLNode);
		void processExternals(XMLElement *XMLNode);
		void processEnvironment(XMLElement *XMLNode);
		void processTerrain(XMLElement *XMLNode);
		void processUserDataReference(XMLElement *XMLNode, SceneNode *pParent = 0);
		void processUserDataReference(XMLElement *XMLNode, Entity *pEntity);
		void processOctree(XMLElement *XMLNode);
		void processLight(XMLElement *XMLNode, SceneNode *pParent = 0);
		void processCamera(XMLElement *XMLNode, SceneNode *pParent = 0);

		void processNode(XMLElement *XMLNode, SceneNode *pParent = 0);
		void processLookTarget(XMLElement *XMLNode, SceneNode *pParent);
		void processTrackTarget(XMLElement *XMLNode, SceneNode *pParent);
		void processEntity(XMLElement *XMLNode, SceneNode *pParent);
		void processParticleSystem(XMLElement *XMLNode, SceneNode *pParent);
		void processBillboardSet(XMLElement *XMLNode, SceneNode *pParent);
		void processPlane(XMLElement *XMLNode, SceneNode *pParent);

		void processFog(XMLElement *XMLNode);
		void processSkyBox(XMLElement *XMLNode);
		void processSkyDome(XMLElement *XMLNode);
		void processSkyPlane(XMLElement *XMLNode);
		void processClipping(XMLElement *XMLNode);

		void processLightRange(XMLElement *XMLNode, Light *pLight);
		void processLightAttenuation(XMLElement *XMLNode, Light *pLight);

		String getAttrib(XMLElement *XMLNode, const String &parameter, const String &defaultValue = "");
		Real getAttribReal(XMLElement *XMLNode, const String &parameter, Real defaultValue = 0);
		bool getAttribBool(XMLElement *XMLNode, const String &parameter, bool defaultValue = false);

		Vector3 parseVector3(XMLElement *XMLNode);
		Quaternion parseQuaternion(XMLElement *XMLNode);
		ColourValue parseColour(XMLElement *XMLNode);


		SceneManager *mSceneMgr;
		SceneNode *mAttachNode;
		String m_sGroupName;
		String m_sPrependNode;
	};
}

#endif // DOT_SCENELOADER_H
