--drop schema wiezienie cascade; 
create schema wiezienie;

set SEARCH_PATH to wiezienie;

create table wiezienie(id_wiezienie int primary key, 
						nazwa_wiezienia varchar(255) unique);

create table blok(id_blok int primary key, 
				nazwa_bloku varchar(255) not null,
				id_wiezienie int not null,
				foreign key(id_wiezienie) references wiezienie(id_wiezienie));

create table cela(id_cela int primary key,
				id_blok int not null,
				foreign key (id_blok) references blok(id_blok));

create table skazany(id_skazany int primary key,
					pesel int[11] unique,
					imie varchar(255) not null,
					nazwisko varchar(255) not null,
					data_urodzenia date not null,
					id_cela int not null,
					foreign key(id_cela) references cela(id_cela));
				
create table praca(id_praca int  primary key,
					nazwa_pracy varchar(255) not null unique,
					wymiar_godzin int not null,
					id_wiezienie int not null,
					foreign key(id_wiezienie) references wiezienie(id_wiezienie));

create table praca_skazany(id_praca int not null,
							id_skazany int not null unique,
--							constraint unique_job unique (id_praca, id_skazany),
							foreign key (id_praca) references praca(id_praca),
							foreign key (id_skazany) references skazany(id_skazany));

create table pracownik(id_pracownik int primary key,
					pesel int[11] unique,
					imie varchar(255) not null,
					nazwisko varchar(255) not null,
					data_urodzenia date not null,
					stanowisko varchar(255) not null,
					id_wiezienie int not null,
					haslo varchar(255) not null,
					foreign key (id_wiezienie) references wiezienie (id_wiezienie));
create table blok_pracownik(id_blok int not null,
							id_pracownik int not null unique,
							czas_pracy_od time not null,
							czas_pracy_do time not null,
							foreign key(id_blok) references blok(id_blok),
							foreign key(id_pracownik) references pracownik(id_pracownik));

create table wizytator(id_wizytator int primary key,
						pesel int[11] unique,
						imie varchar(255) not null,
						nazwisko varchar(255) not null,
						data_urodzenia date not null);

create table zbrodnia(id_zbrodnia int primary key,
						nazwa_zbrodni varchar(255) not null unique,
						dlugosc_pozbawienia_wolnosci varchar(255) not null);

create table skazany_zbrodnia(id_skazany int not null,
								id_zbrodnia int not null,
								opis_zbrodni varchar(255) not null,
								foreign key(id_skazany) references skazany(id_skazany),
								foreign key (id_zbrodnia) references zbrodnia(id_zbrodnia));

create table skazany_wizytator(id_skazany int not null,
								id_wizytator int not null,
								dzien_widzenia date not null,
								godzina_widzenia time  not null,
								foreign key(id_skazany) references skazany(id_skazany),
								foreign key (id_wizytator) references wizytator(id_wizytator));
							
create table zajecia_resocjalizacyjne(id_zajecia int primary key,
										nazwa_zajec varchar(255) not null,
										opis_zajec varchar(255) not null,
										id_pracownik int not null,
										constraint unique_zajecia unique (id_zajecia, id_pracownik),
										foreign key (id_pracownik) references pracownik(id_pracownik));

create table grupa_zajeciowa(id_grupa int primary key,
							id_zajecia int not null,
							zajecia_od time not null,
							zajecia_do time not null,
							constraint unique_time unique (id_zajecia, zajecia_od, zajecia_do),
							foreign key (id_zajecia) references zajecia_resocjalizacyjne(id_zajecia));
						
create table skazany_grupa(id_skazany int not null,
							id_grupa int not null,
							constraint unique_job unique (id_grupa, id_skazany),
							foreign key (id_skazany) references skazany(id_skazany),
							foreign key (id_grupa) references grupa_zajeciowa(id_grupa));

insert into wiezienie values (1, 'Wiezienie A');
insert into pracownik values (0, '{8,3,0,4,2,7,7,3,9,1,6}', 'Pawel', 'Kolejkowicz', '1983-04-27', 'Naczelnik', 1, 'admin');
insert into zbrodnia values 
(1, 'Zabojstwo', '15 lat'),
(2, 'Napasc', '1 rok'),
(3, 'Porwanie', '4 lata'),
(4, 'Napad', '2 lata'),
(5, 'Wlamanie', '9 miesiecy'),
(6, 'Kradziez', '6 miesiecy'),
(7, 'Podpalenie', '1,5 roku'),
(8, 'Wandalizm', '3 miesiace'),
(9, 'Oszustwo finansowe', '7 lat'),
(10, 'Malwersacja', '6 lat'),
(11, 'Pranie brudnych pieniedzy', '12 lat'),
(12, 'Kradziez tozszamosci', '2 lata'),
(13, 'Gwalt', '4 lata'),
(14, 'Napasc na tle seksualnym', '1 rok'),
(15, 'Molestowanie nieletnich', '4 lata'),
(16, 'Prostytucja', '5 miesiecy'),
(17, 'Posiadanie substancji nielegalnych', '1 rok'),
(18, 'Dystrybuowanie substancji nielegalnych', '2 lata'),
(19, 'Przemyt', '4 lata'),
(20, 'Terroryzm', '10 lat'),
(21, 'Produkcja substancji nielegalnych', '5 lat'),
(22, 'Wykorzystywanie informacji poufnych', '3 lata'),
(23, 'Przekupstwo', '4 lata'),
(24, 'Cyberprzestepstwo', '2 lata'),
(25, 'Prowadzenie pod wplywem substancji oduzajacych', '1 rok'),
(26, 'Potracenie', '9 miesiecy'),
(27, 'Spozywanie alkoholu w miejscach publicznych', '1 miesiac'),
(28, 'Rozboj', '1 rok')