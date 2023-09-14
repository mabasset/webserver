#include "../includes/Request.hpp"

Request::Request(std::string &all, const sCMap &locationMap ) {

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
	_location = this->detectLocation(locationMap);
}

bool	Request::checkLocationName( const std::string &locationName ) const {
	size_t		pos;
	std::string	extension;
	
	if (locationName.at(0) == '~')
	{
		pos = locationName.find_first_of(".") + 1;
		extension = locationName.substr(pos, locationName.find_first_of("$") - pos);
		if (extension.size() <= _uri.size() && std::equal(extension.rbegin(), extension.rend(), locationName.rbegin()))
			return true ;
	}
	else if (locationName.size() <= _uri.size() && std::equal(locationName.begin(), locationName.end(), _uri.begin()))
		return true ;
	return false ;
}

Config	Request::detectLocation( const sCMap &locationMap ) {

	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
	{
		if (it->first != "/" && this->checkLocationName(it->first))
		{
			_uri.replace(_uri.find(it->first), it->first.length(), it->second.getRoot());
			return it->second;
		}
	}
	return locationMap.at("/");
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

Config	Request::getLocation( void ) const {

	return _location;
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

void	Request::setLocation( const Config &location ) {

	this->_location = location;
}

Request::~Request(void) {

}


