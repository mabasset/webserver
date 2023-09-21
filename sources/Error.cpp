#include "../includes/Response.hpp"

Response::Error::Error( Response *response, const int &code )
	: _response(response), _code(code) {

}

void	Response::Error::editResponse( std::string const &root, iSMap const &errorPageMap ) {

	switch (_code) {
		case 400: _response->setStatus("400 Bad Request"); break ;
		case 401: _response->setStatus("401 Unauthorized"); break ;
		case 402: _response->setStatus("402 Payment Required"); break ;
		case 403: _response->setStatus("403 Forbidden"); break ;
		case 404: _response->setStatus("404 Not Found"); break ;
		case 405: _response->setStatus("405 Method Not Allowed"); _response->setAllowHeader(); break ;
		case 406: _response->setStatus("406 Not Acceptable"); break ;
		case 411: _response->setStatus("411 Length Required"); break ;
		case 413: _response->setStatus("413 Request Entity Too Large"); break ;
		case 501: _response->setStatus("501 Not Implemented"); break ;
		case 510: _response->setStatus("510 Not Extended"); break ;
		default:
			_response->setStatus("500 Internal Server Error");
	}
	if (errorPageMap.find(_code) == errorPageMap.end())
		return ;
	std::string			uri(root + errorPageMap.at(_code));
	Request::fixUri(uri);
	std::ifstream		in(uri.c_str());
	_response->setTypeHeader();
	std::stringstream	ss;
	ss << in.rdbuf();
	_response->setBody(ss.str());
	_response->setLenghtHeader();
}

Response    Response::Error::getResponse( void ) const {
	
	return *_response;
}

void	Response::Error::setResponse( Response response ) {

	this->_response = &response;
}

Response::Error::~Error( void ) {

}
