#include "../includes/Cluster.hpp"

Cluster::Cluster(const char *filePath) {
	std::ifstream	configFile;
	std::string		fileContent;
	sVec			serverBodyVec;
	sCMap			locationMap;

	if (filePath == NULL)
		filePath = "default.conf";
	configFile.open(filePath);
	if (!configFile.is_open())
		throw wrongFilePath();
	std::getline(configFile, fileContent, '\0');
	for(size_t pos = fileContent.find_first_of("#"); pos != std::string::npos; pos = fileContent.find_first_of("#"))
		fileContent.erase(pos, fileContent.find_first_of("\n", pos) - pos);
	serverBodyVec = this->divideByServer(fileContent);
	for(sVec::iterator it = serverBodyVec.begin(); it != serverBodyVec.end(); it++)
	{
		Config	config(*it);
		locationMap = config.getLocationMap();
		Server	server(config, locationMap);
		_serverVec.push_back(server);
	}
}

void	Cluster::startListening(void)
{
	while (1)
		for (std::vector<Server>::iterator it = _serverVec.begin(); it != _serverVec.end(); it++)
			it->makePoll();
}

sVec	Cluster::divideByServer(std::string &fileContent) {
	size_t	bodyLen;
	sVec	serverBodyVec;

	if (fileContent.find_first_not_of(" \t\n") < fileContent.find("server"))
		throw badConfigFile();
	else if (fileContent.find_first_not_of(" \t\n", (fileContent.find("server") + 6)) < fileContent.find_first_of("{"))
		throw badConfigFile();
	for (size_t begin = fileContent.find_first_of("{"); begin != std::string::npos; begin = fileContent.find_first_of("{", begin + bodyLen))
	{
		bodyLen = 0;
		for (int bracketsCount = 0; begin + bodyLen < fileContent.length();)
		{
			if (fileContent.at(begin + bodyLen) == '{')
				bracketsCount++;
			else if (fileContent.at(begin + bodyLen) == '}')
				bracketsCount--;
			bodyLen++;
			if (bracketsCount == 0)
				break ;
		}
		if (fileContent.at(begin + bodyLen - 1) != '}')
			throw badConfigFile();
		if (fileContent.find_first_not_of(" \n", begin + bodyLen) < fileContent.find("server", begin + bodyLen))
		 	throw badConfigFile();
		if (fileContent.find("server", begin + bodyLen) != std::string::npos &&
			fileContent.find_first_not_of(" \n", fileContent.find("server", begin + bodyLen) + 6) < fileContent.find_first_of("{", begin + bodyLen))
				throw badConfigFile();
		serverBodyVec.push_back(fileContent.substr(begin, bodyLen));
	}
	return serverBodyVec;
}


const char *Cluster::wrongFilePath::what() const throw() {
	return "Wrong file path";
}

const char *Cluster::badConfigFile::what() const throw() {
	return "Bad config file";
}

Cluster::~Cluster(void) {

}