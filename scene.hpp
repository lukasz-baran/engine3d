
#ifndef __scene_hpp__
#define __scene_hpp__

////////////////////////////////////////////////////////////////////

class CScene
{

	CListNamed<CObject>	ObjectList;
	CListNamed<CMaterial>	MaterialList;
	CListNamed<CKeyframer>	KeyframerList;


	/// czy scena jest pusta?
	bool			m_bEmpty;

	/// czy scena zawiera animacj�?
	bool			bAnimation;



	/// wska�niki do aktywnej kamery:
	CCamera			*m_pActiveCamera;
	CCameraTarget		*m_pActiveCameraTgt;


	/// stosunek szeroko�ci ekranu do jego wysoko�ci:
	GLfloat			m_fAspect;

	CTextureManager		texture_mng;

public:
	/// pocz�tek i koniec animacji 
	DWORD			dwFrameStart, dwFrameStop;
	bool			bWasInitialized;

	void AddObject(CObject *, char *);
	void AddMaterial(CMaterial *, char *);
	void AddKeyframer(CKeyframer *, char *);


	/// je�li nie wybrano kamery t� funkcj�
	/// Render() powinna wyrzuca� wyj�tek:
	bool SetActiveCamera(char *);

	/// ustawia "pierwsz� lepsz�" kamer� jako aktywn�:
	void SetFirstCamera() 
	{
		ObjectList.JumpOnHead();
		do {
			if (ObjectList.GetCurrent()->Type() == OBJECT_CAMERA) {
				if (SetActiveCamera(ObjectList.GetCurrentName()))
					return;
			}
		} while (ObjectList.Next());

	}


	CScene();
	~CScene();

	void InitDisplayLists();
	void DeinitDisplayLists();
	void InitTextures();
	void DeinitTextures();
	void PrepareAnimation();

	void SetScreenAspect(float);

	/// organizuje hierarchi� keyframerow na podstawie
	/// wartosci link i hierarchy
	void OrganizeHierarchy();
	CKeyframer* FindChild(WORD father_id);
	CKeyframer* FindKeyframer(WORD keyf_id);
	void ToLocalCoordinateSystem(CKeyframer *pKeyf, CVector3 parentOrigin);



	void RenderFrame(float);

	void Release();

	void PrintInfo(ofstream &);
	void PrintKeyframerHierarchy(int, CKeyframer *pKeyf);

	/// klasy zaprzyja�nione:
	friend class CLoader3DS;
	//friend class CLoader;
};

////////////////////////////////////////////////////////////////////

#endif