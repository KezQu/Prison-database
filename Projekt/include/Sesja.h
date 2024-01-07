#pragma once
#include "Pracownik.h"
#include "Skazany.h"

struct Sesja {
	friend class Window;
public:
	enum Status{
		NIEZALOGOWANY = 0,
		WIZYTATOR = 1,
		STRAZNIK = 2,
		INSTRUKTOR = 3,
		NACZELNIK = 4,
		SKAZANY = 5
	};
	Sesja() :status{ NIEZALOGOWANY } {}
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
	Sesja(Skazany s)
		:status{ SKAZANY },
		id{ std::to_string(s.id) },
		haslo{ s.pesel },
		imie{ s.imie },
		nazwisko{ s.nazwisko },
		pesel{ s.pesel },
		data_urodzenia{ s.data_urodzenia }
	{}
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
