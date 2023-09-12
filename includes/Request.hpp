#ifndef	REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"

enum Method {
	GET,
	POST,
	DELETE,
	HEAD,
	PUT,
};

class Request {

	private:
		enum Method	_method;
		std::string _uri;
		sSMap		_headers;

		Request(void);

	public:
		Request(std::string &all);
		~Request(void);

		void display( void ) const;

		Method		getMethod( void ) const;
		std::string	getUri( void ) const;
		sSMap		getHeaders( void ) const;

		void	setMethod( const Method &method );
		void	setUri( const std::string &uri );
		void	setHeaders( const sSMap &headers );
};

#endif
