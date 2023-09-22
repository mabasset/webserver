#include "../includes/Chunk.hpp"

Chunk::Chunk( const int fd ) {

	char				c;
	std::string			buf;

	while (buf.find("\r\n") == std::string::npos)
	{
		if (recv(fd, &c, 1, 0) < 1)
			throw std::runtime_error("recv error");
		buf += c;
	}
	std::stringstream	ss(buf.substr(0, buf.find("\r\n")));
	ss >> std::hex >> _size;
	std::cout << _size << std::endl;
	char	*str = new char [_size + 1];
	if (recv(fd, str, _size, 0) < 1)
		throw std::runtime_error("recv error");
	str[_size] = '\0';
	_body = str;
	std::cout << _body << std::endl;
	delete[] str;
}	
	
size_t		Chunk::getSize( void ) {

	return (_size);
}

std::string Chunk::getBody( void ) {

	return (_body);
}
	
	
Chunk::~Chunk(void) {

}
