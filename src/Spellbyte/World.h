#include "stdafx.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreMaterialManager.h>

#include "ObjectFactory.h"
#include "CollisionTools.h"
#include "./utilities/tinyxml2.h"
#include "Player.h"
#include "Actor.h"
#include "ObjectGroup.h"
#include "coldet/multiobject.h"
#include "world/WaterMesh.h"
#include "world/Water.h"

#define FLOW_SPEED 0.4
#define FLOW_HEIGHT 5
#define RAIN_HEIGHT_RANDOM 5
#define RAIN_HEIGHT_CONSTANT 5

#define MAJOR_SUPPORT 1
#define MINOR_SUPPORT 0

#include <actor/InteractableObject.h>
#include <graph/Nodes.h>
#include <graph/Edges.h>
#include <graph/Graph.h>
#include <graph/GraphLoader.h>

namespace SpellByte
{
    //! World class, contains world information
    class World
    {
    public:
        enum COLLISION_MASK {
            NONE = 1 << 0,
            STATIC = 1 << 1,
            ACTOR = 1 << 2,
            GRAPH_NODE = 1 << 3
        };

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

        void showVisualGraph() {
            graphLoader.showVisualGraph();
        }
        void hideVisualGraph() {
            graphLoader.hideVisualGraph();
        }

        Graph::Graph *getGraph() {
            return &worldGraph;
        }

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

        void setVector3Height(Ogre::Vector3 &source) {
            source.y = terrainGroup->getHeightAtWorldPosition(source.x, source.y, source.z);
        }

        Ogre::Real getHeight(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
            return terrainGroup->getHeightAtWorldPosition(x, y, z);
        }

        // set collision tools
        void setCollisionTool(MOC::CollisionTools *collisionTool)
        {
            GameCollisionTools = collisionTool;
        }

        virtual void loadData(tinyxml2::XMLElement *dataElt);
        // Get config settings
        bool hasDataFloat(std::string ID) const;
        const float getDataFloat(std::string ID) const;
        bool hasDataInt(std::string ID) const;
        const int getDataInt(std::string ID) const;

        BaseActor *getCoffinPtr() { return &coffin; }

    private:
        struct Layer {
            Ogre::Real worldSize;
            Ogre::String diffuse;
            Ogre::String normal;
        };

        // world related
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

        WaterMesh *waterMesh;
        Ogre::Entity *waterEntity;
        Ogre::AnimationState* waterAnim;
        Ogre::Overlay *waterOverlay;
        Ogre::ParticleSystem *waterParticleSystem;
        Ogre::ParticleEmitter *waterParticleEmitter;
        WaterCircles circles;

        Ogre::SceneManager *SceneMgr;
        Ogre::SceneNode *goatNode;
        Ogre::AnimationState *goatAnimState;

        Ogre::SceneNode *manNode;
        Ogre::AnimationState *manAnimState;

        Ogre::TerrainGlobalOptions *terrainGlobals;
        Ogre::TerrainGroup *terrainGroup;
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
        int terrainRenderQueue;
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
        std::map<std::string, int> worldInts;
        int worldVersionMajor;
        int worldVersionMinor;

        // Graph related
        Graph::Graph worldGraph;
        Graph::GraphLoader graphLoader;

        // World objects, tmp hack
        Coffin coffin;
    };
}
