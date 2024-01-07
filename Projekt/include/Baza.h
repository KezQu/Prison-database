#pragma once

#include <pqxx/pqxx>
#include <iostream>
#include <thread>

class Baza{
private:
	pqxx::connection c;
	pqxx::result wynik_zapytania;
public:
	Baza(std::string const& db_adres)
		:c{ db_adres }
	{ std::cout << "Polaczono z " << c.dbname() << std::endl; }
	template<typename ...Ts>
	auto query(std::string query_str);
	template<typename Ob, typename ...Rt>
	std::vector<Ob> Zaladuj_dane(std::string query_str);
	template<typename Tuple, std::size_t ...i>
	static void resetuj_formularz(Tuple& tpl);
private:
	template<typename Tuple, std::size_t ...i>
	void tmpl_for(std::vector<pqxx::field>& field_vec, Tuple& tpl, std::index_sequence<i...>);
	template<typename Tuple, std::size_t ...i>
	static void tmpl_for(Tuple& tpl, std::index_sequence<i...>);
	template<typename Typ>
	static void reset(Typ& pole, typename std::enable_if<std::is_arithmetic<std::remove_reference_t<Typ>>::value, bool>::type = true);
	template<typename Typ>
	static void reset(Typ& pole, typename std::enable_if<std::is_class<std::remove_reference_t<Typ>>::value, bool>::type = true);
	template<typename Typ, int N>
	static void reset(std::remove_all_extents_t<std::remove_reference_t<Typ>>(&pole)[N]);
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
template<typename Tuple, std::size_t ...i>
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
