%define DIRNAME grid-content
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: grid file handling library
Name: %{SPECNAME}
Version: 18.2.8
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-grid-content
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: boost-devel
BuildRequires: gcc-c++
BuildRequires: hiredis-devel
BuildRequires: libcurl-devel
BuildRequires: libicu-devel
BuildRequires: lua-devel
BuildRequires: make
BuildRequires: omniORB-devel
BuildRequires: smartmet-library-grid-files >= 18.1.22
Requires: hiredis-devel
Requires: libcurl-devel
Requires: lua-devel
Requires: omniORB
Requires: boost-date-time
Provides: %{SPECNAME}

%description
FMI Grid Content handling library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}
 
%build
make %{_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%defattr(0775,root,root,0775)
%{_libdir}/libsmartmet-%{DIRNAME}.so

%package -n %{SPECNAME}-devel
Summary: FMI Grid Content library development files
Provides: %{SPECNAME}-devel
Requires: %{SPECNAME}

%description -n %{SPECNAME}-devel
FMI Grid Content library development files

%files -n %{SPECNAME}-devel
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}

%changelog
* Thu Feb  8 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.2.8-1.fmi
- First packaged version


