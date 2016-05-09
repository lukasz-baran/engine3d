
#ifndef __globals_hpp__
#define __globals_hpp__

#define RELEASE(x)		if (x != NULL) { delete x; x = NULL; }
#define DELETE_ARRAY(t)		if (t != NULL) { delete [] t; t = NULL; }

inline void DELETE_STRING(char *&str)
{
	if (str != NULL) {
		delete [] str;
		str = NULL;
	}
}

void fatal_error(char *message, char *file, int line);
void fatal_error(char *message);


#endif