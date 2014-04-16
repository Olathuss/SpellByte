#ifndef GRAPH_LOADER_H
#define GRAPH_LOADER_H

#include "Graph.h"
#include <utilities/tinyxml2.h>

/*!
    This class is meant to load the graph and also offer graphical representation
    within the world.
*/

namespace SpellByte {
    class World;
    namespace Graph {
        class GraphLoader {
        private:
            Graph *WorldGraph;

            Ogre::SceneNode *graphNode;

            World *worldPtr;

            bool showNodeMarkers;

            std::vector<Ogre::ManualObject*> manualObjects;
            std::vector<Ogre::String> materialNames;

        public:
            GraphLoader(World* world, bool showMarkers = false);
            ~GraphLoader();

            void enableNodeMarkers() {
                showNodeMarkers = true;
            }
            bool getNodeVisibility() { return showNodeMarkers; }

            void disableNodeMarkers() {
                showNodeMarkers = false;
            };

            void loadGraph(Graph *graph, tinyxml2::XMLElement *xmlGraph);

            void clearGraph();
        };
    }
}

#endif // GRAPH_LOADER_H
