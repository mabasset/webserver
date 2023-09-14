#ifndef	REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"

/*enum Method {
	GET,
	POST,
	DELETE,
	HEAD,
	PUT,
};*/

class Request {

	private:
		std::string	_method;
		std::string _uri;
		sSMap		_headers;
		Config		_location;

		Request(void);

		bool	checkLocationName( const std::string &locationName ) const;
		Config	detectLocation( const sCMap &locationMap );
	public:
		Request(std::string &all, const sCMap &locationMap );
		~Request(void);

		void	display( void ) const;

		std::string	getMethod( void ) const;
		std::string	getUri( void ) const;
		sSMap		getHeaders( void ) const;
		Config		getLocation( void ) const;

		void	setMethod( const std::string &method );
		void	setUri( const std::string &uri );
		void	setHeaders( const sSMap &headers );
		void	setLocation( const Config &location );
};

#endif
