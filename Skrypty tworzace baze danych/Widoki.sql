set SEARCH_PATH to wiezienie;

--drop view skazani_informacja;
create view skazani_informacja
as 
with ilosc_uczeszczanych_zajec as
(
	select id_skazany, count(id_grupa) as ilosc_zajec from skazany_grupa sg
	group by id_skazany
), ilosc_wizytacji as(
	select id_skazany, count(*) as ilosc_wizyt from skazany_wizytator sw 
	group by id_skazany
)
select s.id_skazany, s.pesel, s.imie, s.nazwisko, s.data_urodzenia, s.id_cela, z.nazwa_zbrodni, z.dlugosc_pozbawienia_wolnosci, sz.opis_zbrodni, p.nazwa_pracy, p.wymiar_godzin, iuz.ilosc_zajec, iw.ilosc_wizyt
from skazany s 
join skazany_zbrodnia sz on sz.id_skazany = s.id_skazany
join zbrodnia z on z.id_zbrodnia = sz.id_zbrodnia
left join praca_skazany ps on ps.id_skazany = s.id_skazany
left join praca p on p.id_praca = ps.id_praca
left join ilosc_uczeszczanych_zajec iuz on iuz.id_skazany = s.id_skazany 
left join ilosc_wizytacji iw on iw.id_skazany = s.id_skazany order by 5;

create view zajecia_informacja
as select zr.id_zajecia, zr.nazwa_zajec, zr.opis_zajec, p.id_pracownik, p.imie, p.nazwisko
from zajecia_resocjalizacyjne zr 
join pracownik p on p.id_pracownik = zr.id_pracownik;

--drop view pracownicy_informacja;
create view pracownicy_informacja
as 
with ilosc_prowadzonych_zajec as
(
	select id_pracownik, count(id_zajecia) as ilosc_zajec from zajecia_resocjalizacyjne zr
	group by id_pracownik
)
select p.id_pracownik, p.pesel, p.imie, p.nazwisko, p.data_urodzenia, p.stanowisko, p.id_wiezienie, p.haslo, b.id_blok, b.nazwa_bloku, bp.czas_pracy_od, bp.czas_pracy_do, ipz.ilosc_zajec
from wiezienie w
join pracownik p on p.id_wiezienie = w.id_wiezienie
left join blok_pracownik bp on bp.id_pracownik = p.id_pracownik
left join blok b on b.id_blok = bp.id_blok
left join ilosc_prowadzonych_zajec ipz on ipz.id_pracownik = p.id_pracownik;

create view statystyka_blokow
as select w.nazwa_wiezienia, b.nazwa_bloku, count(distinct bp.id_pracownik) as straznicy_ilosc, count(distinct s.id_skazany) as skazani_ilosc
from wiezienie w
left join blok b on b.id_wiezienie = w.id_wiezienie
left join blok_pracownik bp on bp.id_blok = b.id_blok
left join cela c on c.id_blok = b.id_blok
left join skazany s on s.id_cela = c.id_cela
group by w.nazwa_wiezienia, b.nazwa_bloku order by 1;

create view statystyka_zajec as 
select w.nazwa_wiezienia, zr.nazwa_zajec, count(distinct p.id_pracownik) as instruktorzy_ilosc, count(distinct sg.id_skazany) as skazani_ilosc
from wiezienie w
join pracownik p on p.id_wiezienie = w.id_wiezienie 
left join zajecia_resocjalizacyjne zr on zr.id_pracownik = p.id_pracownik 
left join grupa_zajeciowa gz on gz.id_zajecia = zr.id_zajecia
left join skazany_grupa sg on sg.id_grupa = sg.id_grupa where p.stanowisko = 'Instruktor'
group by w.nazwa_wiezienia, zr.nazwa_zajec order by 1;

create view statystyka_wiezienia
as select w.nazwa_wiezienia, count(distinct p.id_pracownik) as pracownik_count, count(distinct wi.id_wizytator) as wizytator_count, count(distinct s.id_skazany) as skazany_count
from wiezienie w 
left join pracownik p on p.id_wiezienie = w.id_wiezienie
left join blok b on b.id_wiezienie = w.id_wiezienie
left join cela c on c.id_blok = b.id_blok
left join skazany s on s.id_cela = c.id_cela 
left join skazany_wizytator sw on sw.id_skazany = s.id_skazany
left join wizytator wi on wi.id_wizytator = sw.id_wizytator
group by w.nazwa_wiezienia order by 1;

--drop view zajecia_zapisani_skazani;
create view zajecia_zapisani_skazani
as select zr.nazwa_zajec, zr.id_zajecia, gz.id_grupa, gz.zajecia_od, gz.zajecia_do, sg.id_skazany
from zajecia_resocjalizacyjne zr  
join grupa_zajeciowa gz on gz.id_zajecia = zr.id_zajecia
join skazany_grupa sg on sg.id_grupa = gz.id_grupa order by 1;

--drop view statystyka_praca;
create view statystyka_praca
as select w.nazwa_wiezienia, p.nazwa_pracy, p.wymiar_godzin, count(s.id_skazany)
from praca p
join wiezienie w on w.id_wiezienie = p.id_wiezienie
left join praca_skazany ps on ps.id_praca = p.id_praca
left join skazany s on s.id_skazany = ps.id_skazany
group by w.nazwa_wiezienia, p.nazwa_pracy, p.wymiar_godzin order by 1;
