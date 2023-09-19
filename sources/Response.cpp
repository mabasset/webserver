#include "../includes/Response.hpp"

Response::Response( const Request &request, const int fd )
	: _request (&request), _socket(fd), _location(request.getLocation()) {

}

void	Response::compile( ) {

	std::string	methods[] = { "GET", "POST", "DELETE", "HEAD", "PUT" };
	int i;

	_status = 
	i = 0;
	while (i < 5)
	{
		if (_request->getMethod() == methods[i])
			break ;
		i++;
	}
	if (i == 5)
		throw Error(this, NOT_IMPLEMENTED);
	sBMap allowed_methods = _location.getAllowedMethods();
	if (allowed_methods[_request->getMethod()] == false){
		_status = "405 Not Allowed";
		return ;
	}
	switch (i) {
		case HEAD:
		case GET: this->handleGet(); break ;
		case POST: this->handlePOST(); break ;
		// case DELETE: this->handleDelete(); break ;
		case PUT: this->handlePUT(); break ;
		default : throw Error(this, SERVER_ERROR);
	}
}

void	Response::commit( void ) const {

	std::string	buffer("HTTP/1.1 ");

	buffer += _status + "\r\n";
	for(sSMap::const_iterator it = _headers.begin(); it != _headers.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	if (!_headers.empty())
		buffer += "\r\n";
	buffer += "\r\n" + _body;
	std::cout << buffer << std::endl;
	send(_socket, buffer.c_str(), buffer.size(), 0);
}

void	Response::handleGet( void ) {

	DIR *dir = opendir(_request->getUri().c_str());
	std::ifstream in;

	_status = "200 OK";
	if (dir != NULL)
	{
		closedir(dir);
		sVec index = _location.getIndex();
		for (sVec::const_iterator it = index.begin(); it != index.end(); it++)
		{
			in.open(_request->getUri() + "/" + *it);
			if (in.is_open())
				break ;
		}
	}
	else
		in.open(_request->getUri());
	if (!in.is_open())
	{
		in.open(_location.getRoot() + _location.getErrorPage().at(404));
		_status = "404 Not Found";
	}
	std::stringstream ss;
	ss << in.rdbuf();
	_body = ss.str();
	in.close();
}

void	Response::handlePUT( ) {
	std::cout<<_socket<<" matte handle PUT "<<std::endl;//<<location._location_name<<std::endl<<"LAST:"<<_requestMap["Last"]<<std::endl;
	//if (request.getHeaders().at("Transfer-Encoding") == "chunked")
		handlePUTChunked();
}

void Response::handlePUTChunked() {
	std::string body = getChunks();
	std::string filepath("fake_site"); //getRoot
	//std::string line = request.getUri();
	std::string line = "/put_test/file_should_exist_after";
	std::cout<<"uri:"<<line<<std::endl;
	filepath += line.substr(line.find("/put_test/") + 9);
	//if (checkTryFiles("$uri", request.getLocation.getPeniGrossi())) {
	std::ofstream file(filepath.c_str(), std::ios::out | std::ios::trunc);
		//std::cout<< "tentativo prima: " << filepath << std::endl;
		std::cout<<"sono fuori"<<std::endl;
	if (file.is_open()) {
		std::cout<<"sono dentro"<<std::endl;
		file << body;
		file.close();
		_status = "201 Created";
		_headers["Content-Type"] = "application/json";
		_headers["Transfer-Encoding"] = "chunked\r\n";
		_headers["Accept-Encoding"] = "gzip";
		return ;
	}
	file.close();
	//if (checkTryFiles("$uri/", location)){
	if (filepath.at(filepath.size() - 1) != '/')
		filepath.push_back('/');
	//sVec indexes = location.getIndex();
	sVec indexes;
	indexes.push_back("index.html");
	for (sVec::iterator it = indexes.begin(); it != indexes.end(); it++)
	{
		std::ofstream file(filepath.c_str(), std::ios::out | std::ios::trunc) ;
		//std::cout<< "tentativo: " << filepath + *it << " it:"<<*it<< std::endl;
		file.open((filepath + *it).c_str());
		if (file.is_open() == true){
			file << body;
			file.close();
			_status = "201 Created";
			_headers["Content-Type"] = "application/json";
			_headers["Transfer-Encoding"] = "chunked\r\n";
			_headers["Accept-Encoding"] = "gzip";
			return ;
		}
		file.close();
	}

	std::cout<<"error"<<std::endl;
	_status = "HTTP/1.1 404 Not Found";

}

std::string Response::getChunks() {

	char c;
	size_t size;
	std::string buf;
	std::string body;
	sSMap chunks;

	std::cout<<"wait..."<<std::endl;
	while(body.size() < 100000000)
	{
		while (buf.find("\r\n") == std::string::npos)
		{
			if (recv(_socket, &c, 1, 0) == 0) //condizione un po' a caso
				return body;
			buf += c;

		}
		std::stringstream conv(buf.substr(0, buf.find("\r\n")));
		size = 0;
		conv >> std::hex >> size;
		if(size == 0)
			goto pene;
		buf.clear();
		for(size_t i = 0; i < size + 2; i++)
		{
			// std::cout<<"i:"<<i<<std::endl;
			if (recv(_socket, &c, 1, 0) == 0) //condizione un po' a caso
				return body;
			buf += c;
			// std::cout<<"buf2 size:"<<buf.size()<<std::endl;
		}
		buf.pop_back();
		buf.pop_back();
		//std::cout<<"buf2 size:"<<buf.size()<<std::endl;
		body += buf;
		buf.clear();
	}
	pene:
		;
	std::cout<<"vado dopo pene "<<body.size()<<std::endl;
	return body;
}

void	 Response::handlePOST( void ) {

		std::string response_body;
		_request->display();
		std::string body = getChunks();
		// if (body.size() == 0) {
		// 	_status = "404 Not Found";
		// 	return ;
		// }
		std::string filepath("fake_site/YoupiBanane/"); //getRoot
		std::string line = "/directory/youpi.bla"; //getUri
		filepath += line.substr(line.find("/directory/") + 11);
		std::ofstream file(filepath.c_str(), std::ios::out | std::ios::trunc);
		if (file.is_open()) {
			std::cout<<"sono dentro"<<std::endl;
			response_body = executeCGI(body);
			file << body;
			file.close();
			_status = "201 Created";
			_headers["Content-Type"] = "application/json";
			_headers["Transfer-Encoding"] = "chunked";
			_headers["Accept-Encoding"] = "gzip";
			_headers["Content-Length"] = std::to_string(body.size()) + "\r\n";
			return ;
		}
		file.close();
		//if (checkTryFiles("$uri/", location)){
		if (filepath.at(filepath.size() - 1) != '/')
			filepath.push_back('/');
		//sVec indexes = location.getIndex();
		sVec indexes;
		indexes.push_back("index.html");
		for (sVec::iterator it = indexes.begin(); it != indexes.end(); it++)
		{
			std::ofstream file(filepath.c_str(), std::ios::out | std::ios::trunc) ;
			//std::cout<< "tentativo: " << filepath + *it << " it:"<<*it<< std::endl;
			file.open((filepath + *it).c_str());
			if (file.is_open() == true){
				response_body = executeCGI(body);
				file << body;
				file.close();
				_status = "201 Created";
				_headers["Content-Type"] = "application/json";
				_headers["Transfer-Encoding"] = "chunked";
				_headers["Accept-Encoding"] = "gzip";
				_headers["Content-Length"] = std::to_string(body.size()) + "\r\n";
				return ;
			}
			file.close();
		}
		std::cout<<"error"<<std::endl;
		_status = "HTTP/1.1 404 Not Found";
}

std::string	Response::executeCGI(std::string &content){
	pid_t	pid;
	int		std_cpy[2] = { dup(0), dup(1) };
	char	**env = getEnvCgi();
	std::string _retBody = "";

	FILE*	in = tmpfile();
	FILE*	out = tmpfile();
	int		fd_in = fileno(in);
	int		fd_out = fileno(out);

	// UNCOMMENT TO PRINT ENV
	// int i = 0;
	// while (env[i])
	//		printf("%s\n", env[i++]);

	// use tmpFile() instead of pipe() to handle big amount of data

	write(fd_in, content.c_str(), content.size());
	lseek(fd_in, 0, SEEK_SET);

	pid = fork();

	if (pid == -1) {
		std::cout << "Fork failed" << std::endl;
		return ("Status: 500\r\n\r\n");
	}
	else if (!pid) {
		std::string uri = _request->getUri();
		size_t pos = uri.find("//");
		if (pos != std::string::npos)
			uri.erase(pos, 1);
		const char *filepath = uri.c_str();
		char *const args[3] = {strdup(_location.getCgiPass().c_str()), strdup(filepath), NULL};
		dup2(fd_in, 0);
		dup2(fd_out, 1);
/*		std::cout<<"cgipass "<<location.getCgiPass()<<std::endl;
		std::cout<<"args0"<<args[0]<<std::endl;
		std::cout<<"args1"<<args[1]<<std::endl;
		std::cout<<"args2"<<args[2]<<std::endl;*/
		execve(_location.getCgiPass().c_str(), args, env);
		std::cout << "exxxecve failed" << std::endl;
		write(1, "Status: 500\r\n\r\n", 15);
		exit (0);
	}
	else {
		waitpid(-1, NULL, 0);

		lseek(fd_out, 0, SEEK_SET);

		char	buffer[1024];
		size_t  dataRead = 0;
		while ((dataRead = read(fd_out, buffer, sizeof buffer)) > 0) {
			for (size_t i = 0; i < dataRead; i++)
				_retBody.push_back(buffer[i]);
			memset(buffer, 0, sizeof buffer);
		}
			std::ofstream file("prova.txt", std::ios::out | std::ios::trunc) ;
			file << _retBody.c_str();
	}
	fclose(in);
	fclose(out);
	close(fd_in);
	close(fd_out);

	dup2(std_cpy[0], 0);
	dup2(std_cpy[1], 1);

	for (short i = 0; env[i] != NULL; i++)
		delete[] env[i];
	delete[] env;
	return _retBody;
}

char	**Response::getEnvCgi() {
	sSMap envMap;

	envMap.insert(std::make_pair("AUTH_TYPE", ""));
	envMap.insert(std::make_pair("CONTENT_LENGTH", ""));
	envMap.insert(std::make_pair("CONTENT_TYPE", "application/x-www-form-urlencoded"));
	envMap.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	envMap.insert(std::make_pair("PATH_INFO", _request->getUri()));
	envMap.insert(std::make_pair("PATH_TRANSLATED", "mettere qua uri translated"));
	envMap.insert(std::make_pair("QUERY_STRING", ""));
	envMap.insert(std::make_pair("REMOTE_ADDR", _headers["Host"]));
	envMap.insert(std::make_pair("REMOTE_HOST", ""));
	envMap.insert(std::make_pair("REMOTE_IDENT", ""));
	envMap.insert(std::make_pair("REMOTE_USER", ""));
	envMap.insert(std::make_pair("REQUEST_METHOD",  _request->getMethod()));
	envMap.insert(std::make_pair("REQUEST_URI", _request->getUri()));
	envMap.insert(std::make_pair("SCRIPT_NAME", "ubuntu_cgi_tester"/*location.cgi_pass.substr(location.cgi_pass().find_last_of("/") + 1)*/));
	envMap.insert(std::make_pair("SERVER_NAME", "http://" + _headers["Host"]));
	envMap.insert(std::make_pair("SERVER_PORT", "8080" /*locatio.serverport()*/));
	envMap.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envMap.insert(std::make_pair("SERVER_SOFTWARE", "Webserv/1.0"));
	envMap.insert(std::make_pair("REDIRECT_STATUS", "200"));
	//if (conn.headers.find("X-Secret") != conn.headers.npos)
	//    envMap.insert(std::make_pair("HTTP_X_SECRET_HEADER_FOR_TEST", conn.headers.substr(conn.headers.find("X-Secret") + 26, conn.headers.find_first_of("\r\n"))));
	char	**env = new char*[envMap.size() + 1];
	int	j = 0;
	for (sSMap::const_iterator i = envMap.begin(); i != envMap.end(); i++) {
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		//std::cout<<"riga env:"<<env[j]<<std::endl;
		j++;
	}
	env[j] = NULL;
	return env;
}

Request Response::getRequest( void ) const {

	return *this->_request;
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

void Response::setBody( const std::string &body ) {
	
	this->_body = body;
}

void	Response::setRequest( Request request ) {

	this->_request = &request;
}
 
Response::~Response( void ) {

}
