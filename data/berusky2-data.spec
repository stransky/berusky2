%global game_name berusky2

Summary:        A datafile for Berusky
Name:           berusky2-data
Version:        0.6
Release:        1%{?dist}
License:        GPLv2+
Group:          Amusements/Games
Source:         http://downloads.sourceforge.net/%{game_name}/%{name}-%{version}.tar.bz2
URL:            http://www.anakreon.cz/en/Berusky2.htm
BuildArch:      noarch


%description
This package contains the game data for %{game_name}, i.e. files with graphics,
levels, game rules and configuration.


%prep
%setup -q


%install
mkdir -p %{buildroot}%{_datadir}/%{game_name}

mv bitmap \
   data \
   game \
   game_data \
   items \
   materials \
   out \
   textures \
   music \
   sound \
   %{buildroot}%{_datadir}/%{game_name}


%files
%{_datadir}/%{game_name}


%changelog
* Sun Mar 4 2012 Martin Stransky <stransky@redhat.com> 0.6-1
- new upstream tarball

* Thu Jan 12 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 0.5-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Aug 30 2011 Martin Stransky <stransky@redhat.com> 0.5-1
- new upstream tarball
- spec clean up (by Richard Shaw)

* Sat Aug 20 2011 Martin Stransky <stransky@redhat.com> 0.4-1
- ini file and save/profile dir were moved to binary package

* Mon Aug 15 2011 Martin Stransky <stransky@redhat.com> 0.3-1
- initial build
