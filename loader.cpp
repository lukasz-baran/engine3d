
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

//#define __logging__ 1

/// przeksztalca soczewke (zamiast FOV w 3ds przechowywana jest inforamcja o
/// ogniskowej soczewki kamery) na rozpietosc otroslupa widzenia (FOV)
float CLoader3DS::LensToFOV(float lens)
{
	int  		i;
	float 		fov;

	struct { float lens, fov; } lens_table[] = {
		{15.0f,  115.0f},
		{20.0f, 94.28571f},
		{24.0f, 84.0f},
		{28.0f,  76.36364f},
		{35.0f,  63.0f},
		{50.0f, 46.0f},
		{85.0f, 28.0f},
		{135.0f, 18.0f},
		{200.0f, 12.0f} };

	for (i = 0; i < 9; i++)
		if (lens == lens_table[i].lens)
			fov = lens_table[i].fov;
		else
			fov = 15.0f / lens * 160.0f;

	return fov;
}

////////////////////////////////////////////////////////////////////

SColor CLoader3DS::ReadByteRGB()
{
	SColor	ret;
	ReadDWord();

	ret.rgba[0] = ReadByte();
	ret.rgba[1] = ReadByte();
	ret.rgba[2] = ReadByte();

	return ret;
}

////////////////////////////////////////////////////////////////////

SColor CLoader3DS::ReadFloatRGB()
{
	SColor	ret;
	ReadDWord();

	ret.rgba[0] = ReadFloat();
	ret.rgba[1] = ReadFloat();
	ret.rgba[2] = ReadFloat();

	return ret;
}

////////////////////////////////////////////////////////////////////

WORD CLoader3DS::ReadPercentInt()
{
	ReadDWord();
	return ReadWord();
}

////////////////////////////////////////////////////////////////////

float CLoader3DS::ReadPercentFloat()
{
	ReadDWord();
	return ReadFloat();
}

////////////////////////////////////////////////////////////////////

BYTE CLoader3DS::ReadByte()
{
	BYTE	b;
	fread(&b, 1, 1, m_File);
	return b;
}

////////////////////////////////////////////////////////////////////

WORD CLoader3DS::ReadWord()
{
	WORD	temp;
	fread(&temp, sizeof(WORD), 1, m_File);

	return temp;
}

////////////////////////////////////////////////////////////////////

DWORD CLoader3DS::ReadDWord()
{
	DWORD	temp;
	fread(&temp, sizeof(DWORD), 1, m_File);
	return temp;
}

////////////////////////////////////////////////////////////////////

float CLoader3DS::ReadFloat()
{
	float	temp;
	fread(&temp, sizeof(float), 1, m_File);
	return temp;
}

////////////////////////////////////////////////////////////////////

char *CLoader3DS::ReadASCIIZ()
{
	/// TODO: Ktoœ móg³by siê ruszyæ i pomyœleæ nad tym jak
	/// 	  uproœciæ t¹ funkcjê... :)
	DWORD	fpos = ftell(m_File);
	char	ch;
	int	namelen = 0;
	char	*ret = NULL;

	do {
		fread(&ch, 1, 1, m_File);
		namelen++;
	} while (ch);

	ret = new char[namelen];
	fseek(m_File, fpos, SEEK_SET);
	fread(ret, namelen, 1, m_File);
	return ret;
}

////////////////////////////////////////////////////////////////////

/// zamienia wspó³rzêdne 'y' i 'x' w wektorze
CVector3 CLoader3DS::ReadVector()
{
	float	x, y, z;
	x = ReadFloat();
	y = ReadFloat();
	z = ReadFloat();
	return CVector3(x, z, y);
}

////////////////////////////////////////////////////////////////////

DWORD CLoader3DS::ReadVersion()
{
	ReadDWord();
	DWORD ret = ReadDWord();

	if (feof(m_File))
		throw CException(error_bad_file);

	return ret;
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadVertexListChunk(CMesh *t)
{
	DWORD	len = ReadDWord() - 6-2;
	t->m_nVertices = (DWORD)ReadWord();

	t->m_pVertices = new CMesh::SVertex[t->m_nVertices];
	ZeroMemory(t->m_pVertices, t->m_nVertices * sizeof(CMesh::SVertex));

	for (DWORD i = 0; i < t->m_nVertices; i++)
		t->m_pVertices[i].p = ReadVector();
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadFacesListChunk(CMesh *t)
{
	DWORD		id, len = ReadDWord() - 6;

	t->m_nIndices = (DWORD)ReadWord();
	len -= 2;

	t->m_pIndices = new CMesh::STriangle[t->m_nIndices];
	ZeroMemory(t->m_pIndices, t->m_nIndices * sizeof(CMesh::STriangle));

	for (DWORD i = 0; i < t->m_nIndices; i++) {
		t->m_pIndices[i].a = (DWORD)ReadWord();
		t->m_pIndices[i].b = (DWORD)ReadWord();
		t->m_pIndices[i].c = (DWORD)ReadWord();
		t->m_pIndices[i].flag = ReadWord();
		len -= (4*sizeof(WORD));
	}

	if (feof(m_File))
		throw CException(error_bad_file);

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);


		switch (id) {


		case FACES_MATLIST: {
			ReadDWord();

			char *matname = ReadASCIIZ();

			CMaterial *pmat = m_refScene.MaterialList.Get(matname);
			if (!pmat) {
				throw CException("Nie wczytano wszystkich materialow!");
			}

			DWORD entries = (DWORD)ReadWord();

			for (DWORD i = 0; i < entries; i++) {
				DWORD numer = (DWORD)ReadWord();
				t->m_pIndices[numer].pmat = pmat;
			}

			DELETE_STRING(matname);
			}

			break;

		/// TODO:
		///	dopisz wczytywanie:
		///		Smoothing groups list (0x4150)

		default:
			SkipChunk();
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////

/// ta funkcja musi tak¿e przekszta³ciæ wspó³rzêdne z uk³adu œwiata
/// do uk³adu lokalnego obiektu
void CLoader3DS::ReadLocalCoords(CMesh *t)
{
	CVector3	Origin;
	DWORD		i, j;
	float		scale, x, y, z;
	float		M[3][3], L[3][3];
	float		tmp;

	ReadDWord();
	for (i = 0; i < 3; i++)
		for (j = 0; j <  3; j++)
			M[i][j] = ReadFloat();	

	for (i = 0; i < 3; i++) {
		tmp = M[i][1];
		M[i][1] = M[i][2];
		M[i][2] = tmp;
	}

	for (i = 0; i < 3; i++) {
		tmp = M[1][i];
		M[1][i] = M[2][i];
		M[2][i] = tmp;
	}
	
	for (i = 0; i < 3; i++) {
		scale = MATH3D_SQR(M[i][0]) + MATH3D_SQR(M[i][1]) + MATH3D_SQR(M[i][2]);
		if (scale == 0.0f) 
			scale = 1.0f;

		scale = 1.0f/scale;

		for (j = 0; j < 3; j++)
			L[i][j] = M[i][j] * scale;		
	}

	Origin = ReadVector();
	for (i = 0; i < t->m_nVertices; i++) {
		t->m_pVertices[i].p -= Origin;

		x = t->m_pVertices[i].p.x*L[0][0] + 
		    t->m_pVertices[i].p.y*L[0][1] + 
		    t->m_pVertices[i].p.z*L[0][2];

		y = t->m_pVertices[i].p.x*L[1][0] + 
		    t->m_pVertices[i].p.y*L[1][1] + 
		    t->m_pVertices[i].p.z*L[1][2];

		z = t->m_pVertices[i].p.x*L[2][0] + 
		    t->m_pVertices[i].p.y*L[2][1] + 
		    t->m_pVertices[i].p.z*L[2][2];		

		t->m_pVertices[i].p.x = x;
		t->m_pVertices[i].p.y = y;
		t->m_pVertices[i].p.z = z;
	}
	t->Origin = Origin;
	t->X1 = CVector3(M[0][0], M[0][1], M[0][2]);
	t->X2 = CVector3(M[1][0], M[1][1], M[1][2]);
	t->X3 = CVector3(M[2][0], M[2][1], M[2][2]);
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadMappingCoords(CMesh *t)
{
	ReadDWord();

	DWORD	number = (DWORD)ReadWord();
	for (DWORD i = 0; (!feof(m_File)) && i < number; i++) {
		t->m_pVertices[i].u = ReadFloat();
		t->m_pVertices[i].v = ReadFloat();
	}

	if (feof(m_File))
		throw CException(error_bad_file);
}

////////////////////////////////////////////////////////////////////

CMesh *CLoader3DS::ReadTriMeshChunk()
{
	DWORD		id, len = ReadDWord() - 6;

	CMesh		*trimesh = new CMesh;

	if (!trimesh)
		throw CException(error_memory);

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File) - 4, SEEK_SET);
		switch (id) {

			case TRI_VERTEXL:
				ReadVertexListChunk(trimesh);
				break;

			case TRI_FACES:
				/// wiadomo do jakiej sceny siê odwo³ujemy:
				ReadFacesListChunk(trimesh);
				break;

			case TRI_LOCAL_COORDS:
				ReadLocalCoords(trimesh);
				break;

			case TRI_MAPPING_COORDS:
				ReadMappingCoords(trimesh);
				break;

			default:
				SkipChunk();
				break;
		}
	}

	return trimesh;
}

////////////////////////////////////////////////////////////////////

///	obj1 - camera (wypelniono juz "name")
///	obj2 - target
void CLoader3DS::ReadCameraChunk(CCamera *&pcamera, CCameraTarget *&pcamtarget)
{
	pcamera		= new CCamera;
	pcamtarget	= new CCameraTarget;

	if ((!pcamera) || (!pcamtarget))
		throw CException(error_memory);

	DWORD	pos = ftell(m_File);
	ReadDWord();	/// size

	pcamera->Position = ReadVector();
	pcamtarget->Target = ReadVector();

	pcamera->roll = ReadFloat();

	float lens = ReadFloat();
	pcamera->FOV = LensToFOV(lens);

	/// skip the whole chunk:
	fseek(m_File, pos, SEEK_SET);
	SkipChunk();
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadLightChunk(CLight*& light, CLightTarget*& target)
{
	DWORD		id, len = ReadDWord()-6;
	CLight*		plight = new CLight;
	CLightTarget*	ptarget = NULL;
	
	if (!plight) throw CException(error_memory);

	plight->Position = ReadVector();
	len -= 12;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {
		case FLOAT_RGB_CHUNK: 
			plight->Color = ReadFloatRGB();
			break;

		case BYTE_RGB_CHUNK:
			plight->Color = ReadByteRGB();
			break;

		case LIT_SPOTLIGHT: {
			ptarget = new CLightTarget;
			if (!ptarget) throw CException(error_memory);
			DWORD pos = ftell(m_File);
			ReadDWord();
			ptarget->Target = ReadVector();
			fseek(m_File, pos, SEEK_SET);
			SkipChunk();
			} break;

		default:
			SkipChunk();
			break;
		}
	}
	
	light = plight;
	target = ptarget;
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadObjectChunk()
{
	DWORD		id, len = ReadDWord() - 6;

	char		*name = ReadASCIIZ();

	len -= (strlen(name) + 1);
	while (len) {

		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);
		switch (id) {

			case OBJ_TRIMESH: {
				CMesh *pmesh = ReadTriMeshChunk();

				pmesh->CalcNormals();

				m_refScene.AddObject(pmesh, name);

				#ifdef __logging__
				REPORT << "..... Mesh loaded: " << name << endl;
				REPORT.flush();
				#endif

				}
				break;

			case OBJ_CAMERA: {
				CCamera		*pcamera = NULL;
				CCameraTarget	*pcamtarget = NULL;

				ReadCameraChunk(pcamera, pcamtarget);

				char		*target_name = NULL;
				target_name = new char[strlen(name) + 10];

				if (!target_name)
					throw CException(error_memory);

				strcpy(target_name, name);
				strcat(target_name, ".TARGET");

				#ifdef __logging__
				REPORT << "..... Camera loaded: " << name << endl;
				REPORT << "      position = "<<pcamera->Position<<endl;
				REPORT << "      target   = "<<pcamtarget->Target<<endl;
				REPORT << "      roll     = "<<pcamera->roll<<endl;
				REPORT.flush();
				#endif

				m_refScene.AddObject(pcamera, name);
				m_refScene.AddObject(pcamtarget, target_name);
				}
				break;

			case OBJ_LIGHT: {
				CLight		*plight = NULL;
				CLightTarget	*plighttarget = NULL;

				ReadLightChunk(plight, plighttarget);

				plight->number = m_nLightsCounter++;

				m_refScene.AddObject(plight, name);
				if (plighttarget != NULL) {
					char *target_name = NULL;
					target_name = new char[strlen(name) + 10];
					if (!target_name) throw CException(error_memory);
					strcpy(target_name, name);
					strcat(target_name, ".TARGET");
					m_refScene.AddObject(plighttarget, target_name);
				}

				}
				break;

			default:
				SkipChunk();
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadMapChunk(CMaterial::SMap *&map)
{
	DWORD	id, len = ReadDWord() - 6;

	map = new CMaterial::SMap;
	ZeroMemory(map, sizeof(CMaterial::SMap));

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

		case MAP_FILENAME:
			ReadDWord();
			map->name = ReadASCIIZ();
			break;

		case MAP_VSCALE:
			ReadDWord();
			map->Vscale = ReadFloat();
			break;

		case MAP_USCALE:
			ReadDWord();
			map->Uscale = ReadFloat();
			break;

		case MAP_UOFFSET:
			ReadDWord();
			map->Uoffset = ReadFloat();
			break;

		case MAP_VOFFSET:
			ReadDWord();
			map->Voffset = ReadFloat();
			break;

		case MAP_ROTANGLE:
			ReadDWord();
			map->rot_angle = ReadFloat();
			break;

		default:
			SkipChunk();
			break;
		}

	}
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadMaterialColor(SColor &col)
{
	DWORD		id, len = ReadDWord() - 6;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

		case BYTE_RGB_CHUNK:
			col = ReadByteRGB();
			break;

		case FLOAT_RGB_CHUNK:
			col = ReadFloatRGB();
			break;

		default:
			SkipChunk();
			break;
		}
	}

}

////////////////////////////////////////////////////////////////////

float CLoader3DS::ReadShininessChunk()
{
	DWORD		id, len = ReadDWord() - 6;

	float		ret_val;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

		case PERCENT_INT_CHUNK:
			ret_val = (float)ReadPercentInt();
			break;

		case PERCENT_FLOAT_CHUNK:
			ret_val = ReadPercentFloat();
			break;

		default:
			SkipChunk();
			break;
		}
	}
	return ret_val;
}

////////////////////////////////////////////////////////////////////

float CLoader3DS::ReadTransparencyChunk()
{
	DWORD		id, len = ReadDWord() - 6;

	float		ret_val;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

		case PERCENT_INT_CHUNK:
			ret_val = ReadPercentInt();
			break;

		case PERCENT_FLOAT_CHUNK:
			ret_val = ReadPercentFloat();
			break;

		default:
			SkipChunk();
			break;
		}
	}
	return ret_val;
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadMaterialChunk()
{
	DWORD		id, len = ReadDWord() - 6;
	char		*material_name = NULL;

	CMaterial	*pmaterial = new CMaterial;

	if (!pmaterial)
		throw CException(error_memory);

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

		case MATERIAL_NAME:
			ReadDWord();
			material_name = ReadASCIIZ();
			break;

		case MAT_AMBIENT_COLOR:
			ReadMaterialColor(pmaterial->ambient);
			break;

		case MAT_DIFFUSE_COLOR:
			ReadMaterialColor(pmaterial->diffuse);
			break;

		case MAT_SPECULAR_COLOR:
			ReadMaterialColor(pmaterial->specular);
			break;

		case MAT_SHININESS:
			pmaterial->m_fShininess = ReadShininessChunk();
			break;

		case MAT_TRANSPARENCY:
			pmaterial->m_fTransparency = ReadTransparencyChunk();
			break;

		case TEXTURE_MAP1:
			ReadMapChunk(pmaterial->text1);
			break;

		case TEXTURE_MAP2:
			ReadMapChunk(pmaterial->text2);
			break;

		default:
			SkipChunk();
			break;
		}
	}

	m_refScene.AddMaterial(pmaterial, material_name);

	#ifdef __logging__
	REPORT << "..... Material loaded: " << material_name << endl;
	REPORT.flush();
	#endif
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadEditor()
{
	DWORD	id, len = ReadDWord() - 6;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);
		switch (id) {

			case OBJECT_BLOCK:
				ReadObjectChunk();
				break;

			case EDIT_MATERIAL:
				ReadMaterialChunk();
				break;

			default:
				SkipChunk();
				break;
		}
	}
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadKeyfFrames()
{
	if (ReadDWord() != 14)
		throw CException(error_unknown_format);

	m_refScene.dwFrameStart = ReadDWord();
	m_refScene.dwFrameStop = ReadDWord();
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadPivotChunk(CKeyframer *&k)
{
	ReadDWord();
	k->Pivot = ReadVector();
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadObjectName(CKeyframer *&k, char *&name)
{
	ReadDWord();

	name = ReadASCIIZ();

	if (!name)
		throw CException(error_memory);

	/// tutaj s¹ jakieœ flagi, ale je pomijamy:
	ReadWord();
	ReadWord();

	/// identyfikator rodzica
	k->link = ReadWord();
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadHierarchyPos(CKeyframer *&k)
{
	ReadDWord();
	k->hierarchy = ReadWord();
}

////////////////////////////////////////////////////////////////////


/// poni¿sza funkcja mo¿e wczytywaæ kilka typów œcie¿ek:
///	track - pamiêæ musi byæ na to ju¿ przydzielona!
void CLoader3DS::ReadTrack(CTrack *track, WORD chunk_id)
{
	CMorphTrack	*morphtrk;
	CPositionTrack	*postrk;
	CScaleTrack	*scaletrk;
	CRotationTrack	*rottrk;
	
	DWORD		i;	
	DWORD		numOfKeys; 
	WORD		flag;
	WORD		b;
	int		hide = 0;

	ReadDWord();

	/// TODO: rozszyfrowaæ znaczenie tych flag:
	WORD		flag_tmp = ReadWord();

	/// nieznane:
	ReadDWord();
	ReadDWord();

	numOfKeys = ReadDWord();

	switch (chunk_id) {
	case KEYF_POSITION_TRACK: 
		postrk = (CPositionTrack*)track;
		postrk->Type = flag_tmp & 3;
		postrk->pKeys = new CVectorTrack::SVectorKey[numOfKeys];
		postrk->nKeys = numOfKeys;
		for (i = 0; i < numOfKeys; i++) {
			postrk->pKeys[i].frame = (float)ReadDWord();
			postrk->pKeys[i].T = 0.0f;
			postrk->pKeys[i].C = 0.0f;
			postrk->pKeys[i].B = 0.0f;
			postrk->pKeys[i].easeto = 0.0f;
			postrk->pKeys[i].easefrom = 0.0f;
			flag = ReadWord();
			for (b = 0; b < 16; b++) {
				if (flag & (1<<b))
					switch (b) {
					case 0: postrk->pKeys[i].T = ReadFloat(); break;
					case 1: postrk->pKeys[i].C = ReadFloat(); break;
					case 2: postrk->pKeys[i].B = ReadFloat(); break;
					case 4: postrk->pKeys[i].easefrom = ReadFloat(); break;
					case 3: postrk->pKeys[i].easeto = ReadFloat(); break;
					default: ReadFloat(); break;
					}
			}
			postrk->pKeys[i].vec = ReadVector();
		}
		break;

	case KEYF_SCALE_TRACK:
		scaletrk = (CScaleTrack*)track;
		scaletrk->Type = flag_tmp & 3;
		scaletrk->pKeys = new CVectorTrack::SVectorKey[numOfKeys];
		scaletrk->nKeys = numOfKeys;
		for (i = 0; i < numOfKeys; i++) {
			scaletrk->pKeys[i].frame = (float)ReadDWord();
			scaletrk->pKeys[i].T = 0.0f;
			scaletrk->pKeys[i].C = 0.0f;
			scaletrk->pKeys[i].B = 0.0f;
			scaletrk->pKeys[i].easeto = 0.0f;
			scaletrk->pKeys[i].easefrom = 0.0f;
			flag = ReadWord();
			for (b = 0; b < 16; b++) {
				if (flag & (1<<b))
					switch (b) {
					case 0: scaletrk->pKeys[i].T = ReadFloat(); break;
					case 1: scaletrk->pKeys[i].C = ReadFloat(); break;
					case 2: scaletrk->pKeys[i].B = ReadFloat(); break;
					case 4: scaletrk->pKeys[i].easefrom = ReadFloat(); break;
					case 3: scaletrk->pKeys[i].easeto = ReadFloat(); break;
					default: ReadFloat(); break;
					}
			}
			scaletrk->pKeys[i].vec = ReadVector();
		}
		break;

	case KEYF_ROTATION_TRACK: 
		rottrk = (CRotationTrack*)track;
		rottrk->Type = flag_tmp & 3;
		rottrk->pKeys = new CRotationTrack::SQuatKey[numOfKeys];
		rottrk->nKeys = numOfKeys;
		for (i = 0; i < numOfKeys; i++) {
			rottrk->pKeys[i].frame = (float)ReadDWord();
			rottrk->pKeys[i].T = 0.0f;
			rottrk->pKeys[i].C = 0.0f;
			rottrk->pKeys[i].B = 0.0f;
			rottrk->pKeys[i].easeto = 0.0f;
			rottrk->pKeys[i].easefrom = 0.0f;
			flag = ReadWord();

			for (b = 0; b < 16; b++) {
				if (flag & (1<<b))
					switch (b) {
					case 0: rottrk->pKeys[i].T = ReadFloat(); break;
					case 1: rottrk->pKeys[i].C = ReadFloat(); break;
					case 2: rottrk->pKeys[i].B = ReadFloat(); break;
					case 4: rottrk->pKeys[i].easefrom = ReadFloat(); break;
					case 3: rottrk->pKeys[i].easeto = ReadFloat(); break;
					default: ReadFloat(); break;
					}
			}
			rottrk->pKeys[i].angle = ReadFloat();
			rottrk->pKeys[i].axis.x = ReadFloat();
			rottrk->pKeys[i].axis.z = ReadFloat();
			rottrk->pKeys[i].axis.y = ReadFloat();
		}
		break;

	case KEYF_MORPH_TRACK:
		morphtrk = (CMorphTrack*)track;
		morphtrk->Type = flag_tmp & 3;
		morphtrk->pKeys = new CMorphTrack::SObjectKey[numOfKeys];
		morphtrk->nKeys = numOfKeys;
		for (i = 0; i < numOfKeys; i++) {
			morphtrk->pKeys[i].frame = (float)ReadDWord();
			morphtrk->pKeys[i].T = 0.0f;
			morphtrk->pKeys[i].C = 0.0f;
			morphtrk->pKeys[i].B = 0.0f;
			morphtrk->pKeys[i].easeto = 0.0f;
			morphtrk->pKeys[i].easefrom = 0.0f;
			flag = ReadWord();
			for (b = 0; b < 16; b++) {
				if (flag & (1<<b))
					switch (b) {
					case 0: morphtrk->pKeys[i].T = ReadFloat(); break;
					case 1: morphtrk->pKeys[i].C = ReadFloat(); break;
					case 2: morphtrk->pKeys[i].B = ReadFloat(); break;
					case 4: morphtrk->pKeys[i].easefrom = ReadFloat(); break;
					case 3: morphtrk->pKeys[i].easeto = ReadFloat(); break;
					default: ReadFloat(); break;
					}
			}
			/// tutaj konieczna jest konwersja na kwaternion:
			morphtrk->pKeys[i].name = ReadASCIIZ();
		}
		break;

	default:
		throw CException(error_unknown_format);
	}
}


////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadKeyframerDataChunk()
{
	DWORD		id;
	DWORD		len = ReadDWord() - 6;

	CKeyframer	*k = new CKeyframer;
	char		*name = NULL;
	char		*dummy_name = NULL;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

			case KEYF_OBJ_NAME:
				ReadObjectName(k, name);
				break;

			case KEYF_DUMMY_NAME: 
				ReadDWord(); /// rozmiar
				dummy_name = ReadASCIIZ();
				break;

			case KEYF_OBJ_PIVOT:
				ReadPivotChunk(k);
				break;

			case KEYF_HIERARCHY:
				ReadHierarchyPos(k);
				break;

			case KEYF_POSITION_TRACK:
				k->m_pPositionTrack = new CPositionTrack;
				ReadTrack(k->m_pPositionTrack, KEYF_POSITION_TRACK);
				break;

			case KEYF_ROTATION_TRACK:
				#ifdef __logging__
				REPORT<<"        wczytuje œcie¿kê rotacji "<<endl;
				REPORT.flush();
				#endif

				k->m_pRotationTrack = new CRotationTrack;
				ReadTrack((CTrack*)k->m_pRotationTrack, KEYF_ROTATION_TRACK);
				break;

			case KEYF_SCALE_TRACK:
				#ifdef __logging__
				REPORT<<"        wczytuje œcie¿kê skalowania "<<endl;
				REPORT.flush();
				#endif

				k->m_pScaleTrack = new CScaleTrack;
				ReadTrack((CTrack*)k->m_pScaleTrack, KEYF_SCALE_TRACK);
				break;

			case KEYF_MORPH_TRACK:
				k->m_pMorphTrack = new CMorphTrack;
				ReadTrack((CTrack*)k->m_pMorphTrack, KEYF_MORPH_TRACK);
				break;

			case KEYF_HIDE_TRACK:
				/// TODO: rozgryŸæ to!
				SkipChunk();
				break;

			default:
				SkipChunk();
				break;
		}

	}

	/// sprawdŸ czy keyframer jest dla obiektu '$$$DUMMY':
	if (strcmp(name, "$$$DUMMY") == 0) {
		C3DSDummy *dummy_object = new C3DSDummy;
		m_refScene.AddObject(dummy_object, dummy_name);
		m_refScene.AddKeyframer(k, dummy_name);
	} else 
		m_refScene.AddKeyframer(k, name); /// dodaj keyframer do sceny
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadKeyframerCameraTargetChunk()
{
	DWORD		id;
	DWORD		len = ReadDWord() - 6;

	CKeyframer	*k = new CKeyframer;
	char		*name = NULL;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);

		switch (id) {

			case KEYF_OBJ_NAME: {

				char *tempname = NULL;

				ReadDWord();
				tempname = ReadASCIIZ();
				name = new char[strlen(tempname) + 10];

				strcpy(name, tempname);
				strcat(name, ".TARGET");
				DELETE_STRING(tempname);

				/// tutaj s¹ jakieœ flagi, ale je pomijamy:
				ReadWord();
				ReadWord();

				/// identyfikator rodzica
				k->link = ReadWord();

				}

				break;

			case KEYF_OBJ_PIVOT:
				ReadPivotChunk(k);
				break;

			case KEYF_HIERARCHY:
				ReadHierarchyPos(k);
				break;

			case KEYF_POSITION_TRACK:
				k->m_pPositionTrack = new CPositionTrack;
				ReadTrack(k->m_pPositionTrack, KEYF_POSITION_TRACK);
				break;

			case KEYF_ROTATION_TRACK:
				k->m_pRotationTrack = new CRotationTrack;
				ReadTrack((CTrack*)k->m_pRotationTrack, KEYF_ROTATION_TRACK);
				break;

			case KEYF_SCALE_TRACK:
				k->m_pScaleTrack = new CScaleTrack;
				ReadTrack((CTrack*)k->m_pScaleTrack, KEYF_SCALE_TRACK);
				break;

			case KEYF_MORPH_TRACK:
				k->m_pMorphTrack = new CMorphTrack;
				ReadTrack((CTrack*)k->m_pMorphTrack, KEYF_MORPH_TRACK);
				break;

			case KEYF_HIDE_TRACK:
				/// TODO: rozgryŸæ to!
				SkipChunk();
				break;

			default:
				SkipChunk();
				break;
		}

	}

	/// dodaj keyframer do sceny:
	m_refScene.AddKeyframer(k, name);
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::ReadKeyframer()
{
	DWORD		id;
	DWORD		len = ReadDWord() - 6;

	while (len) {
		id = ReadWord();
		len -= ReadDWord();
		fseek(m_File, ftell(m_File)-4, SEEK_SET);
		switch (id) {

			case KEYF_MESH:
				//ReadMeshTracksChunk();
				ReadKeyframerDataChunk();
				break;

			case KEYF_CAMERA:
				ReadKeyframerDataChunk();
				break;

			case KEYF_CAMERA_TARGET:
				ReadKeyframerCameraTargetChunk();
				break;

			case KEYF_OMNILIGHT:
				//ReadLightTracksChunk();
				ReadKeyframerDataChunk();
				break;

			case KEYF_SPOT:
				ReadKeyframerDataChunk();
				break;

			case KEYF_SPOT_TARGET:
				ReadKeyframerCameraTargetChunk();
				break;

			case KEYF_FRAMES:
				ReadKeyfFrames();
				break;

			default:
				SkipChunk();
				break;
		}
	}

}

////////////////////////////////////////////////////////////////////

/// pomija chunk, na którym stoimy, tzn. wlasnie wczytalismy jego id
void CLoader3DS::SkipChunk()
{
	DWORD	len = ReadDWord();
	fseek(m_File, len - 6, SEEK_CUR);
}

////////////////////////////////////////////////////////////////////

void CLoader3DS::Load(char *_fname)
{
	m_refScene.Release();

	WORD	id;

	m_File = fopen(_fname, "r+b");
	if (m_File == NULL)
		throw CException(error_file_not_found);

	id = ReadWord();
	if (id != MAIN3DS) {
		fclose(m_File);
		throw CException(error_bad_file);
	}

	ReadDWord();

	while (!feof(m_File)) {

		id = ReadWord();

		if (feof(m_File)) break;

		switch (id) {
			case VERSION3DS: ReadVersion(); break;
			case EDIT3DS: ReadEditor(); break;
			case KEYF3DS: ReadKeyframer(); break;
			default: SkipChunk(); break;
		}
	}


	fclose(m_File);
}
