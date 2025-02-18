/*
 * This file is a part of vconsole-setup.
 * Copyright (C) Rahul Sandhu <rahul@sandhuservices.dev>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <iostream>
#include <functional>
#include <string_view>

namespace [[gnu::visibility("default")]] env_parser
{

using on_push = std::function<bool (unsigned, std::string_view, std::string_view)>;

bool
from_ifstream (std::istream &input, const on_push &push);

} // namespace env_parser
