/**
 * @file Window.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy klase reprezentujaca okno kontekstowe oraz cala logike stojaca za wyswietlanymi tresciami
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "Pracownik.h"
#include "Skazany.h"
#include "Wizytator.h"
#include "Zajecia.h"
#include "Sesja.h"

#include <iostream>
#include <algorithm>
/**
 * @brief klasa reprezentujaca okno kontekstowe oraz cala logike stojaca za wyswietlanymi tresciami
 * 
 */
class Window
{
private:
	std::string _title;
	ImGuiIO& _io;
	ImGuiStyle& _style;
	GLFWwindow* _window{ nullptr };
	std::pair<int, int> _winSize;
	ImGuiWindowFlags _winFlags{ 0 };
	ImGuiViewport* vp{ nullptr };

	bool zaktualizuj_dane{ false };
	Sesja zu;
public:
	bool close{ true };
public:
	/**
	 * @brief Tworzy nowy obiekt reprezentujacy okno o zadanych parametrach, oraz tytule
	 * 
	 * @param w szerokosc okna
	 * @param h wysokosc okna
	 * @param title tytul okna
	 */
	Window(int w, int h, std::string title);
	/**
	 * @brief Niszczy stworzone okno
	 * 
	 */
	~Window();
	/**
	 * @brief Metoda sprawdzajaca, czy aplikacja dalej powinna dzialac
	 * 
	 * @return true 
	 * @return false 
	 */
	bool Loop();
	/**
	 * @brief Metoda tworzaca nowa klatke wyswietlanego obrazu
	 * 
	 */
	void Refresh();
	/**
	 * @brief Metoda podmieniajaca bufory oraz wyswietlajaca wygenerowana zawartosc na okno
	 * 
	 */
	void Render();
	/**
	 * @brief Metoda tworzaca okno odpowiadajace na logowanie
	 * 
	 * @param baza interfejs bazodanowy, gdzie wprowadzane beda zapytania
	 */
	void Logowanie(Baza& baza);
	/**
	 * @brief Metoda tworzaca okno odpowiadajace za dodawanie danych poprzez wygenerowane formularze
	 * 
	 * @param baza interfejs bazodanowy, gdzie wprowadzane beda zapytania
	 */
	void Dodawanie(Baza& baza);
	/**
	 * @brief Metoda tworzaca okno odpowiadajace za wyszkuwanie porzadanych danych z posrod danych otrzymanych z bazy danych
	 * 
	 * @param baza interfejs bazodanowy, gdzie wprowadzane beda zapytania
	 */
	void Wyszukiwanie(Baza& baza);
	/**
	 * @brief Metoda tworzaca okno odpowiadajace za dane zalogowanego uzytkownika
	 * 
	 */
	void Dane();
	/**
	 * @brief Metoda zwracajaca aktualny status sesji
	 * 
	 * @return Sesja::Status const& status sesji
	 */
	Sesja::Status const& GetStatus();
private:
	/**
	 * @brief Metoda tworzaca interfejs Im Gui do generowania widgetow
	 * 
	 * @return ImGuiIO& 
	 */
	static ImGuiIO& CreateImGui();
	/**
	 * @brief Metoda pozwalajaca na uzyskanie danych potrzebnych do poprawnego wygenreowania list do formularzy
	 * 
	 * @param baza interfejs bazodanowy sluzacy do polaczenia z porzadana baza
	 * @param zapytanie zapytanie pozwalajace na utworzenie odpowieniej listy do formularza
	 * @return std::vector<std::string> lista
	 */
	std::vector<std::string> PobierzWybor(Baza& baza, std::string zapytanie) {
		auto zapytanie_wynik = baza.query<std::string>(zapytanie);
		std::vector<std::string> rezultat{ "" };
		for (auto [pole] : zapytanie_wynik) {
			rezultat.push_back(pole);
		}
		return rezultat; 
	};
	/**
	 * @brief Metoda pozwalajaca na wygenerowanie raportu w formie tabeli
	 * 
	 * @tparam Typ typ danych majacy znalezc sie w raporcie
	 * @param tytul tytul raportu
	 * @param col_nazwy nazwy poszczegolnych kolumn
	 * @param dane dane umieszczane w raporcie
	 * @return auto 
	 */
	template<typename Typ>
	auto StworzTabela(std::string const& tytul, std::vector<std::string> const& col_nazwy, std::vector<Typ> const& dane) {
		ImGui::BeginTable(tytul.c_str(), 3);
		ImGui::TableNextRow();
		for (int i = 0; i < 3; i++) {
			ImGui::TableSetColumnIndex(i);
			ImGui::Text(col_nazwy[i].c_str());
		}
		for (auto& rekord : dane) {
			ImGui::TableNextRow();
			StworzWiersz(rekord, std::make_index_sequence<std::tuple_size_v<Typ> - 1>{});
		}
		ImGui::EndTable();
		ImGui::Separator();
	};
	/**
	 * @brief Metoda pozwalajaca na stworzenie wiersza wewnatrz raportu
	 * 
	 * @tparam Tuple krotka zawierajaca typy danych umieszczanych w rekordzie raportu
	 * @tparam i ilosc danych umieszczanych w kolejnych kolumnach
	 * @param tpl krotka zawierajaca dane umieszczane w raporcie
	 */
	template<typename Tuple, std::size_t ...i>
	void StworzWiersz(Tuple& tpl, std::index_sequence<i...>);
	/**
	 * @brief Metoda pozwalajaca na umieszczenie danej typu arytmetycznego w raporcie
	 * 
	 * @tparam Typ typ wprowadzanego pola
	 * @param pole wartosc wprowadzanego pola
	 * @param i numer kolumny, do ktorej nalezy wstawic dane
	 */
	template<typename Typ>
	void Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_arithmetic_v<std::remove_reference_t<Typ>>, bool>::type = true);
	/**
	 * @brief Metoda pozwalajaca na umieszczenie danej typu zlozonego w raporcie
	 * 
	 * @tparam Typ typ wprowadzanego pola
	 * @param pole wartosc wprowadzanego pola
	 * @param i numer kolumny, do ktorej nalezy wstawic dane
	 */
	template<typename Typ>
	void Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_class_v<std::remove_reference_t<Typ>>, bool>::type = true);
};
/**
 * @brief Funkcja pozwalajaca na wypisane bledow zwiazanych z generowaniem okna kontekstowego na standardowe wyjscie
 * 
 * @param error_code kod bledu
 * @param description opis bledu
 */
void ErrorFun(int error_code, const char* description);
template<typename Tuple, std::size_t ...i>
void Window::StworzWiersz(Tuple& tpl, std::index_sequence<i...>) {
	(Wiersz(std::get<i+1>(tpl), i), ...);
}
template<typename Typ>
void Window::Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_arithmetic_v<std::remove_reference_t<Typ>>, bool>::type){
	ImGui::TableSetColumnIndex(i);
	ImGui::Text(std::to_string(pole).c_str());
}
template<typename Typ>
void Window::Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_class_v<std::remove_reference_t<Typ>>, bool>::type) {
	ImGui::TableSetColumnIndex(i);
	ImGui::Text(pole.c_str());
}