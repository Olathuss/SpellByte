#include <cassert>
#include <iostream>
#include "Object.h"
#include "define.h"
#include "World.h"
#include "utilities/utils.h"

namespace SpellByte {
    void getMeshInformation(const Ogre::Mesh* const mesh,
                        size_t &vertex_count,
                        Ogre::Vector3* &vertices,
                        size_t &index_count,
                        unsigned long* &indices,
                        const Ogre::Vector3 &position,
                        const Ogre::Quaternion &orient,
                        const Ogre::Vector3 &scale)
    {
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices) {
            if( !added_shared ) {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // Add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared)) {
            if(submesh->useSharedVertices) {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes ) {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        } else {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
    }

    // Only bind to LUA once
    bool Object::BoundToLUA = false;

    int Object::nextValidID = 0;

    Object::Object(int id) {
        setID(id);
    }

    Object::Object() {
        entityCount = 0;
        setID(nextValidID);
        ysnap = Y_TERRAIN;
    }

    Object::~Object() {
        // Destroy object nodes children,
        // but not the node itself, it's parent or owner will destroy
        // that.
        ObjectNode->removeAndDestroyAllChildren();
        delete coldetModel;
    }

    bool Object::init() {
        WorldPtr = APP->getWorldPtr();
        createNode();
        return true;
    }

    // load object using XML input
    bool Object::init(tinyxml2::XMLElement *objElt,
                      Ogre::SceneNode *parentNode) {
        // Get pointer to world
        WorldPtr = APP->getWorldPtr();
        coldetModel = newCollisionModel3D(true);

        // Get object name and create unique name for it
        const char* objName = objElt->Attribute("name");
        if (objName != NULL) {
            ObjectName = std::string(objName);
            createNode(objName, parentNode);
        } else {
            createNode(parentNode);
        }

        // Check if object uses particular Ogre group
        // For resources
        const char* grp = objElt->Attribute("group");
        if (grp != NULL) {
            DefaultGroup = std::string(grp);
        } else {
            DefaultGroup = "General";
        }

        // Load object's meshes
        tinyxml2::XMLElement *meshElt = objElt->FirstChildElement("mesh");
        while (meshElt != 0) {
            Ogre::String meshFile;
            // Get filename of mesh
            meshFile = Ogre::String(meshElt->Attribute("file"));
            // See if mesh has particular group
            const char* entGroup = meshElt->Attribute("group");
            Ogre::String meshGroup;
            if (entGroup != NULL) {
                meshGroup = Ogre::String(entGroup);
            } else {
                meshGroup = DefaultGroup;
            }
            LOG("Loading mesh: " + meshFile);
            // Create entity with mesh
            Ogre::Entity* entity = addEntity(meshFile, meshGroup);
            addTrianglesToColdet(entity);
            coldetModel->finalize();
            if (entity != NULL) {
                // Check to see if entity requires material
                // If so, this needs to be kept in memory for saving later
                const char* matName = meshElt->Attribute("material");
                if (matName != NULL) {
                    entity->setMaterialName(matName);
                    meshMaterials[meshFile] = matName;
                }

                const char* maskName = meshElt->Attribute("mask");
                if (maskName != NULL) {
                    entity->setQueryFlags(WorldPtr->getMaskFromString(maskName));
                }

                if(meshElt->Attribute("renderq") != NULL) {
                    int renderqueue = meshElt->IntAttribute("renderqueue");
                    LOG("RenderQueueSet:" + renderqueue);
                    entity->setRenderQueueGroup(renderqueue);
                }

                // check if particular mesh is visible
                if (meshElt->Attribute("visible") != NULL) {
                    if (!meshElt->BoolAttribute("visible")) {
                        entity->setVisible(false);
                    }
                }
            }
            meshElt = meshElt->NextSiblingElement("mesh");
        }

        // Load settings, set some defaults
        const int DEGREES = 0;
        const int RADIANS = 0;
        int rotateType = 0;

        // Object by default will snap its Y position to terrain,
        // As of now this is only done once, is not persist through updates
        setYsnap(Y_TERRAIN);

        // Load settings from XML
        tinyxml2::XMLElement *settingElt = objElt->FirstChildElement("setting");
        if (settingElt) {
            // check if object is visible
            if (settingElt->Attribute("visible") != NULL) {
                if (!settingElt->BoolAttribute("visible")) {
                    ObjectNode->setVisible(false);
                }
            }

            const char *ypos = settingElt->Attribute("ysnap");
            if (ypos != NULL) {
                if (strcmp(ypos, "relative") == 0) {
                    setYsnap(Y_RELATIVE);
                } else if (strcmp(ypos, "absolute") == 0) {
                    setYsnap(Y_ABSOLUTE);
                } else {
                    // Assume object snaps to terrain by default
                    // Correct id for this is "terrain" but
                    // will accept anything and replace it with terrain
                    setYsnap(Y_TERRAIN);
                }
            }

            // If object has particular rotation, load it
            // Degrees and Radians are supported thanks to Ogre3d
            const char *rot = settingElt->Attribute("rotation");
            if (rot != NULL) {
                if(strcmp(rot, "degrees") == 0) {
                    rotateType = DEGREES;
                } else if(strcmp(rot, "radians") == 0) {
                    rotateType = RADIANS;
                } else {
                    rotateType = DEGREES;
                }
            }
        }

        // Load position and set dependent on Y_SNAP
        float x, y, z;
        x = y = z = 0.0;
        loadPosition(objElt, x, y, z);
        if (ysnap == Y_ABSOLUTE) {
            ObjectNode->setPosition(x, y, z);
        } else if (ysnap == Y_RELATIVE) {
            y += WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
            ObjectNode->setPosition(x, y, z);
        } else {
            ObjectNode->setPosition(x, WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z), z);
        }

        // Now load rotation
        // Rotation can be either YAW/PITCH/ROLL or QUATERNION
        float roll, pitch, yaw, qw, qx, qy, qz;
        roll = pitch = yaw = qw = qx = qy = qz = 0;
        // Get rotation data from helper function
        int rotResult = loadRotate(objElt, roll, pitch, yaw, qw, qx, qy, qz);
        if(rotResult == 2) {
            if(rotateType == DEGREES) {
                ObjectNode->roll(Ogre::Radian(Ogre::Math::DegreesToRadians(roll)));
                ObjectNode->pitch(Ogre::Radian(Ogre::Math::DegreesToRadians(pitch)));
                ObjectNode->yaw(Ogre::Radian(Ogre::Math::DegreesToRadians(yaw)));
            }
            else if(rotateType == RADIANS) {
                ObjectNode->roll(Ogre::Radian(roll));
                ObjectNode->pitch(Ogre::Radian(pitch));
                ObjectNode->yaw(Ogre::Radian(yaw));
            }
        }
        else if(rotResult == 1) {
            Ogre::Quaternion rotQ = Ogre::Quaternion(qw, qx, qy, qz);
            ObjectNode->setOrientation(rotQ);
        }

        //worldPtr->worldObjectSystem->add_object
        /*LOG("MATRIX: ");
        Ogre::Matrix4 matrix4 = ObjectNode->_getFullTransform();
        for(int i = 0; i < 4; i++) {
             LOG("[" + Ogre::StringConverter::toString(matrix4[i][0]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][1]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][2]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][3]) + "]");
        }*/

        /*matrix4 = ObjectNode->_getFullTransform().transpose();
        LOG("MATRIX.transpose(): ");
        for(int i = 0; i < 4; i++) {
             LOG("[" + Ogre::StringConverter::toString(matrix4[i][0]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][1]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][2]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][3]) + "]");
        }*/

        if (loadScale(objElt, x, y, z)) {
            ObjectNode->setScale(x, y, z);
        }

        /*matrix4 = ObjectNode->_getFullTransform();
        LOG("!!!MATRIX: ");
        for(int i = 0; i < 4; i++) {
             LOG("[" + Ogre::StringConverter::toString(matrix4[i][0]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][1]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][2]) + "]" +
             "[" + Ogre::StringConverter::toString(matrix4[i][3]) + "]");
        }*/

        return true;
    }

    // Set Y_SNAP, helper function
    void Object::setYsnap(int newSnap) {
        // snap to terrain by default in case of
        // invalid value
        if (newSnap < Y_TERRAIN || newSnap >= Y_INVALID) {
            ysnap = Y_TERRAIN;
        }
        else {
            ysnap = newSnap;
        }
    }

    const int Object::getYsnap() const {
        return ysnap;
    }

    void Object::update(const Ogre::FrameEvent &evt)
    {
        // empty for now
    }

    // Saves object to XML
    void Object::saveObject(tinyxml2::XMLDocument *xmlDoc, tinyxml2::XMLElement *elt) {
        tinyxml2::XMLElement *objElt = xmlDoc->NewElement("object");
        elt->InsertEndChild(objElt);
        // Set object name/group
        objElt->SetAttribute("name", ObjectName.c_str());
        objElt->SetAttribute("group", DefaultGroup.c_str());

        // Loop through different meshes and save to XML
        const Ogre::SceneNode::ObjectIterator it = ObjectNode->getAttachedObjectIterator();
        while(it.hasMoreElements()) {
            tinyxml2::XMLElement *meshElt = xmlDoc->NewElement("mesh");
            objElt->InsertEndChild(meshElt);
            const Ogre::Entity *entity = static_cast<const Ogre::Entity*>(it.getNext());
            // Save meshes group and file, group is not saved in memory
            // so it will always be saved out
            meshElt->SetAttribute("file", entity->getMesh()->getName().c_str());
            meshElt->SetAttribute("group", entity->getMesh()->getGroup().c_str());

            // If mesh had particular material, save this to XML
            std::map<std::string, std::string>::iterator matIterator;
            matIterator = meshMaterials.find(entity->getMesh()->getName().c_str());
            if (matIterator != meshMaterials.end()) {
                meshElt->SetAttribute("material", matIterator->second.c_str());
            }
        }

        // Save out settings
        tinyxml2::XMLElement *settingElt = xmlDoc->NewElement("setting");
        settingElt->SetAttribute("ysnap", "absolute");
        objElt->InsertEndChild(settingElt);

        // Save position
        tinyxml2::XMLElement *posElt = xmlDoc->NewElement("position");
        // Use Ogre3D's derived position for absolute position
        Ogre::Vector3 pos = ObjectNode->_getDerivedPosition();
        posElt->SetAttribute("x", pos.x);
        posElt->SetAttribute("y", pos.y);
        posElt->SetAttribute("z", pos.z);
        objElt->InsertEndChild(posElt);

        // Save rotation/orientation
        tinyxml2::XMLElement *rotElt = xmlDoc->NewElement("rotate");
        // Use Ogre3's derived orientation for absolute rotation
        Ogre::Quaternion quat = ObjectNode->_getDerivedOrientation();
        // Can only be done in quaternion, so saving out world file
        // will always result in quaternion
        rotElt->SetAttribute("qw", quat.w);
        rotElt->SetAttribute("qx", quat.x);
        rotElt->SetAttribute("qy", quat.y);
        rotElt->SetAttribute("qz", quat.z);
        objElt->InsertEndChild(rotElt);

        // Save scale
        tinyxml2::XMLElement *scaleElt = xmlDoc->NewElement("scale");
        // Use Ogre's derived scale for absolute value
        Ogre::Vector3 scale = ObjectNode->_getDerivedScale();
        scaleElt->SetAttribute("x", scale.x);
        scaleElt->SetAttribute("y", scale.y);
        scaleElt->SetAttribute("z", scale.z);
        objElt->InsertEndChild(scaleElt);
    }

    // Sort of like a factory for object's scenenodes
    // This creates a unique name for the node
    void Object::createNode(Ogre::SceneNode *parentNode) {
        // Call other createNode with name
        this->createNode("DEFAULT_NAME", parentNode);
    }

    void Object::createNode(std::string objectName, Ogre::SceneNode *parentNode) {
        Ogre::String nodeName = "NODE" + objectName + Ogre::StringConverter::toString((int)ID);
        if (parentNode) {
            ObjectNode = parentNode->createChildSceneNode(nodeName);
        } else {
            ObjectNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
        }
        // Allow Ogre's SceneNode to refer to this as its owner
        // Useful for colliision callback, event referal etc.
        ObjectNode->setUserAny(Ogre::Any(this));
    }

    void Object::handleMessage(const Telegram &msg) {
        // Handle message code for actor here

        // Pass message to each component
        std::map<componentType, ComponentPtr>::iterator it;
        for(it = components.begin(); it != components.end(); ++it) {
            it->second->handleMessage(msg);
        }
    }

    // Add entity
    Ogre::Entity* Object::addEntity(std::string meshName, std::string groupName) {
        // Create unique name for Entity
        Ogre::String entityName = "ENT" + Ogre::StringConverter::toString(ID) + "_" + Ogre::StringConverter::toString(entityCount++);
        Ogre::Entity *entity = NULL;
        try {
            entity = APP->SceneMgr->createEntity(entityName, meshName, groupName);

            ObjectNode->attachObject(entity);
        } catch (Ogre::Exception &e) {
            return NULL;
        }
        return entity;
    }

    // ResetY, this is used to do objects resnapping in case its group
    // is moved to new location
    void Object::resetY() {
        Ogre::Vector3 newPosition = ObjectNode->_getDerivedPosition();
        if (ysnap == Y_TERRAIN) {
            newPosition.y = WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        } else if (ysnap == Y_RELATIVE) {
            newPosition.y += WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        ObjectNode->_setDerivedPosition(newPosition);
    }

    // Set position of Object based on Ogre::Real XYZ
    void Object::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z) {
        if(ysnap == Y_TERRAIN) {
            y = WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
        } else if(ysnap == Y_RELATIVE) {
            y += WorldPtr->terrainGroup->getHeightAtWorldPosition(x, y, z);
        }
        ObjectNode->setPosition(x, y, z);
    }

    // Set position based on Vector3
    void Object::setPosition(Ogre::Vector3 newPosition) {
        if(ysnap == Y_TERRAIN) {
            newPosition.y = WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        } else if(ysnap == Y_RELATIVE) {
            newPosition.y += WorldPtr->terrainGroup->getHeightAtWorldPosition(newPosition.x, newPosition.y, newPosition.z);
        }
        ObjectNode->setPosition(newPosition);
    }

    void Object::setGroup(ObjectGroup *parent) {
        ObjectParent = parent;
    }

    ObjectGroup *Object::getGroup() const {
        return ObjectParent;
    }

    Ogre::SceneNode *Object::getNode() const {
        return ObjectNode;
    }

    void Object::setID(int val) {
        assert((val >= nextValidID) && "<Object::setID>: invalid ID");

        ID = val;
        nextValidID = ID + 1;
    }

    void Object::resetNextValidID() {
        nextValidID = 0;
    }

    int Object::getNextValidID() {
        return nextValidID;
    }

    // This method adds the object's mesh into the coldetModel
    void Object::addTrianglesToColdet(Ogre::Entity *entity) {
        size_t vertex_count, index_count;
        Ogre::Vector3 *vertices;
        unsigned long *indices;

        // Get triangle/vertex information
        getMeshInformation(entity->getMesh().get(), vertex_count, vertices, index_count, indices,
                           Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY,
                           Ogre::Vector3::UNIT_SCALE);

        coldetModel->setTriangleNumber(index_count / 3);
        // Loop through triangles and add to physics model
        for(size_t i = 0; i < index_count; i += 3) {
            // Each triangle has three vertices of 3 floats each, so 9 total
            const float triangle[9] = { vertices[indices[i]][0], vertices[indices[i]][1], vertices[indices[i]][2],
                                        vertices[indices[i + 1]][0], vertices[indices[i + 1]][1], vertices[indices[i + 1]][2],
                                        vertices[indices[i + 2]][0], vertices[indices[i + 2]][1], vertices[indices[i + 2]][2] };

            coldetModel->addTriangle(&triangle[0], &triangle[3], &triangle[6]);
        }
        // Remove allocated memory
        delete[] vertices;
        delete[] indices;
    }
}
