#ifndef MODEL_DEFINES_H
#define MODEL_DEFINES_H

#include <vector>
#include <list>
using namespace std;

typedef unsigned short ushort;
typedef unsigned char uchar;

////////////////
// Constants //
//////////////

const int kMaxStringLen = 256;

// *** 3DS Constants ***
// The following are chunk defines for parsing data from the .3DS file

const ushort kBegin = 0x4D4D; // Primary chunk at the beginning of the file

const ushort kObject = 0x3D3D; // Marks the beginning of an object
const ushort kMaterial = 0xAFFF; // Marks the beginning of a material
const ushort kKeyFrame = 0xB000; // Marks the beginning of keyframe data

const ushort kObjectName = 0x4000;
const ushort kTriMesh = 0x4100; // Marks the start of a triangle mesh
const ushort kVertList = 0x4110; // Marks the start of a list of vertex (x,y,z) data
const ushort kFaceList = 0x4120;
const ushort kFaceMaterialList = 0x4130;
const ushort kTexCoordList = 0x4140;
const ushort kTransformMatrix = 0x4160;

const ushort kMaterialName = 0xA000;
const ushort kMaterialTexture = 0xA200;
const ushort kMaterialTextureName = 0xA300;

// Miscellaneous chunk defines that we care about
const ushort kVersion = 0x0002; // Version of .3ds file
 
class CTexCoord
{
	public:

		CTexCoord() : u(0.0f), v(0.0f) { /* do nothing */ }
		CTexCoord(float U, float V) : u(U), v(V) { /* do nothing */ }

		CTexCoord operator -(const CTexCoord &tc) const
		{
			return CTexCoord(u - tc.u, v - tc.v);
		}

		float u;
		float v;
};

class CColor
{
	public:
		
		CColor() : r(0), g(0), b(0) { /* do nothing */ }
		CColor(uchar R, uchar G, uchar B) : r(R), g(G), b(B) { /* do nothing */ }
		
		uchar r, g, b;
};

// This holds the chunk info
#pragma pack(push)
#pragma pack(1)
struct SChunk
{
	unsigned short id;
	unsigned int length; 
};
#pragma pack(pop)

// A SFace contains the three triangle indices and a flag
// whether the triangle is visible or not
struct SFace
{
	ushort indices[3];
	ushort visible;
};

// A SMeshObject contains vectors of the vertices, texture coordinates and
// faces that make up a mesh.  A SMeshObjec also contains a material index which
// is a index to the array of material objects corresponding to the material used for
// this mesh object
struct SMeshObject
{
	vector<CVector> verts;
	vector<CTexCoord> texCoords;
	vector<SFace> faces;
	
	int materialIndex;
};

struct SMaterialObject
{
	char  name[kMaxStringLen]; // The texture name
	char  texFileName[kMaxStringLen]; // The texture file name (If this is set it's a texture map)
	
	CColor ambient;
	CColor diffuse;
	CColor specular;
};

#endif