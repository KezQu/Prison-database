/**
 * @file Skazany.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy strukture reprezentujaca skazanego
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "Osoba.h"
#include "Zajecia.h"
#include "Grupa.h"

#define SKAZANY_WSTAW int, std::string, std::string, std::string, int, int, std::string, std::string, std::string
#define SKAZANY_INFO int, std::string, std::string, std::string, std::string, int, std::string, std::string, std::string, std::string, int, int, int
/**
 * @brief Struktura reprezentujaca skazanego
 * 
 */
struct Skazany : public Osoba {
	int id_cela{};
	int id_zbrodnia{};
	std::string nazwa_zbrodni;
	std::string dlugosc_pozbawienia_wolnosci;
	std::string opis_zbrodni;
	std::string nazwa_pracy;
	int wymiar_godzin{};
	int ilosc_zajec{};
	int ilosc_wizytacji{};
	std::pair <std::vector<std::tuple<std::string>>, std::vector<Grupa>> zajecia_vec;
	std::vector<std::tuple<std::string, std::string>> wizytacje_vec;
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie skazanego na podstawie rekordu zwroconego z bazy danych
	 * 
	 * @param row rekord zwrocony z bazy danych
	 */
	Skazany(std::tuple<SKAZANY_INFO> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row), std::get<4>(row) },
		id_cela{ std::get<5>(row) },
		nazwa_zbrodni{ std::get<6>(row) },
		dlugosc_pozbawienia_wolnosci{ std::get<7>(row) },
		opis_zbrodni{ std::get<8>(row) },
		nazwa_pracy{ std::get<9>(row) },
		wymiar_godzin{ std::get<10>(row) },
		ilosc_zajec{ std::get<11>(row) },
		ilosc_wizytacji{ std::get<12>(row) }
	{}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie skazanego na podstawie danych z formularza
	 * 
	 * @param row dane zwrocone z formularza
	 */
	Skazany(std::tuple<SKAZANY_WSTAW> row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row) },
		id_cela{ std::get<4>(row) },
		id_zbrodnia{ std::get<5>(row) },
		opis_zbrodni{ std::get<8>(row) }
	{}
	/**
	 * @brief Metoda pozwalajaca na dodanie nowego skazanego do bazy danych
	 * 
	 * @param baza interfejs bazodanowy, wykorzystany do wstawienia danych
	 * @return std::string ewentualne tresci bledow
	 */
	std::string Wstaw(Baza& baza) override{
		try {
			std::string zapytanie = "INSERT INTO skazany VALUES \
				(" + std::to_string(id) +
				", " + Pesel_arr(pesel) +
				", '" + imie + "'" +
				", '" + nazwisko + "'" +
				", '" + data_urodzenia + "'," +
				std::to_string(id_cela) + ");";
			
			zapytanie += "INSERT INTO skazany_zbrodnia VALUES \
				(" + std::to_string(id) +
				", " + std::to_string(id_zbrodnia) +
				", " + (opis_zbrodni.length() == 0 ? "NULL" : "'" + opis_zbrodni + "'") + ");";
			baza.query<>(zapytanie);
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};