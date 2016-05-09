
#ifndef __buffer_hpp__
#define __buffer_hpp__

#include <stdio.h>

#define	BUFFER_SIZE			0x10000

template <class Type>
class CBuffer 
{
	DWORD		size;
	FILE		*file;
	Type		*data;
	char		*fname;
	DWORD		page; // biezacy numer strony

	DWORD		page_number(DWORD) const;
public:
	CBuffer();
	~CBuffer();

	BOOL Init(const char *);
	DWORD Size() const;

	Type operator[](DWORD);
};

template <class Type>
DWORD CBuffer<Type>::page_number(DWORD index) const
{
	return (index / BUFFER_SIZE);
}

template <class Type>
DWORD CBuffer<Type>::Size() const
{
	return size;
}

template <class Type>
CBuffer<Type>::CBuffer() : data(NULL), file(NULL), fname(NULL), size(0), page(0)
{
	data = new Type [BUFFER_SIZE];
}

template <class Type>
CBuffer<Type>::~CBuffer()
{
	if (file != NULL)
		fclose(file);
	if (data != NULL) {
		delete [] data;
		data = NULL;
	}
	if (fname != NULL) {
		delete [] fname;
		fname = NULL;
	}
	size = page = 0;
}

template <class Type>
BOOL CBuffer<Type>::Init(const char *_fname)
{
	fname = new char [strlen(_fname) + 1];
	strcpy(fname, _fname);

	file = fopen(fname, "rb");
	if (file == NULL)
		return FALSE;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	
	fseek(file, 0, SEEK_SET);

	DWORD num;
	if (size >= BUFFER_SIZE)
		num = BUFFER_SIZE;
	else
		num = size;
	fread(data, num, 1, file);

	return TRUE;
}

template <class Type>
Type CBuffer<Type>::operator [](DWORD index)
{
	if (index >= 0 && index < Size()) {

		if (page_number(index) != page) {
		
			// trzeba wczytac odpowiednia strone:
			DWORD pos = page_number(index)*BUFFER_SIZE;
			fseek(file, pos, SEEK_SET);

			DWORD num = ((size - pos) >= BUFFER_SIZE)?BUFFER_SIZE:(size - pos);
/*			if ((size - pos) >= 0x10000)
				num = 0x10000;
			else
				num = size - pos;*/

			fread(data, num, 1, file);
			page = page_number(index);
		}
		return data[index % BUFFER_SIZE];
			
		
	};
//	else 
//		ErrorAndExit("Nastapilo przekroczenie zakresu przy dostepie do tablicy!");
	return 0;
}


#endif