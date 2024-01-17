#include "Window.h"
#include <fstream>
#include <thread>

int main()
{
	Window window(1280, 720, "Test");

	try {
		std::ifstream login_info;
		std::string login_details;
		std::string path = __FILE__;
		path.erase(path.find_last_of("/\\"));
		login_info.open(path + "/\\db_login.txt");
		std::getline(login_info, login_details);
		login_info.close();
		std::cout << login_details << std::endl;
		Baza baza("host=flora.db.elephantsql.com port=5432 dbname=hsyqexiy user=hsyqexiy password=tTWWg_ANr4mgZJC9xARLKBeAcfRkH9ej connect_timeout=10");

		while (window.Loop()) {
			window.Refresh();
		
			if (window.GetStatus() == Sesja::NIEZALOGOWANY)
				window.Logowanie(baza);
			else {
				window.Wyszukiwanie(baza);
				window.Dodawanie(baza);
				window.Dane();
			}
			window.Render();
		}
	}
	catch (std::exception const& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}