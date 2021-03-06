//
//  Deformabel Simplicial Complex (DSC) method
//  Copyright (C) 2013  Technical University of Denmark
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  See licence.txt for a copy of the GNU General Public License.

#include "mesh_io.h"
#include "tetralizer.h"

using namespace std;

#ifdef _WIN32
const std::string file_path = "@PROJECT_SOURCE_DIR@/data/";
#else
const std::string file_path = "./data/";
#endif
const string extension = ".dsc";

void generate_from_obj(const string& input_file_name, const string& output_file_name)
{
    vector<vec3> points;
    vector<int> tets;
    vector<int> tet_labels;
    
    std::vector<vec3> points_interface;
    std::vector<int> faces_interface;
    is_mesh::import_surface_mesh(file_path + input_file_name, points_interface, faces_interface);
    
    Tetralizer::tetralize(vec3(3.), 0.5, points_interface, faces_interface, points, tets, tet_labels);
    
    is_mesh::export_tet_mesh(file_path + output_file_name + extension, points, tets, tet_labels);
}

int main(int argc, const char * argv[])
{
    if(argc > 2)
    {
        string output_file_name = string(argv[1]);
        string input_file_name = string(argv[2]);
        generate_from_obj(input_file_name, output_file_name);
        
        std::cout << "Generated " << output_file_name + extension << std::endl;
    }
    
    return 0;
}

