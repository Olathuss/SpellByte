#include "stdafx.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreMaterialManager.h>

#include "ObjectFactory.h"
#include "CollisionTools.h"
#include "./utilities/tinyxml2.h"
#include "Player.h"
#include "ObjectGroup.h"
#include "world/WaterMesh.h"
#include "world/Water.h"

#define FLOW_SPEED 0.4
#define FLOW_HEIGHT 5
#define RAIN_HEIGHT_RANDOM 5
#define RAIN_HEIGHT_CONSTANT 5

#define MAJOR_SUPPORT 1
#define MINOR_SUPPORT 0

namespace SpellByte
{
    class World
    {
    public:
        enum COLLISION_MASK {
            NONE = 1<<0,
            STATIC = 1<<1,
        };
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

        int getMaskFromString(std::string maskType) {
            if (maskType == "NONE") {
                return COLLISION_MASK::NONE;
            } else if (maskType == "STATIC") {
                return COLLISION_MASK::STATIC;
            } else {
                return COLLISION_MASK::NONE;
            }
        }

        bool reload();

        bool update(const Ogre::FrameEvent &evt);
        void handleEvent(int event);

        // set collision tools
        void setCollisionTool(MOC::CollisionTools *collisionTool)
        {
            GameCollisionTools = collisionTool;
        }

        virtual void loadData(tinyxml2::XMLElement *dataElt);
        // Get config float setting
        const float getDataFloat(std::string ID) const;

    private:
        struct Layer {
            Ogre::Real worldSize;
            Ogre::String diffuse;
            Ogre::String normal;
        };
        typedef std::vector<WaterCircle*> WaterCircles;
        void loadTerrain(tinyxml2::XMLElement *terrainElt);
        void loadObjects(tinyxml2::XMLElement *worldElt);
        void processGroup(tinyxml2::XMLElement *groupElt, ObjectFactory *objFactory, Ogre::SceneNode *grpNode = NULL);
        void loadIndependentObjects(tinyxml2::XMLElement *elt, ObjectFactory *objFactory);
        void processCircles(Real timeSinceLastFrame);
        void processParticles();

        // Clear World
        void clearWorld();

        // Bind world API to LUA
        void bindToLUA();

        /* Water */
        WaterMesh *waterMesh;
        Ogre::Entity *waterEntity;
        Ogre::AnimationState* waterAnim;
        Ogre::Overlay* waterOverlay;
        Ogre::ParticleSystem *waterParticleSystem;
        Ogre::ParticleEmitter *waterParticleEmitter;
        WaterCircles circles;

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
        Ogre::SceneNode *objectsNode;
        // For groups and their groups/objects
        std::vector<ObjectGroup*> WorldGroups;
        // For independent objects not in group
        std::vector<Object*> WorldObjects;

        MOC::CollisionTools *GameCollisionTools;

        std::vector<Ogre::String> heightMaps;
        std::vector<Layer> terrainLayers;
        std::map<std::string, float> worldFloats;
        int worldVersionMajor;
        int worldVersionMinor;
    };
}
