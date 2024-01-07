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
	bool odswiez{ false };
	Sesja zu;
public:
	bool close{ true };
public:
	Window(int w, int h, std::string title);
	~Window();
	bool Loop();
	void Refresh();
	void Render();
	void Logowanie(Baza& baza);
	void Dodawanie(Baza& baza);
	void Wyszukiwanie(Baza& baza);
	void Dane();
	Sesja::Status const& GetStatus();
private:
	static ImGuiIO& CreateImGui();
	std::string PobierzWybor(Baza& baza, std::string zapytanie) {
		auto zapytanie_wynik = baza.query<std::string>(zapytanie);
		std::string rezultat{ ' ' };
		rezultat += '\0';
		for (auto [pole] : zapytanie_wynik) {
			rezultat += pole;
			rezultat += '\0';
		}
		return rezultat; };
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
	template<typename Tuple, std::size_t ...i>
	void StworzWiersz(Tuple& tpl, std::index_sequence<i...>);
	template<typename Typ>
	void Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_arithmetic_v<std::remove_reference_t<Typ>>, bool>::type = true);
	template<typename Typ>
	void Wiersz(Typ& pole, std::size_t i, std::enable_if <std::is_class_v<std::remove_reference_t<Typ>>, bool>::type = true);
};
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