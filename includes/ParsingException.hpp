#ifndef PARSING_EXCEPTION_HPP
# define PARSING_EXCEPTION_HPP

# include <string>

class ParsingException : public std::exception
{
public:
	
	ParsingException(void);
	ParsingException(std::string errorMessage);
	~ParsingException(void) _NOEXCEPT;

	ParsingException(const ParsingException & src);
	ParsingException & operator=(const ParsingException & rhs);

	const char *	what(void) const throw();

private:

	std::string		_errorMessage;

};

#endif
