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

#include "base_global.h"
#include "base_graphic.h"
#include <vector>

namespace PspNsBase {
    class BASE_SHARED GraphicManager {
    private:
        GraphicManager();

    public:
        static GraphicManager *GetInstance() {
            static GraphicManager graphicManager;
            return &graphicManager;
        }

        ~GraphicManager();

        void AddGraphic(Graphic *graphic);

        const std::vector<Graphic *> &GetGraphics() const;

        void DestroyAllGraphics();
        void Clear();

    private:
        std::vector<Graphic *> mGraphics;
    };

}
