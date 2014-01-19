#include "stdafx.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreMaterialManager.h>

#include "ObjectFactory.h"
#include "CollisionTools.h"
#include "./utilities/tinyxml2.h"
#include "Player.h"
#include "ObjectGroup.h"

#define FLOW_SPEED 0.4
#define FLOW_HEIGHT 5

namespace SpellByte
{
    class World
    {
    public:
        Ogre::TerrainGroup *terrainGroup;

        World();
        ~World();

        bool init(Ogre::Camera *cam, Player *player);

        // load world from file
        bool loadWorld(std::string worldFile);
        // save world to file
        void saveWorld(std::string worldFile);
        bool newWorldLoad();
        void createScene();
        void destroyScene();

        bool reload();

        void update(const Ogre::FrameEvent &evt);
        void handleEvent(int event);

        // set collision tools
        void setCollisionTool(MOC::CollisionTools *collisionTool)
        {
            GameCollisionTools = collisionTool;
        }

    private:

        void loadTerrain();
        void loadObjects(tinyxml2::XMLDocument *worldDoc);
        void processGroup(tinyxml2::XMLElement *groupElt, ObjectFactory *objFactory, Ogre::SceneNode *grpNode = NULL);
        void loadIndependentObjects(tinyxml2::XMLElement *elt, ObjectFactory *objFactory);

        // Clear World
        void clearWorld();

        // Bind world API to LUA
        void bindToLUA();

        Ogre::SceneManager *SceneMgr;
        Ogre::SceneNode *goatNode;
        Ogre::AnimationState *goatAnimState;

        Ogre::SceneNode *manNode;
        Ogre::AnimationState *manAnimState;

        Ogre::TerrainGlobalOptions *terrainGlobals;
        bool terrainsImported;

        void defineTerrain(long x, long y);
        void initBlendMaps(Ogre::Terrain *terrain);
        void configureTerrainDefaults(Ogre::Light *light);

        // Lighting
        Ogre::Real lightChangeValue;
        bool lightChanged;
        bool ambientLightUp;
        bool ambientLightDown;
        bool sunLightUp;
        bool sunLightDown;
        Ogre::Light* dirLight;
        Ogre::Light* sunLight;
        Ogre::ColourValue dirLightColor;
        Ogre::ColourValue sunLightColor;
        Ogre::ColourValue ambientLightColor;

        // Get camera for light relative
        Ogre::Camera *Camera;
        Player *GamePlayer;

        Ogre::String worldName;
        Ogre::String heightMap;
        int mapwidth;
        int mapheight;
        bool multi_terrain;
        std::vector<Ogre::String> heightMaps;
        Ogre::SceneNode *objectsNode;
        // For groups and their groups/objects
        std::vector<ObjectGroup*> WorldGroups;
        // For independent objects not in group
        std::vector<Object*> WorldObjects;

        MOC::CollisionTools *GameCollisionTools;

        // Water
        Entity *pWaterEntity;
        Plane nWaterPlane;
    };
}
