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
#include "op/op_undo_redo_stack.h"

namespace PspNsOp {
    UndoRedoStack::UndoRedoStack(int aStackSize)
            : mStackSize(aStackSize) {
    }

    UndoRedoStack::~UndoRedoStack() {
        Destroy();
    }

    void UndoRedoStack::UnDo() {
        if (mUndoStack.empty()) return;

        UndoRedo *pItem = mUndoStack.back();
        pItem->Execute(true);

        mRedoStack.push_back(pItem);
        mUndoStack.pop_back();
    }

    void UndoRedoStack::ReDo() {
        if (mRedoStack.empty()) return;

        UndoRedo *pItem = mRedoStack.back();
        pItem->Execute(false);

        mUndoStack.push_back(pItem);
        mRedoStack.pop_back();
    }

    void UndoRedoStack::AddOperator(UndoRedo *aUndoRedo) {
        if (mUndoStack.size() > (size_t) mStackSize)
            mUndoStack.pop_front();

        mUndoStack.push_back(aUndoRedo);
        for (auto Itr = mRedoStack.begin(); Itr != mRedoStack.end(); ++Itr) {
            delete *Itr;
            (*Itr) = nullptr;
        }

        mRedoStack.clear();
    }

    void UndoRedoStack::Destroy() {
        for (auto Itr = mUndoStack.begin(); Itr != mUndoStack.end(); ++Itr) {
            delete *Itr;
            *Itr = nullptr;
        }
        mUndoStack.clear();

        for (auto Itr = mRedoStack.begin(); Itr != mRedoStack.end(); ++Itr) {
            delete *Itr;
            *Itr = nullptr;
        }
        mRedoStack.clear();
    }

}
