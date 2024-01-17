#pragma once

#define GRUPA_INFO int, int, std::string, std::string

struct Grupa : public Obiekt{
	int id_zajecia;
	std::string zajecia_od{"00:00"};
	std::string zajecia_do{"00:00"};

	Grupa(int id, int new_id_zajecia)
		:Obiekt{ id },
		id_zajecia{ new_id_zajecia }
	{}
	Grupa(std::tuple<GRUPA_INFO> const& row)
		:Obiekt{std::get<0>(row)},
		id_zajecia{std::get<1>(row)},
		zajecia_od{ std::get<2>(row) },
		zajecia_do{ std::get<3>(row) }
	{}
	std::string Wstaw(Baza& baza) override {
		try {
			baza.query("INSERT INTO grupa_zajeciowa VALUES (" +
				std::to_string(id) +
				", '" + std::to_string(id_zajecia)+"'" +
				", " + (zajecia_od.length() == 0 ? "NULL" : "'" + zajecia_od + "'") +
				", " + (zajecia_do.length() == 0 ? "NULL" : "'" + zajecia_do + "'") + ");");
		}
		catch (std::exception& e) {
			return e.what();
		}
		return "";
	}
};
