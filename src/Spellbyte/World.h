#include "stdafx.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreMaterialManager.h>

#include "Actor.h"

namespace SpellByte
{
    class World
    {
    public:
        Ogre::TerrainGroup *terrainGroup;

        World();
        ~World();

        bool init(Ogre::SceneManager *sMgr);

        bool loadWorld();
        void createScene();
        void destroyScene();

        bool reload();

        void update(const Ogre::FrameEvent &evt);
        void handleEvent(int event);

    private:
        struct WorldObject
        {
            Ogre::String scene;
            float x, y, z;
            bool setZTerrain;
        };

        void loadTerrain();

        Ogre::SceneManager *SceneMgr;

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
        bool dirLightUp;
        bool dirLightDown;
        Ogre::Light* dirLight;
        Ogre::Light* sunLight;
        Ogre::ColourValue dirLightColor;
        Ogre::ColourValue ambientLightColor;

        Ogre::String heightMap;
        std::vector<Actor*> worldActors;
    };
}
