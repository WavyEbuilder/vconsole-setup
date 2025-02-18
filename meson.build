# This file is a part of vconsole-setup.
# Copyright (C) Rahul Sandhu <rahul@sandhuservices.dev>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <https://www.gnu.org/licenses/>.
#
# SPDX-License-Identifier: LGPL-2.1-or-later

project(
    'vconsole-setup',
    'cpp',
    version: '0.1',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

vconsole_setup_sources = [
    'src/main.cc',
    'src/env_parser.cc',
]

subdir('include')

executable(
    'vconsole-setup',
    vconsole_setup_sources,
    include_directories: incdir,
    install: true,
)
