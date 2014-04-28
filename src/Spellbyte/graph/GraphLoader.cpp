#include "GraphLoader.h"

#include "define.h"
#include <utilities/utils.h>
#include <stdafx.h>
#include <World.h>

namespace SpellByte {
    namespace Graph {
        GraphLoader::GraphLoader(World *world, bool visualMarkers):
                    WorldGraph(nullptr), worldPtr(world), visualNodeMarkers(visualMarkers) {
            graphNode = APP->SceneMgr->getRootSceneNode()->createChildSceneNode("GRAPH_NODE");
        }

        GraphLoader::~GraphLoader() {
        }

        void GraphLoader::clearGraph() {
            if(WorldGraph)
                WorldGraph->clearGraph();
            graphNode->removeAndDestroyAllChildren();
            for (unsigned int i = 0; i < manualObjects.size(); ++i) {
                Ogre::MaterialManager::getSingleton().remove(materialNames[i]);
                APP->SceneMgr->destroyManualObject(manualObjects[i]);
            }
            manualObjects.clear();
        }

        void GraphLoader::showVisualGraph() {
            if (visualNodeMarkers)
                graphNode->setVisible(true);
        }

        void GraphLoader::hideVisualGraph() {
            if (visualNodeMarkers)
                graphNode->setVisible(false);
        }

        void GraphLoader::loadGraph(Graph *graph, tinyxml2::XMLElement *xmlGraph) {
            WorldGraph = graph;
            clearGraph();
            tinyxml2::XMLElement *nextNode = xmlGraph->FirstChildElement("nodes")->FirstChildElement("node");
            LOG("Loading graphs");
            while(nextNode) {
                int id = nextNode->IntAttribute("id");
                float x, y, z;
                loadPosition(nextNode, x, y, z);
                Ogre::Vector3 nodePosition = Ogre::Vector3(x, y, z);
                nodePosition.y = worldPtr->getHeight(x, y, z);
                if(visualNodeMarkers) {
                    Ogre::String nodeName = "GRAPH_NODE_" + Ogre::StringConverter::toString(id);
                    Ogre::Entity *entity = APP->SceneMgr->createEntity(nodeName, "marker.mesh");
                    Ogre::SceneNode *childNode = graphNode->createChildSceneNode();
                    childNode->setPosition(nodePosition);
                    LOG("Node(" + Ogre::StringConverter::toString(id) + ") Position:" +
                        Ogre::StringConverter::toString(nodePosition));
                    entity->setQueryFlags(World::COLLISION_MASK::GRAPH_NODE);
                    childNode->attachObject(entity);
                }
                NavNode newNode = NavNode(id, nodePosition);
                WorldGraph->addNode(newNode);
                nextNode = nextNode->NextSiblingElement("node");
            }
            // Now edges
            tinyxml2::XMLElement *nextEdge = xmlGraph->FirstChildElement("edges")->FirstChildElement("edge");
            while(nextEdge) {
                int originNode = nextEdge->IntAttribute("origin");
                int destNode = nextEdge->IntAttribute("dest");
                Ogre::Vector3 oPosition = WorldGraph->getNode(originNode).getPos();
                Ogre::Vector3 dPosition = WorldGraph->getNode(destNode).getPos();
                Ogre::Real cost = oPosition.distance(dPosition);
                Edge newEdge = Edge(originNode, destNode, cost);
                if(visualNodeMarkers) {
                    Ogre::String manualName = "EDGE_" + Ogre::StringConverter::toString(originNode) + "_" +
                                                Ogre::StringConverter::toString(destNode);
                    Ogre::ManualObject* myManualObject =  APP->SceneMgr->createManualObject(manualName);
                    manualObjects.push_back(myManualObject);
                    Ogre::SceneNode* myManualObjectNode = graphNode->createChildSceneNode(manualName + "_NODE");

                    // NOTE: The second parameter to the create method is the resource group the material will be added to.
                    // If the group you name does not exist (in your resources.cfg file) the library will assert() and your program will crash
                    Ogre::MaterialPtr myManualObjectMaterial = Ogre::MaterialManager::getSingleton().create(manualName + "_MAT","General");
                    materialNames.push_back(myManualObjectMaterial->getName());
                    myManualObjectMaterial->setReceiveShadows(false);
                    myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
                    myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0);
                    myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
                    myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);
                    //myManualObjectMaterial->dispose();  // dispose pointer, not the material

                    myManualObject->begin(manualName + "_MAT", Ogre::RenderOperation::OT_LINE_LIST);
                    oPosition.y = oPosition.y + 2.5;
                    dPosition.y = dPosition.y + 2.5;
                    myManualObject->position(oPosition);
                    myManualObject->position(dPosition);
                    // etc
                    myManualObject->end();

                    myManualObjectNode->attachObject(myManualObject);
                }
                WorldGraph->addEdge(newEdge);
                nextEdge = nextEdge->NextSiblingElement("edge");
            }
            graphNode->setVisible(false);
        }
    }
}
