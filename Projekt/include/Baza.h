/**
 * @file Baza.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy klase reprezentujaca interfejs pomiedzy baza a aplikacja
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <thread>
/**
 * @brief Klasa reprezentujaca interfejs polaczeniowy miedzy baza danych a aplikacja
 * 
 */
class Baza{
private:
	/**
	 * @brief Skladowa reprezentujaca polaczenie do bazy danych
	 * 
	 */
	pqxx::connection c;
	/**
	 * @brief Wyniki zwracane przez baze danych poprzez wywoływanie odpowiednich metod
	 * 
	 */
	pqxx::result wynik_zapytania;
public:
	/**
	 * @brief Konstruktor tworzacy nowy interfejs
	 * 
	 * @param db_adres zmienne potrzebne do poprawnego polaczenia z baza danych
	 */
	Baza(std::string const& db_adres)
		:c{ db_adres }
	{ std::cout << "Polaczono z " << c.dbname() << std::endl; }
	/**
	 * @brief Metoda pozwalajaca na wywolanie zapytania ktore zwraca okreslone dane
	 * 
	 * @tparam Ts typy zwracanych danych
	 * @param query_str zapytanie do bazy danych
	 * @return auto iterator zawierajacy zwrocone dane
	 */
	template<typename ...Ts>
	auto query(std::string query_str);
	/**
	 * @brief Metoda ladujaca dane z bazy do odpowiedniego wektora
	 * 
	 * @tparam Ob obiekt do ktorego dane zwrocone sa ladowane
	 * @tparam Rt typy danych zwracane przez baze
	 * @param query_str zapytanie do bazy danych
	 * @return std::vector<Ob> wektor obiektow stworzonych na podstawie danych z bazy
	 */
	template<typename Ob, typename ...Rt>
	std::vector<Ob> Zaladuj_dane(std::string query_str);
	/**
	 * @brief Metoda pozwalajaca na zresetowanie danych wewnatrz foormularza
	 * 
	 * @tparam Tuple krotka reprezentujaca formularz
	 * @param tpl formularz
	 */
	template<typename Tuple>
	static void resetuj_formularz(Tuple& tpl);
private:
	/**
	 * @brief Petla umozliwiajaca na konwersje wektora pol na krotke
	 * 
	 * @tparam Tuple typ krotki po ktorej bedziemy sie poruszac
	 * @tparam i poszczegolne indeksy typow w krotkach
	 * @param field_vec wektor pol do konwersji
	 * @param tpl krotka z typami, na ktore nalezy przekonwertowac dane
	 */
	template<typename Tuple, std::size_t ...i>
	void tmpl_for(std::vector<pqxx::field>& field_vec, Tuple& tpl, std::index_sequence<i...>);
	/**
	 * @brief Petla pozwalajaca na zresetowanie pol zadanej krotki
	 * 
	 * @tparam Tuple typ krotki do wyczyszczenia
	 * @tparam i indeks pola krotki do wyczyszczenia
	 * @param tpl krotka do wyczyszczenia
	 */
	template<typename Tuple, std::size_t ...i>
	static void tmpl_for(Tuple& tpl, std::index_sequence<i...>);
	/**
	 * @brief Pozwala na usuniecie zawartosci dowolnej zmiennej ktora jest typu arytmetycznego
	 * 
	 * @tparam Typ typ zmiennej
	 * @param pole zmienna do wyczyszczenia
	 */
	template<typename Typ>
	static void reset(Typ& pole, typename std::enable_if<std::is_arithmetic<std::remove_reference_t<Typ>>::value, bool>::type = true);
	/**
	 * @brief Pozwala na usuniecie zawartosci dowolnej zmiennej ktora jest typu zlozonego
	 * 
	 * @tparam Typ typ zmiennej
	 * @param pole zmienna do wyczyszczenia
	 */
	template<typename Typ>
	static void reset(Typ& pole, typename std::enable_if<std::is_class<std::remove_reference_t<Typ>>::value, bool>::type = true);
	/**
	 * @brief Pozwala na usuniecie zawartosci dowolnej zmiennej ktora jest typu tablicowego
	 * 
	 * @tparam Typ typ zmiennej
	 * @param pole zmienna do wyczyszczenia
	 */
	template<typename Typ, int N>
	static void reset(std::remove_all_extents_t<std::remove_reference_t<Typ>>(&pole)[N]);
	/**
	 * @brief Umozliwia przekonwertowanie pola ktorego wartosc zwrocona z bazy danych wynosi null
	 * 
	 * @tparam Tuple typ krotki ktorej pole wymaga konwersji z wartosci null, przed wstawieniem do niej
	 * @tparam i numer pola do konwersji
	 * @param field_vec pole typu arytmetycznego wymagajace konwersji
	 * @param tpl krotka ktorej pole wymaga konwersji z wartosci null, przed wstawieniem do niej
	 */
	template<typename Tuple, std::size_t i>
	void konwertuj(std::vector<pqxx::field>& field_vec, Tuple& tpl, typename std::enable_if<std::is_arithmetic<std::remove_reference_t<decltype(std::get<i>(tpl))>>::value, bool>::type = true) {
		auto pole = field_vec[i];
		if (pole.is_null()) {
			std::get<i>(tpl) = -1;
		}
		else {
			std::get<i>(tpl) = pole.as<std::remove_reference_t<decltype(std::get<i>(tpl))>>();
		}
	}
	/**
	 * @brief Umozliwia przekonwertowanie pola ktorego wartosc zwrocona z bazy danych wynosi null
	 * 
	 * @tparam Tuple typ krotki ktorej pole wymaga konwersji z wartosci null, przed wstawieniem do niej
	 * @tparam i numer pola do konwersji
	 * @param field_vec pole typu zlozonego wymagajace konwersji
	 * @param tpl krotka ktorej pole wymaga konwersji z wartosci null, przed wstawieniem do niej
	 */
	template<typename Tuple, std::size_t i>
	void konwertuj(std::vector<pqxx::field>& field_vec, Tuple& tpl, typename std::enable_if<std::is_class<std::remove_reference_t<decltype(std::get<i>(tpl))>>::value, bool>::type = true) {
		auto pole = field_vec[i];
		if (pole.is_null()) {
			std::get<i>(tpl) = "brak";
		}
		else {
			std::get<i>(tpl) = pole.as<std::remove_reference_t<decltype(std::get<i>(tpl))>>();
		}
	}
};

template<typename ...Ts>
inline auto Baza::query(std::string query_str) {
	pqxx::work w{ c };
	w.query<>("set SEARCH_PATH to wiezienie;");
	auto zwrocone_zapytanie = w.query<Ts...>(query_str);
	w.commit();
	return zwrocone_zapytanie;
}
template<>
inline auto Baza::query(std::string query_str) {
	pqxx::work w{ c };
	w.query<>("set SEARCH_PATH to wiezienie;");
	auto zwrocone_zapytanie = w.exec(query_str);
	w.commit();
	return zwrocone_zapytanie;
}
template<typename Ob, typename ...Rt>
inline std::vector<Ob> Baza::Zaladuj_dane(std::string query_str) {
	std::vector<Ob> dane;
	wynik_zapytania = query(query_str);
	dane.clear();
	for (auto const& row : wynik_zapytania) {
		std::vector<pqxx::field> field_vec;
		for (auto const& field : row) {
			field_vec.push_back(field);
		}
		using tuple_row = std::tuple<Rt...>;
		tuple_row rekord;
		tmpl_for <tuple_row>(field_vec, rekord, std::make_index_sequence<std::tuple_size<tuple_row>::value>{});
		dane.emplace_back(rekord);
	}
	return dane;
}
template<typename Tuple>
inline void Baza::resetuj_formularz(Tuple& tpl) {
	tmpl_for<Tuple>(tpl, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}
template<typename Tuple, std::size_t ...i>
inline void Baza::tmpl_for(std::vector<pqxx::field>& field_vec, Tuple& tpl, std::index_sequence<i...>) {
	(konwertuj<Tuple, i>(field_vec, tpl), ...);
}
template<typename Tuple, std::size_t ...i>
inline void Baza::tmpl_for(Tuple& tpl, std::index_sequence<i...>) {
	(reset<std::remove_const_t<decltype(std::get<i>(tpl))>>(std::get<i>(tpl)), ...);
}
template<typename Typ>
inline void Baza::reset(Typ& pole, typename std::enable_if<std::is_arithmetic<std::remove_reference_t<Typ>>::value, bool>::type) {
	pole = 0;
}
template<typename Typ>
inline void Baza::reset(Typ& pole, typename std::enable_if<std::is_class<std::remove_reference_t<Typ>>::value, bool>::type) {
	pole = {};
}
template<typename Typ, int N>
inline void Baza::reset(std::remove_all_extents_t<std::remove_reference_t<Typ>>(&pole)[N]) {
	for (int i = 0; i < N; i++) pole[i] = std::remove_all_extents_t<std::remove_reference_t<Typ>>(0);
}
