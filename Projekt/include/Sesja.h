/**
 * @file Sesja.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy strukture reprezentujaca sesje
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Pracownik.h"
#include "Skazany.h"

/**
 * @brief Struktura zawierajaca informacje o zalogowanym uzytkowniku
 * 
 */
struct Sesja {
	friend class Window;
public:
	/**
	 * @brief Typ wyliczeniowy definiujacy sposob dostepu do bazy danych
	 * 
	 */
	enum Status{
		NIEZALOGOWANY = 0,
		WIZYTATOR = 1,
		STRAZNIK = 2,
		INSTRUKTOR = 3,
		NACZELNIK = 4,
		SKAZANY = 5
	};
	/**
	 * @brief Domyslny konstruktor 
	 * 
	 */
	Sesja() :status{ NIEZALOGOWANY } {}
	/**
	 * @brief Konstruktor tworzacy sesje na podstawie danych pracownika
	 * 
	 * @param p pracownik logujacy sie do aplikacji
	 */
	Sesja(Pracownik p)
		:id{ std::to_string(p.id) },
		haslo{ p.haslo },
		imie{ p.imie },
		nazwisko{ p.nazwisko },
		pesel{ p.pesel },
		data_urodzenia{ p.data_urodzenia }
	{
		if (p.stanowisko == "Straznik")
			status = STRAZNIK;
		else if (p.stanowisko == "Naczelnik")
			status = NACZELNIK;
		else if (p.stanowisko == "Instruktor")
			status = INSTRUKTOR;
	}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie sesji na podstawie zalogowanego skazanego
	 * 
	 * @param s logujacy sie skazany
	 */
	Sesja(Skazany s)
		:status{ SKAZANY },
		id{ std::to_string(s.id) },
		haslo{ s.pesel },
		imie{ s.imie },
		nazwisko{ s.nazwisko },
		pesel{ s.pesel },
		data_urodzenia{ s.data_urodzenia }
	{}
	/**
	 * @brief Metoda zwracajaca dane przechowywane w sesji 
	 * 
	 * @return std::tuple<std::string, std::string, std::string, std::string, std::string> 
	 */
	std::tuple<std::string, std::string, std::string, std::string, std::string> Dane() {
		return std::make_tuple(id, imie, nazwisko, pesel, data_urodzenia);
	}
public:
	Status status;
	std::string id{};
	std::string imie{};
	std::string nazwisko{};
	std::string pesel{};
	std::string data_urodzenia{};
private:
	std::string haslo{};
};
