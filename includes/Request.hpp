#ifndef	REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"

typedef	std::map<std::string, std::string> sSMap;

typedef enum	_method {
	GET,
	POST,
	DELETE,
	HEAD,
	PUT,
}				 Method;

class Request {

	private:
		Method		_method;
		std::string _uri;
		sSMap       _headers;

		Request(void);

	public:
		Request(const std::string all);
		~Request(void);

		void display( void );

		Method		getMethod( void );
		std::string	getUri( void );
		sSMap		getHeaders( void );

		void	setMethod( Method method );
		void	setUri( std::string uri );
		void	getHeaders( sSMap headers );
};

#endif
