
#ifndef __loader_hpp__
#define __loader_hpp__

class CLoader
{
public:
	virtual void Load(char*) = 0;
};

////////////////////////////////////////////////////////////////////

class CLoader3DS : public CLoader
{

private:


	enum {
		FLOAT_RGB_CHUNK		=	0x0010,
		BYTE_RGB_CHUNK		=	0x0011,

		PERCENT_INT_CHUNK	=	0x0030,
		PERCENT_FLOAT_CHUNK	=	0x0031,

		MAIN3DS			=	0x4D4D,
		VERSION3DS		=	0x0002,
		EDIT3DS			=	0x3D3D,
		KEYF3DS			=	0xB000,
		OBJECT_BLOCK		=	0x4000,

		OBJ_TRIMESH		=	0x4100,
		OBJ_LIGHT		=	0x4600,
		OBJ_CAMERA		=	0x4700,


		TRI_VERTEXL		=	0x4110,
		TRI_FACES		=	0x4120,
		TRI_MAPPING_COORDS	=	0x4140,
		TRI_LOCAL_COORDS	=	0x4160,
		FACES_MATLIST		=	0x4130,

		LIT_SPOTLIGHT		=	0x4610,

		EDIT_MATERIAL		=	0xAFFF,

		MATERIAL_NAME		=	0xA000,
		MAT_AMBIENT_COLOR	=	0xA010,
		MAT_DIFFUSE_COLOR	=	0xA020,
		MAT_SPECULAR_COLOR	=	0xA030,
		MAT_SHININESS		=	0xA040,
		MAT_TRANSPARENCY	=	0xA050,

		TEXTURE_MAP1		=	0xA200,
		TEXTURE_MAP2		=	0xA33A,

		MAP_FILENAME		=	0xA300,
		MAP_VSCALE		=	0xA354,
		MAP_USCALE		=	0xA356,
		MAP_UOFFSET		=	0xA358,
		MAP_VOFFSET		=	0xA35A,
		MAP_ROTANGLE		=	0xA35C,

		KEYF_AMBIENT		=	0xB001,
		KEYF_MESH		=	0xB002,
		KEYF_CAMERA		=	0xB003,
		KEYF_CAMERA_TARGET	=	0xB004,
		KEYF_OMNILIGHT		=	0xB005,
		KEYF_SPOT_TARGET	=	0xB006,
		KEYF_SPOT		=	0xB007,

		KEYF_FRAMES		=	0xB008,
		KEYF_OBJ_NAME		=	0xB010,
		
		KEYF_DUMMY_NAME		=	0xB011, 

		KEYF_OBJ_PIVOT		=	0xB013,

		KEYF_POSITION_TRACK	=	0xB020,
		KEYF_ROTATION_TRACK	=	0xB021,
		KEYF_SCALE_TRACK	=	0xB022,
		KEYF_FOV_TRACK		=	0xB023,
		KEYF_ROLL_TRACK		=	0xB024,
		KEYF_COLOR_TRACK	=	0xB025,
		KEYF_MORPH_TRACK	=	0xB026,
		KEYF_HOTSPOT_TRACK	=	0xB027,
		KEYF_FALLOFF_TRACK	=	0xB028,
		KEYF_HIDE_TRACK		=	0xB029,

		KEYF_HIERARCHY		=	0xB030
	};


	float LensToFOV(float);

	/// poni¿sze metody do wczytywania poszczególnych chunków:
	BYTE ReadByte();
	WORD ReadWord();
	DWORD ReadDWord();
	float ReadFloat();
	char *ReadASCIIZ();

	DWORD ReadVersion();
	CVector3 ReadVector();

	SColor ReadByteRGB();
	SColor ReadFloatRGB();

	WORD ReadPercentInt();
	float ReadPercentFloat();

	void ReadVertexListChunk(CMesh*);
	void ReadFacesListChunk(CMesh*);
	void ReadLocalCoords(CMesh*);

	void ReadMappingCoords(CMesh*);

	CMesh* ReadTriMeshChunk();
	void ReadCameraChunk(CCamera*&, CCameraTarget*&);
	void ReadLightChunk(CLight*&, CLightTarget*&);

	void ReadObjectChunk();

	void ReadMapChunk(CMaterial::SMap*&);

	/// mo¿e wczytywaæ ambient, diffuse i specular:
	void ReadMaterialColor(SColor&);
	float ReadShininessChunk();
	float ReadTransparencyChunk();
	void ReadMaterialChunk();


	void ReadEditor();

	void ReadKeyfFrames();
	void ReadPivotChunk(CKeyframer*&);
	void ReadObjectName(CKeyframer*&, char*&);
	void ReadHierarchyPos(CKeyframer*&);
	void ReadTrack(CTrack*, WORD);

	/// dla poszczególnych obiektów:
	void ReadKeyframerDataChunk();
	void ReadKeyframerCameraTargetChunk();
	void ReadKeyframer();

	void SkipChunk();

	/// referencja do sceny, która ten loader obs³uguje
	CScene&		m_refScene;
	FILE*		m_File;

	int		m_nLightsCounter;

public:

	CLoader3DS(CScene& ref_scene) :
		m_refScene(ref_scene),
		m_File(NULL),
		m_nLightsCounter(0)
	{
	}

	virtual ~CLoader3DS() 
	{ 

	}



	virtual void Load(char*);
};

////////////////////////////////////////////////////////////////////

class CLoaderASE : public CLoader
{
public:
	virtual void Load(char*);
};

#endif