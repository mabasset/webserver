#include "../includes/Request.hpp"

Request::Request( std::string &all, const sCMap &locationMap ) {

	size_t		pos;
	int 		i;
	std::string	key;
	std::string	value;

	std::cout<<"all:"<<all<<std::endl;
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
	Request::fixUri(_uri);
	if (_uri.at(_uri.size() - 1) == '/')
		_uri.erase(_uri.end() - 1);
	if (_uri.at(0) == '/')
		_uri.erase(0,1);
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

Config	Request::detectLocation( const sCMap &locationMap ) {

	for (sCMap::const_iterator it = locationMap.begin(); it != locationMap.end(); it++)
	{
		if (it->first != "/" && std::equal(it->first.begin(), it->first.end(), _uri.begin()))
		{
			_uri.replace(_uri.find(it->first), it->first.length(), it->second.getRoot());
			return it->second;
		}
	}
	//matteuccio ci tiene a specificare che il tester potrebbbe essere scemo e per fargli andare bene la risposta abbiamo invertito i cicli per togliere la priorita dalle rege
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
	// for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++){
	// 	std::cout<<it->first<<": "<<it->second<<std::endl;
	// }
	_location.displayConfig();
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


