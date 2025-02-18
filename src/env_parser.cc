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

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "env_parser.hh"

constexpr std::string_view WHITESPACE = " \t\n\r";
constexpr std::string_view NEWLINE = "\n\r";
constexpr std::string_view COMMENTS = ";#";
constexpr std::string_view SHELL_NEED_ESCAPE = "$`\"\\";

bool
env_parser::from_ifstream (std::istream &input, const on_push &push)
{
  std::string contents ((std::istreambuf_iterator<char> (input)),
                        std::istreambuf_iterator<char> ());

  std::string key, value;
  unsigned line = 1;
  size_t last_value_whitespace = std::string::npos;
  size_t last_key_whitespace = std::string::npos;

  enum class State
  {
    PreKey,
    Key,
    PreValue,
    Value,
    ValueEscape,
    SingleQuoteValue,
    DoubleQuoteValue,
    DoubleQuoteValueEscape,
    Comment,
    CommentEscape
  } state
      = State::PreKey;

  auto push_kv = [&] () {
    if (last_key_whitespace != std::string::npos)
      key.erase (last_key_whitespace);
    if (state == State::Value && last_value_whitespace != std::string::npos)
      value.erase (last_value_whitespace);
    return push (line, key, value);
  };

  for (char c : contents)
    {
      switch (state)
        {
        case State::PreKey:
          if (COMMENTS.find (c) != std::string::npos)
            state = State::Comment;
          else if (WHITESPACE.find (c) == std::string::npos)
            {
              state = State::Key;
              last_key_whitespace = std::string::npos;
              key.push_back (c);
            }
          break;

        case State::Key:
          if (NEWLINE.find (c) != std::string::npos)
            {
              state = State::PreKey;
              line++;
              key.clear ();
            }
          else if (c == '=')
            {
              state = State::PreValue;
              last_value_whitespace = std::string::npos;
            }
          else
            {
              if (WHITESPACE.find (c) == std::string::npos)
                last_key_whitespace = std::string::npos;
              else if (last_key_whitespace == std::string::npos)
                last_key_whitespace = key.length ();
              key.push_back (c);
            }
          break;

        case State::PreValue:
          if (NEWLINE.find (c) != std::string::npos)
            {
              if (!push_kv ())
                return false;
              goto next_line;
            }
          else if (c == '\'')
            state = State::SingleQuoteValue;
          else if (c == '"')
            state = State::DoubleQuoteValue;
          else if (c == '\\')
            state = State::ValueEscape;
          else if (WHITESPACE.find (c) == std::string::npos)
            {
              state = State::Value;
              value.push_back (c);
            }
          break;

        case State::Value:
          if (NEWLINE.find (c) != std::string::npos)
            {
              if (!push_kv ())
                return false;
              goto next_line;
            }
          else if (c == '\\')
            {
              state = State::ValueEscape;
              last_value_whitespace = std::string::npos;
            }
          else
            {
              if (WHITESPACE.find (c) == std::string::npos)
                last_value_whitespace = std::string::npos;
              else if (last_value_whitespace == std::string::npos)
                last_value_whitespace = value.length ();
              value.push_back (c);
            }
          break;

        case State::ValueEscape:
          state = State::Value;
          if (NEWLINE.find (c) == std::string::npos)
            value.push_back (c);
          break;

        case State::SingleQuoteValue:
          if (c == '\'')
            state = State::PreValue;
          else
            value.push_back (c);
          break;

        case State::DoubleQuoteValue:
          if (c == '"')
            state = State::PreValue;
          else if (c == '\\')
            state = State::DoubleQuoteValueEscape;
          else
            value.push_back (c);
          break;

        case State::DoubleQuoteValueEscape:
          state = State::DoubleQuoteValue;
          if (SHELL_NEED_ESCAPE.find (c) != std::string::npos)
            value.push_back (c);
          else if (c != '\n')
            {
              value.push_back ('\\');
              value.push_back (c);
            }
          break;

        case State::Comment:
          if (c == '\\')
            state = State::CommentEscape;
          else if (NEWLINE.find (c) != std::string::npos)
            goto next_line;
          break;

        case State::CommentEscape:
          if (NEWLINE.find (c) != std::string::npos)
            goto next_line;
          else
            state = State::Comment;
          break;
        }
      continue;

    next_line:
      state = State::PreKey;
      line++;
      key.clear ();
      value.clear ();
    }

  if (state == State::PreValue
      || state == State::Value
      || state == State::ValueEscape
      || state == State::SingleQuoteValue
      || state == State::DoubleQuoteValue
      || state == State::DoubleQuoteValueEscape)
    {
      return push_kv ();
    }

  return true;
}
