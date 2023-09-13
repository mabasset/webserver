#include "../includes/Request.hpp"

Request::Request(std::string &all) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	size_t		pos;
	int 		i;
	std::string	key;
	std::string	value;

	pos = all.find_first_of(" ");
	_method = all.substr(0, pos);
	pos += 1;
	_uri = all.substr(pos, all.find_first_of(" ", pos) - pos);
	all.erase(0, all.find_first_of("\n") + 1);
	all.erase(all.find("\r\n\r\n"), 4);
	std::stringstream ss(all);
	while (std::getline(ss, key, ':'))
	{
		std::getline(ss, value);
		value.erase(0, value.find_first_not_of(" \n\t"));
		_headers.insert(make_pair(key, value));
	}
}

void	Request::display( void ) const {

	std::cout << "method: " << _method << std::endl;
	std::cout << "uri: " << _uri << std::endl;
	sSMap tmp = _headers;
	for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
}

std::string	Request::getMethod( void ) const {

	return _method;
}

std::string	Request::getUri( void ) const {

	return _uri;
}

sSMap	Request::getHeaders( void ) const {

	return _headers;
}

void	Request::setMethod( const std::string &method ) {

	this->_method = method;
}

void	Request::setUri( const std::string &uri ) {

	this->_uri = uri;
}

void	Request::setHeaders( const sSMap &headers ) {

	this->_headers = headers;
}

Request::~Request(void) {

}


