/**
 * @file Wizytator.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy strukture reprezentujaca wizytatora
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Osoba.h"

#define WIZYTATOR_INFO int, std::string, std::string, std::string, std::string
#define WIZYTATOR_WSTAW int, std::string, std::string, std::string
/**
 * @brief Struktura reprezentujaca wizytatora 
 * 
 */
struct Wizytator : public Osoba {
	/**
	 * @brief Konstruktor domyslny
	 * 
	 */
	Wizytator(): Osoba{} {}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie nowego wizytatora na podstawie rekordu zwroconego z bazy danych
	 * 
	 * @param row rekord otrzymany z bazy danych
	 */
	Wizytator(std::tuple<WIZYTATOR_INFO> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row), std::get<4>(row) }
	{}
	/**
	 * @brief Konstruktor tworzacy nowego wizytatora na podstawie danych z formularza
	 * 
	 * @param row formularz
	 */
	Wizytator(std::tuple<WIZYTATOR_WSTAW> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row) }
	{}
	/**
	 * @brief Metoda pozwalajaca na dodanie nowego wizytatora do bazy danych
	 * 
	 * @param baza interfejs polaczeniowy z baza danych
	 * @return std::string ewentualne tresci bledow
	 */
	std::string Wstaw(Baza& baza) override{
		try {
			baza.query("INSERT INTO wizytator VALUES(" +
				std::to_string(id) +
				", " + Pesel_arr(pesel) + 
				", '" + imie + "'" +
				", '" + nazwisko + "'" +
				", '" + data_urodzenia + "');");
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};
