/*
 * Copyright (C) 2024 Arthur Zhang <arthur.zhang.cx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "base/base_markerpoint.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

class JsonProcess {
private:
    JsonProcess();

public:
    ~JsonProcess();

    static JsonProcess *GetInstance();

    Json qstringToJson(const QString &str);

    static bool readJsonFile(const QString &filePath, Json &j);
    static bool writeJsonFile(const QString &filePath, const Json &j);

    static std::vector<PspNsBase::MarkerPoint> readMarkerFromFile(const QString &filePath);
};
