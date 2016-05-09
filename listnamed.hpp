///
///	Status:
///		szablon/template
///		podw�jna lista z nazwami
///		nazwy s� unikalne
///
///	autor �ukasz Baran

#ifndef __listnamed_hpp__
#define __listnamed_hpp__


template <class TYPE>
class CListNamed
{
	class CNode
	{
	public:
		CNode		*Next;
		CNode		*Prev;

		TYPE		*pElement;
		char		*szName;

		CNode(const char *name, TYPE *t) :
			Next(NULL),
			Prev(NULL),
			szName(NULL),
			pElement(t)
		{
			szName = strdup(name);
		}

		~CNode()
		{
			if (pElement) {
				delete pElement;
				pElement = NULL;
			}
			if (szName) {
				delete [] szName;
				szName = NULL;
			}
		}

	};

	CNode*		m_pHead;
	CNode*		m_pCurrent;

	DWORD		m_nElements; /// liczba element�w na li�cie


public:
	CListNamed() : m_pHead(NULL), m_pCurrent(NULL), m_nElements(0)
	{
	}

	~CListNamed()
	{
		this->Release();
	}

	void Release() 
	{
		for (CNode *tmp = m_pHead; tmp; ) {
			m_pHead = tmp->Next;
			delete tmp;
			tmp = m_pHead;
		}
		m_pHead = NULL;
		m_pCurrent = NULL;
		m_nElements = 0;
	}

	/// dodaj element z nazw� (name) na ko�cu listy
	void Add(const char *name, TYPE *elem)
	{
		if (Get(name) != NULL)
			return;

		m_nElements++;

		CNode *nowy = new CNode(name, elem);
		if (m_pHead == NULL) {
			m_pHead = nowy;
			m_pCurrent = nowy;
		} else {
			/// znajd� ogon
			CNode *tmp = m_pHead;

			/// wiemy, �e 'tmp' nie mo�e by� NULL:
			while (tmp->Next != NULL)
				tmp = tmp->Next;

			nowy->Prev = tmp;
			tmp->Next = nowy;

		}
	}

	/// usu� element o okre�lonej nazwie
	void Remove(const char *name)
	{
		for (CNode *tmp = m_pHead; tmp; tmp = tmp->Next)
			if (strcmp(tmp->szName, name) == 0) {

				if (tmp->Prev != NULL)
					(tmp->Prev)->Next = tmp->Next;
				else
					m_pHead = tmp->Next;

				if (tmp->Next != NULL)
					(tmp->Next)->Prev = tmp->Prev;

				delete tmp;

				m_nElements--;

				return;  /// bo nazwy na li�cie s� _unikalne_
			}
	}

	/// zwr�� element o nazwie 'name'
	TYPE *Get(const char *name)
	{
		for (CNode *tmp = m_pHead; tmp; tmp = tmp->Next)
			if (strlen(tmp->szName) == strlen(name))
				if (strcmp(tmp->szName, name) == 0)
					return tmp->pElement;
		return NULL;
	}


	TYPE *GetCurrent()
	{
		if (m_pCurrent != NULL)
			return m_pCurrent->pElement;
		else
			return NULL;
	}

	/// przenie� m_pCurrent na nast�pny element
	/// 	true = uda�o si�
	///	false = lista pusta
	bool Next()
	{
		if (m_pCurrent->Next != NULL) {
			m_pCurrent = m_pCurrent->Next;
			return true;
		} else
			return false;
	}

	char *GetCurrentName() const
	{
		if (m_pCurrent != NULL)
			return m_pCurrent->szName;
		return NULL;
	}


	bool IsEmpty() const
	{
		return (!m_pHead);
	}

	void JumpOnHead()
	{
		m_pCurrent = m_pHead;
	}

	/// zwraca n-ty element na li�cie
	/// zwraca NULL je�li nie podano w�a�ciwego indeksu
	TYPE* GetNth(DWORD i)
	{
		if (i > m_nElements)
			return NULL;
		
		DWORD	counter = 0;
		for (CNode *tmp = m_pHead; tmp; tmp = tmp->Next) {
			if (counter == i)
				return tmp->pElement;
			counter++;
		}

		return NULL;
	}
	
	/// alternatywa dla metody 'GetNth':
	TYPE* operator [] (DWORD i)
	{
		return GetNth(i);
	}

	/// zwraca d�ugo�� listy, czyli liczb� element�w na niej:
	DWORD GetLength() const
	{
		return m_nElements;
	}
};


#endif