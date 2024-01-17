/**
 * @file Obiekt.h
 * @author Jakub Mikusek
 * @brief Abstrakcyjna struktura przedstawiajaca obiekt bazodanowy
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Baza.h"
/**
 * @brief Struktura przedstawiajaca najbardizej podstawowy obiekt bazy danych
 * 
 */
struct Obiekt {
	int id;
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie nowego obiektu
	 * 
	 * @param new_id ID nowego obiektu
	 */
	Obiekt(int new_id = 0) : id{ new_id } 
	{}
	static auto id_szukanie(Baza& baza, std::string zapytanie) {
		unsigned wolne_id{ 1 };
		auto ids = baza.query<unsigned>(zapytanie);
		for (auto& [id] : ids) {
			if (id == wolne_id) wolne_id++;
		}
		return wolne_id;
	}
	/**
	 * @brief Abstrakcyjna metoda majaca na celu ujednolicenie interfejsu do wstawiania danych do bazy 
	 * 
	 * @param baza interfejs polaczeniowy do bazy uzyty do wykonania zadanych transakcji
	 * @return std::string tresc ewentualnych bledow
	 */
	virtual std::string Wstaw(Baza& baza) = 0;
	/**
	 * @brief Abstrakcyjna metoda majaca na celu ujednolicenie interfejsu do edytowania danych w bazie 
	 * 
	 * @param baza interfejs polaczeniowy do bazy uzyty do wykonania zadanych transakcji
	 * @return std::string tresc ewentualnych bledow
	 */
	virtual std::string Edytuj(Baza& baza) { return ""; };
};
