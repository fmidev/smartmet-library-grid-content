%define DIRNAME grid-content
%define LIBNAME smartmet-%{DIRNAME}
%define SPECNAME smartmet-library-%{DIRNAME}
Summary: grid file handling library
Name: %{SPECNAME}
Version: 22.1.13
Release: 1%{?dist}.fmi
License: MIT
Group: Development/Libraries
URL: https://github.com/fmidev/smartmet-library-grid-content
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: boost169-devel
BuildRequires: gcc-c++
BuildRequires: gdal33-devel
BuildRequires: hiredis-devel
BuildRequires: libcurl-devel
BuildRequires: libicu-devel
BuildRequires: lua-devel
BuildRequires: make
BuildRequires: omniORB-devel
BuildRequires: rpm-build
BuildRequires: smartmet-library-gis-devel >= 21.12.7
BuildRequires: smartmet-library-grid-files-devel >= 22.1.13
BuildRequires: smartmet-library-macgyver-devel >= 22.1.10
BuildRequires: smartmet-library-spine-devel >= 22.1.5
Requires: boost169-date-time
Requires: hiredis-devel
Requires: libcurl-devel
Requires: lua-devel
Requires: omniORB
Requires: smartmet-library-gis >= 21.12.7
Requires: smartmet-library-grid-files >= 22.1.13
Requires: smartmet-library-macgyver >= 22.1.10
Requires: smartmet-library-spine >= 22.1.5
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
Requires: smartmet-library-macgyver-devel >= 22.1.10
Requires: smartmet-library-gis-devel >= 21.12.7
Requires: smartmet-library-spine-devel >= 22.1.5
Requires: smartmet-library-grid-files-devel >= 22.1.13

%description -n %{SPECNAME}-devel
FMI Grid Content library development files

%files -n %{SPECNAME}-devel
%defattr(0664,root,root,0775)
%{_includedir}/smartmet/%{DIRNAME}

%changelog
* Thu Jan 13 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.1.13-1.fmi
- Improved climatological parameter handling
* Wed Jan  5 2022 Mika Heiskanen <mika.heiskanen@fmi.fi> - 22.1.5-1.fmi
- Improved parameter mappings
* Tue Dec  7 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.12.7-1.fmi
- Update to postgresql 13 and gdal 3.3
* Thu Nov 18 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.11.18-1.fmi
- Fixed content record removal
* Mon Nov 15 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.11.15-1.fmi
- Added modification time information
* Thu Nov 11 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.11.11-1.fmi
- Minor updates
* Fri Oct 29 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.29-1.fmi
- New service methods
* Tue Oct 19 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.19-1.fmi
- Added low level filtering capability
* Mon Oct 11 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.11-1.fmi
- Simplified grid storage structures
* Mon Oct  4 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.10.4-1.fmi
- Minor improvements
* Wed Sep 15 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.9.15-1.fmi
- NetCDF support
* Tue Aug 31 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.8.31-1.fmi
- Repackaged due to Spine changes
* Wed Aug 18 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.8.18-1.fmi
- Improved data search when the origintime is given
* Mon Aug  2 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.8.2-1.fmi
- Use atomic_shared_ptr instead of atomic_store/load
* Tue Jul 27 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.7.27-1.fmi
- Fixed some integer to string assignments to use std::to_string
* Tue Jun  8 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.6.8-1.fmi
- Reduced memory use of data archive browsers
* Tue Jun  1 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.6.1-1.fmi
- Reduced memory usage
* Tue May 25 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.5.25-1.fmi
- Repackaged due to grid-files ABI changes
* Mon Mar 29 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.3.29-1.fmi
- Updated content sorting/searching orders
* Mon Mar 15 2021 Andris Pavēnis <andris.pavenis@fmi.fi> - 21.3.15-1.fmi
- Add missing dependencies for devel package
* Thu Mar  4 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.3.4-1.fmi
- Checking that the grid messages are not added multiple times
- Initializing messages before the grid file is added into the GridFileManager
* Wed Mar  3 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.3.3-1.fmi
- Added possibilify to disable grid-services
* Wed Feb  3 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.2.3-1.fmi
- Prefer time_t over std::string for speed
* Wed Jan 27 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.27-1.fmi
- Performance improvements
* Tue Jan 19 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.19-1.fmi
- Performance improvements
* Mon Jan 18 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.18-1.fmi
- Performance improvements
* Thu Jan 14 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.14-1.fmi
- Repackaged smartmet to resolve debuginfo issues
* Mon Jan 11 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.11-1.fmi
- Repackaged due to grid-files API changes
* Mon Jan  4 2021 Mika Heiskanen <mika.heiskanen@fmi.fi> - 21.1.4-1.fmi
- Fixed deprecated copy constructors
* Mon Dec 28 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.28-1.fmi
- Updated GDAL dependencies
- Updated postgresql dependencies
* Thu Dec  3 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.12.3-1.fmi
- Enabled use of a secondary redis server
- Improved level handling
* Mon Nov 30 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.11.30-1.fmi
- Improved updating of configuration structures
* Tue Nov 24 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.11.24-2.fmi
- Fixed conversion of modification times
* Tue Nov 24 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.11.24-1.fmi
- Performance improvements
- New C+ query functions
- Improved redis locking mechanism
- Fixed alias usage as a part of parameter identifier
- Improved content deletion time handling
- Added numeric forecastTime into the ContentInfo class
* Thu Oct 22 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.10.22-1.fmi
- Performance improvements
* Thu Oct 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.10.15-1.fmi
- Added new C++ query functions
- Improved Redis locking mechanism 
- Minor bug fixes
- Refactored code
- Use boost atomic operations
- Removing grid files whose predefined message positions exceed the file size
* Tue Oct  6 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.10.6-1.fmi
- Deadlock fix
* Thu Oct  1 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.10.1-1.fmi
- Removing grid files which predefined message positions exceeds the file size
- Use atomic shared pointers to speed up search structures
* Thu Sep 24 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.24-1.fmi
- Rebuilt due to debuginfo issues
* Wed Sep 23 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.23-1.fmi
- Avoid locking by using copied data structures for R/O operations
- Use Fmi::Exception instead of Spine::Exception
* Mon Sep 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.21-1.fmi
- Fixed WriteLock to ReadLock in getFileById
* Fri Sep 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.18-1.fmi
- Performance improvements
* Tue Sep 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.15-2.fmi
- Fixed time interpolation not to extrapolate
* Tue Sep 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.15-1.fmi
- Performance improvements
* Mon Sep 14 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.14-1.fmi
- GRID file support ABI changed
* Mon Sep  7 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.7-1.fmi
- Updated grid coordinate processing and caching
* Thu Sep  3 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.9.3-1.fmi
- Fixed cache size initialization which caused a crash if redis is empty
* Mon Aug 31 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.31-1.fmi
- New service methods and performance optimizations
- Added automatic content synchronization from CSV files
* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-2.fmi
- Use Fmi::to_string instead of std::to_string for speed
* Fri Aug 21 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.21-1.fmi
- Upgrade to fmt 6.2
* Wed Aug 19 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.19-1.fmi
- Use case sensitive time comparisons for better speed
* Tue Aug 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.18-1.fmi
- More speed improvements
* Fri Aug 14 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.14-1.fmi
- Further speed improvements
* Tue Aug 11 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.8.11-1.fmi
- Speed improvements
* Mon Jun 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.6.15-1.fmi
- Small improvements to file locking
* Mon Jun  8 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.6.8-1.fmi
- Improved virtual file functionality
- Improved forecast number handling
* Tue May 19 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.5.19-1.fmi
- Allow redis locks from different users
* Mon May 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.5.18-1.fmi
- Fixed a bug in redis locking
* Fri May 15 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.5.15-1.fmi
- Added checks for existence of memory mapped files
* Thu Apr 30 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.30-1.fmi
- Improved search with grib-id
- Integrated corba-server
* Sat Apr 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.18-1.fmi
- Upgrade to Boost 1.69
* Fri Apr  3 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.4.3-1.fmi
- Request forwarding is now configurable
- Added producer alias handling
- Minor fixes
* Wed Mar 18 2020 Mika Heiskanen <mika.heiskanen@fmi.fi> - 20.3.18-1.fmi
- Fixed temporary memory allocation from stack to heap
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
