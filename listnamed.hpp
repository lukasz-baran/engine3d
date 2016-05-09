///
///	Status:
///		szablon/template
///		podwójna lista z nazwami
///		nazwy s¹ unikalne
///
///	autor £ukasz Baran

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

	DWORD		m_nElements; /// liczba elementów na liœcie


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

	/// dodaj element z nazw¹ (name) na koñcu listy
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
			/// znajdŸ ogon
			CNode *tmp = m_pHead;

			/// wiemy, ¿e 'tmp' nie mo¿e byæ NULL:
			while (tmp->Next != NULL)
				tmp = tmp->Next;

			nowy->Prev = tmp;
			tmp->Next = nowy;

		}
	}

	/// usuñ element o okreœlonej nazwie
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

				return;  /// bo nazwy na liœcie s¹ _unikalne_
			}
	}

	/// zwróæ element o nazwie 'name'
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

	/// przenieœ m_pCurrent na nastêpny element
	/// 	true = uda³o siê
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

	/// zwraca n-ty element na liœcie
	/// zwraca NULL jeœli nie podano w³aœciwego indeksu
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

	/// zwraca d³ugoœæ listy, czyli liczbê elementów na niej:
	DWORD GetLength() const
	{
		return m_nElements;
	}
};


#endif