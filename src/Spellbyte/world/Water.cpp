#include "Water.h"

namespace SpellByte {
    bool WaterCircle::first = true ;
    HardwareVertexBufferSharedPtr WaterCircle::posnormVertexBuffer =
    HardwareVertexBufferSharedPtr() ;
    HardwareIndexBufferSharedPtr WaterCircle::indexBuffer =
    HardwareIndexBufferSharedPtr() ;
    HardwareVertexBufferSharedPtr* WaterCircle::texcoordsVertexBuffers = 0 ;
}
