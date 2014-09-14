#ifndef MESH_PREPROCESS_H
#define MESH_PREPROCESS_H

#include <string>

/*
 * Batch process all the mesh names passed in
 */
void batch_process(char **argv, int argc);

/*
 * Preprocess a mesh by loading the obj file into a model and
 * serializing in a binary format that can be read in much faster later
 * The binary model data will be written to the same file name passed in
 * but with the extension bobj.
 *
 * The binary model format produced will contain:
 * uint32: number of vertices
 * uint32: number of triangles
 * [float]: 3 * num verts positions
 * [float]: 3 * num verts texcoords
 * [float]: 3 * num verts normals
 * [int]: num tris indices
 */
bool process_wobj(const std::string &file);

#endif

