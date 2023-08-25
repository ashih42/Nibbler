#include "ParsingException.hpp"

ParsingException::ParsingException(void) : _errorMessage() { }

ParsingException::ParsingException(std::string errorMessage) : _errorMessage(errorMessage) { }

ParsingException::~ParsingException(void) _NOEXCEPT { }

ParsingException::ParsingException(const ParsingException & src)
{
	*this = src;
}

ParsingException & ParsingException::operator=(const ParsingException & rhs)
{
	this->_errorMessage = rhs._errorMessage;
	return (*this);
}

const char *		ParsingException::what(void) const throw()
{
	return (this->_errorMessage.c_str());
}
