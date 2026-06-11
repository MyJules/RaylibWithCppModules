#[[
enable_std_module(<target> [<scope>])

Purpose
	Makes C++23 `import std;` work with CMake's modules support by adding the
	compiler/vendor-provided standard library module interface units to a target.

Arguments
	<target>
		CMake target name to modify.
	<scope>
		Optional: one of PRIVATE / PUBLIC / INTERFACE (defaults to PRIVATE).
		Use PUBLIC if consumers of the target also need the same std-module setup.

Behavior
	- MSVC:
			* Enables modules with /experimental:module.
			* Adds MSVC's shipped STL module units (std.ixx, std.compat.ixx) from the
				compiler's "modules" directory via a CXX_MODULES FILE_SET.

	- Clang (non-clang-cl):
			* Requires libc++ module units (std.cppm, optionally std.compat.cppm).
			* Adds -stdlib=libc++ and -fexperimental-library to compile options, and
				-stdlib=libc++ to link options, to match the produced BMIs.
			* Adds libc++'s module units via a CXX_MODULES FILE_SET.
			* Errors at configure-time if std.cppm cannot be found.

Notes
	- This function is a no-op for other compilers.
	- Clang path hints are geared toward typical LLVM/libc++ installs on Linux.

Example
	add_library(mylib)
	# ... add your own module interface units ...
	enable_std_module(mylib PUBLIC)
]]

function(enable_std_module target_name)
	set(_scope PRIVATE)
	if(ARGC GREATER 1)
		set(_scope "${ARGV1}")
	endif()

	set(_clang_simulates_msvc FALSE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		set(_clang_simulates_msvc TRUE)
	endif()

	# MSVC: build MSVC's std module from the shipped .ixx files so `import std;` works.
	if(MSVC OR _clang_simulates_msvc)
		if(MSVC)
			target_compile_options(${target_name} ${_scope} /experimental:module)
		elseif(_clang_simulates_msvc)
			# Avoid noisy warnings coming from Microsoft's own module interface units.
			target_compile_options(${target_name} PRIVATE
				-Wno-include-angled-in-module-purview
				-Wno-reserved-module-identifier
			)
		endif()

		set(_msvc_modules_dir "")
		if(DEFINED CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN AND EXISTS "${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}/modules/std.ixx")
			set(_msvc_modules_dir "${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}/modules")
		endif()

		if(NOT _msvc_modules_dir AND DEFINED ENV{VCToolsInstallDir} AND EXISTS "$ENV{VCToolsInstallDir}/modules/std.ixx")
			file(TO_CMAKE_PATH "$ENV{VCToolsInstallDir}/modules" _msvc_modules_dir)
		endif()

		if(WIN32 AND NOT _msvc_modules_dir)
			set(_vswhere "")
			if(EXISTS "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")
				set(_vswhere "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")
			elseif(DEFINED ENV{ProgramFiles} AND EXISTS "$ENV{ProgramFiles}/Microsoft Visual Studio/Installer/vswhere.exe")
				set(_vswhere "$ENV{ProgramFiles}/Microsoft Visual Studio/Installer/vswhere.exe")
			endif()

			if(_vswhere)
				execute_process(
					COMMAND "${_vswhere}"
						-latest
						-products *
						-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64
						-find VC/Tools/MSVC/**/modules/std.ixx
					OUTPUT_VARIABLE _vswhere_std_cppm
					OUTPUT_STRIP_TRAILING_WHITESPACE
					ERROR_QUIET
				)

				if(_vswhere_std_cppm)
					string(REPLACE "\r\n" ";" _vswhere_std_cppm "${_vswhere_std_cppm}")
					string(REPLACE "\n" ";" _vswhere_std_cppm "${_vswhere_std_cppm}")
					list(GET _vswhere_std_cppm 0 _vswhere_std_cppm)
					get_filename_component(_msvc_modules_dir "${_vswhere_std_cppm}" DIRECTORY)
					file(TO_CMAKE_PATH "${_msvc_modules_dir}" _msvc_modules_dir)
				endif()
			endif()
		endif()

		if(NOT _msvc_modules_dir)
			message(FATAL_ERROR
				"MSVC-compatible build is using `import std;` but the MSVC STL module interface units were not found.\n"
				"Expected to find std.ixx under the active Visual C++ tools installation (for example %VCToolsInstallDir%/modules/std.ixx)."
			)
		endif()

		set(_stl_modules "")
		foreach(_f IN ITEMS std.ixx std.compat.ixx)
			if(EXISTS "${_msvc_modules_dir}/${_f}")
				list(APPEND _stl_modules "${_msvc_modules_dir}/${_f}")
			endif()
		endforeach()

		if(_stl_modules)
			target_sources(${target_name} ${_scope}
				FILE_SET std_modules TYPE CXX_MODULES
				BASE_DIRS "${_msvc_modules_dir}"
				FILES ${_stl_modules}
			)
		endif()
		return()
	endif()

	# Clang (non-MSVC): add libc++'s std module interface units so `import std;` works.
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# The apt.llvm.org packages install these under /usr/lib/llvm-<major>/share/libc++/v1.
		string(REGEX MATCH "^[0-9]+" _clang_major "${CMAKE_CXX_COMPILER_VERSION}")
		set(_hints
			"/usr/lib/llvm-${_clang_major}/share/libc++/v1"
			"/usr/lib/llvm-${_clang_major}/share/libc++"
			"/usr/local/lib/llvm-${_clang_major}/share/libc++/v1"
			"/usr/share/libc++/v1"
		)

		find_file(_libcxx_std_cppm
			NAMES std.cppm
			HINTS ${_hints}
			NO_CACHE
		)
		find_file(_libcxx_std_compat_cppm
			NAMES std.compat.cppm
			HINTS ${_hints}
			NO_CACHE
		)

		if(NOT _libcxx_std_cppm)
			message(FATAL_ERROR
				"Clang build is using `import std;` but libc++'s std module interface unit (std.cppm) was not found.\n"
				"On Ubuntu with apt.llvm.org LLVM 20 packages, install: libc++-20-dev and libc++abi-20-dev.\n"
				"Expected to find it under something like /usr/lib/llvm-20/share/libc++/v1/std.cppm."
			)
		endif()

		get_filename_component(_libcxx_cppm_dir "${_libcxx_std_cppm}" DIRECTORY)

		# Ensure the whole target (and its direct consumers) builds/links against
		# libc++ with the same feature set as the generated std BMIs.
		target_compile_options(${target_name} PUBLIC -stdlib=libc++ -fexperimental-library)
		target_link_options(${target_name} PUBLIC -stdlib=libc++)

		# Avoid noisy warnings coming from libc++'s own module units.
		target_compile_options(${target_name} PRIVATE -Wno-reserved-module-identifier)

		set(_libcxx_modules "${_libcxx_std_cppm}")
		if(_libcxx_std_compat_cppm)
			list(APPEND _libcxx_modules "${_libcxx_std_compat_cppm}")
		endif()

		target_sources(${target_name} ${_scope}
			FILE_SET std_modules TYPE CXX_MODULES
			BASE_DIRS "${_libcxx_cppm_dir}"
			FILES ${_libcxx_modules}
		)
	endif()
endfunction()
