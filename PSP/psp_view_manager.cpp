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
#include "psp_view_manager.h"

PspViewManager::PspViewManager()
        : mViewDisp(nullptr) {

}

PspViewManager *PspViewManager::GetInstance() {
    static PspViewManager pspViewManager;
    return &pspViewManager;
}

void PspViewManager::SetView(ViewDisp *pViewDisp) {
    mViewDisp = pViewDisp;
}

void PspViewManager::UpdateView() {
    mViewDisp->UpdateView();
}
