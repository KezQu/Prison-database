#pragma once
#include "Osoba.h"

#define WIZYTATOR_INFO int, std::string, std::string, std::string, std::string
#define WIZYTATOR_WSTAW int, std::string, std::string, std::string

struct Wizytator : public Osoba {
	Wizytator(): Osoba{} {}
	Wizytator(std::tuple<WIZYTATOR_INFO> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row), std::get<4>(row) }
	{}
	Wizytator(std::tuple<WIZYTATOR_WSTAW> const& row)
		:Osoba{ std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row) }
	{}
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
