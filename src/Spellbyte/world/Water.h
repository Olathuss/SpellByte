
/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/


#ifndef _WATER_H_
#define _WATER_H_

#include <iostream>
#include <OgreBillboardParticleRenderer.h>

//#include "SdkSample.h"

#include "WaterMesh.h"

using namespace Ogre;
//using namespace OgreBites;

// Mesh stuff
#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_PREFIX "Water"
#define MATERIAL_NAME "Water0"
#define COMPLEXITY 32 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 3000.0f
#define CIRCLES_MATERIAL "Examples/Water/Circles"

namespace SpellByte {
    /* =========================================================================*/
    /*               WaterCircle class                                          */
    /* =========================================================================*/
    #define CIRCLE_SIZE 500.0
    #define CIRCLE_TIME 0.5f
    class WaterCircle
    {
    private:
        String name ;
        SceneNode *node ;
        MeshPtr mesh ;
        SubMesh *subMesh ;
        Entity *entity ;
        Real tm ;
        SceneManager *sceneMgr ;
        static bool first ;
        // some buffers shared by all circles
        static HardwareVertexBufferSharedPtr posnormVertexBuffer;
        static HardwareIndexBufferSharedPtr indexBuffer; // indices for 2 faces
        static HardwareVertexBufferSharedPtr *texcoordsVertexBuffers;

        float *texBufData;
        void _prepareMesh()
        {
            int i,texLvl ;

            mesh = MeshManager::getSingleton().createManual(name,
                                                            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) ;
            subMesh = mesh->createSubMesh();
            subMesh->useSharedVertices=false;

            int numVertices = 4 ;

            if (first) { // first Circle, create some static common data
                first = false ;

                // static buffer for position and normals
                posnormVertexBuffer =
                HardwareBufferManager::getSingleton().createVertexBuffer(
                                                                         6*sizeof(float), // size of one vertex data
                                                                         4, // number of vertices
                                                                         HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
                                                                         false); // no shadow buffer
                float *posnormBufData = (float*) posnormVertexBuffer->
                lock(HardwareBuffer::HBL_DISCARD);
                for(i=0;i<numVertices;i++) {
                    posnormBufData[6*i+0]=((Real)(i%2)-0.5f)*CIRCLE_SIZE; // pos X
                    posnormBufData[6*i+1]=0; // pos Y
                    posnormBufData[6*i+2]=((Real)(i/2)-0.5f)*CIRCLE_SIZE; // pos Z
                    posnormBufData[6*i+3]=0 ; // normal X
                    posnormBufData[6*i+4]=1 ; // normal Y
                    posnormBufData[6*i+5]=0 ; // normal Z
                }
                posnormVertexBuffer->unlock();

                // static buffers for 16 sets of texture coordinates
                texcoordsVertexBuffers = new HardwareVertexBufferSharedPtr[16];
                for(texLvl=0;texLvl<16;texLvl++) {
                    texcoordsVertexBuffers[texLvl] =
                    HardwareBufferManager::getSingleton().createVertexBuffer(
                                                                             2*sizeof(float), // size of one vertex data
                                                                             numVertices, // number of vertices
                                                                             HardwareBuffer::HBU_STATIC_WRITE_ONLY, // usage
                                                                             false); // no shadow buffer
                    float *texcoordsBufData = (float*) texcoordsVertexBuffers[texLvl]->
                    lock(HardwareBuffer::HBL_DISCARD);
                    float x0 = (Real)(texLvl % 4) * 0.25 ;
                    float y0 = (Real)(texLvl / 4) * 0.25 ;
                    y0 = 0.75-y0 ; // upside down
                    for(i=0;i<4;i++) {
                        texcoordsBufData[i*2 + 0]=
                        x0 + 0.25 * (Real)(i%2) ;
                        texcoordsBufData[i*2 + 1]=
                        y0 + 0.25 * (Real)(i/2) ;
                    }
                    texcoordsVertexBuffers[texLvl]->unlock();
                }

                // Index buffer for 2 faces
                unsigned short faces[6] = {2,1,0,  2,3,1};
                indexBuffer =
                HardwareBufferManager::getSingleton().createIndexBuffer(
                                                                        HardwareIndexBuffer::IT_16BIT,
                                                                        6,
                                                                        HardwareBuffer::HBU_STATIC_WRITE_ONLY);
                indexBuffer->writeData(0,
                                       indexBuffer->getSizeInBytes(),
                                       faces,
                                       true); // true?
            }

            // Initialize vertex data
            subMesh->vertexData = new VertexData();
            subMesh->vertexData->vertexStart = 0;
            subMesh->vertexData->vertexCount = 4;
            // first, set vertex buffer bindings
            VertexBufferBinding *vbind = subMesh->vertexData->vertexBufferBinding ;
            vbind->setBinding(0, posnormVertexBuffer);
            vbind->setBinding(1, texcoordsVertexBuffers[0]);
            // now, set vertex buffer declaration
            VertexDeclaration *vdecl = subMesh->vertexData->vertexDeclaration ;
            vdecl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
            vdecl->addElement(0, 3*sizeof(float), VET_FLOAT3, VES_NORMAL);
            vdecl->addElement(1, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES);

            // Initialize index data
            subMesh->indexData->indexBuffer = indexBuffer;
            subMesh->indexData->indexStart = 0;
            subMesh->indexData->indexCount = 6;

            // set mesh bounds
            AxisAlignedBox circleBounds(-CIRCLE_SIZE/2.0f, 0, -CIRCLE_SIZE/2.0f,
                                        CIRCLE_SIZE/2.0f, 0, CIRCLE_SIZE/2.0f);
            mesh->_setBounds(circleBounds);
            mesh->load();
            mesh->touch();
        }
    public:
        int lvl ;
        void setTextureLevel()
        {
            subMesh->vertexData->vertexBufferBinding->setBinding(1, texcoordsVertexBuffers[lvl]);
        }
        WaterCircle(SceneManager *mgr, const String& inName, Real x, Real y)
        {
            sceneMgr = mgr;
            name = inName ;
            _prepareMesh();
            node = static_cast<SceneNode*> (sceneMgr->getRootSceneNode()->createChild(name));
            node->translate(x*(PLANE_SIZE/COMPLEXITY), 10, y*(PLANE_SIZE/COMPLEXITY));
            entity = sceneMgr->createEntity(name, name);
            entity->setMaterialName(CIRCLES_MATERIAL);
            node->attachObject(entity);
            tm = 0 ;
            lvl = 0 ;
            setTextureLevel();
        }
        ~WaterCircle()
        {
            MeshManager::getSingleton().remove(mesh->getHandle());
            sceneMgr->destroyEntity(entity->getName());
            static_cast<SceneNode*> (sceneMgr->getRootSceneNode())->removeChild(node->getName());
        }
        void animate(Real timeSinceLastFrame)
        {
            int lastlvl = lvl ;
            tm += timeSinceLastFrame ;
            lvl = (int) ( (Real)(tm)/CIRCLE_TIME * 16 );
            if (lvl<16 && lvl!=lastlvl) {
                setTextureLevel();
            }
        }
        static void clearStaticBuffers()
        {
            posnormVertexBuffer = HardwareVertexBufferSharedPtr() ;
            indexBuffer = HardwareIndexBufferSharedPtr() ;
            for(int i=0;i<16;i++) {
                texcoordsVertexBuffers[i] = HardwareVertexBufferSharedPtr() ;
            }
            delete [] texcoordsVertexBuffers;
        }
    };
}

#endif
