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

#include <cstring>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include "env_parser.hh"
#include "setup_console.hh"

static void
usage (std::string_view progname, bool help)
{
  const auto USAGE = std::format (R"(Usage: {} [/dev/ttyN] [--help]
Set up consoles according to /etc/vconsole.conf.

Options:
  /dev/ttyN    Specify a virtual console to operate on
  --help       Display this help message and exit

If no virtual console is specified, attempt to find and set up
an available console automatically.

Configuration is read from /etc/vconsole.conf and sets up:
- Console keymap
- Console font
- Font map and unimap
- UTF-8 mode

Exit status:
 0  if OK,
 1  if non-fatal problems (e.g., cannot set font),
>1  if fatal problems (e.g., cannot set keymap).
)", progname);

  help ? std::cout << USAGE
       : std::cerr << std::format ("Usage: {} [/dev/ttyN] [--help]\n",
                                   progname);
}

int
main (int argc, char **argv)
{
  std::optional<std::string> specified_vc;
  if (argc == 2)
    {
      if (std::strcmp (argv[1], "--help") == 0)
        {
          usage (argv[0], true);
          return EXIT_SUCCESS;
        }
      else if (std::strncmp (argv[1], "/dev/tty", 8) == 0)
        specified_vc = argv[1];
      else
        {
          usage (argv[0], false);
          return EXIT_FAILURE;
        }
    }
  else
    {
      usage (argv[0], false);
      return EXIT_FAILURE;
    }

  return setup_console (specified_vc) ? EXIT_SUCCESS : EXIT_FAILURE;
}
