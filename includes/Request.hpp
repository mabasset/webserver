#ifndef	REQUEST_HPP
# define REQUEST_HPP

# include "Server.hpp"
# include "Chunk.hpp"

typedef std::pair<std::string, Config> sCPair;

class Request {

	private:
		std::string	_method;
		std::string _uri;
		sSMap		_headers;
		Config		_location;
		std::string	_body;

		Request(void);

		bool	checkLocationName( const std::string &locationName ) const;
		Config	detectLocation( const sCMap &locationMap );

	public:
		Request(std::string &all, const sCMap &locationMap, const int fd );
		Request( const Request &src );
		Request	&operator=( const Request &rhs );
		~Request(void);

		static void	fixUri( std::string &uri );
		void		display( void ) const;

		std::string	getMethod( void ) const;
		std::string	getUri( void ) const;
		sSMap		getHeaders( void ) const;
		Config		getLocation( void ) const;
		std::string	getBody( void ) const;

		void	setMethod( const std::string &method );
		void	setUri( const std::string &uri );
		void	setHeaders( const sSMap &headers );
		void	setLocation( const Config &location );
		void	setBody( const std::string &body );
};

#endif
