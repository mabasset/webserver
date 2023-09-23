#include "../includes/Config.hpp"

typedef	std::map<std::string, Config>	sCMap;

const uint16_t		&Config::getListen(void) const {
	return _listen;
}

const sVec			&Config::getServerName(void) const {
	return _server_name;
}

const std::string	&Config::getRoot(void) const {

	return _root;
}

const sVec			&Config::getIndex(void) const {

	return _index;
}

const iSMap			&Config::getErrorPage(void) const {

	return _error_page;
}

const std::string	&Config::getErrorPage(int err) {

	return _error_page[err];
}

const size_t		&Config::getClientMaxBodySize(void) const {

	return _client_max_body_size;
}

const sBMap			&Config::getAllowedMethods(void) const {

	return _allowed_methods;
}

const bool			&Config::getAutoindex(void) const {

	return _autoindex;
}

const sVec			&Config::getTryFiles(void) const {

	return _try_files;
}

const std::string	&Config::getCgiPass(void) const {

	return _cgi_pass;
}

const sVec			&Config::getExtensionCgi(void) const {

	return _extension_cgi;
}

const iSPair		&Config::getReturn(void) const {

	return _return;
}

const sCMap			&Config::getLocationMap(void) const {

	return _locationMap;
}

const std::string	&Config::getLocationName(void) const {

	return _locationName;
}