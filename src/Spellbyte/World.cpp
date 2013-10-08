#include "World.h"
#include "define.h"
#include "ActorManager.h"
#include "ActorFactory.h"

namespace SpellByte
{
    World::World()
    {
        dirLightColor.r = APP->getConfigFloat("directionalr");
        dirLightColor.g = APP->getConfigFloat("directionalr");
        dirLightColor.b = APP->getConfigFloat("directionalb");
        ambientLightColor.r = APP->getConfigFloat("ambientr");
        ambientLightColor.g = APP->getConfigFloat("ambientg");
        ambientLightColor.b = APP->getConfigFloat("ambientb");
        lightChangeValue = APP->getConfigFloat("lightChange");
        lightChanged = false;
        ambientLightUp = false;
        ambientLightDown = false;
        dirLightUp = false;
        dirLightDown = false;

        SceneMgr = APP->SceneMgr;
    }

    World::~World()
    {
        destroyScene();
    }

    bool World::init(Ogre::SceneManager *sMgr)
    {
        SceneMgr = sMgr;

        return true;
    }

    bool World::loadWorld()
    {
        LOG("World: Loading world file");
        std::string worldFile = "world.xml";
        XMLResourcePtr xmlFile = APP->xmlManager->load(worldFile,"General");
        tinyxml2::XMLDocument *worldDoc = xmlFile->getXML();
        if(!worldDoc)
        {
            Ogre::String desc = "Error, world file missing";
            throw(Ogre::Exception(20, desc, "World"));
        }

        LOG("World: Loading terrain");
        tinyxml2::XMLElement *element;

        element = worldDoc->FirstChildElement("terrain");
        if(element)
        {
            LOG("World: Getting heightmap name");
            tinyxml2::XMLElement *next = element->FirstChildElement("heightmap");
            if(!next)
            {
                Ogre::String desc = "Error, no heightmap defined";
                throw(Ogre::Exception(20, desc, "World"));
            }
            heightMap = Ogre::String(next->Attribute("value"));
            LOG("World: heightmap name: " + heightMap);
        }
        else
        {
            Ogre::String desc = "Error, terrain failed";
            throw(Ogre::Exception(20, desc, "World"));
        }

        loadTerrain();

        LOG("World: Parsing objects");
        element = worldDoc->FirstChildElement("objects");
        if(element)
        {
            tinyxml2::XMLElement *object = element->FirstChildElement("static");
            ActorFactory actFactory = ActorFactory();
            while(object)
            {
                LOG("Creating Actor Name: " + Ogre::String(object->Attribute("name")));
                Actor *newActor = actFactory.createActor(SceneMgr, terrainGroup, object);
                worldActors.push_back(newActor);
                //ActorMgr->registerActor(newActor);
                object = object->NextSiblingElement();
            }
        }
        LOG("World: World loading complete");
        return true;
    }

    void World::loadTerrain()
    {
        SceneMgr->setSkyBox(true, "Examples/StormySkyBox");
        SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
        SceneMgr->setShadowFarDistance(APP->getConfigFloat("shadowDistance"));

        Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
        lightdir.normalise();

        dirLight = SceneMgr->createLight("directionalLight");
        dirLight->setType(Ogre::Light::LT_DIRECTIONAL);
        dirLight->setDirection(Ogre::Vector3(0, -1, 1));
        dirLight->setDiffuseColour(Ogre::ColourValue::White);
        dirLight->setSpecularColour(dirLightColor);
        dirLight->setCastShadows(false);

        sunLight = SceneMgr->createLight("sunLight");
        sunLight->setType(Ogre::Light::LT_POINT);
        sunLight->setPosition(Ogre::Vector3(-2000, 2000, 0));
        sunLight->setDiffuseColour(dirLightColor);
        sunLight->setSpecularColour(dirLightColor);
        sunLight->setShadowFarClipDistance(5000);
        sunLight->setShadowFarDistance(5000);

        SceneMgr->setAmbientLight(ambientLightColor);

        terrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

        terrainGroup = OGRE_NEW Ogre::TerrainGroup(SceneMgr, Ogre::Terrain::ALIGN_X_Z, 1025, 12000.0f);
        terrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
        terrainGroup->setOrigin(Ogre::Vector3::ZERO);

        configureTerrainDefaults(dirLight);

        for(long x = 0; x <= 0; ++x)
            for(long y = 0; y <= 0; ++y)
                defineTerrain(x, y);

        terrainGroup->loadAllTerrains(true);

        if(terrainsImported)
        {
            Ogre::TerrainGroup::TerrainIterator ti = terrainGroup->getTerrainIterator();
            while(ti.hasMoreElements())
            {
                Ogre::Terrain *t = ti.getNext()->instance;
                initBlendMaps(t);
            }
        }
        terrainGroup->freeTemporaryResources();
    }

    void World::createScene()
    {
        //loadWorld();
    }

    void World::update(const Ogre::FrameEvent &evt)
    {
        if(ambientLightUp)
        {
            ambientLightColor.r += lightChangeValue;
            ambientLightColor.g += lightChangeValue;
            ambientLightColor.b += lightChangeValue;
            SceneMgr->setAmbientLight(ambientLightColor);
        }
        if(ambientLightDown)
        {
            ambientLightColor.r -= lightChangeValue;
            ambientLightColor.g -= lightChangeValue;
            ambientLightColor.b -= lightChangeValue;
            SceneMgr->setAmbientLight(ambientLightColor);
        }
        if(dirLightUp)
        {
            dirLightColor.r += lightChangeValue;
            dirLightColor.g += lightChangeValue;
            dirLightColor.b += lightChangeValue;
            dirLight->setSpecularColour(dirLightColor);
        }
        if(dirLightDown)
        {
            dirLightColor.r -= lightChangeValue;
            dirLightColor.g -= lightChangeValue;
            dirLightColor.b -= lightChangeValue;
            dirLight->setSpecularColour(dirLightColor);
        }
    }

    void World::handleEvent(int event)
    {
        switch(event)
        {
            case UserEvent::AMBIENT_LIGHT_UP_ON:
                ambientLightDown = false;
                ambientLightUp = true;
                break;
            case UserEvent::AMBIENT_LIGHT_UP_OFF:
                ambientLightUp = false;
                break;
            case UserEvent::AMBIENT_LIGHT_DOWN_ON:
                ambientLightDown = true;
                ambientLightUp = false;
                break;
            case UserEvent::AMBIENT_LIGHT_DOWN_OFF:
                ambientLightDown = false;
                break;
            case UserEvent::DIR_LIGHT_UP_ON:
                dirLightUp = true;
                dirLightDown = false;
                break;
            case UserEvent::DIR_LIGHT_UP_OFF:
                dirLightUp = false;
                break;
            case UserEvent::DIR_LIGHT_DOWN_ON:
                dirLightDown = true;
                dirLightUp = false;
                break;
            case UserEvent::DIR_LIGHT_DOWN_OFF:
                dirLightDown = false;
                break;
            default:
                break;
        }
    }

    void getTerrainImage(bool flipX, bool flipY, Ogre::Image &img, Ogre::String heightMap)
    {
        img.load(heightMap, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        if(flipX)
            img.flipAroundY();
        if(flipY)
            img.flipAroundX();
    }

    void World::defineTerrain(long x, long y)
    {
        Ogre::String filename = terrainGroup->generateFilename(x, y);

        if(Ogre::ResourceGroupManager::getSingleton().resourceExists(terrainGroup->getResourceGroup(), filename))
        {
            terrainGroup->defineTerrain(x, y);
        }
        else
        {
            Ogre::Image img;
            getTerrainImage(x % 2 != 0, y % 2 != 0, img, heightMap);
            terrainGroup->defineTerrain(x, y, &img);
            terrainsImported = true;
        }
    }

    void World::initBlendMaps(Ogre::Terrain *terrain)
    {
        Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
        Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
        Ogre::Real minHeight0 = 70;
        Ogre::Real fadeDist0 = 160;
        Ogre::Real minHeight1 = 70;
        Ogre::Real fadeDist1 = 60;
        float* pBlend0 = blendMap0->getBlendPointer();
        float* pBlend1 = blendMap1->getBlendPointer();
        for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
        {
            for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
            {
                Ogre::Real tx, ty;

                blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
                Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
                Ogre::Real val = (height - minHeight0) / fadeDist0;
                val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
                *pBlend0++ = val;

                val = (height - minHeight1) / fadeDist1;
                val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
                *pBlend1++ = val;
            }
        }
        blendMap0->dirty();
        blendMap1->dirty();
        blendMap0->update();
        blendMap1->update();
    }

    void World::configureTerrainDefaults(Ogre::Light *light)
    {
        // configure globals
        terrainGlobals->setMaxPixelError(8);
        // testing composite map
        terrainGlobals->setCompositeMapDistance(3000);

        // Important to set these so that the terrain knows what to use for derived (non-realtime) data
        terrainGlobals->setLightMapDirection(light->getDerivedDirection());
        terrainGlobals->setCompositeMapAmbient(SceneMgr->getAmbientLight());
        terrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

        Ogre::Terrain::ImportData &defaultimp = terrainGroup->getDefaultImportSettings();
        defaultimp.terrainSize = 1025;
        defaultimp.worldSize = 12000.0f;
        defaultimp.inputScale = 4800; // due terrain.png is 8 bpp
        defaultimp.minBatchSize = 33;
        defaultimp.maxBatchSize = 65;

        // textures
        defaultimp.layerList.resize(3);
        defaultimp.layerList[0].worldSize = 100;
        defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
        defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
        defaultimp.layerList[1].worldSize = 30;
        defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
        defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
        defaultimp.layerList[2].worldSize = 200;
        defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
        defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
    }

    void World::destroyScene()
    {
        OGRE_DELETE terrainGroup;
        OGRE_DELETE terrainGlobals;
    }
}
