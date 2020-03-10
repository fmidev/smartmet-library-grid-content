%define DIRNAME grid-content
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: grid file handling library
Name: %{SPECNAME}
Version: 20.3.10
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
BuildRequires: smartmet-library-gis-devel >= 20.2.18
BuildRequires: smartmet-library-grid-files-devel >= 20.3.10
BuildRequires: smartmet-library-macgyver-devel >= 20.3.5
BuildRequires: smartmet-library-spine-devel >= 20.3.9
Requires: boost-date-time
Requires: hiredis-devel
Requires: libcurl-devel
Requires: lua-devel
Requires: omniORB
Requires: postgresql95
Requires: smartmet-library-gis >= 20.2.18
Requires: smartmet-library-grid-files >= 20.3.10
Requires: smartmet-library-macgyver >= 20.3.5
Requires: smartmet-library-spine >= 20.3.9
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
* Tue Mar 10 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.10-1.fmi
- Memory locking is now configurable, since EC locking may take too much memory on small servers
* Thu Mar  5 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.5-1.fmi
- Added dem and land cover information access into the QueryServer
* Tue Feb 25 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.2.25-1.fmi
- Improved producer information gathering
* Wed Feb 19 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.2.19-1.fmi
- Enabled native message fetching
- Performance improvements
- Disabling geometry chanegs when producer is not defined
* Wed Jan 29 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.1.29-1.fmi
- Fixed memory leaks
- Removed unnecessary attributes
- Make sure configuration updates are noticed
* Tue Jan 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.1.21-1.fmi
- Added thread protection for the content cache
- Significant performance improvements
- Added message position and size to the CSV header
* Thu Jan 16 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.1.16-1.fmi
- Performance improvements
- Fixed event synchronization
* Fri Dec 13 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.13-1.fmi
- Fixed content synchronization
* Wed Dec 11 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.11-1.fmi
- Added size checks on redis operations
* Wed Dec  4 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.12.4-1.fmi
- Added new return attributes
- Fixed crop area coordinates
* Fri Nov 22 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.22-1.fmi
- Fixed grid conversion bug
* Wed Nov 20 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.20-1.fmi
- Repackaged due to newbase and spine ABI changes
* Wed Nov 13 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.13-1.fmi
- Fixed polygon based search when a LUA function is used
* Thu Nov  7 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.11.7-1.fmi
- Added alternative query methods
* Fri Oct 25 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.25-1.fmi
- New service methods
- Performance improvements
- Minor fixes
* Tue Oct  1 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.10.1-1.fmi
- Repackaged due to ABI changes in SmartMet libraries
* Fri Sep 20 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.20-1.fmi
- Added -fno-omit-frame-pointer for better profiling
* Thu Sep 19 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.9.19-1.fmi
- New information flags
- Improvements to virtual file management
- Fixed time interpolation of vector data
* Fri Aug  9 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.8.9-1.fmi
- Numerous improvements
* Tue May 14 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.5.15-1.fmi
- Updated grid-files dependency
* Fri May 10 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.5.10-1.fmi
- Use original parameter values in new grib files
* Mon May  6 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.5.6-1.fmi
- Support for downloading GRIB files
* Tue Apr  2 2019 Mika Heiskanen <mika.heiskanen@fmi.fi> - 19.4.2-1.fmi
- Bug fix to origintime handling
- Default forecastType is now 'undefined'
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
