#include "../includes/Response.hpp"

Response::Response( const Request &request, const sCMap &locationMap ) {

	_status = detectStatus(request, locationMap);
	if (_status != OK)
		return ;
}

enum Status Response::detectStatus( const Request &request, const sCMap &locationMap ) {

	std::string	uri;
	sBMap		allowedMethods;

	uri = request.getUri();
	if (locationMap.find(uri) == locationMap.end())
		return NOT_FOUND;
	allowedMethods = locationMap.at(uri).getAllowedMethods();
	if (allowedMethods.at(request.getMethod()) == false)
		return METHOD_NOT_ALLOW;
	return OK;
}

void	Response::send( const int fd ) const {

}

enum Status Response::getStatus( void ) const {

	return (_status);
}

sSMap Response::getHaders( void ) const {

	return (_headers);
}

void Response::setStatus( const enum Status &status ) {

	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {
	
	this->_headers = headers;
}
 
Response::~Response( void ) {

}
