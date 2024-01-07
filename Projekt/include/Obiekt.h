#pragma once
#include "Baza.h"

struct Obiekt {
	int id;
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
	virtual std::string Wstaw(Baza& baza) = 0;
	virtual std::string Edytuj(Baza& baza) { return ""; };
};
