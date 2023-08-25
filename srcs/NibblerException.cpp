#include "NibblerException.hpp"

NibblerException::NibblerException(void) : _errorMessage() { }

NibblerException::NibblerException(std::string errorMessage) : _errorMessage(errorMessage) { }

NibblerException::~NibblerException(void) _NOEXCEPT { }

NibblerException::NibblerException(const NibblerException & src)
{
	*this = src;
}

NibblerException & NibblerException::operator=(const NibblerException & rhs)
{
	this->_errorMessage = rhs._errorMessage;
	return (*this);
}

const char *		NibblerException::what(void) const throw()
{
	return (this->_errorMessage.c_str());
}
