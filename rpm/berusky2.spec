Summary:        Sokoban clone
Name:           berusky2
Version:        0.3
Release:        1%{?dist}
License:        GPLv2+
Group:          Amusements/Games
Source:         http://www.anakreon.cz/download/%{name}-%{version}.tar.gz
Source1:        berusky2.desktop
Source2:        berusky2.png
Source3:        berusky3d.ini
URL:            http://www.anakreon.cz/en/Berusky2.htm
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       berusky2-data
Requires:       SDL
Requires:       SDL_image
BuildRequires:  SDL-devel
BuildRequires:  SDL_image-devel
BuildRequires:  gtk2-devel
BuildRequires:  desktop-file-utils


%description
Berusky 2 is a game that challenges your visual/spatial thinking
and ability to find a way to resolve a logic task. Using five bugs,
you'll go through an adventure full of various puzzles spread across
nine episodes. Individual episodes differ in appearance and difficulty,
which increases throughout the game.

This package contains a binary for the game.

%prep
%setup -q -n %{name}-%{version}

%build
%configure \
    CFLAGS="$RPM_OPT_FLAGS"

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

mkdir -p %{buildroot}/%{_docdir}/%{name}-%{version}

pushd %{buildroot}/usr/doc/%{name}
mv * %{buildroot}%{_docdir}/%{name}-%{version}
popd

mkdir -p %{buildroot}/var/games/%{game_name}
install -m 644 %{SOURCE3} %{buildroot}/var/games/%{game_name}

rm -rf %{buildroot}/%{_datadir}/%{name}

# Install icon and desktop file
mkdir -p $RPM_BUILD_ROOT%{_datadir}/icons/hicolor/32x32/apps
cp %{SOURCE2} $RPM_BUILD_ROOT%{_datadir}/icons/hicolor/32x32/apps

desktop-file-install --vendor fedora --dir $RPM_BUILD_ROOT%{_datadir}/applications --add-category X-Fedora %{SOURCE1}

%clean
rm -rf %{buildroot}

%post
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
 %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%postun
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
 %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%files
%defattr(-, root, root)
%doc %dir %{_docdir}/%{name}-%{version}
%doc %{_docdir}/%{name}-%{version}/*
%{_bindir}/berusky2
%{_datadir}/applications/fedora-berusky2.desktop
%{_datadir}/icons/hicolor/32x32/apps/berusky2.png
%dir /var/games/%{game_name}
/var/games/%{game_name}/*

%changelog
* Mon Aug 15 2011 Martin Stransky <stransky@redhat.com> 0.3-1
- initial build
