%define game_name berusky2

Summary:        A datafile for Berusky
Name:           berusky2-data
Version:        0.3
Release:        1%{?dist}
License:        GPLv2+
Group:          Amusements/Games
Source:         http://www.anakreon.cz/download/%{name}-%{version}.tar.gz
URL:            http://www.anakreon.cz/en/Berusky2.htm
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:      noarch

%description
Berusky 2 is a game that challenges your visual/spatial thinking and ability
to find a way to resolve a logic task. Using five bugs, you'll go
through an adventure full of various puzzles spread across nine episodes.

Individual episodes differ in appearance and difficulty,
which increases throughout the game.

This package contains a data for the game, i.e. files with graphics, levels,
game rules and configuration.

%prep
%setup -q -n %{name}-%{version}

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_datadir}/%{game_name}

mv bitmap  %{buildroot}%{_datadir}/%{game_name}
mv data %{buildroot}%{_datadir}/%{game_name}
mv game %{buildroot}%{_datadir}/%{game_name}
mv game_data %{buildroot}%{_datadir}/%{game_name}
mv items %{buildroot}%{_datadir}/%{game_name}
mv materials %{buildroot}%{_datadir}/%{game_name}
mv out %{buildroot}%{_datadir}/%{game_name}
mv Save %{buildroot}%{_datadir}/%{game_name}
mv textures %{buildroot}%{_datadir}/%{game_name}

mkdir -p %{buildroot}/var/games/%{game_name}
install -m 644 berusky3d.ini %{buildroot}/var/games/%{game_name}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%dir %{_datadir}/%{game_name}
%{_datadir}/%{game_name}/*
%dir /var/games/%{game_name}
/var/games/%{game_name}/*

%changelog
* Mon Aug 15 2011 Martin Stransky <stransky@redhat.com> 0.3-1
- initial build

