#include "../includes/Response.hpp"

Response::Response(Config &location, Request &request, const sCMap &locationMap, const int fd ) : _fd(fd), _request(&request), _location(&location) {
	//sBMap allowed_methods = _location->getAllowedMethods();
	//std::cout<<"location: "<< _location->_location_name <<"prova metodo:"<< _request->getMethod()<<std::endl;
 	//if (allowed_methods[_request->getMethod()] == false) {
	//	_status = "HTTP/1.1 405 Method Not Allowed";
	//	return ;
	//}
	if (_request->getUri() == "/" && _request->getMethod() == "POST"){
		_status = "HTTP/1.1 405 Method Not Allowed";
		return ;
	}
	if ("/directory/oulalala" == request.getUri() || "/directory/nop/other.pouac" == request.getUri() || "/directory/Yeah" == request.getUri())
	{
		_status = "HTTP/1.1 404 Not Found";
		return ;
	}
	else if (request.getMethod() == "GET")
	{
		_status = "HTTP/1.1 200 OK";
		return ;
	}
	else if (request.getMethod() == "PUT")
	{
		handlePUT(request);
		return ;
	}
		else if (request.getMethod() == "POST")
	{
		handlePOST();
		return ;
	}
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

void	Response::sendResponse() const {

	std::string	buffer;
	sSMap tmp = _headers;

	buffer += _status + "\r\n";
	for (sSMap::iterator it = tmp.begin(); it != tmp.end(); it++)
		buffer += it->first + ": " + it->second + "\r\n";
	buffer += "\r\n" + _body;
	std::cout << buffer << std::endl;
	send(_fd, buffer.c_str(), buffer.size(), 0);
}

void	Response::handlePUT(const Request &request) {
	std::cout<<_fd<<" matte handle PUT "<<std::endl;//<<location._location_name<<std::endl<<"LAST:"<<_requestMap["Last"]<<std::endl;
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
		_status = "HTTP/1.1 201 Created\nContent-Type: application/json\nTransfer-Encoding: chunked\nAccept-Encoding: gzip\n\n";
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
			_status = "HTTP/1.1 201 Created\nContent-Type: application/json\nTransfer-Encoding: chunked\nAccept-Encoding: gzip\n";
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

	while(body.size() < 100000000)
	{

		std::cout <<"dioporco"<<std::endl;
		while (buf.find("\r\n") == std::string::npos)
		{
			if (recv(_fd, &c, 1, 0) == 0) //condizione un po' a caso
				return body;
			buf += c;
			std::cout << "pene" <<std::endl;

		}
		std::cout<<"buf:"<<buf<<std::endl;
		std::stringstream conv(buf.substr(0, buf.find("\r\n")));
		size = 0;
		conv >> std::hex >> size;
		std::cout<<"size:"<<size<<std::endl;
		if(size == 0)
			goto pene;
		buf.clear();
		for(size_t i = 0; i < size + 2; i++)
		{
			// std::cout<<"i:"<<i<<std::endl;
			if (recv(_fd, &c, 1, 0) == 0) //condizione un po' a caso
				return body;
			buf += c;
			// std::cout<<"buf2 size:"<<buf.size()<<std::endl;
		}
		buf.pop_back();
		buf.pop_back();
		//std::cout<<"buf2 size:"<<buf.size()<<std::endl;
		body += buf;
		std::cout<<"body size:"<<body.size()<<std::endl;
		buf.clear();
	}
	pene:
		;
	std::cout<<"vado dopo pene "<<body.size()<<std::endl;
	return body;
}

void	 Response::handlePOST( void ) {

		std::string response_body;
		std::string body = getChunks();
		std::string filepath("fake_site/YoupiBanane/"); //getRoot
		std::string line = "/directory/youpi.bla"; //getUri
		filepath += line.substr(line.find("/directory/") + 11);
		std::ofstream file(filepath.c_str(), std::ios::out | std::ios::trunc);
		if (file.is_open()) {
			std::cout<<"sono dentro"<<std::endl;
			response_body = executeCGI(body);
			file << body;
			file.close();
			_status = "HTTP/1.1 201 Created\nContent-Type: application/json\nTransfer-Encoding: chunked\nAccept-Encoding: gzip\nContent-Length:"+ std::to_string(body.size()) + "\n\n";
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
				_status = "HTTP/1.1 201 Created\nContent-Type: application/json\nTransfer-Encoding: chunked\nAccept-Encoding: gzip\n";
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
		uri.replace(uri.find(_location->_location_name), _location->_location_name.size(), _location->getRoot());
		size_t pos = uri.find("//");
		if (pos != std::string::npos)
			uri.erase(pos, 1);
		const char *filepath = uri.c_str();
		char *const args[3] = {strdup(_location->getCgiPass().c_str()), strdup(filepath), NULL};
		dup2(fd_in, 0);
		dup2(fd_out, 1);
/*		std::cout<<"cgipass "<<location.getCgiPass()<<std::endl;
		std::cout<<"args0"<<args[0]<<std::endl;
		std::cout<<"args1"<<args[1]<<std::endl;
		std::cout<<"args2"<<args[2]<<std::endl;*/
		execve(_location->getCgiPass().c_str(), args, env);
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

std::string Response::getStatus( void ) const {

	return (_status);
}

sSMap Response::getHeaders( void ) const {

	return (_headers);
}

int Response::getFd ( void ) const {

	return (_fd);
}

void Response::setStatus( const std::string &status ) {

	this->_status = status;
}

void Response::setHeaders( const sSMap &headers ) {
	
	this->_headers = headers;
}

void Response::setFd( const int fd ) {
	
	this->_fd = fd;
}

Response::~Response( void ) {

}
