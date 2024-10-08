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

#include "op/op_global.h"
#include <map>
#include <QString>

using namespace std;

namespace PspNsOp {
    class OP_SHARED OperatorMenu {
    public:
        OperatorMenu() : mChoice(-1) {}

        bool isChoiceValid() const { return (mChoice >= 0 && mChoice < (int) mMenuItems.size()); }
        bool isValid() const { return !mMenuItems.empty(); }
        void Clear() {
            mChoice = -1;
            mMenuItems.clear();
        }

        void addMenuItem(const QString &item) { mMenuItems[mMenuItems.size()] = item; }
        void setChoice(int choice) { mChoice = choice; }
        int getChoice() { return mChoice; }

        const map<int, QString> &getMenuItems() const { return mMenuItems; }
        const QString &getItem(int index) { return mMenuItems[index]; }
    private:
        int mChoice;
        map<int, QString> mMenuItems;
    };
}
