#include "../includes/Request.hpp"

Request::Request( std::string &all, const sCMap &locationMap, const int fd ) {

	size_t		pos;
	std::string	key, val;

	pos = all.find_first_of(" ");
	_method = all.substr(0, pos);
	pos += 1;
	_uri = all.substr(pos, all.find_first_of(" ", pos) - pos);
	all.erase(0, all.find_first_of("\n") + 1);
	std::stringstream ss(all);
	while (std::getline(std::getline(ss, key, ':') >> std::ws, val))
		_headers.insert(std::make_pair(key, val.substr(0, val.size() - 1)));
	_location = this->detectLocation(locationMap);
	Request::fixUri(_uri);
	this->detectBody(fd);
}


Config	Request::detectLocation( const sCMap &locationMap ) {

	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
	{
		if (it->first != "/" && std::equal(it->first.begin(), it->first.end(), _uri.begin()))
		{
			_uri.replace(_uri.find(it->first), it->first.length(), it->second.getRoot());
			return it->second;
		}
	}
	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
	{
		if (it->first == "/")
			continue ;
		if (it->first != "/" && it->first.at(0) == '~')
		{
			size_t		pos = it->first.find_last_of(".") + 1;
			std::string extension = it->first.substr(pos, it->first.find_last_of("$") - pos);
			if (std::equal(extension.rbegin(), extension.rend(), _uri.rbegin()))
			{
				_uri = it->second.getRoot() + _uri;
				return it->second;
			}
		}
	}
	_uri = locationMap.at("/").getRoot() + _uri;
	return locationMap.at("/");
}

void	Request::detectBody( const int fd ) {

	int size;

	do {
		Chunk chunk()
	} while (size > 0);
}

void	Request::fixUri( std::string &uri ) {

    char prev;
    for (std::string::iterator it = uri.begin(); it != uri.end(); it++)
    {
        if (prev == *it && *it == '/')
        {
            uri.erase(it);
            it--;
        }
        else
            prev = *it;
    }
}

void	Request::display( void ) const {

	std::cout << "method: " << _method << std::endl;
	std::cout << "uri: " << _uri << std::endl;
	sSMap tmp = _headers;
	for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++){
		std::cout<<it->first<<": "<<it->second<<std::endl;
	}
	_location.displayConfig();
}

Request::Request( const Request &src ) {
	*this = src;
}

Request	&Request::operator=( const Request &rhs ) {
	if (this == &rhs)
		return *this;
	_method = rhs.getMethod();
	_uri = rhs.getUri();
	_headers = rhs.getHeaders();
	_location = rhs.getLocation();
	return *this;
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

std::string	Request::getBody( void ) const {

	return _body;
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

void	Request::setBody( const std::string &body ) {

	this->_body = body;
}

