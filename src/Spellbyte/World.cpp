#include <cstdlib>
#include <time.h>
#include "World.h"
#include "define.h"
#include "ActorManager.h"
#include "ActorFactory.h"
#include "console/LuaManager.h"
#include "DotSceneLoader.h"
#include "utilities/utils.h"
#include "world/Water.h"

// Does this need to be somewhere?
// WaterCircle::clearStaticBuffers();

namespace SpellByte
{
    void prepareCircleMaterial()
    {
        char *bmap = new char[256 * 256 * 4] ;
        memset(bmap, 127, 256 * 256 * 4);
        for(int b=0;b<16;b++) {
            int x0 = b % 4 ;
            int y0 = b >> 2 ;
            Real radius = 4.0f + 1.4 * (float) b ;
            for(int x=0;x<64;x++) {
                for(int y=0;y<64;y++) {
                    Real dist = Math::Sqrt((x-32)*(x-32)+(y-32)*(y-32)); // 0..ca.45
                    dist = fabs(dist -radius -2) / 2.0f ;
                    dist = dist * 255.0f;
                    if (dist>255)
                        dist=255 ;
                    int colour = 255-(int)dist ;
                    colour = (int)( ((Real)(15-b))/15.0f * (Real) colour );

                    bmap[4*(256*(y+64*y0)+x+64*x0)+0]=colour ;
                    bmap[4*(256*(y+64*y0)+x+64*x0)+1]=colour ;
                    bmap[4*(256*(y+64*y0)+x+64*x0)+2]=colour ;
                    bmap[4*(256*(y+64*y0)+x+64*x0)+3]=colour ;
                }
            }
        }

        DataStreamPtr imgstream(new MemoryDataStream(bmap, 256 * 256 * 4));
        //~ Image img;
        //~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 );
        //~ TextureManager::getSingleton().loadImage( CIRCLES_MATERIAL , img );
        TextureManager::getSingleton().loadRawData(CIRCLES_MATERIAL,
                                                   ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                   imgstream, 256, 256, PF_A8R8G8B8);
        MaterialPtr material =
        MaterialManager::getSingleton().create( CIRCLES_MATERIAL,
                                               ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        TextureUnitState *texLayer = material->getTechnique(0)->getPass(0)->createTextureUnitState( CIRCLES_MATERIAL );
        texLayer->setTextureAddressingMode( TextureUnitState::TAM_CLAMP );
        material->setSceneBlending( SBT_ADD );
        material->setDepthWriteEnabled( false ) ;
        material->load();
        // finished with bmap so release the memory
        delete [] bmap;
    }

    World::World()
    {
        srand(time(NULL));

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

        terrainLayers = std::vector<Layer>(3);

        SceneMgr = APP->SceneMgr;
        APP->attachWorld(this);
        objectsNode = SceneMgr->getRootSceneNode()->createChildSceneNode("objectsNode");


        Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

        Ogre::MeshManager::getSingleton().createPlane("water", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                        plane, 300, 220, 20, 20, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);

        Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                        plane, 170, 280, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

        Ogre::MeshManager::getSingleton().createPlane("ocean", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                        plane, 10000, 10000, 100, 100, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

        // Create lights
        SceneMgr->setSkyBox(true, "GrimmNight");
        //sSceneMgr->setSkyDome(true, "CloudyGrey", 5, 4);
        //SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
        //SceneMgr->setShadowFarDistance(APP->getConfigFloat("shadowDistance"));
        //SceneMgr->setShadowFarDistance(5000);

        Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
        lightdir.normalise();

        sunLight = SceneMgr->createLight("sunLight");
        sunLight->setType(Ogre::Light::LT_POINT);
        sunLight->setPosition(Ogre::Vector3(0, 1000, 0));
        sunLight->setDiffuseColour(sunLightColor);
        sunLight->setSpecularColour(sunLightColor);

        Ogre::SceneNode* lightNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
        lightNode->attachObject(sunLight);

        SceneMgr->setAmbientLight(ambientLightColor);

        waterMesh = NULL;
        // Create water
        /*waterMesh = new WaterMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY);
        waterEntity = SceneMgr->createEntity(ENTITY_NAME, MESH_NAME);
        waterEntity->setMaterialName("Water0");
        SceneNode *waterNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
        waterNode->attachObject(waterEntity);
        waterNode->setPosition(0, 100, 0);*/

        // set up spline animation of light node
        /*Ogre::Animation *anim = SceneMgr->createAnimation("WaterLight", 20);
        Ogre::NodeAnimationTrack *track;
        Ogre::TransformKeyFrame *key;
        // create a random spline for light
        track = anim->createNodeTrack(0, lightNode);
        key = track->createNodeKeyFrame(0);
        for(int ff = 1; ff <= 19; ff++) {
            key = track->createNodeKeyFrame(ff);
            Ogre::Vector3 lpos (
                                rand() % (int)PLANE_SIZE, // - PLANE_SIZE / 2
                                rand() % 30 + 10,
                                rand() % (int)PLANE_SIZE // - PLANE_SIZE / 2
                                );
            key->setTranslate(lpos);
        }
        key = track->createNodeKeyFrame(20);*/

        // Create a new animation state to track this
        //waterAnim = SceneMgr->createAnimationState("WaterLight");
        //waterAnim->setEnabled(true);

        // Put in a bit of fog for the hell of it
        //mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);

        // Let there be rain
        /*waterParticleSystem = SceneMgr->createParticleSystem("rain",
                                                         "Examples/Water/Rain");
		waterParticleEmitter = waterParticleSystem->getEmitter(0);
        SceneNode* rNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
        rNode->translate(0, 500, 0);
        rNode->attachObject(waterParticleSystem);
        // Fast-forward the rain so it looks more natural
        waterParticleSystem->fastForward(20);
		// It can't be set in .particle file, and we need it ;)
		static_cast<BillboardParticleRenderer*>(waterParticleSystem->getRenderer())->setBillboardOrigin(BBO_BOTTOM_CENTER);

		prepareCircleMaterial();*/

        terrainGlobals = NULL;
        terrainGroup = NULL;
        GameCollisionTools = NULL;
        worldObjectSystem = NULL;
        worldObjectSystem = COLDET::newSweepPruneSystem(2048);
    }

    World::~World()
    {
        APP->detachWorld();
        destroyScene();
        if (waterMesh) {
            delete waterMesh;
            waterMesh = NULL;
        }
        if(worldObjectSystem) {
            delete worldObjectSystem;
            worldObjectSystem = NULL;
        }
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

        // Save world information
        tinyxml2::XMLElement *xml_world = xmlFile.NewElement("world");
        // Build world version number to store
        Ogre::String version;
        version = Ogre::StringConverter::toString(worldVersionMajor) + ".";
        if (worldVersionMinor < 10) {
            version += "0";
        }
        version += worldVersionMinor;
        LOG("World Version: " + version);
        xml_world->SetAttribute("version", version.c_str());
        xmlFile.InsertEndChild(xml_world);

        // Save terrain information to file
        tinyxml2::XMLElement *xml_data = xmlFile.NewElement("data");
        xml_world->InsertEndChild(xml_data);
        std::map<std::string, float>::iterator it;
        for(it = worldFloats.begin(); it != worldFloats.end(); ++it) {
            tinyxml2::XMLElement *xml_float = xmlFile.NewElement("float");
            xml_data->InsertEndChild(xml_float);
            xml_float->SetAttribute("id", it->first.c_str());
            xml_float->SetAttribute("value", it->second);
        }
        tinyxml2::XMLElement *xml_terrain = xmlFile.NewElement("terrain");
        xml_world->InsertEndChild(xml_terrain);
        tinyxml2::XMLElement *xml_heightmap = xmlFile.NewElement("heightmap");
        xml_terrain->InsertEndChild(xml_heightmap);
        xml_heightmap->SetAttribute("name", heightMap.c_str());
        xml_heightmap->SetAttribute("multi", multi_terrain);
        xml_heightmap->SetAttribute("height", mapheight);
        xml_heightmap->SetAttribute("width", mapwidth);

        for(int i = 0; i < 3; ++i) {
            tinyxml2::XMLElement *xml_layer = xmlFile.NewElement("layer");
            xml_terrain->InsertEndChild(xml_layer);
            xml_layer->SetAttribute("id", i);
            xml_layer->SetAttribute("worldSize", terrainLayers[i].worldSize);
            xml_layer->SetAttribute("diffuse", terrainLayers[i].diffuse.c_str());
            xml_layer->SetAttribute("normal", terrainLayers[i].normal.c_str());
        }

        tinyxml2::XMLElement *xml_objects = xmlFile.NewElement("static");
        xml_world->InsertEndChild(xml_objects);
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

    bool World::loadWorld(std::string worldFile) {
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

        tinyxml2::XMLElement *worldElt = worldDoc->FirstChildElement("world");
        if (!worldElt) {
            Ogre::String desc = "Error, invalid world file";
            throw(Ogre::Exception(20, desc, "World"));
        }

        const char* tmp = worldElt->Attribute("version");
        if (tmp == NULL) {
            Ogre::String desc = "Error, invalid version";
            throw(Ogre::Exception(20, desc, "World"));
            return false;
        }
        char* worldVersion = new char[std::strlen(tmp) + 1];
        std::strcpy(worldVersion, tmp);
        char *major = strtok(worldVersion, ".");
        worldVersionMajor = atoi(major);
        char *minor = strtok(NULL, ".");
        worldVersionMinor = atoi(minor);
        delete[] worldVersion;

        if (worldVersionMajor > MAJOR_SUPPORT || (worldVersionMajor <= MAJOR_SUPPORT && worldVersionMinor > MINOR_SUPPORT)) {
            Ogre::String desc = "Error, unsupported version";
            throw(Ogre::Exception(20, desc, "World"));
            return false;
        }

        tinyxml2::XMLElement *element;
        element = worldElt->FirstChildElement("data");

        LOG("World: Loading settings");
        if(element) {
            loadData(element);
        } else {
            Ogre::String desc = "Error, data fail";
            throw(Ogre::Exception(20, desc, "World"));
        }

        LOG("World: Loading terrain");
        element = worldElt->FirstChildElement("terrain");
        if(element) {
            loadTerrain(element);
        } else {
            Ogre::String desc = "Error, terrain failed";
            throw(Ogre::Exception(20, desc, "World"));
        }

        loadObjects(worldElt);

        xmlFile->unload();
        APP->xmlManager->unload(worldFile);
        LOG("World: World loading complete");
        LOG("World: Populating with actors");
        ActorFactory actorFactory;
        for(int i = 0; i < 25; i++) {
            actorFactory.createActor(SceneMgr, this);
        }
        LOG("World: Actor population complete");
        return true;
    }

    void World::clearWorld()
    {
        DeleteSTLContainer(WorldGroups);
        WorldGroups.clear();
        DeleteSTLContainer(WorldObjects);
        WorldObjects.clear();
        ActorMgr->clearActors();
        if(objectsNode)
            objectsNode->removeAndDestroyAllChildren();
        SceneMgr->destroyAllEntities();
        SceneMgr->destroyAllLights();
        ActorMgr->reset();
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
        LOG("World: Reloading world");
        loadWorld(worldName);
        return true;
    }

    void World::loadObjects(tinyxml2::XMLElement *worldElt)
    {
        LOG("World: Parsing Static Objects");
        tinyxml2::XMLElement *element = worldElt->FirstChildElement("static");
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
        while (group)
        {
            ObjectGroup *objGroup = new ObjectGroup();
            objGroup->init(group, objFactory, grpNode);
            WorldGroups.push_back(objGroup);
            group = group->NextSiblingElement("group");
        }
    }

    void World::loadIndependentObjects(tinyxml2::XMLElement *elt, ObjectFactory *objFactory)
    {
        tinyxml2::XMLElement *object = elt->FirstChildElement("object");
        while(object)
        {
            LOG("Creating Objects: " + Ogre::String(object->Attribute("name")));
            WorldObjects.push_back(objFactory->createObject(object, objectsNode));

            object = object->NextSiblingElement("object");
        }
    }

    void World::loadTerrain(tinyxml2::XMLElement *terrainElt)
    {
        LOG("World: Getting heightmap name");
        tinyxml2::XMLElement *next = terrainElt->FirstChildElement("heightmap");
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

        next = terrainElt->FirstChildElement("layer");
        if(!next) {
            Ogre::String desc = "Error, layer missing from terrain data";
            throw(Ogre::Exception(20, desc, "World"));
        } else {
            int layerCount = 0;
            while(next) {
                Layer layer;
                int layerId = next->IntAttribute("id");
                if(layerId > 2 || layerId < 0) {
                    LOG("Only 3 layers supported, ignoring layer " + Ogre::StringConverter::toString(layerId));
                    next = next->NextSiblingElement();
                }
                layer.worldSize = next->FloatAttribute("worldSize");
                layer.diffuse = next->Attribute("diffuse");
                layer.normal = next->Attribute("normal");
                terrainLayers[layerId] = layer;
                layerCount++;
                next = next->NextSiblingElement();
            }
            if(layerCount != 3) {
                Ogre::String desc = "Error, terrain requires 3 layers";
                throw(Ogre::Exception(20, desc, "World"));
            }
        }

        dirLight = SceneMgr->createLight("directionalLight");
        dirLight->setType(Ogre::Light::LT_DIRECTIONAL);
        dirLight->setDirection(Ogre::Vector3(0, -1, 1));
        dirLight->setDiffuseColour(Ogre::ColourValue::White);
        dirLight->setSpecularColour(dirLightColor);
        //dirLight->setCastShadows(true);

        terrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

        terrainGroup = OGRE_NEW Ogre::TerrainGroup(SceneMgr, Ogre::Terrain::ALIGN_X_Z, getDataFloat("heightmapsize") + 1, getDataFloat("worldsize"));
        terrainGroup->setFilenameConvention(Ogre::String("SpellByteTerrain"), Ogre::String("dat"));
        terrainGroup->setOrigin(mapwidth * Ogre::Vector3(getDataFloat("heightmapsize") / 2, 0, mapheight * getDataFloat("heightmapsize") / 2));
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
        terrainGroup->saveAllTerrains(false);
    }

    void World::createScene()
    {
    }

    bool World::update(const Ogre::FrameEvent &evt) {
        ActorMgr->update(evt);
        // TBR
        /* Update Water */
        /* float fWaterFlow = FLOW_SPEED * evt.timeSinceLastFrame;
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
        }*/
        // TBR

        //goatAnimState->addTime(evt.timeSinceLastFrame);
        //manAnimState->addTime(evt.timeSinceLastFrame);
        /*if(manAnimState->getAnimationName() == "NPCSitDown" && manAnimState->hasEnded()) {
            LOG("Animation OVER!!!");
            manAnimState = SceneMgr->getEntity("male")->getAnimationState("NPCTalking");
            manAnimState->setLoop(true);
            manAnimState->setEnabled(true);
        }*/

        /* Update Water */
        //waterAnim->addTime(evt.timeSinceLastFrame);

        //processCircles(evt.timeSinceLastFrame);
        //processParticles();
        //waterMesh->updateMesh(evt.timeSinceLastFrame);

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

        return true;
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
        Ogre::Real minHeight0 = getDataFloat("minHeight0");
        Ogre::Real fadeDist0 = getDataFloat("fadeDist0");
        Ogre::Real minHeight1 = getDataFloat("minHeight1");
        Ogre::Real fadeDist1 = getDataFloat("fadeDist1");
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
        if(hasDataInt("renderqueue")) {
            terrainGlobals->setRenderQueueGroup(getDataInt("renderqueue"));
        };

        Ogre::Terrain::ImportData &defaultimp = terrainGroup->getDefaultImportSettings();
        defaultimp.terrainSize = getDataFloat("heightmapsize") + 1;
        defaultimp.worldSize = getDataFloat("worldsize");
        defaultimp.inputScale = getDataFloat("inputscale"); // due terrain.png is 8 bpp
        defaultimp.minBatchSize = APP->getConfigFloat("minBatchSize");
        defaultimp.maxBatchSize = APP->getConfigFloat("maxBatchSize");

        // textures
        defaultimp.layerList.resize(3);
        for(int i = 0; i < 3; ++i) {
            defaultimp.layerList[i].worldSize = terrainLayers[i].worldSize;
            defaultimp.layerList[i].textureNames.push_back(terrainLayers[i].diffuse);
            defaultimp.layerList[i].textureNames.push_back(terrainLayers[i].normal);
        }
    }

    void World::destroyScene() {
        if (terrainGroup) {
            OGRE_DELETE terrainGroup;
            terrainGroup = NULL;
        }
        if (terrainGlobals) {
            OGRE_DELETE terrainGlobals;
            terrainGlobals = NULL;
        }

        for (unsigned int i = 0; i < circles.size(); ++i) {
            delete (circles[i]);
        }

        circles.clear();
    }

    void World::processCircles(Real timeSinceLastFrame) {
        for(unsigned int i=0;i<circles.size();i++) {
			circles[i]->animate(timeSinceLastFrame);
		}
		bool found ;
		do {
			found = false ;
			for(WaterCircles::iterator it = circles.begin() ;
                it != circles.end();
                ++it) {
				if ((*it)->lvl>=16) {
					delete (*it);
					circles.erase(it);
					found = true ;
					break ;
				}
			}
		} while (found) ;
    }

    void World::processParticles() {
		static int pindex = 0 ;
		ParticleIterator pit = waterParticleSystem->_getIterator() ;
		while(!pit.end()) {
			Particle *particle = pit.getNext();
			Vector3 ppos = particle->position;
			if (ppos.y<=0 && particle->timeToLive>0) { // hits the water!
				// delete particle
				particle->timeToLive = 0.0f;
				// push the water
				float x = ppos.x / PLANE_SIZE * COMPLEXITY ;
				float y = ppos.z / PLANE_SIZE * COMPLEXITY ;
				float h = rand() % RAIN_HEIGHT_RANDOM + RAIN_HEIGHT_CONSTANT ;
				if (x<1) x=1 ;
				if (x>COMPLEXITY-1) x=COMPLEXITY-1;
				if (y<1) y=1 ;
				if (y>COMPLEXITY-1) y=COMPLEXITY-1;
				waterMesh->push(x,y,-h);
				WaterCircle *circle = new WaterCircle(SceneMgr,
                                                      "Circle#"+Ogre::StringConverter::toString(pindex++),
                                                      x, y);
				circles.push_back(circle);
			}
		}
	}

    void World::loadData(tinyxml2::XMLElement *dataElt) {
        if(!dataElt)
            return;

        // Load floats
        for(tinyxml2::XMLElement *element = dataElt->FirstChildElement("float"); element;
            element = element->NextSiblingElement()) {

            const char *key=element->Attribute("id");
            float value=element->FloatAttribute("value");
            if(key) {
                worldFloats[key] = value;
            }
        }
        for(tinyxml2::XMLElement *element = dataElt->FirstChildElement("int"); element;
            element = element->NextSiblingElement()) {

            const char *key=element->Attribute("id");
            int value=element->FloatAttribute("value");
            if(key) {
                worldInts[key] = value;
            }
        }

        return;
    }

    bool World::hasDataFloat(std::string ID) const {
        std::map<std::string, float>::const_iterator it;
        it = worldFloats.find(ID);
        if(it == worldFloats.end()) {
                return false;
        }
        return true;
    }

    const float World::getDataFloat(std::string ID) const {
        std::map<std::string, float>::const_iterator it;
        it = worldFloats.find(ID);
        if(it == worldFloats.end()) {
            Ogre::String desc = "Error finding setting " + ID;
            throw(Ogre::Exception(20, desc, "World"));
        }

        return it->second;
    }

    bool World::hasDataInt(std::string ID) const {
        std::map<std::string, int>::const_iterator it;
        it = worldInts.find(ID);
        if(it == worldInts.end()) {
                return false;
        }
        return true;
    }

    const int World::getDataInt(std::string ID) const {
        std::map<std::string, int>::const_iterator it;
        it = worldInts.find(ID);
        if(it == worldInts.end()) {
            Ogre::String desc = "Error finding setting " + ID;
            throw(Ogre::Exception(20, desc, "World"));
        }

        return it->second;
    }
}
