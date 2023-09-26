#ifndef	ERROR_HPP
# define ERROR_HPP

# include "Response.hpp"

class	Response;

class Error {

	private:
		Response	*_response;
		int			_code;

		Error( void );
		void		defaultErrorBody( void );

	public:
		Error( Response *response, const int &code );
		~Error( void );

		void		editResponse( std::string const &root, iSMap const &errorPageMap );

		Response	getResponse( void ) const;
		int			getCode( void ) const;

		void		setResponse( Response response );
		void		setCode( const int code );
};
#endif