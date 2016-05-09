
#ifndef __exception_hpp__
#define __exception_hpp__

enum errors
{
	error_ok	= 0,
	error_file_not_found,
	error_bad_file,
	error_memory,
	error_unknown_format
};


class CException
{
public:
	enum {
		EXCEPTION_STRING,
		EXCEPTION_NUMBER
	};

	int	type;

	int	error;
	char	*error_string;

	CException(int _error) :
		error(_error),
		error_string(NULL),
		type(EXCEPTION_NUMBER)
	{
	}

	CException(const char *_error) :
		error_string(strdup(_error)),
		type(EXCEPTION_STRING)
	{
	}

	~CException()
	{
		DELETE_STRING(error_string);
	}
};

#endif