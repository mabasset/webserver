#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include <map>
# include <cstdlib>
# include <utility>
# include <stdint.h>
# include <sstream>
# include <dirent.h>

typedef	std::vector<std::string> 		sVec;
typedef	std::map<std::string, bool> 	sBMap;
typedef	std::pair<int, std::string> 	iSPair;
typedef std::map<int, std::string>		iSMap;

enum Directives {
	LISTEN,
	SERVER_NAME,
	ROOT,
	INDEX,
	ERROR_PAGE, 
	CLIENT_MAX_BODY_SIZE,
	ALLOWED_METHODS,
	LOCATION,
	AUTOINDEX,
	TRY_FILES,
	CGI_PASS,
	EXTENSION_CGI,
	RETURN
};

class Config {
	typedef	std::map<std::string, Config> sCMap;

	private:
		uint16_t	_listen;
		sVec		_server_name;
		std::string	_root;
		sVec		_index;
		iSMap		_error_page;
		size_t		_client_max_body_size;
		sBMap		_allowed_methods;
		bool		_autoindex;
		sVec		_try_files;
		std::string	_cgi_pass;
		sVec		_extension_cgi;
		iSPair		_return;

		sCMap		_locationMap;
		std::string	_locationName;

	public:

		Config(void);
		Config(std::string &serverBody);
		Config(std::string &locationBody, Config &mainConfig, const std::string &locationName);
		Config	&operator=(const Config &rhs);
		~Config(void);

		int			doDirective(std::string &line);
		void		addLocation(std::string &serverBody);
		void		displayConfig(void) const;

		void	setListen(std::string &configStr);
		void	setServerName(std::string &configStr);
		void	setRoot(std::string &configStr);
		void	setIndex(std::string &configStr);
		void	setErrorPage(std::string &configStr);
		void	setClientMaxBodySize(std::string &configStr);
		void	setAllowedMethods(std::string &configStr);
		void	setAutoindex(std::string &configStr);
		void	setTryFiles(std::string &configStr);
		void	setCgiPass(std::string &configStr);
		void	setExtensionCgi(std::string &configStr);
		void	setReturn(std::string &configStr);
		void	setLocationName(std::string &locationName);

		const uint16_t		&getListen(void) const;
		const sVec			&getServerName(void) const;
		const std::string	&getRoot(void) const;
		const sVec			&getIndex(void) const;
		const iSMap			&getErrorPage(void) const;
		const std::string	&getErrorPage(int err);
		const size_t		&getClientMaxBodySize(void) const;
		const sBMap			&getAllowedMethods(void) const;
		const bool			&getAutoindex(void) const;
		const sVec			&getTryFiles(void) const;
		const std::string	&getCgiPass(void) const;
		const sVec			&getExtensionCgi(void) const;
		const iSPair		&getReturn(void) const;
		const sCMap			&getLocationMap(void) const;
		const std::string	&getLocationName(void) const;

		struct badConfigFile : public std::exception {
			virtual const char *what() const throw();
		};
};

#endif
