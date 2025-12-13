// BeamMP, the BeamNG.drive multiplayer mod.
// Copyright (C) 2025 BeamMP Ltd., BeamMP team and contributors.
//
// BeamMP Ltd. can be contacted by electronic mail via contact@beammp.com.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Module.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #include <windows.h>

    dll_handle dll_load(std::string_view path) {
        return reinterpret_cast<dll_handle>(LoadLibraryA(std::string(path).c_str()));
    }

    dll_handle dll_unload(dll_handle dll) {
        FreeLibrary(reinterpret_cast<HMODULE>(dll));
    }

    dll_sym dll_get_proc_address(dll_handle dll) {
        return reinterpret_cast<dll_sym>(GetProcAddress(reinterpret_cast<HMODULE>(dll), std::string(proc_name).c_str()));
    }

    const char* dll_load_error() {
        return std::string("win32 error code: " + GetLastError()).c_str();
    }
#else
    #include <dlfcn.h>

    dll_handle dll_load(std::string_view path) {
        return reinterpret_cast<dll_handle>(dlopen(std::string(path).c_str(), RTLD_LAZY | RTLD_GLOBAL));
    }

    dll_handle dll_unload(dll_handle dll) {
        dlclose(reinterpret_cast<void*>(dll));
    }

    dll_sym dll_get_proc_address(dll_handle dll, std::string_view proc_name) {
        return reinterpret_cast<dll_sym>(dlsym(dll, std::string(proc_name).c_str()));
    }

    const char* dll_load_error() {
        return dlerror();
    }
#endif

std::tuple<bool, std::string_view> Module::Load() {
    // attempt to load the module
    auto dll = dll_load(this->modPath.string());
    if (dll == nullptr) {
        // module failed to load
        return { false, dll_load_error() };
    }

    this->modHandle = dll;

    // module load successful, we now need to fetch our "mp_module_load", and "mp_module_unload" procedures.
    auto mp_module_load = dll_get_proc_address(this->modHandle, std::string_view(MODULE_LOAD_SYM));
    if (mp_module_load == nullptr) {
        // failed to fetch proc
        return { false, dll_load_error() };
    }

    auto mp_module_unload = dll_get_proc_address(this->modHandle, std::string_view(MODULE_UNLOAD_SYM));
    if (mp_module_unload == nullptr) {
        // failed to fetch proc
        return { false, dll_load_error() };
    }

    // both procs were found, store them to the class.
    this->modLoadSym = mp_module_load;
    this->modUnloadSym = mp_module_unload;

    return { true, std::string_view() };
}

void Module::Unload() {
    dll_unload(this->modHandle);
}
