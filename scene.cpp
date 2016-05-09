
#include "includes.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

CScene::CScene() :
	m_bEmpty(true),
	bWasInitialized(false),
	bAnimation(false),
	m_pActiveCamera(NULL),
	m_pActiveCameraTgt(NULL),
	dwFrameStart(0),
	dwFrameStop(0)
{
}

////////////////////////////////////////////////////////////////////

CScene::~CScene()
{
	this->Release();
}

////////////////////////////////////////////////////////////////////

void CScene::AddObject(CObject *obj, char *name)
{
	ObjectList.Add(name, obj);
}

////////////////////////////////////////////////////////////////////

void CScene::AddMaterial(CMaterial *mat, char *name)
{
	MaterialList.Add(name, mat);
}

////////////////////////////////////////////////////////////////////

void CScene::AddKeyframer(CKeyframer *keyframer, char *name)
{
	KeyframerList.Add(name, keyframer);
}

////////////////////////////////////////////////////////////////////

void CScene::Release()
{
	ObjectList.Release();
	MaterialList.Release();
	KeyframerList.Release();
	m_bEmpty = true;
	bAnimation = false;
	bWasInitialized = false;
	m_pActiveCamera = NULL;
	m_pActiveCameraTgt = NULL;
	dwFrameStart = dwFrameStop = 0;
	texture_mng.Release();
}

////////////////////////////////////////////////////////////////////

void CScene::RenderFrame(float time)
{
	/// TODO: zast¹piæ 'return' na 'throw ...':
	if (m_pActiveCamera == NULL || m_pActiveCameraTgt == NULL)
		return;

	if (ObjectList.IsEmpty())
		return;

	/// update klatki:
	KeyframerList.JumpOnHead();
	KeyframerList.GetCurrent()->Transform(time);

	/// przygotowanie kamery:
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_pActiveCamera->FOV, m_fAspect, 1, 10000);

	CMatrix4x4 x(CMatrix4x4::INIT_IDENTITY);
	x._11 = -x._11;
	x.Transpose();
	glMultMatrixf(x._mat_1D);
	
	glMatrixMode(GL_MODELVIEW);
	CMatrix4x4 testmtx;
	testmtx.CameraMtx(m_pActiveCamera->Position,
			m_pActiveCameraTgt->Target,
			m_pActiveCamera->roll);
	testmtx.Transpose(); /// ze wzglêdu na g³upi¹ konwencjê zapisu macierzy w OpenGL
	glLoadMatrixf( testmtx._mat_1D );

	ObjectList.JumpOnHead();
	do {
		CObject *pObject = ObjectList.GetCurrent();
		pObject->Render();
	} while (ObjectList.Next());
}

////////////////////////////////////////////////////////////////////

static char* nazwy_typow_sciezek[] = {
	"SINGLE", "UNKNOWN", "REPEAT", "LOOP" };
char *DecodeTrackType(int type) 
{
	return nazwy_typow_sciezek[type];
}

void CScene::PrintInfo(ofstream &ostr)
{
	if (!ObjectList.IsEmpty()) {
		ObjectList.JumpOnHead();
		ostr << "CScene::PrintInfo"<<endl<<
			"Mamy takie obiekty: "<< endl;
		do {
			ostr<<" - "<<ObjectList.GetCurrentName() << endl;
			if (ObjectList.GetCurrent()->Type() == OBJECT_MESH) {
				CMesh *pmesh = (CMesh*)ObjectList.GetCurrent();
				ostr<<"    - to jest MESH"<<endl;
				ostr<<"    - Origin = "<<pmesh->Origin<<endl;
				ostr<<"    - X1     = "<<pmesh->X1<<endl;
				ostr<<"    - X2     = "<<pmesh->X2<<endl;
				ostr<<"    - X3     = "<<pmesh->X3<<endl;

				/*for (DWORD i = 0; i < pmesh->m_nVertices; i++) {
					ostr << pmesh->m_pVertices[i].p << endl;
					ostr << "u = " << pmesh->m_pVertices[i].u <<
						"   v = " << pmesh->m_pVertices[i].v << endl;
				}*/
			}

			if (ObjectList.GetCurrent()->Type() == OBJECT_LIGHT) {
				CLight *plight = (CLight*)ObjectList.GetCurrent();
				ostr<<"    - to jest LIGHT"<<endl;
				ostr<<"    - Position = "<<plight->Position<<endl;
				ostr<<"    - Color = "<<plight->Color<<endl;
			}
			if (ObjectList.GetCurrent()->Type() == OBJECT_3DS_DUMMY) {
				CLight *plight = (CLight*)ObjectList.GetCurrent();
				ostr<<"    - to jest DUMMY"<<endl;
			}

		} while (ObjectList.Next());
		ostr<<"-----------------------------------------------------------"<<endl;
	}


	if (!MaterialList.IsEmpty()) {
		MaterialList.JumpOnHead();
		ostr<<endl<<"Mamy takie materialy: "<<endl;
		do {
			CMaterial *pMaterial = MaterialList.GetCurrent();
			ostr<<" - "<<MaterialList.GetCurrentName() << endl;
			ostr<<"      Shininess = "<<pMaterial->m_fShininess<<endl;
			ostr<<"      Transparency = "<<pMaterial->m_fTransparency<<endl;
			ostr<<"      ambient = " << pMaterial->ambient<< endl;
			ostr<<"      diffuse = " << pMaterial->diffuse<< endl;

			if (pMaterial->text1 != NULL) {
				ostr<<"      textura nr 1 obecna: " << endl;
				ostr<<"      map1 name = " << pMaterial->text1->name << endl;
			}
			if (pMaterial->text2 != NULL) {
				ostr<<"      textura nr 2 obecna: " << endl;
				ostr<<"      map2 name = " << pMaterial->text2->name << endl;
			}


		} while (MaterialList.Next());
		ostr<<"-----------------------------------------------------------"<<endl;
	}

	if (!KeyframerList.IsEmpty()) {
		KeyframerList.JumpOnHead();
		ostr<<endl<<"Mamy takie keyframer'y: "<<endl;
		do {
			CKeyframer *pKeyf = KeyframerList.GetCurrent();
			ostr<<" - "<<KeyframerList.GetCurrentName()<<endl;
			ostr<<"   link = "<<pKeyf->link<<endl;
			ostr<<"   hierarchy = "<<pKeyf->hierarchy<<endl;
			ostr<<"   Pivot = "<<pKeyf->Pivot<<endl;
			if (pKeyf->m_pPositionTrack != NULL) {
				ostr<<"   - œcie¿ka pozycji  (kluczy "<<
				    pKeyf->m_pPositionTrack->nKeys<<")"<<endl;
				ostr<<"     typ = "<<DecodeTrackType(pKeyf->m_pPositionTrack->Type)<<endl;
				CVectorTrack::SVectorKey *pKeys = pKeyf->m_pPositionTrack->pKeys;
				/*for (DWORD i = 0; i < pKeyf->m_pPositionTrack->nKeys; i++) {
					ostr<<"       - vec= "<<pKeys[i].vec<<endl;
					ostr<<"       - frame= "<<pKeys[i].frame<<endl;
					ostr<<"       - T    = "<<pKeys[i].T<<endl;
					ostr<<"       - B    = "<<pKeys[i].B<<endl;
					ostr<<"       - C    = "<<pKeys[i].C<<endl;
					ostr<<"       - easefrom = "<<pKeys[i].easefrom<<endl;
					ostr<<"       - easeto    = "<<pKeys[i].easeto<<endl;
				}*/
			}

			if (pKeyf->m_pRotationTrack != NULL) {
				ostr<<"   - œcie¿ka rotacji     (kluczy "<<
				pKeyf->m_pRotationTrack->nKeys<<")"<<endl;
				ostr<<"     typ = "<<DecodeTrackType(pKeyf->m_pRotationTrack->Type)<<endl;
				CRotationTrack::SQuatKey *pKeys = pKeyf->m_pRotationTrack->pKeys;
				/*for (DWORD i = 0; i < pKeyf->m_pRotationTrack->nKeys; i++) {
					ostr<<"     * frame= "<<pKeys[i].frame<<endl;
					ostr<<"       - angle= "<<pKeys[i].angle<<endl;
					ostr<<"       - axis= "<<pKeys[i].axis<<endl;
					ostr<<"       - T    = "<<pKeys[i].T<<endl;
					ostr<<"       - B    = "<<pKeys[i].B<<endl;
					ostr<<"       - C    = "<<pKeys[i].C<<endl;
					ostr<<"       - easefrom = "<<pKeys[i].easefrom<<endl;
					ostr<<"       - easeto    = "<<pKeys[i].easeto<<endl;
				}*/
			}
 
			if (pKeyf->m_pScaleTrack != NULL) {
				ostr<<"   - œcie¿ka skalowania  (kluczy "<<
				pKeyf->m_pScaleTrack->nKeys<<")"<<endl;
				ostr<<"     typ = "<<DecodeTrackType(pKeyf->m_pScaleTrack->Type)<<endl;

				CVectorTrack::SVectorKey *pKeys = pKeyf->m_pScaleTrack->pKeys;
				/*for (DWORD i = 0; i < pKeyf->m_pScaleTrack->nKeys; i++) {
					ostr<<"       - vec= "<<pKeys[i].vec<<endl;
					ostr<<"       - frame= "<<pKeys[i].frame<<endl;
					ostr<<"       - T    = "<<pKeys[i].T<<endl;
					ostr<<"       - B    = "<<pKeys[i].B<<endl;
					ostr<<"       - C    = "<<pKeys[i].C<<endl;
					ostr<<"       - easefrom = "<<pKeys[i].easefrom<<endl;
					ostr<<"       - easeto    = "<<pKeys[i].easeto<<endl;
				}*/
			}

			if (pKeyf->m_pMorphTrack != NULL)
				ostr<<"   - œcie¿ka morphingu   (kluczy "<<
				pKeyf->m_pMorphTrack->nKeys<<")"<<endl;


		} while (KeyframerList.Next());
	}
}

////////////////////////////////////////////////////////////////////

bool CScene::SetActiveCamera(char *name)
{
	char		*target_name = NULL;


	m_pActiveCamera = (CCamera*)ObjectList.Get(name);

	if (m_pActiveCamera == NULL)
		return false;

	target_name = new char[strlen(name) + 10];
	if (!target_name) throw CException(error_memory);

	strcpy(target_name, name);
	strcat(target_name, ".TARGET");

	m_pActiveCameraTgt = (CCameraTarget*)ObjectList.Get(target_name);


	DELETE_STRING(target_name);

	return true;
}

////////////////////////////////////////////////////////////////////

void CScene::InitDisplayLists()
{
	ObjectList.JumpOnHead();
	do {
		if (ObjectList.GetCurrent()->Type() == OBJECT_MESH)
			((CMesh*)ObjectList.GetCurrent())->CreateDisplayList();
	} while(ObjectList.Next());
}

void CScene::DeinitDisplayLists()
{
	ObjectList.JumpOnHead();
	do {
		if (ObjectList.GetCurrent()->Type() == OBJECT_MESH) {
			glDeleteLists(((CMesh*)ObjectList.GetCurrent())->ListIndex, 1);
		}
	} while(ObjectList.Next());
}

void CScene::DeinitTextures()
{
	texture_mng.Release();
}

void CScene::PrepareAnimation()
{
	/// czêœæ inicjalizacyjna OpenGL'a (przenieœæ to w inne miejsce):
	/// poni¿szy kod ustawia ju¿ konkretne rzeczy w Open GL'u
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);

	if (!bWasInitialized) {

		bWasInitialized = true;

		/// ka¿dy keyframer posiada wskaŸnik na obiekt, który jest jego w³aœcicielem (m_pOwnerObject) -
		/// poni¿szy kod przegl¹da wszystkie keyframery i zamienia nazwê obiektu na wskaŸnik do obiektu:
		if (!KeyframerList.IsEmpty()) {
			bAnimation = true;
			KeyframerList.JumpOnHead();
			do {
				CKeyframer* pKeyf = KeyframerList.GetCurrent();
				char *name = KeyframerList.GetCurrentName();

				ObjectList.JumpOnHead();
				do {
					if (strcmp(name, ObjectList.GetCurrentName())==0)
						pKeyf->m_pOwnerObject = ObjectList.GetCurrent();
				} while (ObjectList.Next());

			} while (KeyframerList.Next());
		} else
			bAnimation = false;

		/// obliczenie wspó³czynników dla interpolacji:
		if (!KeyframerList.IsEmpty()) {
			KeyframerList.JumpOnHead();
			do {
				KeyframerList.GetCurrent()->PreCompute();
			} while (KeyframerList.Next());
		}
		
		/// przygotowanie hierarchii keyframerów:
		OrganizeHierarchy();
		
		/// od ka¿dego punktu w obiekcie nale¿y odj¹æ PivotPoint zawarty
		/// w keyframerze dla tego obiektu:
		if (!KeyframerList.IsEmpty()) {
			KeyframerList.JumpOnHead();
			do {
				CKeyframer* pKeyf = KeyframerList.GetCurrent();
				
				if (pKeyf->m_pOwnerObject != NULL)
					if (pKeyf->m_pOwnerObject->Type() == OBJECT_MESH) {
						CMesh *pMesh = (CMesh*)pKeyf->m_pOwnerObject;
						
						for (DWORD i = 0; i < pMesh->m_nVertices; i++) 
							pMesh->m_pVertices[i].p -= pKeyf->Pivot;
					}
					
			} while (KeyframerList.Next());
		}
	}
}

////////////////////////////////////////////////////////////////////

void CScene::SetScreenAspect(float aspect)
{
	m_fAspect = (GLfloat)aspect;
}

////////////////////////////////////////////////////////////////////

CKeyframer* CScene::FindChild(WORD father_id)
{
	DWORD	len = KeyframerList.GetLength();

	for (DWORD i = 0; i < len; i++) {
		CKeyframer *pKeyf = KeyframerList[i];
		if (pKeyf->link == father_id)
			return pKeyf;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////

CKeyframer* CScene::FindKeyframer(WORD keyf_id)
{
	DWORD	len = KeyframerList.GetLength();
	for (DWORD i = 0; i < len; i++) {
		CKeyframer *pKeyf = KeyframerList[i];
		if (pKeyf->hierarchy == keyf_id)
			return pKeyf;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////

void CScene::OrganizeHierarchy()
{
	DWORD		len = KeyframerList.GetLength(); /// d³ugoœæ listy

	bool		*tabHierarchy = NULL; /// okreœla który element nale¿y do drzewa
	tabHierarchy = new bool[len];
	DWORD		i, j;
	for (i = 0; i < len; i++) 
		tabHierarchy[i] = false;

	for (i = 0; i < len; i++) {

		
		CKeyframer *pKeyf = KeyframerList[i];
		assert(pKeyf);

		pKeyf->m_pkBrother = pKeyf->m_pkChild = pKeyf->m_pkFather = NULL;


		if (pKeyf->link == 0xFFFF) {

			for (j = 0; j < len; j++) 
				if ((tabHierarchy[j] == true) && (KeyframerList[j]->link == 0xFFFF)) {
					CKeyframer *left = KeyframerList[j];
					for (; left->m_pkBrother; left = left->m_pkBrother);
					left->m_pkBrother = pKeyf;
					break;
				}
		} else {
			pKeyf->m_pkFather = FindKeyframer(pKeyf->link);
		
			/// jeœli ojciec nie ma dzieci, to wystarczy dopisaæ go jako ojca:
			if ((pKeyf->m_pkFather)->m_pkChild == NULL)
				(pKeyf->m_pkFather)->m_pkChild = pKeyf;
			else {

				/// jeœli ojciec ma dzieci, to nale¿y wyszukaæ ostatniego syna:
				CKeyframer *last_child = (pKeyf->m_pkFather)->m_pkChild;
				for (; last_child->m_pkBrother; last_child = last_child->m_pkBrother);

				/// dopisz nowy keyframer:
				last_child->m_pkBrother = pKeyf;
			}


		}


		tabHierarchy[i] = true;
	}

	DELETE_ARRAY(tabHierarchy);
}

////////////////////////////////////////////////////////////////////

void CScene::InitTextures()
{
	DWORD	ile_textur = 0;
	
	for (DWORD i = 0; i < MaterialList.GetLength(); i++) 
		if (MaterialList[i]->text1 != NULL) 
			ile_textur++;

	texture_mng.SetSize(ile_textur);
	for (i = 0; i < MaterialList.GetLength(); i++)  {
		CMaterial *pMat = MaterialList[i];
		if (pMat->text1 != NULL) {
			DWORD	nr;
			if (texture_mng.LoadTexture(nr, MaterialList[i]->text1->name)) {
				pMat->text1->id = texture_mng.GetTexture(nr);
			} else {
				delete pMat->text1;
				pMat->text1 = NULL;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
