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

#include "ModuleManager.h"
#include "Common.h"

#include <vector>

/**
 * Fetches the file extension for a shared library on certain platforms.
 * 
 * Windows: .dll
 * macOS: .dylib
 * Unix-like: .so
 */
std::string_view GetPlatformExtension() {
    #if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
        return ".dll";
    #elif defined(__APPLE__)
        return ".dylib";
    #elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
        return ".so";
    #else
        #error "Unknown platform"
    #endif
}

std::tuple<bool, std::string_view, size_t> ModuleManager::ScanForModules() {
    const auto 
        resourcePath = Application::Settings.getAsString(Settings::Key::General_ResourceFolder);

    // Check if module path exists, if not create it.
    // If it fails to create, return an error.
    if (!std::filesystem::exists(std::filesystem::path(resourcePath + "/Modules"))) {
        try {
            std::filesystem::create_directory(std::filesystem::path(resourcePath + "/Modules"));
        } catch (const std::exception& e) {
            return { false, e.what(), 0 };
        }
    }
    
    const auto path = std::filesystem::path(resourcePath + "/Modules");
    const auto extension = GetPlatformExtension();

    const auto scanDirectory = [&](std::filesystem::directory_entry dir) {
        // Simple helper function to avoid repetitive code
        if (dir.exists() && dir.path().extension() == extension) {
            // Valid module found for this platform.
            modules.insert({ std::string_view(dir.path().stem().string()), dir });
        }
    };

    // Begin scanning module path for modules
    for (const auto& entry : fs::directory_iterator(path)) {
        // Check for any nested directories - they can also contain modules.
        // They can only be nested once.
        if (entry.is_directory()) {
            for (const auto& file : fs::directory_iterator(entry)) 
                scanDirectory(file);
        }

        // Scan the root module folder
        scanDirectory(entry);
    }

    // Module scanning complete. If any were scanned, they will be stored as part of this class.

    return { true, std::string_view(), modules.size() };
}

std::tuple<bool, std::string_view> ModuleManager::Load(std::string_view moduleName) {
    return std::tuple<bool, std::string_view>();
}

std::tuple<bool, std::string_view> ModuleManager::Unload(std::string_view moduleName) {
    return std::tuple<bool, std::string_view>();
}

std::tuple<bool, std::string_view> ModuleManager::LoadAll() {
    return std::tuple<bool, std::string_view>();
}

std::tuple<bool, std::string_view> ModuleManager::UnloadAll() {
    return std::tuple<bool, std::string_view>();
}

bool ModuleManager::IsModuleLoaded(std::string_view moduleName) {
    return moduleLoaded.find(moduleName) != moduleLoaded.end();
}

bool ModuleManager::DoesModuleExist(std::string_view moduleName) {
    return modules.find(moduleName) != modules.end();
}