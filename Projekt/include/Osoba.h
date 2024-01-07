#pragma once
#include <string>
#include <vector>
#include "Obiekt.h"

struct Osoba : public Obiekt{
	std::string pesel;
	std::string imie;
	std::string nazwisko;
	std::string data_urodzenia;
	Osoba() : Osoba{ 0, "", "", "", "" } {}
	Osoba(int id, std::string new_pesel, std::string new_imie, std::string new_nazwisko)
		:Obiekt{ id },
		pesel{ new_pesel },
		imie{ new_imie },
		nazwisko{ new_nazwisko },
		data_urodzenia{ Data_urodzenia(new_pesel) }
	{}
	Osoba(int id, std::string new_pesel, std::string new_imie, std::string new_nazwisko, std::string new_data_urodzenia)
		:Obiekt{ id },
		pesel{ Czytalny_pesel(new_pesel) },
		imie{ new_imie },
		nazwisko{ new_nazwisko },
		data_urodzenia{ new_data_urodzenia }
	{}
	static std::string Pesel_arr(std::string pesel) {
		std::string bufor = "ARRAY[";
		for (auto l : pesel) {
			bufor += l;
			bufor += ",";
		}
		bufor.back() = ']';
		return bufor;
	}
protected:
	std::string Czytalny_pesel(std::string pesel) {
		std::string r_pesel;
		for (int i = 1; i < pesel.length(); i += 2) r_pesel += pesel[i];
		return r_pesel;
	}
	std::string Czytalny_czas(int const czas[2]) {
		return std::to_string(czas[0]) + ":" + std::to_string(czas[1]);
	}
	std::string Data_urodzenia(std::string pesel) {
		if (pesel.empty())
			return "";
		std::string data_urodzenia;
		switch (pesel[2])
		{
		case '8':	data_urodzenia += "18"; break;
		case '9':	data_urodzenia += "18"; break;
		case '0':	data_urodzenia += "19"; break;
		case '1':	data_urodzenia += "19"; break;
		case '2':	data_urodzenia += "20"; break;
		case '3':	data_urodzenia += "20"; break;
		}
		data_urodzenia += pesel[0];
		data_urodzenia += pesel[1];
		data_urodzenia += "-";
		auto spr_miesiac = pesel[2];
		if (atoi(&spr_miesiac) % 2)
			data_urodzenia += "1";
		else 
			data_urodzenia += "0";
		data_urodzenia += pesel[3];
		data_urodzenia += "-";
		data_urodzenia += pesel[4];
		data_urodzenia += pesel[5];
		return data_urodzenia;
	}
};
