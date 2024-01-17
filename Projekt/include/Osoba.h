/**
 * @file Osoba.h
 * @author Jakub Mikusek
 * @brief Plik zawierajacy klase reprezentujaca osobe
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <string>
#include <vector>
#include "Obiekt.h"
/**
 * @brief Klasa reprezentujaca osobe
 * 
 */
struct Osoba : public Obiekt{
	std::string pesel;
	std::string imie;
	std::string nazwisko;
	std::string data_urodzenia;
	/**
	 * @brief Domyslny konstruktor pozwalajacy na stworzenie obiektu osoba bez zadnych danych
	 * 
	 */
	Osoba() : Osoba{ 0, "", "", "", "" } {}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie obosy o zadanych wlasnosciach, gdzie data urodzenia jest wyciagana z numeru pesel
	 * 
	 * @param id ID nowej osoby
	 * @param new_pesel numer pesel nowej osoby
	 * @param new_imie imie nowej osoby
	 * @param new_nazwisko nazwisko nowej osoby
	 */
	Osoba(int id, std::string new_pesel, std::string new_imie, std::string new_nazwisko)
		:Obiekt{ id },
		pesel{ new_pesel },
		imie{ new_imie },
		nazwisko{ new_nazwisko },
		data_urodzenia{ Data_urodzenia(new_pesel) }
	{}
	/**
	 * @brief Konstruktor pozwalajacy na stworzenie obosy o zadanych wlasnosciach wraz z podana data urodzenia
	 * 
	 * @param id ID nowej osoby
	 * @param new_pesel numer pesel nowej osoby
	 * @param new_imie imie nowej osoby
	 * @param new_nazwisko nazwisko nowej osoby
	 * @param new_data_urodzenia data urodzenia nowej osoby
	 */
	Osoba(int id, std::string new_pesel, std::string new_imie, std::string new_nazwisko, std::string new_data_urodzenia)
		:Obiekt{ id },
		pesel{ Czytalny_pesel(new_pesel) },
		imie{ new_imie },
		nazwisko{ new_nazwisko },
		data_urodzenia{ new_data_urodzenia }
	{}
	/**
	 * @brief Metoda pozwalajaca na przeksztalcenie numeru pesel na format odpowiedni dla bazy danych
	 * 
	 * @param pesel numer pesel do przeksztalczenia
	 * @return std::string przeksztalcony numer pesel
	 */
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
	/**
	 * @brief Metoda przeksztalcajaca numer pesel z formatu bazodanowego na format czytelny
	 * 
	 * @param pesel pesel do przeksztalcenia
	 * @return std::string czytalna forma numeru pesel
	 */
	std::string Czytalny_pesel(std::string pesel) {
		std::string r_pesel;
		for (int i = 1; i < pesel.length(); i += 2) r_pesel += pesel[i];
		return r_pesel;
	}
	/**
	 * @brief Metoda pozwalajaca wyciagniecie daty urodzenia z poprawnego numeru pesel
	 * 
	 * @param pesel numer pesel
	 * @return std::string otrzymana data urodzenia
	 */
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
