// #include "../incl/Server.hpp"
// #include "../incl/UserAuthenticaton.hpp"
// #include "../incl/Libraries.hpp"

// UserAuthentication::UserAuthentication(Server *server) : _server(server)
// {
// 	std::string root = this->_server->getRoot();
// 	this->_userDB_path = root + "/userDB.csv";
	
// 	loadUser();

// 	std::cout << "User Authentication constructed\n";
// }

// UserAuthentication::~UserAuthentication()
// {
// 	std::cout << "User Authentication destructed\n";
// }

// void	UserAuthentication::saveUser(const std::string &queryString)
// {
// 	(void)queryString;
// }

// void	UserAuthentication::loadUser()
// {
// 	std::string email;
// 	std::string password;
// 	std::string username;

// 	std::ifstream file(this->_userDB_path.c_str());
	
// 	if (file.is_open())
// 	{
// 		std::string line;
// 		while (getline(file, line))
// 		{
// 			size_t commaPos1 = line.find_first_of(',');
// 			size_t commaPos2 = line.find_last_of(',');
			
// 			email = trim(line.substr(0, commaPos1));
// 			password = trim(line.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1));
// 			username = trim(line.substr(commaPos2 + 1));

// 			std::cout << email << " " << password << " " << username << '\n';

// 			//store userDB in UserData struct
// 			UserData user;
// 			user.email = email;
// 			user.password = password;
// 			user.username = username;
// 			this->_users[user.email] = user;
// 		}
// 	}
// 	file.close();
// }

// void	UserAuthentication::deleteUser(const std::string &username)
// {
// 	(void)username;
// }
