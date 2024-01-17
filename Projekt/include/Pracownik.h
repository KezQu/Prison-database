#pragma once
#include "Osoba.h"
#include "Zajecia.h"

#define PRACOWNIK_INFO int, std::string, std::string, std::string, std::string, std::string, int, std::string, int, std::string, std::string, std::string, int
#define PRACOWNIK_WSTAW int, std::string, std::string, std::string, std::string, int, std::string, int, std::string, std::string, std::string

struct Pracownik : public Osoba {
	std::string stanowisko;
	int id_wiezienie{};
	std::string haslo;
	int id_blok{};
	std::string nazwa_bloku;
	std::string czas_pracy_od;
	std::string czas_pracy_do;
	std::vector<Zajecia> zajecia;
	int ilosc_zajec{};

	Pracownik() :Osoba{} {}
	Pracownik(std::tuple<PRACOWNIK_INFO> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row), std::get<4>(row)},
		stanowisko{ std::get<5>(row) },
		id_wiezienie{ std::get<6>(row) },
		haslo{ std::get<7>(row) },
		id_blok{ std::get<8>(row) },
		nazwa_bloku{ std::get<9>(row) },
		czas_pracy_od{ std::get<10>(row) },
		czas_pracy_do{ std::get<11>(row) },
		ilosc_zajec { std::get<12>(row) }
	{}
	Pracownik(std::tuple<PRACOWNIK_WSTAW> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row) },
		stanowisko{ std::get<4>(row) },
		id_wiezienie{ std::get<5>(row) },
		haslo{ std::get<6>(row) },
		id_blok{ std::get<7>(row) },
		nazwa_bloku{ std::get<8>(row) },
		czas_pracy_od{ std::get<9>(row) },
		czas_pracy_do{ std::get<10>(row) }
	{}
	std::string Wstaw(Baza& baza) override {
		try {
			std::string zapytanie = "INSERT INTO pracownik VALUES \
				(" + std::to_string(id) +
				", " + Pesel_arr(pesel) +
				", '" + imie + "'" +
				", '" + nazwisko + "'" +
				", '" + data_urodzenia + "'" +
				", " + (stanowisko.length() == 0 ? "NULL" : "'" + stanowisko + "'") + ", " +
				std::to_string(id_wiezienie) +
				", " + (haslo.length() == 0 ? "NULL" : "'" + haslo + "'") + ");";
			if (stanowisko == "Straznik") {
				zapytanie += "INSERT INTO blok_pracownik VALUES \
					(" + std::to_string(id_blok) +
					", " + std::to_string(id) +
					", " + (czas_pracy_od.length() == 0 ? "NULL" : "'" + czas_pracy_od + "'") +
					", " + (czas_pracy_do.length() == 0 ? "NULL" : "'" + czas_pracy_do + "'") + ");";
			}
			baza.query<>(zapytanie);
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};
