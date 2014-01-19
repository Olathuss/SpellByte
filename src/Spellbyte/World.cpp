#include <OgreSubEntity.h>

#include "World.h"
#include "define.h"
//#include "ActorManager.h"
#include "console/LuaManager.h"
#include "DotSceneLoader.h"
#include "utilities/utils.h"

namespace SpellByte
{
    World::World()
    {
        dirLightColor.r = APP->getConfigFloat("directionalr");
        dirLightColor.g = APP->getConfigFloat("directionalg");
        dirLightColor.b = APP->getConfigFloat("directionalb");
        sunLightColor.r = APP->getConfigFloat("sunr");
        sunLightColor.g = APP->getConfigFloat("sung");
        sunLightColor.b = APP->getConfigFloat("sunb");
        ambientLightColor.r = APP->getConfigFloat("ambientr");
        ambientLightColor.g = APP->getConfigFloat("ambientg");
        ambientLightColor.b = APP->getConfigFloat("ambientb");
        lightChangeValue = APP->getConfigFloat("lightChange");
        lightChanged = false;
        ambientLightUp = false;
        ambientLightDown = false;
        sunLightUp = false;
        sunLightDown = false;

        SceneMgr = APP->SceneMgr;
        APP->attachWorld(this);
        objectsNode = SceneMgr->getRootSceneNode()->createChildSceneNode("objectsNode");

        Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

        Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                        plane, 1500, 1500, 20, 20, true, 1, 1, 5, Ogre::Vector3::UNIT_Z);

        // create a water plane/scene node
        nWaterPlane.normal = Vector3::UNIT_Y;
        nWaterPlane.d = -1.5;
        MeshManager::getSingleton().createPlane(
            "WaterPlane",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            nWaterPlane,
            8000, 8000,
            20, 20,
            true, 1,
            1, 1,
            Vector3::UNIT_Z);

        terrainGlobals = NULL;
        terrainGroup = NULL;
        GameCollisionTools = NULL;
    }

    World::~World()
    {
        APP->detachWorld();
        destroyScene();

    }

    bool World::init(Ogre::Camera *cam, Player *player)
    {
        bindToLUA();
        SLB::setGlobal<World*>(LUAMANAGER->LUA, this, "world");
        Camera = cam;
        GamePlayer = player;

        return true;
    }

    void World::bindToLUA()
    {
        SLB::Class< World >("SpellByte::World")
            .set("loadWorld", ( bool(World::*)(std::string) ) &World::loadWorld)
            .set("saveWorld", ( bool(World::*)(std::string) ) &World::saveWorld)
            .set("reload", &World::reload);
    }

    void World::saveWorld(std::string worldFile)
    {
        LOG("World: Saving world file to (WARNING: WILL OVERWRITE): " + worldFile);
        tinyxml2::XMLDocument xmlFile;

        // Save terrain information to file
        tinyxml2::XMLElement *xml_terrain = xmlFile.NewElement("terrain");
        xmlFile.InsertEndChild(xml_terrain);
        tinyxml2::XMLElement *xml_heightmap = xmlFile.NewElement("heightmap");
        xml_terrain->InsertEndChild(xml_heightmap);
        xml_heightmap->SetAttribute("name", heightMap.c_str());
        xml_heightmap->SetAttribute("multi", multi_terrain);
        xml_heightmap->SetAttribute("height", mapheight);
        xml_heightmap->SetAttribute("width", mapwidth);

        tinyxml2::XMLElement *xml_objects = xmlFile.NewElement("objects");
        xmlFile.InsertEndChild(xml_objects);
        for(unsigned int i = 0; i < WorldObjects.size(); i++)
        {
            WorldObjects[i]->saveObject(&xmlFile, xml_objects);
        }
        for(unsigned int i = 0; i < WorldGroups.size(); i++)
        {
            WorldGroups[i]->saveGroup(&xmlFile, xml_objects);
        }

        xmlFile.SaveFile(worldFile.c_str());
    }

    bool World::loadWorld(std::string worldFile)
    {
        clearWorld();
        LOG("World: Loading world file");
        worldName = worldFile;
        XMLResourcePtr xmlFile = APP->xmlManager->load(worldName,"General");
        tinyxml2::XMLDocument *worldDoc = xmlFile->getXML();
        if(!worldDoc)
        {
            Ogre::String desc = "Error, world file missing";
            throw(Ogre::Exception(20, desc, "World"));
        }

        LOG("World: Loading terrain");
        tinyxml2::XMLElement *element;

        element = worldDoc->FirstChildElement("terrain");
        mapwidth = mapheight = 0;
        //int width, height;
        //width = height = 0;
        if(element)
        {
            LOG("World: Getting heightmap name");
            tinyxml2::XMLElement *next = element->FirstChildElement("heightmap");
            if(!next)
            {
                Ogre::String desc = "Error, no heightmap defined";
                throw(Ogre::Exception(20, desc, "World"));
            }
            heightMap = next->Attribute("name");
            if(next->BoolAttribute("multi"))
            {
                Ogre::String baseName;
                Ogre::String ext;
                Ogre::StringUtil::splitBaseFilename(heightMap, baseName, ext);
                multi_terrain = true;
                //heightMapExt = heightMap.;
                mapwidth = next->IntAttribute("width");
                mapheight = next->IntAttribute("height");
                LOG("World: heightmap name: " + heightMap);

                // x + y * width
                for(int x = 0; x < mapwidth; ++x)
                {
                    for(int y = 0; y  < mapheight; ++y)
                    {
                        int fileNumber = x + y * mapwidth + 1;
                        Ogre::String tmp;
                        if(fileNumber < 10)
                        {
                            tmp = "0" + Ogre::StringConverter::toString(fileNumber);
                        }
                        else
                        {
                            tmp = Ogre::StringConverter::toString(fileNumber);
                        }
                        Ogre::String fileName = baseName + tmp + "." + ext;
                        heightMaps.push_back(fileName);
                    }
                }
            }
            else
            {
                multi_terrain = false;
                heightMaps.push_back(heightMap);
                mapwidth = 1;
                mapheight = 1;
            }
        }
        else
        {
            Ogre::String desc = "Error, terrain failed";
            throw(Ogre::Exception(20, desc, "World"));
        }

        loadTerrain();

        loadObjects(worldDoc);

        xmlFile->unload();
        APP->xmlManager->unload(worldFile);
        LOG("World: World loading complete");
        return true;
    }

    void World::clearWorld()
    {
        DeleteSTLContainer(WorldGroups);
        WorldGroups.clear();
        DeleteSTLContainer(WorldObjects);
        WorldObjects.clear();
        if(objectsNode)
            objectsNode->removeAndDestroyAllChildren();
        SceneMgr->destroyAllEntities();
        SceneMgr->destroyAllLights();
        /*for(unsigned int i = 0; i < ObjectData.size(); i++)
        {
            delete ObjectData[i];
        }
        ObjectData.clear();*/
        destroyScene();
        heightMaps.clear();
    }

    bool World::reload()
    {
        clearWorld();
        LOG("World: Reloading world objects");
        loadWorld(worldName);
        return true;
    }

    void World::loadObjects(tinyxml2::XMLDocument *worldDoc)
    {
        LOG("World: Parsing objects");
        tinyxml2::XMLElement *element = worldDoc->FirstChildElement("objects");
        ObjectFactory objFactory = ObjectFactory();
        if(element)
        {
            LOG("Loading Independent Objects");
            loadIndependentObjects(element, &objFactory);

            LOG("Loading Groups");
            //Ogre::SceneNode *groupNode = objectsNode->createChildSceneNode(grpName);
            //processGroup(element, &objFactory);
            processGroup(element, &objFactory, objectsNode);
        }

        for(unsigned int i = 0; i < WorldGroups.size(); i++)
        {
            WorldGroups[i]->resetY();
        }
    }

    void World::processGroup(tinyxml2::XMLElement *groupElt, ObjectFactory *objFactory, Ogre::SceneNode *grpNode)
    {
        tinyxml2::XMLElement *group = groupElt->FirstChildElement("group");
        while(group)
        {
            ObjectGroup *objGroup = new ObjectGroup();
            objGroup->init(group, objFactory, grpNode);
            WorldGroups.push_back(objGroup);
            group = group->NextSiblingElement("group");
        }
    }

    void World::loadIndependentObjects(tinyxml2::XMLElement *elt, ObjectFactory *objFactory)
    {
        tinyxml2::XMLElement *object = elt->FirstChildElement("static");
        while(object)
        {
            LOG("Creating Objects: " + Ogre::String(object->Attribute("name")));
            WorldObjects.push_back(objFactory->createObject(object, objectsNode));

            object = object->NextSiblingElement("static");
        }
    }

    void World::loadTerrain()
    {
        SceneMgr->setSkyBox(true, "GrimmNight");
        //SceneMgr->setSkyDome(true, "CloudyGrey", 5, 4);
        //SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
        //SceneMgr->setShadowFarDistance(APP->getConfigFloat("shadowDistance"));
        //SceneMgr->setShadowFarDistance(5000);

        Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
        lightdir.normalise();

        dirLight = SceneMgr->createLight("directionalLight");
        dirLight->setType(Ogre::Light::LT_DIRECTIONAL);
        dirLight->setDirection(Ogre::Vector3(0, -1, 1));
        dirLight->setDiffuseColour(Ogre::ColourValue::White);
        dirLight->setSpecularColour(dirLightColor);
        //dirLight->setCastShadows(true);

        sunLight = SceneMgr->createLight("sunLight");
        sunLight->setType(Ogre::Light::LT_POINT);
        sunLight->setPosition(Ogre::Vector3(0, 1000, 0));
        sunLight->setDiffuseColour(sunLightColor);
        sunLight->setSpecularColour(sunLightColor);
        //sunLight->setShadowFarClipDistance(50000);
        //sunLight->setShadowFarDistance(100);

        SceneMgr->setAmbientLight(ambientLightColor);

        terrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

        terrainGroup = OGRE_NEW Ogre::TerrainGroup(SceneMgr, Ogre::Terrain::ALIGN_X_Z, APP->getConfigFloat("heightmapsize") + 1, APP->getConfigFloat("worldsize"));
        terrainGroup->setFilenameConvention(Ogre::String("SpellByteTerrain"), Ogre::String("dat"));
        terrainGroup->setOrigin(mapwidth * Ogre::Vector3(APP->getConfigFloat("heightmapsize") / 2, 0, mapheight * APP->getConfigFloat("heightmapsize") / 2));
        //terrainGroup->setOrigin(Ogre::Vector3::ZERO);

        configureTerrainDefaults(dirLight);

        for(long x = 0; x < mapwidth; ++x)
            for(long y = 0; y < mapheight; ++y)
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
        if(GameCollisionTools)
            GameCollisionTools->terrGroup = terrainGroup;
        //terrainGroup->saveAllTerrains(false);
    }

    void World::createScene()
    {
        // create a water plane/scene node
        pWaterEntity = SceneMgr->createEntity("water", "WaterPlane");
        pWaterEntity->setMaterialName(APP->getConfigString("WaterMat"));
        SceneNode *waterNode = SceneMgr->getRootSceneNode()->createChildSceneNode("WaterNode");
        waterNode->attachObject(pWaterEntity);
        waterNode->translate(0, APP->getConfigFloat("WaterHeight"), 0);

        //DotSceneLoader *dsl = new DotSceneLoader();
        //dsl->parseDotScene("city.scene", "City", SceneMgr);
        /*goatNode = SceneMgr->getRootSceneNode()->createChildSceneNode("goat");
        Ogre::DotSceneLoader *dsl = new Ogre::DotSceneLoader();
        dsl->parseDotScene("goat.scene", "Test", SceneMgr, goatNode);
        goatNode->setPosition(100, terrainGroup->getHeightAtWorldPosition(100, 0, 100), 100);
        goatAnimState = SceneMgr->getEntity("geomU3D1")->getAnimationState("my_animation");
        goatAnimState->setLoop(true);
        goatAnimState->setEnabled(true);
        LOG("Anim length: " + Ogre::StringConverter::toString(goatAnimState->getLength()));
        delete dsl;*/

        //manNode->pitch(Ogre::Radian(90));
        /*Ogre::AnimationStateSet *animSet = ent->getAllAnimationStates();
        Ogre::AnimationStateIterator iter = animSet->getAnimationStateIterator();
        while(iter.hasMoreElements())
        {
            LOG("Animation name: " + iter.getNext()->getAnimationName());
        }
        manAnimState = ent->getAnimationState("default_skl");
        manAnimState->setLoop(true);
        manAnimState->setEnabled(true);*/

        /*Ogre::Entity *ent = SceneMgr->createEntity("male", "NoNameMat0.mesh");
        manNode = SceneMgr->getRootSceneNode()->createChildSceneNode("male");
        manNode->attachObject(ent);
        manNode->setScale(10, 10, 10);

        Ogre::AnimationStateSet *animSet = ent->getAllAnimationStates();
        Ogre::AnimationStateIterator iter = animSet->getAnimationStateIterator();
        while(iter.hasMoreElements())
        {
            LOG("Animation name: " + iter.getNext()->getAnimationName());
        }*/

        /*std::vector<Ogre::String> entParts;
        entParts.push_back("BaseArms");
        entParts.push_back("BaseHands");
        entParts.push_back("BaseHairC");
        entParts.push_back("Head");
        entParts.push_back("Teeth");
        entParts.push_back("Lowerteeth");
        entParts.push_back("ThiefHood");
        entParts.push_back("ThiefTorso");
        entParts.push_back("ThiefTrousers_MinerMeshExchange_");
        entParts.push_back("MinerBoots");*/
        //entParts.push_back("FarmerTorso");das
        /*for(unsigned int i = 0; i < ent->getNumSubEntities(); ++i)
        {
            Ogre::SubEntity *sEnt;
            sEnt = ent->getSubEntity(i);
            sEnt->setVisible(false);
        }
        for(unsigned int i = 0; i < entParts.size(); ++i)
        {
            Ogre::SubEntity *sEnt;
            sEnt = ent->getSubEntity(entParts[i]);
            sEnt->setVisible(true);
        }*/
        /*manNode->setPosition(0, terrainGroup->getHeightAtWorldPosition(0, 0, 0), 0);
        manAnimState = ent->getAnimationState("default_skl");
        manAnimState->setLoop(true);
        manAnimState->setEnabled(true);*/
    }

    void World::update(const Ogre::FrameEvent &evt)
    {
        /* Update Water */
        float fWaterFlow = FLOW_SPEED * evt.timeSinceLastFrame;
        static float fFlowAmount = 0.0f;
        static bool fFlowUp = true;
        SceneNode *pWaterNode = static_cast<SceneNode*>(
            Camera->getSceneManager()->getRootSceneNode()->getChild("WaterNode"));
        if(pWaterNode)
        {
            if(fFlowUp)
                fFlowAmount += fWaterFlow;
            else
                fFlowAmount -= fWaterFlow;

            if(fFlowAmount >= FLOW_HEIGHT)
                fFlowUp = false;
            else if(fFlowAmount <= 0.0f)
                fFlowUp = true;

            pWaterNode->translate(0, (fFlowUp ? fWaterFlow : -fWaterFlow), 0);
        }

        //goatAnimState->addTime(evt.timeSinceLastFrame);
        //manAnimState->addTime(evt.timeSinceLastFrame);
        /*if(manAnimState->getAnimationName() == "NPCSitDown" && manAnimState->hasEnded()) {
            LOG("Animation OVER!!!");
            manAnimState = SceneMgr->getEntity("male")->getAnimationState("NPCTalking");
            manAnimState->setLoop(true);
            manAnimState->setEnabled(true);
        }*/

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
        if(sunLightUp)
        {
            sunLightColor.r += lightChangeValue;
            sunLightColor.g += lightChangeValue;
            sunLightColor.b += lightChangeValue;
            //dirLight->setSpecularColour(sunLightColor);
            sunLight->setSpecularColour(sunLightColor);
            sunLight->setDiffuseColour(sunLightColor);
        }
        if(sunLightDown)
        {
            sunLightColor.r -= lightChangeValue;
            sunLightColor.g -= lightChangeValue;
            sunLightColor.b -= lightChangeValue;
            sunLight->setDiffuseColour(sunLightColor);
        }
    }

    void World::handleEvent(int event)
    {
        switch(event)
        {
        case UserEvent::RELOAD:
            reload();
            break;
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
            sunLightUp = true;
            sunLightDown = false;
            break;
        case UserEvent::DIR_LIGHT_UP_OFF:
            sunLightUp = false;
            break;
        case UserEvent::DIR_LIGHT_DOWN_ON:
            sunLightDown = true;
            sunLightUp = false;
            break;
        case UserEvent::DIR_LIGHT_DOWN_OFF:
            sunLightDown = false;
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
            int index = x + y * mapwidth;
            img.load(heightMaps[index], Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            //getTerrainImage(x % 2 != 0, y % 2 != 0, img, heightMap);
            terrainGroup->defineTerrain(x, y, &img);
            terrainsImported = true;
        }
    }

    void World::initBlendMaps(Ogre::Terrain *terrain)
    {
        Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
        Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
        Ogre::Real minHeight0 = 260;
        Ogre::Real fadeDist0 = 10;
        Ogre::Real minHeight1 = 340;
        Ogre::Real fadeDist1 = 5;
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
        defaultimp.terrainSize = APP->getConfigFloat("heightmapsize") + 1;
        defaultimp.worldSize = APP->getConfigFloat("worldsize");
        defaultimp.inputScale = APP->getConfigFloat("inputscale"); // due terrain.png is 8 bpp
        defaultimp.minBatchSize = 33;
        defaultimp.maxBatchSize = 65;

        // textures
        defaultimp.layerList.resize(3);
        defaultimp.layerList[0].worldSize = 5;
        defaultimp.layerList[0].textureNames.push_back("wildgrass_4_seamless_1024.jpg");
        defaultimp.layerList[0].textureNames.push_back("wildgrass_4_seamless_1024.jpg");
        defaultimp.layerList[1].worldSize = 2;
        defaultimp.layerList[1].textureNames.push_back("cgrass1.jpg");
        defaultimp.layerList[1].textureNames.push_back("cgrass1.jpg");
        defaultimp.layerList[2].worldSize = 1;
        defaultimp.layerList[2].textureNames.push_back("cgrass1.jpg");
        defaultimp.layerList[2].textureNames.push_back("cgrass1.jpg");
    }

    void World::destroyScene()
    {
        if(terrainGroup)
        {
            OGRE_DELETE terrainGroup;
            terrainGroup = NULL;
        }
        if(terrainGlobals)
        {
            OGRE_DELETE terrainGlobals;
            terrainGlobals = NULL;
        }
    }
}
