set SEARCH_PATH to wiezienie;

create or replace procedure poprawnosc_pesel(nowy_pesel int[11]) as
$$
	declare
		wagi int[11] := '{1,3,7,9,1,3,7,9,1,3}';
		suma int := 0;
	begin
		for i in 1..10 loop
			suma = suma + nowy_pesel[i] * wagi[i];
			suma = suma % 10;
		end loop;
		suma = 10 - suma;
		suma = suma % 10;
		if nowy_pesel[11] <> suma then
			raise exception 'Niepoprawny numer PESEL';
		end if;
	end;	
$$
language plpgsql;

create or replace procedure poprawnosc_godnosc(nowy_imie varchar(255), nowy_nazwisko varchar(255)) as
$$
	begin
		if length(nowy_imie) = 0 then 
			raise exception 'Imie nie moze byc puste';
		end if;
		if length(nowy_nazwisko) = 0 then 
			raise exception 'Nazwisko nie moze byc puste';
		end if;
	end;	
$$
language plpgsql;

create or replace procedure liczebnosc_cela(id int) as
$$
	declare
		liczba int;
	begin
		select COUNT(*) into liczba from skazany s where s.id_cela = id;
		if liczba >= 2 then
			raise exception 'Cela jest juz pelna';
		end if;
	end;
$$
language plpgsql; 

call liczebnosc_cela(1); 

create or replace function poprawnosc_danych()
returns trigger as
$$
	begin
		call poprawnosc_pesel(new.pesel);
		call poprawnosc_godnosc(new.imie, new.nazwisko);
		return new;
	end;
$$
language plpgsql;

create or replace function poprawnosc_skazany()
returns trigger as
$$
	begin
		call poprawnosc_pesel(new.pesel);
		call poprawnosc_godnosc(new.imie, new.nazwisko);
		call liczebnosc_cela(new.id_cela);
		return new;
	end;
$$
language plpgsql;

--drop trigger poprawnosc_skazany on skazany;
create trigger poprawnosc_skazany
	before insert or update on skazany
	for each row 
	execute function poprawnosc_skazany();

create trigger poprawnosc_danych
	before insert or update on pracownik
	for each row 
	execute function poprawnosc_danych();
	
create trigger poprawnosc_danych
	before insert or update on wizytator
	for each row 
	execute function poprawnosc_danych();
