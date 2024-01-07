#pragma once
#include "Obiekt.h"
#include "Grupa.h"

#define ZAJECIA_INFO int, std::string, std::string, int, std::string, std::string

struct Zajecia : public Obiekt{
	std::string nazwa_zajec;
	std::string opis_zajec;
	int id_pracownik{};
	std::string imie;
	std::string nazwisko;
	std::vector<Grupa> grupa_vec;

	Zajecia() {}
	Zajecia(std::tuple<ZAJECIA_INFO> const& row)
		:Obiekt{ std::get<0>(row) },
		nazwa_zajec{ std::get<1>(row) },
		opis_zajec{ std::get<2>(row) },
		id_pracownik{ std::get<3>(row) },
		imie{ std::get<4>(row) },
		nazwisko{ std::get<5>(row) }
	{}
	std::string Wstaw(Baza& baza) override{
		try {
			baza.query("INSERT INTO zajecia_resocjalizacyjne VALUES (" +
				std::to_string(id) +
				", '" + nazwa_zajec + "'" +
				", '" + opis_zajec + "'" +
				", " + std::to_string(id_pracownik) + ");");
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
	std::string Edytuj(Baza& baza) override {
		try {
			baza.query("UPDATE zajecia_resocjalizacyjne \
				SET opis_zajec = '" + opis_zajec + "'" +
				"WHERE id_zajecia = " + std::to_string(id) + ";");
			for (auto& grupa : grupa_vec) {
				baza.query("UPDATE grupa_zajeciowa \
							SET zajecia_od = '" + grupa.zajecia_od + "'" +
							", zajecia_do = '" + grupa.zajecia_do + "'" +
					"WHERE id_grupa = " + std::to_string(grupa.id) + ";");
			}
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};
