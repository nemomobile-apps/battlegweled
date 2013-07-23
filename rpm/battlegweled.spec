Summary: Fast action puzzle game
Name: battlegweled
Version: 0.9
Release: 7
Source: %{name}-%{version}.tar.gz
URL: http://maemo.org/downloads/product/Maemo5/battlegweled/
License: GPL2
Group: System/GUI/Other
BuildRequires: pkgconfig(sdl2)
BuildRequires: pkgconfig(glesv1_cm)
BuildRequires: pkgconfig(egl)


%description
Swap jewels' positions to build up rows and columns as quickly as you can. Play
in single player mode. Based on the famous game Bejeweled.

%prep
%setup -q 

%build
make

%install
make DESTDIR=%{?buildroot} install

%files
%defattr(-,root,root,-)
%doc ChangeLog SOURCES
/opt/battlegweled/*
/usr/share/applications/*
