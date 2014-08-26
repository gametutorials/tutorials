#ifndef MODEL_H
#define MODEL_H

// Allow using of the "unsafe" versions of the standard C-library
// Search on this define to learn more about the topic
#define _CRT_SECURE_NO_DEPRECATE

#include <list>
#include "d3d_mesh.h"

using namespace std;

class CModel
{
	public:
	
		CModel() { /* do nothing */ }
	
		bool load3DS(const char *fileName);
		
		void render();
		
		// Destructor
		~CModel() { freeMem(); }
	
	private:
	
		list<CD3DMesh*> mMeshes;
		
		// Once the data is parsed from the 3DS file, this method
		// takes that data and places it in the mesh list
		bool buildFrom3DS();
		
		// Frees up all the memory in the model
		void freeMem();
	
};

#endif