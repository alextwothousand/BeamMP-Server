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

#ifndef BEAMMP_MODULE_MANAGER_H
#define BEAMMP_MODULE_MANAGER_H

#include <string_view>
#include <filesystem>

#include <vector>
#include <tuple>

#include <map>

class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager() = default;

    std::tuple<bool, std::string_view, size_t> ScanForModules();

    std::tuple<bool, std::string_view> LoadAll();
    std::tuple<bool, std::string_view> UnloadAll();

    std::tuple<bool, std::string_view> Load(std::string_view moduleName);
    std::tuple<bool, std::string_view> Unload(std::string_view moduleName);

    bool IsModuleLoaded(std::string_view moduleName);
    bool DoesModuleExist(std::string_view moduleName);
private:
    std::map<std::string_view, std::filesystem::path> modules;
    std::map<std::string_view, bool> moduleLoaded;
};

#endif