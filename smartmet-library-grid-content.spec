%define DIRNAME grid-content
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: grid file handling library
Name: %{SPECNAME}
Version: 19.3.19
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-grid-content
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: boost-devel
BuildRequires: gcc-c++
BuildRequires: gdal-devel
BuildRequires: hiredis-devel
BuildRequires: libcurl-devel
BuildRequires: libicu-devel
BuildRequires: lua-devel
BuildRequires: make
BuildRequires: omniORB-devel
BuildRequires: postgresql95-devel
BuildRequires: rpm-build
BuildRequires: smartmet-library-gis-devel >= 19.3.14
BuildRequires: smartmet-library-grid-files-devel >= 19.3.19
BuildRequires: smartmet-library-macgyver-devel >= 18.11.24
BuildRequires: smartmet-library-spine-devel >= 19.3.14
Requires: boost-date-time
Requires: hiredis-devel
Requires: libcurl-devel
Requires: lua-devel
Requires: omniORB
Requires: postgresql95
Requires: smartmet-library-gis >= 19.3.14
Requires: smartmet-library-grid-files >= 19.3.19
Requires: smartmet-library-macgyver >= 18.11.24
Requires: smartmet-library-spine >= 19.3.14
Provides: %{SPECNAME}

%description
FMI Grid Content handling library

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{SPECNAME}

# rpm -qi --filesbypkg lua-devel.x86_64 0:5.1.4-15.el7

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
* Tue Mar 19 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.3.19-1.fmi
- Recompiled due to grid-files header changes
* Fri Mar 15 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.3.15-1.fmi
- Various improvements
* Fri Feb 15 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.2.15-1.fmi
- Various improvements
* Thu Jan 17 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.1.17-1.fmi
- Various improvements
* Tue Oct 23 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.10.23-1.fmi
- New service methods
* Mon Oct 15 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.10.15-1.fmi
- Added a possibility to query information by forecast number range
- Fixed the alias list cleaning before reloading
* Mon Oct  1 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.10.1-1.fmi
- New release
* Wed Sep 26 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.26-1.fmi
- Version update
* Mon Sep 10 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.10-2.fmi
- Bug fix to forecast time handling
* Mon Sep 10 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.10-1.fmi
- Version update
* Mon Sep  3 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.9.3-1.fmi
- Bug fix release
* Thu Aug 30 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.30-1.fmi
- Silenced CodeChecker warnings
* Tue Aug 28 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.28-1.fmi
- Package latest version
* Mon Aug 27 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.8.27-1.fmi
- Package latest version
* Thu Jun 14 2018 Roope Tervo <roope.tervo@fmi.fi> - 18.6.14-1.fmi
- Build for testing
* Thu Feb  8 2018 Mika Heiskanen <mika.heiskanen@fmi.fi> - 18.2.8-1.fmi
- First packaged version
