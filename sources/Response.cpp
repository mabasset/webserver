#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _socket(fd), _request (&request), _location(request.getLocation()), _uri(request.getUri()) {

}

void	Response::compile( ) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	int i = 0;
	while (i < 5)
	{
		if (_request->getMethod() == methods[i])
			break ;
		i++;
	}
	if (i == 5)
		throw Error(this, NOT_IMPLEMENTED);
	if (_location.getAllowedMethods().at(_request->getMethod()) == false)
		throw Error(this, NOT_ALLOWED);

	_uri = _request->getUri();
	if (_location.getLocationName() == "/")
		_uri = _location.getRoot() + "/" + _request->getUri();
	else
		_uri.replace(0, _location.getLocationName().size(), _location.getRoot());
	_uri = Request::fixUri(_uri);
	this->setTypeHeader();
	switch (i) {
		case HEAD:
		case GET: this->handleGet(); break ;
		case POST:
		case PUT: this->handlePut(); break ;
		case DELETE: this->handleDelete(); break ;
		default : throw Error(this, SERVER_ERROR);
	}
}

void	Response::commit( void ) const {

	std::string	buffer("HTTP/1.1 ");

	buffer += _status + "\r\n";
	for(sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	buffer += "\r\n";
	buffer += _body;
	send(_socket, buffer.c_str(), buffer.size(), 0);
}

void	Response::handleGet( void ) {

	sVec try_files = _location.getTryFiles();
	std::ifstream in;
	struct stat fileStat;
	stat(_uri.c_str(), &fileStat);
	if (_location.getReturn().first != 0)
		return redirectPage();
	if (_location.getAutoindex() == true && _uri.at(_uri.size() - 1) == '/')
		return autoindexPage();
	if (std::find(try_files.begin(), try_files.end(), "$uri") != try_files.end() && !S_ISDIR(fileStat.st_mode))
		in.open(_uri.c_str());
	_uri += "/";
	if (!in.is_open() && std::find(try_files.begin(), try_files.end(), "$uri/") != try_files.end())
	{
		for (sVec::const_iterator it = _location.getIndex().begin(); it != _location.getIndex().end(); it++)
		{
			in.open(Request::fixUri(_uri + *it).c_str());
			if (in.is_open())
				break ;
		}
	}
	if (_location.getAutoindex() == true && !in.is_open())
		return autoindexPage();
	if (!in.is_open())
		throw Error(this, NOT_FOUND);
	std::stringstream ss;
	ss << in.rdbuf();
	_body = ss.str();
	if (_request->getHeaders().count("Cookie") == 0){
		_headers["Set-Cookie"] = gen_random(5);}
	else {
		_headers["Cookie"] = _request->getHeaders().at("Cookie");
		if (_body.find("No cookie") != std::string::npos)
			_body.replace(_body.find("No cookie"), 9, "Il tuo cookie &eacute;: " + _headers["Cookie"]);
	}
	this->setTypeHeader();
	this->setLenghtHeader();
	if (_request->getMethod() == "HEAD")
		_body.clear();
	_status = "200 OK";
}

void	Response::handlePut( void ) {

	if (_request->getHeaders().at("Transfer-Encoding") != "chunked")
		throw Error(this, NOT_IMPLEMENTED);
	sVec try_files = _location.getTryFiles();
	std::ofstream out;
	struct stat fileStat;
	stat(_uri.c_str(), &fileStat);
	if (std::find(try_files.begin(), try_files.end(), "$uri") != try_files.end() && !S_ISDIR(fileStat.st_mode))
		out.open(_uri.c_str(), std::ios::out | std::ios::trunc);
	else if (std::find(try_files.begin(), try_files.end(), "$uri/") != try_files.end())
	{
		for (sVec::const_iterator it = _location.getIndex().begin(); it != _location.getIndex().end(); it++)
		{
			out.open(Request::fixUri(_uri + "/" + *it).c_str(), std::ios::out | std::ios::trunc);
			if (out.is_open())
				break ;
		}
	}
	if (!out.is_open())
		throw Error(this, FORBIDDEN);
	for (sVec::const_iterator it = _request->getChunks().begin(); it != _request->getChunks().end(); it++)
		_body += *it;
	
	if (_body.size() == 0)
		throw Error(this, NO_CONTENT);
	if (_location.getClientMaxBodySize() != 0 && _body.size() > _location.getClientMaxBodySize())
		throw Error(this, TOO_LARGE);
	if (_request->getMethod() == "POST")
	{
		executeCGI();
		if (_body.find("Status") != std::string::npos)
			_body.erase(0, _body.find("\n"));
		if (_body.find("Content-Type") != std::string::npos)
			_body.erase(0, _body.find("\r\n\r\n") + 4);
	}
	out << _body;
	_headers["Content-Location"] = _uri;
	this->setLenghtHeader();
	_status = "201 Created";
}

void Response::handleDelete( void ) {
	std::string resource = _request->getUri();
	resource.replace(resource.find(_location.getLocationName()), _location.getLocationName().size(), _location.getRoot());
	std::cout << resource << std::endl;
	if (std::remove(resource.c_str()) == 0){
		std::string res = "HTTP/1.1 200 OK\r\n\r\n";
		send(_socket, res.c_str(), res.size(), 0);
		_status = "NOT";
		return ;
	}
	else {
		std::string res = "HTTP/1.1 403 Forbidden\r\n\r\n";
		send(_socket, res.c_str(), res.size(), 0);
		_status = "NOT";
		return ;
	}
}

void Response::autoindexPage( void ){
	_status = "200 OK";
	_headers["Content-type"] = "text/html";
	_body += "<html><head><title>Index of " + _request->getUri() + "</title></head><body><h1>Index of " + _request->getUri() + "</h1><hr><pre>";

	std::string tmp;
	DIR* dir;
	struct dirent* ent;
	tmp = _location.getRoot();
	tmp.erase(tmp.size() - 1, 1);
	if ((dir = opendir((tmp + _request->getUri()).c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			_body += "<a href=\"";
			_body += _request->getUri();
			if (_request->getUri().at(_request->getUri().size() - 1) != '/' )
				_body += "/";
			_body += ent->d_name ;
			_body += "\">" ;
			_body += ent->d_name;
				_body += "</a><br>";
		}
		closedir(dir);
	}

  	_body += "</pre><hr></body></html>";
}

void Response::redirectPage( void ) {
	iSPair loc_ret = _location.getReturn();
	if (loc_ret.first == 301) {
		_status = "301 Moved Permanently";
		_headers["Location"] = loc_ret.second;
	}

}

void	Response::executeCGI( void ) {

	
	int fd = fileno(tmpfile());

	write(fd, _body.c_str(), _body.size());
	lseek(fd, 0, SEEK_SET);

	char *const	args[3] = {strdup(_location.getCgiPass().c_str()), strdup(_uri.c_str()), NULL};

	char		*env[5];
	env[0] = strdup(("PATH_INFO=" + _request->getUri()).c_str());
	env[1] = strdup("REQUEST_METHOD=POST");
	env[2] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	sSMap tmp(_request->getHeaders());
	if (tmp.find("X-Secret-Header-For-Test") != tmp.end())
		env[3] = strdup(("HTTP_X_SECRET_HEADER_FOR_TEST=" + tmp.at("X-Secret-Header-For-Test")).c_str());
	else
		env[3] = NULL;
	env[4] = NULL;

	pid_t	pid = fork();
	if (pid == -1)
		throw Error(this, SERVER_ERROR);
	else if (!pid) {
		dup2(fd, 0);
		dup2(fd, 1);
		close(fd);
		execve(args[0], args, env);
		throw Error(this, SERVER_ERROR);
	}
	else {
		waitpid(-1, NULL, 0);
		lseek(fd, 0, SEEK_SET);
		struct stat st;
		fstat(fd, &st);
		size_t	size = st.st_size;
		size_t	n_bytes;
		size_t	i = 0;
		char	*buffer = (char *) calloc (size + 1, sizeof(char));
		while (size)
		{
			n_bytes = read(fd, &buffer[i], size);
			i += n_bytes;
			size -= n_bytes;
		}
		_body = buffer;
		free(buffer);
	}
	close(fd);

	for (int i = 0; env[i] != NULL; i++)
		delete[] env[i];

	for (int i = 0; args[i] != NULL; i++)
		delete[] args[i];
}

std::string Response::gen_random(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(len);
	srand(time(NULL));
	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return tmp_s;
}

void Response::setAllowHeader( void ) {

	std::string	methods;
	sBMap		tmp = _location.getAllowedMethods();

	for (sBMap::const_iterator it = tmp.begin(); it != tmp.end(); it++)
		if (it->second)
	  		methods += it->first + ", ";
	if (!methods.empty())
		methods.erase(methods.length() - 2);
	_headers["Allow"] = methods;
}

void Response::setTypeHeader( void ) {
	_headers["Content-Type"] = "text/html; charset=utf-8";
}

void Response::setLenghtHeader( ) {

	std::stringstream ss;
	ss << _body.size();   
	_headers["Content-Length"] = ss.str();
}

const int			&Response::getSocket( void ) const {

	return _socket;
}

const Request		&Response::getRequest( void ) const {

	return *this->_request;
}

const std::string	&Response::getUri( void ) const {

	return _uri;
}

const std::string	&Response::getStatus( void ) const {

	return _status;
}

const sSMap			&Response::getHaders( void ) const {

	return _headers;
}

const std::string	&Response::getBody( void ) const {

	return _body;
}

void Response::setSocket( const int &socket ) {
	
	this->_socket = socket;
}

void	Response::setRequest( const Request &request ) {

	this->_request = &request;
}

void Response::setUri( const std::string &uri ) {

	this->_uri = uri;
}

void Response::setStatus( const std::string &status ) {

	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {

	this->_headers = headers;
}

void Response::setBody( const std::string &body ) {

	this->_body = body;
}
 
Response::~Response( void ) {

	_headers.clear();
}
