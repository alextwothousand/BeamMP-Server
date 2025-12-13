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

#include <cstdbool>

const char* MODULE_LOAD_SYM = "bmp_module_load"; // bool bmp_module_load(ModuleInfo* info, lua_State* state);
const char* MODULE_UNLOAD_SYM = "bmp_module_unload"; // void bmp_module_unload();

typedef bool (*bmp_module_load_t)(ModuleInfo*, void*);
typedef void (*bmp_module_unload_t)(void);

struct ModuleVersion {
    uint major;
    uint minor;
    uint patch;
};

struct ModuleInfo {
    const char* name;
    const char* author;
    const char* path;
    ModuleVersion version;
};

// Global defintions
struct Error {
    bool ok;
    const char* error;
};

// Global functions
typedef void (*bmp_print_t)(...);
typedef void (*bmp_printRaw_t)(...);
typedef void (*bmp_exit_t)(void);

// MP functions
// TODO: provide 1:1 api coverage
typedef ModuleVersion ServerVersion;

typedef const char* (*bmp_mp_getosname_t)(void);
typedef ServerVersion (*bmp_mp_getserverversion_t)(void);
typedef void (*bmp_mp_registerevent_t)(const char*, const char*);

typedef void (*bmp_mp_triggerglobalevent_t)(const char*, ...); // TODO: confirm signature
typedef void (*bmp_mp_triggerlocalevent_t)(const char*, ...); // TODO: confirm signature

typedef Error (*bmp_mp_triggerclientevent_t)(uint, const char*, void*); // TODO: confirm signature
typedef Error (*bmp_mp_triggerclienteventjson_t)(uint, const char*, void*); // TODO: confirm signature

typedef size_t (*bmp_mp_getplayercount_t)(void);
typedef bool (*bmp_mp_isplayerconnected_t)(uint);

typedef uint (*bmp_mp_getplayeridbyname_t)(const char*);
typedef const char* (*bmp_mp_getplayername_t)(uint);

typedef Error (*bmp_mp_removevehicle_t)(uint, uint);
typedef void* (*bmp_mp_getplayervehicles_t)(uint); // TODO: need to re-impl sol::table class

typedef void* (*bmp_mp_getpositionraw_t)(uint, uint); // TODO: confirm signature
typedef Error (*bmp_mp_sendchatmessage_t)(uint, uint);

// Util functions
typedef void* (*bmp_util_logdebug_t)(...);
typedef void* (*bmp_util_loginfo_t)(...);

typedef void* (*bmp_util_logwarn_t)(...);
typedef void* (*bmp_util_logerror_t)(...);

// Structs containing all functions
struct MpFunctions {
    bmp_mp_getosname_t get_os_name;
    bmp_mp_getserverversion_t get_server_version;
    bmp_mp_registerevent_t register_event;
    
    bmp_mp_triggerglobalevent_t trigger_global_event;
    bmp_mp_triggerlocalevent_t trigger_local_event;
    
    bmp_mp_triggerclientevent_t trigger_client_event;
    bmp_mp_triggerclienteventjson_t trigger_client_event_json;
    
    bmp_mp_getplayercount_t get_player_count;
    bmp_mp_isplayerconnected_t is_player_connected;
    
    bmp_mp_getplayeridbyname_t get_player_id_by_name;
    bmp_mp_getplayername_t get_player_name;
    
    bmp_mp_removevehicle_t remove_vehicle;
    bmp_mp_getplayervehicles_t get_player_vehicles;
    
    bmp_mp_getpositionraw_t get_position_raw;
    bmp_mp_sendchatmessage_t send_chat_message;
};

struct UtilFunctions {
    bmp_util_logdebug_t log_debug;
    bmp_util_loginfo_t log_info;

    bmp_util_logwarn_t log_warn;
    bmp_util_logerror_t log_error;
};

struct ModuleFunctions {
    MpFunctions mp;
    UtilFunctions util;
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