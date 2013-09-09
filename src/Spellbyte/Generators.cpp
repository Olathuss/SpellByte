/*
 * Cube27
 * 2013 (C) Knightforge Studios(TM)
 *
 * Cube Generator Reference List
 *
 * These functions are used in generating random cubes
 *
 */

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "generators.h"

namespace SpellByte
{
    std::vector<std::pair<std::string,int> > generateBasicRandomCube(int size) // Size must be from 3-5
    {
        //Establish Cube Settings based on the Cube Size
        std::string color_list[6] = {"blue","green","red","orange","yellow","purple"};
        std::vector<std::pair<std::string,int> > t_list;
        std::vector<std::pair<std::string,int> > cube_list;
        int colors = (size * size * size) / 20;
        int solids = (size * size * size) - (colors * 20);

        //Populate a Temp List with all of the Cubes needed for the Random Cube
        for(int curr_color = 0; curr_color < colors; curr_color++)
        {
            std::string color = color_list[curr_color];
            for(int number = 0; number < 10; number++)
            {
                std::pair<std::string,int> cube(color,number);
                t_list.push_back(cube);
                t_list.push_back(cube);
            }
        }

        for(int curr_solid = 0; curr_solid < solids; curr_solid++)
        {
            std::pair<std::string,int> cube("solid",0);
            t_list.push_back(cube);
        }

        // Randomize the Temp List and Store the cubes in the Cube List
        while(!t_list.empty())
        {
            int cube_number = rand() % t_list.size();
            cube_list.push_back(t_list[cube_number]);
            t_list.erase(t_list.begin()+cube_number);
        }

        return cube_list;

    }
}
