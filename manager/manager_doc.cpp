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
#include "manager/manager_doc.h"

namespace PspNsManager {
    PspDocument::PspDocument()
            : QObject(), mTitle("") {
    }

    PspDocument::~PspDocument() {
    }

    PspDocument::PspDocument(const PspDocument &doc)
            : QObject(), mTitle(doc.mTitle) {
    }

    PspDocument::PspDocument(const QString &title)
            : QObject(), mTitle(title) {
    }

    PspDocument &PspDocument::operator=(const PspDocument &doc) {
        mTitle = doc.mTitle;
        return *this;
    }

    void PspDocument::SetTitle(const QString &title) {
        mTitle = title;
    }

    const QString &PspDocument::GetTitle() const {
        return mTitle;
    }
}
