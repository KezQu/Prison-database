#include "Window.h"

void ErrorFun(int error_code, const char* description) {
	std::cout << error_code << ": " << description << std::endl;
}

Window::Window(int w, int h, std::string title)
	:_title{ title },
	_io{ Window::CreateImGui() },
	_style{ ImGui::GetStyle() },
	_winSize{ w, h },
	vp{ ImGui::GetMainViewport() }
{
	glfwSetErrorCallback(ErrorFun);

	if (!glfwInit())
	{
		std::cout << "Unable to initialize GLFW, exiting\n";
		glfwTerminate();
		exit(-1);
	}
	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
	glfwSetWindowPos(_window, (1920 - w) / 2, (1080 - h) / 2);
	if (!_window)
	{
		std::cout << "Unable to initialize GLFW window, exiting\n";
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init("#version 140");

	ImGui::StyleColorsDark();
	_winFlags |= ImGuiWindowFlags_NoMove;
	_winFlags |= ImGuiWindowFlags_NoResize;
	_winFlags |= ImGuiWindowFlags_NoCollapse;
	_winFlags |= ImGuiWindowFlags_NoScrollbar;
	_winFlags |= ImGuiWindowFlags_AlwaysAutoResize;

	_style.WindowRounding = _style.ChildRounding = _style.GrabRounding = _style.FrameRounding = 10.0f;
}

Sesja::Status const& Window::GetStatus()
{
	return zu.status;
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(_window);
	glfwTerminate();
}

bool Window::Loop()
{
	return !glfwWindowShouldClose(_window) && close;
}

void Window::Refresh()
{
	glfwPollEvents();
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::Render()
{
	ImGui::Render();

	glClearColor(0,0,0,0); 
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	if (_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
	glfwSwapBuffers(_window);

	_winSize = { vp->Size.x, vp->Size.y };
}

void Window::Logowanie(Baza& baza)
{
	static bool proba{ false };
	static ImVec2 widSize = { 320, 160 };
	ImGui::SetNextWindowSize(widSize);
	ImGui::SetNextWindowPos({ vp->WorkPos.x + (vp->WorkSize.x - widSize.x) / 2 , vp->WorkPos.y + (vp->WorkSize.y - widSize.y) / 2 });
	ImGui::Begin("Logowanie", &close, _winFlags);
	ImGui::InputText("Identyfikator", &zu.id);
	ImGui::InputText("Klucz dostepu", &zu.haslo, ImGuiInputTextFlags_Password);

	if (ImGui::Button("Zaloguj sie"))
	{
		try {
			proba = false;
			std::string logowanie_zapytanie = "SELECT * FROM pracownicy_informacja pi WHERE pi.id_pracownik = " + (zu.id.length() ? zu.id : "0") + " AND pi.haslo = '" + zu.haslo + "';";
			auto uzytkownik_rekord = baza.Zaladuj_dane<Pracownik, PRACOWNIK_INFO> (logowanie_zapytanie);
		
			if (!uzytkownik_rekord.empty()) {
				zu = Sesja(uzytkownik_rekord.front());
			}
			else {
				std::string logowanie_zapytanie = "SELECT * FROM skazani_informacja s WHERE s.id_skazany = " + (zu.id.length() ? zu.id : "0") + " AND s.pesel = " + Skazany::Pesel_arr(zu.haslo) + ";";
				auto uzytkownik_rekord = baza.Zaladuj_dane<Skazany, SKAZANY_INFO>(logowanie_zapytanie);
				if (uzytkownik_rekord.empty()) {
					throw std::exception();
				}
				zu = Sesja(uzytkownik_rekord.front());
			}
		}
		catch (std::exception& e) {
			proba = true;
		}
	}
	if (ImGui::Button("Kontynuuj jako wizytator"))
		zu.status = Sesja::WIZYTATOR;
	if(proba){
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Text("Bledne dane do logowania");
		ImGui::PopStyleColor(1);
	}
	ImGui::End();
}

void Window::Dodawanie(Baza& baza)
{
	static std::string opcja{"##zapelnijDodawanie"};
	static auto zapelnij = [&]() {};
	static auto d_zajecia = [&]() {
		opcja = "Dodaj zajecia";
		static bool odswiez{ false };
		static std::tuple<ZAJECIA_INFO> zajecia_rekord;
		static std::string log{};
		if (odswiez) {
			odswiez = false;
			Baza::resetuj_formularz(zajecia_rekord);
		}
		ImGui::InputText("Nazwa zajec", &std::get<1>(zajecia_rekord));
		ImGui::InputTextMultiline("Opis zajec", &std::get<2>(zajecia_rekord), ImVec2(0, ImGui::GetFontSize() * 10));
		if (ImGui::Button("Stworz zajecia")) {
			log = {};
			std::get<0>(zajecia_rekord) = Obiekt::id_szukanie(baza, "SELECT id_zajecia FROM zajecia_resocjalizacyjne zr ORDER BY id_zajecia;");
			std::get<3>(zajecia_rekord) = std::atoi(zu.id.c_str());
			Zajecia nowe_zajecia(zajecia_rekord);
			log = nowe_zajecia.Wstaw(baza);
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Zajecia dodane pomyslnie");
			ImGui::PopStyleColor(1);
		}
		};
	static auto d_skazany = [&]() {
		opcja = "Dodaj wieznia";
		static bool odswiez{ false };
		static std::tuple<SKAZANY_WSTAW> skazany_rekord;
		static auto zbrodnie = PobierzWybor(baza, "SELECT nazwa_zbrodni FROM zbrodnia ORDER BY id_zbrodnia;");
		static std::string log;
		static int identyfikator{};
		static std::string haslo;
		if (odswiez) {
			odswiez =  false;
			zbrodnie = PobierzWybor(baza, "SELECT nazwa_zbrodni FROM zbrodnia ORDER BY id_zbrodnia;");
			Baza::resetuj_formularz(skazany_rekord);
		}
		ImGui::InputText("Imie", &std::get<2>(skazany_rekord));
		ImGui::InputText("Nazwisko", &std::get<3>(skazany_rekord));
		ImGui::InputText("PESEL", &std::get<1>(skazany_rekord));
		ImGui::InputInt("Numer celi", &std::get<4>(skazany_rekord));
		ImGui::Combo("Zbrodnia", &std::get<5>(skazany_rekord), zbrodnie, zbrodnie.size());
		ImGui::Text("Opis zbrodni:");
		ImGui::InputTextMultiline(" ", &std::get<8>(skazany_rekord), ImVec2(0, ImGui::GetFontSize() * 10));
		if (ImGui::Button("Zatwierdz dane", { ImGui::GetWindowWidth() - 17, ImGui::GetFontSize() * 2 }))
		{
			log = {};
			std::get<0>(skazany_rekord) = Skazany::id_szukanie(baza, "SELECT id_skazany FROM skazany ORDER BY id_skazany;");
			Skazany nowy_skazany(skazany_rekord);
			log = nowy_skazany.Wstaw(baza);
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
				identyfikator = std::get<0>(skazany_rekord);
				haslo = std::get<1>(skazany_rekord);
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Dodano skazanego -- ID: %d, haslo: %s", identyfikator, haslo.c_str());
			ImGui::PopStyleColor(1);
		}
	};
	static auto d_pracownik = [&]() {
		opcja = "Dodaj pracownika";
		static bool odswiez{ false };
		static std::tuple<PRACOWNIK_WSTAW> pracownik_rekord;
		static std::string log{};
		static int identyfikator{};
		static std::string haslo;
		static auto blok = PobierzWybor(baza, "SELECT nazwa_bloku FROM blok ORDER BY id_blok;");;
		static char const* stanowisko[] = { "", "Straznik", "Instruktor" };
		if (odswiez) {
			odswiez = false;
			Baza::resetuj_formularz(pracownik_rekord);
		}
		ImGui::InputText("Imie", &std::get<2>(pracownik_rekord));
		ImGui::InputText("Nazwisko", &std::get<3>(pracownik_rekord));
		ImGui::InputText("PESEL", &std::get<1>(pracownik_rekord));
		ImGui::InputText("Haslo", &std::get<6>(pracownik_rekord), ImGuiInputTextFlags_Password);
		static int wybor_stanowiska{};
		ImGui::Combo("Stanowisko", &wybor_stanowiska, stanowisko, IM_ARRAYSIZE(stanowisko));
		std::get<4>(pracownik_rekord) = stanowisko[wybor_stanowiska];
		static int wybor_bloku{};
		switch (wybor_stanowiska)
		{
		case 1:
			if (odswiez) blok = PobierzWybor(baza, "SELECT nazwa_bloku FROM blok ORDER BY id_blok;");
			ImGui::Combo("Blok", &wybor_bloku, blok, blok.size());
			std::get<8>(pracownik_rekord) = blok[wybor_bloku];
			ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
			ImGui::Text("Godzina rozpoczecia zmiany");
			ImGui::SameLine();
			ImGui::InputText("##godzina_start", &std::get<9>(pracownik_rekord));

			ImGui::Text("Godzina zakonczenia zmiany");
			ImGui::SameLine();
			ImGui::InputText("##godzina_koniec", &std::get<10>(pracownik_rekord));
			ImGui::PopItemWidth();
			break;
		default:
			break;
		}

		if (ImGui::Button("Zatwierdz dane", { ImGui::GetWindowWidth() - 17, ImGui::GetFontSize() * 2 }))
		{
			log = {};
			std::get<0>(pracownik_rekord) = Pracownik::id_szukanie(baza, "SELECT id_pracownik FROM pracownik ORDER BY id_pracownik;");
			std::get<5>(pracownik_rekord) = std::get<0>(*baza.query<int>("SELECT id_wiezienie FROM pracownik WHERE id_pracownik = " + zu.id + ";").begin());
			std::get<7>(pracownik_rekord) = std::get<0>(*baza.query<int>("SELECT id_blok FROM blok WHERE nazwa_bloku = '" + std::get<8>(pracownik_rekord) + "';").begin());
			Pracownik nowy_pracownik(pracownik_rekord);
			log = nowy_pracownik.Wstaw(baza);
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
				identyfikator = std::get<0>(pracownik_rekord);
				haslo = std::get<6>(pracownik_rekord);
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Dodano pracownika -- ID: %d, haslo: %s", identyfikator, haslo.c_str());
			ImGui::PopStyleColor(1);
		}};
	static auto d_blok = [&]() {
		opcja = "Dodaj blok";
		static bool odswiez{ false };
		static std::tuple<std::string, int> blok_rekord;
		static std::string log{};
		if (odswiez) {
			odswiez = false;
			Baza::resetuj_formularz(blok_rekord);
		}
		ImGui::InputText("Nazwa bloku", &std::get<0>(blok_rekord));
		ImGui::InputInt("Ilosc cel", &std::get<1>(blok_rekord));
		std::get<1>(blok_rekord) = std::get<1>(blok_rekord) <= 0 ? 1 : std::get<1>(blok_rekord);
		if (ImGui::Button("Stworz blok")) {
			log = {};
			auto id_wiezienie = baza.query<int>("SELECT id_wiezienie FROM pracownik p WHERE p.id_pracownik = " + zu.id + ";");
			try {
				auto id_blok = Obiekt::id_szukanie(baza, "SELECT id_blok FROM blok b ORDER BY id_blok;");
				baza.query("INSERT INTO blok VALUES (" +
					std::to_string(id_blok) +
					", " + (std::get<0>(blok_rekord).length() == 0 ? "NULL" : +"'" + std::get<0>(blok_rekord) + "'") +
					", " + std::to_string(std::get<0>(*id_wiezienie.begin())) + ");");
				for (int i = 0; i < std::get<1>(blok_rekord); i++) {
					auto tmp = "INSERT INTO cela VALUES(" +
						std::to_string(Obiekt::id_szukanie(baza, "SELECT id_cela FROM cela c ORDER BY id_cela;")) +
						", " + std::to_string(id_blok) + ");";
					baza.query(tmp);
				}

			}
			catch (std::exception& e) {
				log = e.what();
			}
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Blok dodany pomyslnie");
			ImGui::PopStyleColor(1);
		}
	};
	static auto d_praca = [&]() {
		opcja = "Dodaj prace dla skazanych";
		static bool odswiez{ false };
		static std::tuple<int, std::string, int, int> praca_rekord;
		static std::string log{};
		if (odswiez) {
			odswiez = false;
			Baza::resetuj_formularz(praca_rekord);
		}
		ImGui::InputText("Nazwa pracy", &std::get<1>(praca_rekord));
		ImGui::InputInt("Wymiar godzin", &std::get<2>(praca_rekord));
		if (ImGui::Button("Stworz miejsce pracy")) {
			log = {};
			try {
				std::get<0>(praca_rekord) = Obiekt::id_szukanie(baza, "SELECT id_praca FROM praca p ORDER BY id_praca;");
				std::get<3>(praca_rekord) = std::get<0>(*baza.query<int>("SELECT id_wiezienie FROM pracownik WHERE id_pracownik = " + zu.id).begin());
				baza.query("INSERT INTO praca VALUES (" +
					std::to_string(std::get<0>(praca_rekord)) +
					", " + (std::get<1>(praca_rekord).length() == 0 ? "NULL" : "'" + std::get<1>(praca_rekord) + "'") +
					", " + (std::get<2>(praca_rekord) == 0 ? "NULL" : std::to_string(std::get<2>(praca_rekord))) +
					", " + std::to_string(std::get<3>(praca_rekord)) + ");");
			}
			catch (std::exception& e) {
				log = e.what();
			}
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Praca dodana pomyslnie");
			ImGui::PopStyleColor(1);
		}
	};
	static auto e_zajecia = [&]() {
		opcja = "Edytuj zajecia";
		static bool odswiez{ false };
		static Zajecia zajecia_rekord;
		static auto zajecia = PobierzWybor(baza, "SELECT zr.nazwa_zajec FROM zajecia_resocjalizacyjne zr WHERE zr.id_pracownik = " + zu.id + " ORDER BY zr.id_zajecia;");;
		static std::string log{};
		if (odswiez) {
			zajecia = PobierzWybor(baza, "SELECT zr.nazwa_zajec FROM zajecia_resocjalizacyjne zr WHERE zr.id_pracownik = " + zu.id + " ORDER BY zr.id_zajecia;");
			zajecia_rekord = {};
		}
		static int zajecia_wybor{};
		ImGui::Combo("Wybierz zajecia", &zajecia_wybor, zajecia, zajecia.size());
		if (zajecia_rekord.nazwa_zajec != zajecia[zajecia_wybor] || odswiez) {
			odswiez = false;
			zajecia_rekord.nazwa_zajec = zajecia[zajecia_wybor];
			zajecia_rekord.id = std::get<0>(*baza.query<int>("SELECT id_zajecia FROM zajecia_resocjalizacyjne WHERE nazwa_zajec = '" + zajecia_rekord.nazwa_zajec + "' AND id_pracownik = " + zu.id + ";").begin());
			zajecia_rekord = baza.Zaladuj_dane<Zajecia, ZAJECIA_INFO>("SELECT * FROM zajecia_informacja zr WHERE zr.id_zajecia = " + std::to_string(zajecia_rekord.id) + ";").front();
			zajecia_rekord.grupa_vec = baza.Zaladuj_dane<Grupa, GRUPA_INFO>("SELECT * FROM grupa_zajeciowa gz WHERE id_zajecia = " + std::to_string(zajecia_rekord.id) + ";");
		}
		ImGui::InputTextMultiline("Opis zajec", &zajecia_rekord.opis_zajec, ImVec2(0, ImGui::GetFontSize() * 10));
		for (auto& grupa : zajecia_rekord.grupa_vec) {
			ImGui::Separator();
			ImGui::Text("%s: grupa nr %d", zajecia_rekord.nazwa_zajec.c_str(), grupa.id);
			ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
			ImGui::Text("Godzina rozpoczecia zajec");
			ImGui::SameLine();
			ImGui::InputText(("##rozpoczecie" + std::to_string(grupa.id)).c_str(), &grupa.zajecia_od);
			ImGui::Text("Godzina zakonczenia zajec");
			ImGui::SameLine();
			ImGui::InputText(("##zakonczenie" + std::to_string(grupa.id)).c_str(), &grupa.zajecia_do);
			ImGui::PopItemWidth();
			ImGui::Separator();
		}
		if (zajecia_rekord.id != 0 && ImGui::Button("Dodaj grupe")) {
			log = {};
			Grupa nowa_grupa(Obiekt::id_szukanie(baza, "SELECT id_grupa FROM grupa_zajeciowa ORDER BY id_grupa;"), zajecia_rekord.id);
			log = nowa_grupa.Wstaw(baza);
			log = log.length() ? log : "ok_grupa";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok_grupa") {
				odswiez = true;
				zaktualizuj_dane= true;
			}
		}
		if (zajecia_rekord.id != 0 && ImGui::Button("Zatwierdz edycje zajec")) {
			log = {};
			log = zajecia_rekord.Edytuj(baza);
			log = log.length() ? log : "ok";
			std::cout << log << std::endl;
			for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			if (log == "ok") {
				odswiez = true;
				zaktualizuj_dane = true;
			}
		}
		if (log != "ok" && log != "ok_grupa") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else if (log == "ok_grupa") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Grupa dodana pomyslnie");
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Zajecia zaktualizowane pomyslnie");
			ImGui::PopStyleColor(1);
		}
		};
	static auto z_na_zajecia = [&]() {
		opcja = "Zapisz sie na zajecia";
		static bool odswiez{ false };
		static Zajecia zajecia_rekord;
		static auto zajecia = PobierzWybor(baza, "SELECT zr.nazwa_zajec FROM zajecia_resocjalizacyjne zr ORDER BY zr.id_zajecia;");
		static std::string log{};
		if (odswiez) {
			zajecia = PobierzWybor(baza, "SELECT zr.nazwa_zajec FROM zajecia_resocjalizacyjne zr ORDER BY zr.id_zajecia;");
			zajecia_rekord = {};
		}
		static int zajecia_wybor{};
		ImGui::Combo("Wybierz zajecia", &zajecia_wybor, zajecia, zajecia.size());
		if (zajecia_rekord.nazwa_zajec != zajecia[zajecia_wybor] || odswiez) {
			odswiez = false;
			zajecia_rekord.nazwa_zajec = zajecia[zajecia_wybor];
			zajecia_rekord.id = std::get<0>(*baza.query<int>("SELECT id_zajecia FROM zajecia_resocjalizacyjne WHERE nazwa_zajec = '" + zajecia_rekord.nazwa_zajec + "' AND id_pracownik = " + zu.id + ";").begin());
			zajecia_rekord = baza.Zaladuj_dane<Zajecia, ZAJECIA_INFO>("SELECT * FROM zajecia_informacja zr WHERE zr.id_zajecia = " + std::to_string(zajecia_rekord.id) + ";").front();
			zajecia_rekord.grupa_vec = baza.Zaladuj_dane<Grupa, GRUPA_INFO>("SELECT * FROM grupa_zajeciowa gz WHERE id_zajecia = " + std::to_string(zajecia_rekord.id) + ";");
		}
		ImGui::Text("Opis zajec: ");
		ImGui::Text(zajecia_rekord.opis_zajec.c_str());
		for (auto& grupa : zajecia_rekord.grupa_vec) {
			ImGui::Separator();
			ImGui::Text("%s: grupa nr %d", zajecia_rekord.nazwa_zajec.c_str(), grupa.id);
			ImGui::Text("Godzina rozpoczecia zajec: %s", grupa.zajecia_od.c_str());
			ImGui::Text("Godzina zakonczenia zajec: %s", grupa.zajecia_do.c_str());
			if (ImGui::Button(("Zapisz sie do grupy ##" + std::to_string(grupa.id)).c_str())) {
				log = {};
				try {
					baza.query("INSERT INTO skazany_grupa VALUES(" +
						zu.id + "," +
						std::to_string(grupa.id) + ");");
				}
				catch (std::exception& e) {
					log = e.what();
				}
				log = log.length() ? log : "ok";
				std::cout << log << std::endl;
				for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
				if (log == "ok") {
					odswiez = true;
					zaktualizuj_dane = true;
				}
			}
			ImGui::Separator();
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Pomyslnie zapisano na zajecia");
			ImGui::PopStyleColor(1);
		}
	};
	static auto z_do_pracy = [&]() {
		opcja = "Zapisz sie do pracy";
		static bool odswiez{ false };
		using Praca = std::tuple<int, std::string, int>;
		static std::vector<Praca> praca_vec = baza.Zaladuj_dane< std::tuple<int, std::string, int>, int, std::string, int>("SELECT * FROM praca p;");
		static std::string log{};
		if (odswiez) {
			odswiez = false;
			praca_vec = baza.Zaladuj_dane<Praca, int, std::string, int>("SELECT * FROM praca p;");
		}
		for (auto& praca : praca_vec) {
			ImGui::Separator();
			ImGui::Text("Praca nr %d", std::get<0>(praca));
			ImGui::Text("Nazwa pracy: %s", std::get<1>(praca).c_str());
			ImGui::Text("Wymiar godzin w tygodniu: %d", std::get<2>(praca));
			if (ImGui::Button(("Zapisz sie do pracy ##" + std::to_string(std::get<0>(praca))).c_str())) {
				log = {};
				try {
					baza.query("INSERT INTO praca_skazany VALUES(" +
						std::to_string(std::get<0>(praca)) + "," +
						zu.id + ");");
				}
				catch (std::exception& e) {
					log = e.what();
				}
				log = log.length() ? log : "ok";
				std::cout << log << std::endl;
				for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
				if (log == "ok") {
					odswiez = true;
					zaktualizuj_dane = true;
				}
			}
		}
		ImGui::Separator();
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Pomyslnie zapisno do pracy");
			ImGui::PopStyleColor(1);
		}
		};
	static auto z_na_wizyte = [&]() {
		opcja = "Zapisz sie na wizyte";
		static bool odswiez{ false };
		static std::string log{};
		static std::tuple<WIZYTATOR_WSTAW> wizytator_rekord;
		static auto numery_skazanych = PobierzWybor(baza, "SELECT id_skazany FROM skazany ORDER BY id_skazany;");
		static std::vector<char const*> miesiac = { "sty", "lut" , "mar" , "kwi" , "maj" , "cze" , "lip" , "sie" , "wrz" , "paz", "lis", "gru" };
		std::vector<char const*> dzien = {
			"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
			"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
			"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31" };
		static int skazany_wybor{};
		static int miesiac_wybor{};
		static int dzien_wybor{};
		static int godzina_wybor{};
		if (odswiez) {
			odswiez = false;
			Baza::resetuj_formularz(wizytator_rekord);
			numery_skazanych = PobierzWybor(baza, "SELECT id_skazany FROM skazany ORDER BY id_skazany;");
			skazany_wybor = {};
			dzien_wybor = {};
			miesiac_wybor = {};
			godzina_wybor = {};
		}
		ImGui::Text("W celu zapisania sie na wizyte,\nwypelnij ponizszy formularz");
		ImGui::InputText("Imie", &std::get<2>(wizytator_rekord));
		ImGui::InputText("Nazwisko", &std::get<3>(wizytator_rekord));
		ImGui::InputText("Pesel", &std::get<1>(wizytator_rekord));
		ImGui::Text("Wybierz numer skazanego");
		ImGui::Combo("##numer_skazanego", &skazany_wybor, numery_skazanych, numery_skazanych.size());
		if (skazany_wybor != 0) {
			ImGui::Text("Wybierz dzien");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
			ImGui::Combo("##dzien", &dzien_wybor, dzien.data(), (miesiac_wybor + 1) % 2 == 1 ? 31 : ((miesiac_wybor + 1) == 2 ? 28 : 30));
			ImGui::PopItemWidth();
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
			ImGui::Combo("##miesiac", &miesiac_wybor, miesiac.data(), miesiac.size());
			static std::string dzien_widzenia = "2023-" + std::to_string(miesiac_wybor + 1) + "-" + std::to_string(dzien_wybor + 1);
			ImGui::PopItemWidth();
			std::vector<char const*> wolne_godziny = { " ", "14:00", "15:00", "16:00", "17:00" };
			static auto zajete_godziny = PobierzWybor(baza, "SELECT godzina_widzenia FROM skazany_wizytator WHERE id_skazany = " + std::to_string(skazany_wybor) + " AND dzien_widzenia = '" + dzien_widzenia + "';");
			if (dzien_widzenia != "2023-" + std::to_string(miesiac_wybor + 1) + "-" + std::to_string(dzien_wybor + 1)) {
				dzien_widzenia = "2023-" + std::to_string(miesiac_wybor + 1) + "-" + std::to_string(dzien_wybor + 1);
				zajete_godziny = PobierzWybor(baza, "SELECT godzina_widzenia FROM skazany_wizytator WHERE id_skazany = " + std::to_string(skazany_wybor) + " AND dzien_widzenia = '" + dzien_widzenia + "';");
			}
			if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "14:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "14:00"));
			if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "15:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "15:00"));
			if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "16:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "16:00"));
			if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "17:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "17:00"));
			if (odswiez) {
				wolne_godziny = { " ", "14:00", "15:00", "16:00", "17:00" };
				zajete_godziny = PobierzWybor(baza, "SELECT godzina_widzenia FROM skazany_wizytator WHERE id_skazany = " + std::to_string(skazany_wybor) + ";");
				if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "14:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "14:00"));
				if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "15:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "15:00"));
				if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "16:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "16:00"));
				if (std::find(zajete_godziny.begin(), zajete_godziny.end(), "17:00:00") != zajete_godziny.end()) wolne_godziny.erase(std::find(wolne_godziny.begin(), wolne_godziny.end(), "17:00"));
			}
			ImGui::Text("Wybierz godzine");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
			ImGui::Combo("##godzina", &godzina_wybor, wolne_godziny.data(), wolne_godziny.size());
			ImGui::PopItemWidth();
			if (godzina_wybor != 0 && ImGui::Button(("Zapisz sie na wizyte ##" + std::to_string(skazany_wybor)).c_str())) {
				log = {};
				Wizytator nowy_wizytator;
				try {
					auto wizytator_vec = baza.Zaladuj_dane<Wizytator, WIZYTATOR_INFO>("SELECT * FROM wizytator WHERE pesel = " + Osoba::Pesel_arr(std::get<1>(wizytator_rekord)) + " AND imie = '" + std::get<2>(wizytator_rekord) + "';");
					if (!wizytator_vec.empty())
						nowy_wizytator = wizytator_vec.front();
					else {
						std::get<0>(wizytator_rekord) = Obiekt::id_szukanie(baza, "SELECT id_wizytator FROM wizytator ORDER BY id_wizytator");
						nowy_wizytator = { wizytator_rekord };
						log = nowy_wizytator.Wstaw(baza);
					}
					log = log.length() ? log : "ok";
					std::cout << log << std::endl;
					for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
					if (log == "ok") {
						baza.query("INSERT INTO skazany_wizytator VALUES(" +
							std::to_string(skazany_wybor) + 
							", " + std::to_string(nowy_wizytator.id) +
							", '" + dzien_widzenia + "'" +
							", '" + wolne_godziny[godzina_wybor] + "');");
						zaktualizuj_dane = true;
						odswiez = true;
					}
				}
				catch (std::exception& e) {
					log = e.what();
				}
				log = log.length() ? log : "ok";
				std::cout << log << std::endl;
				for (int i = 43; i < log.length(); i += 43) { log.insert(i, "\n"); }
			}
		}
		if (log != "ok") {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			ImGui::Text(log.c_str());
			ImGui::PopStyleColor(1);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::Text("Wizyta dodana pomyslnie");
			ImGui::PopStyleColor(1);
		}
	};
	static std::function<void()> okno_kontekstowe{ zapelnij };
	if (zu.status != Sesja::NIEZALOGOWANY) {
		ImGui::SetNextWindowPos(vp->WorkPos);
		ImGui::SetNextWindowSize({ 350, 0.7f * _winSize.second });
		ImGui::Begin(opcja.c_str(), &close, _winFlags);
		switch (zu.status) {
	case Sesja::WIZYTATOR:
		okno_kontekstowe = z_na_wizyte;
		break;
	case Sesja::STRAZNIK:
		okno_kontekstowe = d_skazany;
		break;
	case Sesja::INSTRUKTOR:
		if (ImGui::Button("Dodaj zajecia")) {
			okno_kontekstowe = d_zajecia;
		}
		ImGui::SameLine();
		if (ImGui::Button("Edytuj zajecia")) {
			okno_kontekstowe = e_zajecia;
		}
		break;
	case Sesja::NACZELNIK:
		if (ImGui::Button("Dodaj skazanego")) {
			okno_kontekstowe = d_skazany;
		}
		ImGui::SameLine();
		if (ImGui::Button("Dodaj pracownika")) {
			okno_kontekstowe = d_pracownik;
		}
		ImGui::SameLine();
		if (ImGui::Button("Dodaj blok")) {
			okno_kontekstowe = d_blok;
		}
		if (ImGui::Button("Dodaj miejsce pracy")) {
			okno_kontekstowe = d_praca;
		}
		break;
	case Sesja::SKAZANY:
		if (ImGui::Button("Zapisz sie na zajecia")) {
			okno_kontekstowe = z_na_zajecia;
		}
		ImGui::SameLine();
		if (ImGui::Button("Zapisz sie do pracy")) {
			okno_kontekstowe = z_do_pracy;
		}
		break;
	default:
		break;
	}
		okno_kontekstowe();
		ImGui::End();
	}
	else {
		okno_kontekstowe = zapelnij;
		opcja = "##zapelnijDodawanie";
	}
}

void Window::Wyszukiwanie(Baza& baza)
{
	static int wybrany = -1;
	static std::string opcja{"##zapelnijWyszukanie"};
	static auto zapelnij = [&]() {};
	static auto filtry = [&](std::string zapytanie, char const* kolumny, auto parse) {
		static int wybor{};
		static int kierunek{};
		ImGui::PushItemWidth(130);
		ImGui::Text("Sortuj po");
		ImGui::SameLine();
		ImGui::Combo("##Sortuj", &wybor, kolumny);
		ImGui::SameLine();
		ImGui::Text("Kierunek sortowania");
		ImGui::SameLine();
		ImGui::Combo("##Kierunek", &kierunek, "Rosnaco\0Malejaco");
		ImGui::PopItemWidth();
		parse(zapytanie, wybor);
		zapytanie += kierunek == 0 ? "ASC;" : "DESC;";
		static std::string zapytanie_cache;
		if (zapytanie != zapytanie_cache)
			zaktualizuj_dane = true;
		zapytanie_cache = zapytanie;
		return zapytanie;
		};
	static auto s_skazany = [&]() {
		opcja = "Wyszukaj skazanego";
		std::string zapytanie = filtry("SELECT * FROM skazani_informacja ORDER BY ", "Domyslne\0Data urodzenia\0Numer celi\0Zbrodnia", [&](std::string& zapytanie, int& wybor) {
			switch (wybor)
			{
			case 0:
				zapytanie += "id_skazany ";
				break;
			case 1:
				zapytanie += "data_urodzenia ";
				break;
			case 2:
				zapytanie += "id_cela ";
				break;
			case 3:
				zapytanie += "nazwa_zbrodni ";
				break;
			default:
				break;
			}});
		static std::vector<Skazany> skazani_vec = baza.Zaladuj_dane<Skazany, SKAZANY_INFO>(zapytanie);
		if (zaktualizuj_dane) {
			skazani_vec.clear();
			skazani_vec = baza.Zaladuj_dane<Skazany, SKAZANY_INFO>(zapytanie);
			wybrany = -1;
		}
		for (int i = 0; i < skazani_vec.size(); i++) {
			auto& akt_skazany = skazani_vec[i];
			auto etykieta = akt_skazany.imie + " " + akt_skazany.nazwisko + "##" + std::to_string(akt_skazany.id);
			if (ImGui::Selectable(etykieta.c_str(), wybrany == i))
				wybrany = i;
			if (wybrany == i)
			{
				ImGui::Indent(10.f);
				static auto wWidth = ImGui::GetWindowWidth();
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 50. / 255., 100. / 255., 0.7));
				ImGui::BeginChild("Szczegoly", ImVec2(wWidth - 25.f, 205), false, _winFlags | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("Identyfikator: %d", akt_skazany.id);
				if (zu.status != Sesja::WIZYTATOR)
					ImGui::Text("PESEL: %s", akt_skazany.pesel.c_str());
				ImGui::Text("Data urodzenia: %s", akt_skazany.data_urodzenia.c_str());
				ImGui::Text("Numer celi: %d", akt_skazany.id_cela);
				ImGui::Separator();
				ImGui::Text("Zbrodnia: %s", akt_skazany.nazwa_zbrodni.c_str());
				ImGui::Text("Dlugosc pozbawienia wolnosci: %s", akt_skazany.dlugosc_pozbawienia_wolnosci.c_str());
				ImGui::Text("Opis zbrodni: %s", akt_skazany.opis_zbrodni.c_str());
				ImGui::Separator();
				ImGui::Text("Wykonywana praca: %s", akt_skazany.nazwa_pracy.c_str());
				ImGui::Text("Wymiar godzin pracy: %d", akt_skazany.wymiar_godzin);
				ImGui::Separator();
				if (akt_skazany.zajecia_vec.second.empty() && akt_skazany.ilosc_zajec > 0) {
					akt_skazany.zajecia_vec.second = baza.Zaladuj_dane<Grupa, GRUPA_INFO>("SELECT id_grupa, id_zajecia, zajecia_od, zajecia_do FROM zajecia_zapisani_skazani WHERE id_skazany = " + std::to_string(akt_skazany.id) + ";");
					akt_skazany.zajecia_vec.first = baza.Zaladuj_dane<std::tuple<std::string>, std::string>("SELECT nazwa_zajec FROM zajecia_zapisani_skazani WHERE id_skazany = " + std::to_string(akt_skazany.id) + ";");
				}
				for (int i = 0; i < akt_skazany.zajecia_vec.second.size(); i++) {
					ImGui::Separator();
					ImGui::Text("Uczeszczane zajecia: %s", std::get<0>(akt_skazany.zajecia_vec.first[i]).c_str());
					ImGui::Text("Numer grupy zajeciowej: %d", akt_skazany.zajecia_vec.second[i].id);
					ImGui::Text("Czas rozpoczecia zajec: %s", akt_skazany.zajecia_vec.second[i].zajecia_od.c_str());
					ImGui::Text("Czas zakonczenia zajec: %s", akt_skazany.zajecia_vec.second[i].zajecia_do.c_str());
					ImGui::Separator();
				}
				if (zu.status != Sesja::WIZYTATOR) {
					if(akt_skazany.wizytacje_vec.empty() && akt_skazany.ilosc_wizytacji > 0)
						akt_skazany.wizytacje_vec = baza.Zaladuj_dane<std::tuple<std::string, std::string>, std::string, std::string>("SELECT dzien_widzenia, godzina_widzenia FROM skazany_wizytator WHERE id_skazany = " + std::to_string(akt_skazany.id) + " ORDER BY dzien_widzenia, godzina_widzenia;");
					for (auto& wizytacja : akt_skazany.wizytacje_vec) {
						ImGui::Separator();
						ImGui::Text("Dzien wizytacji: %s", std::get<0>(wizytacja).c_str());
						ImGui::Text("Godzina wizytacji: %s", std::get<1>(wizytacja).c_str());
						ImGui::Separator();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::Unindent(10.f);
			}
		}
		};
	static auto s_pracownik = [&]() {
			opcja = "Wyszukaj pracownika";
			std::string zapytanie = filtry("SELECT * from pracownicy_informacja pi WHERE pi.id_wiezienie = \
(SELECT id_wiezienie FROM pracownik p WHERE p.id_pracownik = " + zu.id + ") ORDER BY ", "Domyslne\0Data urodzenia\0Nazwa bloku\0Ilosc zajec\0Stanowisko", [&](std::string& zapytanie, int& wybor) {
					switch (wybor)
					{
					case 0:
						zapytanie += "id_pracownik ";
						break;
					case 1:
						zapytanie += "data_urodzenia ";
						break;
					case 2:
						zapytanie += "nazwa_bloku ";
						break;
					case 3:
						zapytanie += "ilosc_zajec ";
						break;
					case 4:
						zapytanie += "stanowisko ";
						break;
					default:
						break;
					}});
			static std::vector<Pracownik> pracownicy_vec = baza.Zaladuj_dane<Pracownik, PRACOWNIK_INFO>(zapytanie);
			if (zaktualizuj_dane) {
				pracownicy_vec.clear();
				pracownicy_vec = baza.Zaladuj_dane<Pracownik, PRACOWNIK_INFO>(zapytanie);
				wybrany = -1;
			}
			for (int i = 0; i < pracownicy_vec.size(); i++) {
				auto& akt_pracownik = pracownicy_vec[i];
				
				auto etykieta = 
					akt_pracownik.imie + " " + 
					akt_pracownik.nazwisko + " " +
					akt_pracownik.stanowisko + "##" +
					std::to_string(akt_pracownik.id);

				if (ImGui::Selectable(etykieta.c_str(), wybrany == i))
					wybrany = i;
				if (wybrany == i)
				{
					ImGui::Indent(10.f);
					static auto wWidth = ImGui::GetWindowWidth();
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 50. / 255., 100. / 255., 0.7));
					ImGui::BeginChild("Szczegoly", ImVec2(wWidth - 25.f, 100));
					ImGui::Text("Identyfikator: %d", akt_pracownik.id);
					ImGui::Text(("PESEL: " + akt_pracownik.pesel).c_str());
					ImGui::Text(("Data urodzenia: " + akt_pracownik.data_urodzenia).c_str());
					if (akt_pracownik.stanowisko == "Straznik") {
						ImGui::Text(("Nazwa bloku: " + akt_pracownik.nazwa_bloku).c_str());
						ImGui::Text(("Godzina rozpoczecia zmiany: " + akt_pracownik.czas_pracy_od).c_str());
						ImGui::Text(("Godzina zakonczenia zmiany: " + akt_pracownik.czas_pracy_do).c_str());
					}
					if (akt_pracownik.stanowisko == "Instruktor") {
						if(akt_pracownik.zajecia.empty() && akt_pracownik.ilosc_zajec > 0)
							akt_pracownik.zajecia = baza.Zaladuj_dane<Zajecia, ZAJECIA_INFO>("SELECT * FROM zajecia_informacja WHERE id_pracownik = " + std::to_string(akt_pracownik.id) + ";");
						for (int i = 0; i < akt_pracownik.zajecia.size(); i++) {
							if (i == 0 || akt_pracownik.zajecia[i-1].nazwa_zajec != akt_pracownik.zajecia[i].nazwa_zajec) {
								ImGui::Text(("Nazwa zajec: " + akt_pracownik.zajecia[i].nazwa_zajec).c_str());
								ImGui::Text(("	Opis zajec: " + akt_pracownik.zajecia[i].opis_zajec).c_str());
							}
							//if(akt_pracownik.zajecia[i].grupa_vec.empty())
							//	akt_pracownik.zajecia[i].grupa_vec = baza.Zaladuj_dane<Grupa, GRUPA_INFO>("SELECT * FROM grupa_zajeciowa gz WHERE gz.id_zajecia = " + std::to_string(akt_pracownik.zajecia[i].id) + ";");
							//for (auto& grupa : akt_pracownik.zajecia[i].grupa_vec) {
							//	ImGui::Text(("	Numer grupy: " + std::to_string(grupa.id)).c_str());
							//	ImGui::Text(("	Godzina rozpoczecia zajec: " + grupa.zajecia_od).c_str());
							//	ImGui::Text(("	Godzina zakonczenia zajec: " + grupa.zajecia_do).c_str());
							//}
						}
					}
					ImGui::EndChild();
					ImGui::PopStyleColor();
					ImGui::Unindent(10.f);
				}
			}
			};
	static auto s_zajec = [&]() {
		opcja = "Wyszukaj zajec";
		std::string zapytanie = filtry("SELECT * FROM zajecia_informacja ORDER BY ", "Domyslne\0Nazwa zajec\0ID instruktora", [&](std::string& zapytanie, int& wybor) {
			switch (wybor)
			{
			case 0:
				zapytanie += "id_zajecia ";
				break;
			case 1:
				zapytanie += "nazwa_zajec ";
				break;
			case 2:
				zapytanie += "id_pracownik ";
				break;
			default:
				break;
			}});
		static auto ZaladujZajecia = [&]() {
			std::vector<Zajecia> zajecia_vec = baza.Zaladuj_dane<Zajecia, ZAJECIA_INFO>(zapytanie);
			for (auto& akt_zajecia : zajecia_vec) {
				akt_zajecia.grupa_vec = baza.Zaladuj_dane<Grupa, GRUPA_INFO>("SELECT * FROM grupa_zajeciowa gz WHERE gz.id_zajecia = " + std::to_string(akt_zajecia.id) + ";");
			}
			return zajecia_vec;
		};
		static std::vector<Zajecia> zajecia_vec = ZaladujZajecia();
		if (zaktualizuj_dane) {
			zajecia_vec.clear();
			zajecia_vec = ZaladujZajecia();
			wybrany = -1;
		}
		for (int i = 0; i < zajecia_vec.size(); i++) {
			auto& akt_zajecia = zajecia_vec[i];
			auto etykieta = akt_zajecia.nazwa_zajec + "##" + std::to_string(akt_zajecia.id);
			if (ImGui::Selectable(etykieta.c_str(), wybrany == i))
				wybrany = i;
			if (wybrany == i)
			{
				ImGui::Indent(10.f);
				static auto wWidth = ImGui::GetWindowWidth();
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 50. / 255., 100. / 255., 0.7));
				ImGui::BeginChild("Szczegoly", ImVec2(wWidth - 25.f, 205));
				ImGui::Text("Identyfikator zajec: %d", akt_zajecia.id);
				ImGui::Text("Prowadzacy zajecia: %s %s", akt_zajecia.imie.c_str(), akt_zajecia.nazwisko.c_str());
				ImGui::Text("Opis zajec: %s", akt_zajecia.opis_zajec.c_str());
				for (auto& grupa : akt_zajecia.grupa_vec) {
					ImGui::Text(("Numer grupy: " + std::to_string(grupa.id)).c_str());
					ImGui::Text(("Godzina rozpoczecia zajec: " + grupa.zajecia_od).c_str());
					ImGui::Text(("Godzina zakonczenia zajec: " + grupa.zajecia_do).c_str());
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::Unindent(10.f);
			}
		}
	};
	static auto raport_liczebnosc = [&]() {
		opcja = "Raport";
		static auto spacing = [&](std::string const& str) {return ImGui::GetFontSize() * str.length() / 2.f; };
		using Statystyka_all = std::tuple<std::string, int, int, int>;
		using Statystyka_praca = std::tuple<std::string, std::string, int, int>;
		using Statystyka_blok = std::tuple<std::string, std::string, int, int>;
		using Statystyka_zajecia = std::tuple<std::string, std::string, int, int>;

		static auto statystyka_all = baza.Zaladuj_dane<Statystyka_all, std::string, int, int, int>("SELECT * FROM statystyka_wiezienia sw WHERE sw.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id +"));");
		static auto statystyka_blok = baza.Zaladuj_dane<Statystyka_blok, std::string, std::string, int, int>("SELECT * FROM statystyka_blokow sw WHERE sw.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
		static auto statystyka_zajecia = baza.Zaladuj_dane<Statystyka_zajecia, std::string, std::string, int, int>("SELECT * FROM statystyka_zajec sz WHERE sz.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
		static auto statystyka_praca = baza.Zaladuj_dane<Statystyka_praca, std::string, std::string, int, int>("SELECT * FROM statystyka_praca sp WHERE sp.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
		if (zaktualizuj_dane) {
			statystyka_all = baza.Zaladuj_dane<Statystyka_all, std::string, int, int, int>("SELECT * FROM statystyka_wiezienia sw WHERE sw.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
			statystyka_blok = baza.Zaladuj_dane<Statystyka_blok, std::string, std::string, int, int>("SELECT * FROM statystyka_blokow sw WHERE sw.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
			statystyka_zajecia = baza.Zaladuj_dane<Statystyka_zajecia, std::string, std::string, int, int>("SELECT * FROM statystyka_zajec sz WHERE sz.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
			statystyka_praca = baza.Zaladuj_dane<Statystyka_praca, std::string, std::string, int, int>("SELECT * FROM statystyka_praca sp WHERE sp.nazwa_wiezienia = (SELECT w.nazwa_wiezienia FROM wiezienie w WHERE w.id_wiezienie = (SELECT id_wiezienie from pracownik WHERE id_pracownik = " + zu.id + "));");
		}
		auto w = ImGui::GetWindowWidth();
		ImGui::Spacing();
		ImGui::SameLine(w / 2.f - spacing(std::get<0>(statystyka_all.front())));
		ImGui::Text(std::get<0>(statystyka_all.front()).c_str());
		StworzTabela("Statystyka_all", { "Ilosc pracownikow", "Ilosc wizytatorow", "Ilosc skazanych" }, statystyka_all);
		StworzTabela("Statystyka_blok", { "Nazwa bloku", "Ilosc straznikow", "Ilosc skazanych" }, statystyka_blok);
		StworzTabela("Statystyka_zajecia", { "Nazwa zajec", "Ilosc instruktorow", "Ilosc skazanych" }, statystyka_zajecia);
		StworzTabela("Statystyka_praca", { "Nazwa pracy", "Ilosc godzin w tygodniu", "Ilosc skazanych" }, statystyka_praca);
		};
	static std::function<void()> okno_kontekstowe{ zapelnij };
	if (zu.status != Sesja::NIEZALOGOWANY) {
		ImGui::SetNextWindowPos({ vp->WorkPos.x + 350, vp->WorkPos.y });
		ImGui::SetNextWindowSize({ (float)_winSize.first - 350, (float)_winSize.second });
		ImGui::Begin(opcja.c_str(), &close, _winFlags - ImGuiWindowFlags_AlwaysAutoResize);
		switch (zu.status) {
	case Sesja::WIZYTATOR:
		okno_kontekstowe = s_skazany;
		break;
	case Sesja::STRAZNIK:
		okno_kontekstowe = s_skazany;
		break;
	case Sesja::INSTRUKTOR:
		if (ImGui::Button("Szukaj skazanego")) {
			okno_kontekstowe = s_skazany;
			wybrany = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Szukaj zajec")) {
			okno_kontekstowe = s_zajec;
			wybrany = -1;
		}
		break;
	case Sesja::NACZELNIK:
		if (ImGui::Button("Szukaj skazanego")) {
			okno_kontekstowe = s_skazany;
			wybrany = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Szukaj pracownika")) {
			okno_kontekstowe = s_pracownik;
			wybrany = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Szukaj zajec")) {
			okno_kontekstowe = s_zajec;
			wybrany = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stworz raport")) {
			okno_kontekstowe = raport_liczebnosc;
			wybrany = -1;
		}
		break;
	case Sesja::SKAZANY:
		okno_kontekstowe = s_zajec;
		break;
	default:
		break;
	}
		okno_kontekstowe();
		ImGui::End();
	}
	else {
		okno_kontekstowe = zapelnij;
		opcja = "##zapelnijWyszukanie";
	}
	zaktualizuj_dane = false;
}

void Window::Dane()
{
	ImGui::SetNextWindowPos({ vp->WorkPos.x, vp->WorkPos.y + 0.7f * _winSize.second });
	ImGui::SetNextWindowSize({ 350, 0.3f * _winSize.second });
	auto fill = [&]() {
		if (ImGui::Begin("Zaloguj sie", &close, _winFlags))
		{
			ImGui::SameLine(240);
			if (ImGui::Button("Zaloguj sie", { 100,0 })) {
				zu = {};
				zaktualizuj_dane = true;
			}
			ImGui::End();
		}};
	auto dane_str = [&](std::string rola) {
		if (ImGui::Begin(rola.c_str(), &close, _winFlags))
		{
			ImGui::SameLine(240);
			if (ImGui::Button("Wyloguj sie", { 100,0 })) {
				zu = {};
				zaktualizuj_dane = true;
			}
			auto info = zu.Dane();
			ImGui::Text(("ID: " + std::get<0>(info)).c_str());
			ImGui::SameLine();
			ImGui::Text(("- " + rola).c_str());
			ImGui::Text("Dane uzytkownika:");
			ImGui::Text(std::get<1>(info).c_str());
			ImGui::SameLine();
			ImGui::Text(std::get<2>(info).c_str());
			ImGui::Text(std::get<3>(info).c_str());
			ImGui::Text(std::get<4>(info).c_str());
			ImGui::End();
		}};
	switch (zu.status)
	{
	case Sesja::NIEZALOGOWANY:
		break;
	case Sesja::WIZYTATOR:
		fill();
		break;
	case Sesja::STRAZNIK:
		dane_str("Straznik");
		break;
	case Sesja::INSTRUKTOR:
		dane_str("Instruktor");
		break;
	case Sesja::NACZELNIK:
		dane_str("Naczelnik");
		break;
	case Sesja::SKAZANY:
		dane_str("Skazany");
		break;
	default:
		break;
	}
}

ImGuiIO& Window::CreateImGui()
{
	ImGui::CreateContext();
	return ImGui::GetIO();
}