#include "../includes/Request.hpp"

Request::Request(std::string all) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	size_t		pos;
	int 		i;
	std::string	key;
	std::string	value;

	pos = all.find_first_of(" ");
	i = 0;
	while (i < 5)
	{
		if (methods[i] == all.substr(0, pos))
			break ;
		i++;
	}
	if (i == 5)
		std::runtime_error("Bad method");
	_method = Method(i);
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

void	Request::display( void ) {

	std::cout << "method: " << _method << std::endl;
	std::cout << "uri: " << _uri << std::endl;
	for (sSMap::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
}

Method	Request::getMethod( void ) {

	return _method;
}

std::string	Request::getUri( void ) {

	return _uri;
}

sSMap	Request::getHeaders( void ) {

	return _headers;
}

void	Request::setMethod( Method method ) {

	this->_method = method;
}

void	Request::setUri( std::string uri ) {

	this->_uri = uri;
}

void	Request::getHeaders( sSMap headers ) {

	this->_headers = headers;
}

Request::~Request(void) {

}


