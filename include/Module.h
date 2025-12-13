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

/**
 * The point of this system, is to allow for the community to be able to add components to the BeamMP server, without having to re-compile/re-distribute the server each time.
 * This allows for further customisation/extensibility, as there will be server owners who will require additional components, not provided by the stock BeamMP server.
 * 
 * Examples of this are:
 * - A HTTP module, allowing you to make POST/GET/etc requests from the Lua API.
 * - A storage driver, allowing you to store/fetch data from a database.
 * - A realistic weather module, fetching data on the weather in a city in Italy, and replicating that in-game.
 * 
 * Aims of the module:
 * - Load a shared library/dynamic link library (.dll/.so/.dylib) using both the POSIX and Windows APIs, initialise them and check that the required exports are available.
 * - Call the exports where required.
 * - Expose a C ABI, allowing module developers to interact with the Lua scripting engine.
 * - Allow each module to be individually configurable.
 */

#ifndef BEAMMP_MODULE_H
#define BEAMMP_MODULE_H

const char* MODULE_LOAD_SYM = "mp_module_load"; // bool mp_module_load(ModuleInfo* info, lua_State* state);
const char* MODULE_UNLOAD_SYM = "mp_module_unload"; // void mp_module_unload();

struct ModuleVersion {
    uint major;
    uint minor;
    uint patch;
};

struct ModuleInfo {
    const char* name;
    const char* author;
    ModuleVersion version;
};

#ifdef __cplusplus

// Everything here is unnecessary for module developers.

#include <string_view>
#include <string>

#include <filesystem>
#include <tuple>

typedef void* dll_handle;
typedef void (*dll_sym)(void);

dll_handle dll_load(std::string_view path);
dll_handle dll_unload(dll_handle dll);
dll_sym dll_get_proc_address(dll_handle dll, std::string_view proc_name);

/**
 * Allows you to gather and store information about a module, and store it neatly inside an object.
 */
// class ModuleInfo {
// public:
//     /**
//      * Constructs the ModuleInfo class.
//      * 
//      * @param name The name of the module
//      * @param author The name of the module's author.
//      * @param version The module version.
//      * @param file_path The platform-agnostic file path to the module.
//      */
//     inline ModuleInfo(std::string_view name, std::string_view author, std::string_view version, std::filesystem::path file_path) {
//         this->name = name;
//         this->author = author;
//         this->version = version;
//         this->file_path = file_path;
//     };

//     ModuleInfo() = default;
//     ~ModuleInfo() = default;

//     inline std::string_view GetName() const {
//         return this->name;
//     }

//     inline std::string_view GetAuthor() const {
//         return this->author;
//     }

//     inline std::string_view GetVersion() const {
//         return this->version;
//     }

//     inline std::filesystem::path GetFilePath() const {
//         return this->file_path;
//     }

// private:
//     std::string_view name;
//     std::string_view author;
//     std::string_view version;
//     std::filesystem::path file_path;
// };

class Module {
public:
    inline Module(ModuleInfo info, std::filesystem::path path) {
        this->modInfo = info;
        this->modPath = path;
    };

    ~Module() = default;

    inline ModuleInfo GetModuleInfo() const {
        return modInfo;
    };

    std::tuple<bool, std::string_view> Load();
    void Unload();

private:
    ModuleInfo modInfo;
    std::filesystem::path modPath;

    dll_handle modHandle;
    dll_sym modLoadSym;
    dll_sym modUnloadSym;
};

#endif

#endif