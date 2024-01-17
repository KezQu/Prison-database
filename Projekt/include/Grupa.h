/**
 * @file Grupa.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy obiket reprezentujacy grupe zajeciowa
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#define GRUPA_INFO int, int, std::string, std::string
/**
 * @brief Struktura ktora przedstawia grupe zajeciowa o polach jak w bazie danych
 * 
 */
struct Grupa : public Obiekt{
	int id_zajecia;
	std::string zajecia_od{"00:00"};
	std::string zajecia_do{"00:00"};
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie nowej grupy
	 * 
	 * @param id numer ID nowej grupy
	 * @param new_id_zajecia ID zajec do ktorych grupa nalezy
	 */
	Grupa(int id, int new_id_zajecia)
		:Obiekt{ id },
		id_zajecia{ new_id_zajecia }
	{}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie obiektu z danych uzyskanych przy pomocy zapytania do bazy danych
	 * 
	 * @param row rekord zwrocony z zapytania do bazy danych
	 */
	Grupa(std::tuple<GRUPA_INFO> const& row)
		:Obiekt{std::get<0>(row)},
		id_zajecia{std::get<1>(row)},
		zajecia_od{ std::get<2>(row) },
		zajecia_do{ std::get<3>(row) }
	{}
	/**
	 * @brief Metoda pozwalajaca na wstawienie nowej grupy do bazy danych
	 * 
	 * @param baza Interfejs wykorzystany do wyslania danych
	 * @return std::string ewentualne bledy zwiazane z niepoprawnymi danymi
	 */
	std::string Wstaw(Baza& baza) override {
		try {
			baza.query("INSERT INTO grupa_zajeciowa VALUES (" +
				std::to_string(id) +
				", '" + std::to_string(id_zajecia)+"'" +
				", " + (zajecia_od.length() == 0 ? "NULL" : "'" + zajecia_od + "'") +
				", " + (zajecia_do.length() == 0 ? "NULL" : "'" + zajecia_do + "'") + ");");
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};
