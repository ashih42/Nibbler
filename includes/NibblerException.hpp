#ifndef NIBBLER_EXCEPTION_HPP
# define NIBBLER_EXCEPTION_HPP

# include <string>

class NibblerException : public std::exception
{
public:
	
	NibblerException(void);
	NibblerException(std::string errorMessage);
	~NibblerException(void) _NOEXCEPT;

	NibblerException(const NibblerException & src);
	NibblerException & operator=(const NibblerException & rhs);

	const char *	what(void) const throw();

private:

	std::string		_errorMessage;

};

#endif
