// #include "Server.hpp"
// #include "Libraries.hpp"

// #ifndef USERAUTHENTICATION_HPP
// #define USERAUTHENTICATION_HPP

// struct UserData
// {
// 	std::string email;
// 	std::string password;
// 	std::string username;
// 	std::string cookie_login;
// };

// class Server;

// class UserAuthentication
// {
// public:

// 	UserAuthentication(Server *server);
// 	~UserAuthentication();

// 	void		saveUser(const std::string &queryString);
// 	void		loadUser();
// 	void		deleteUser(const std::string &username);

// 	UserAuthentication* getUserAuth();

// private:
// 	std::map<std::string, UserData> _users;
// 	std::string					_userDB_path;
// 	Server						*_server;
// };

// #endif //USERAUTHENTICATION_HPP