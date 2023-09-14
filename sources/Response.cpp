#include "../includes/Response.hpp"

Response::Response( const Request &request, const sCMap &locationMap ) {

	_status = detectStatus(request, locationMap);
	if (_status == "HTTP/1.1 200 OK" && request.getMethod() == "GET")
	{
		_body = readFile(request, locationMap.at(request.getUri()));
		std::stringstream size;
		size << _body.size();
		_headers["Content-Length"] = size.str();
	}
}

std::string Response::readFile( const Request &request, const Config &location ) {

	std::ifstream 		t;
	std::stringstream	buffer;

	sVec tmp = location.getIndex();
	for (sVec::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		t.open(location.getRoot() + "/" + *it);
		if (t.is_open())
			break ;
	}
	buffer << t.rdbuf();
	_headers["Content-Type"] = "text/html";
	return buffer.str();
}

std::string Response::detectStatus( const Request &request, const sCMap &locationMap ) {

	std::string	uri;
	sBMap		allowedMethods;

	uri = request.getUri();
	if (locationMap.find(uri) == locationMap.end())
		return "HTTP/1.1 404 Not Found";
	allowedMethods = locationMap.at(uri).getAllowedMethods();
	if (allowedMethods.at(request.getMethod()) == false)
		return "HTTP/1.1 405 Method Not Allowed";
	return "HTTP/1.1 200 OK";
}

void	Response::sendResponse( const int fd ) const {

	std::string	buffer;
	sSMap tmp = _headers;

	buffer += _status + "\r\n";
	for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	buffer += "\r\n" + _body;
	std::cout << buffer << std::endl;
	send(fd, buffer.c_str(), buffer.size(), 0);
}

std::string Response::getStatus( void ) const {

	return (_status);
}

sSMap Response::getHaders( void ) const {

	return (_headers);
}

void Response::setStatus( const std::string &status ) {

	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {
	
	this->_headers = headers;
}
 
Response::~Response( void ) {

}
