/*
 * SpellByte Engine
 *
 * Main Entry Point
 *
 */

#include "SpellByte.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif // OGRE_PLATFORM

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef WIN32_LEAN_AND_MEAN
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif // WIN32_LEAN_AND_MEAN
    {
#ifdef WIN32_LEAN_AND_MEAN
        /*const char UniqueNamedMutex[] = "SpellByteApp";
        HANDLE hHandle = CreateMutex( NULL, TRUE, UniqueNamedMutex );
        if( ERROR_ALREADY_EXISTS == GetLastError() )
        {
            // Program already exists.
            return(1);
        }*/
#endif // WIN32_LEAN_AND_MEAN

        SpellByte::Application Game;

        try {
            Game.execute();
        } catch(Ogre::Exception &e) {

#ifdef WIN32_LEAN_AND_MEAN
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                        e.getFullDescription().c_str() << std::endl;
#endif // WIN32_LEAN_AND_MEAN
        } catch(CEGUI::Exception &e) {
#ifdef WIN32_LEAN_AND_MEAN
            MessageBox( NULL, e.getMessage().c_str(), "An exception has occured", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                        e.getMessage().c_str() << std::endl;
#endif // WIN32_LEAN_AND_MEAN
        } catch(std::exception &e) {
#ifdef WIN32_LEAN_AND_MEAN
            MessageBox( NULL, e.what(), "An exception has occured", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                        e.what()) << std::endl;
#endif // WIN32_LEAN_AND_MEAN
        }

#ifdef WIN32_LEAN_AND_MEAN
        //ReleaseMutex(hHandle);
        //CloseHandle(hHandle);
#endif // WIN32_LEAN_AND_MEAN

        return 0;
    }
#ifdef __cplusplus
}
#endif // __cplusplus
