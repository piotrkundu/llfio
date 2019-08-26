/* A view of a path to something
(C) 2017 - 2019 Niall Douglas <http://www.nedproductions.biz/> (20 commits)
File Created: Jul 2017


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License in the accompanying file
Licence.txt or at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Distributed under the Boost Software License, Version 1.0.
    (See accompanying file Licence.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef LLFIO_PATH_VIEW_H
#define LLFIO_PATH_VIEW_H

#include "config.hpp"

#include <iterator>
#include <locale>
#include <memory>  // for unique_ptr

//! \file path_view.hpp Provides view of a path

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)  // dll interface
#endif

LLFIO_V2_NAMESPACE_EXPORT_BEGIN

namespace detail
{
  template <class T> constexpr inline size_t constexpr_strlen(const T *s) noexcept
  {
    const T *e = s;
    for(; *e; e++)
    {
    }
    return e - s;
  }
#if !defined(__CHAR8_TYPE__) && __cplusplus < 20200000
  struct char8_t
  {
    char v;
    char8_t() = default;
    constexpr char8_t(char _v) noexcept
        : v(_v)
    {
    }
    constexpr bool operator!() const noexcept { return !v; }
    constexpr explicit operator bool() const noexcept { return !!v; }
  };
  constexpr inline bool operator<(char8_t a, char8_t b) noexcept { return a.v < b.v; }
  constexpr inline bool operator>(char8_t a, char8_t b) noexcept { return a.v > b.v; }
  constexpr inline bool operator<=(char8_t a, char8_t b) noexcept { return a.v <= b.v; }
  constexpr inline bool operator>=(char8_t a, char8_t b) noexcept { return a.v >= b.v; }
  constexpr inline bool operator==(char8_t a, char8_t b) noexcept { return a.v == b.v; }
  constexpr inline bool operator!=(char8_t a, char8_t b) noexcept { return a.v != b.v; }
#endif
#if !defined(__CHAR16_TYPE__) && !defined(_MSC_VER)  // VS2015 onwards has built in char16_t
  enum class char16_t : unsigned short
  {
  };
#endif
  template <class T> struct is_path_view_component_source_type : std::false_type
  {
  };
  template <> struct is_path_view_component_source_type<LLFIO_V2_NAMESPACE::byte> : std::true_type
  {
  };
  template <> struct is_path_view_component_source_type<char> : std::true_type
  {
  };
  template <> struct is_path_view_component_source_type<wchar_t> : std::true_type
  {
  };
  template <> struct is_path_view_component_source_type<char8_t> : std::true_type
  {
  };
  template <> struct is_path_view_component_source_type<char16_t> : std::true_type
  {
  };
  class path_view_iterator;
}  // namespace detail

class path_view;

/*! \class path_view_component
\brief An iterated part of a `path_view`.
*/
class LLFIO_DECL path_view_component
{
  friend class path_view;
  friend class detail::path_view_iterator;

public:
  //! The preferred separator type
  static constexpr auto preferred_separator = filesystem::path::preferred_separator;

  //! Character type for passthrough input
  using byte = LLFIO_V2_NAMESPACE::byte;
#if !defined(__CHAR8_TYPE__) && __cplusplus < 20200000
  using char8_t = detail::char8_t;
#endif
#if !defined(__CHAR16_TYPE__) && !defined(_MSC_VER)  // VS2015 onwards has built in char16_t
  using char16_t = detail::char16_t;
#endif

private:
  template <class Char> static constexpr bool _is_constructible = detail::is_path_view_component_source_type<std::decay_t<Char>>::value;
  static constexpr auto _npos = string_view::npos;
  union {
    const byte *_bytestr{nullptr};
    const char *_charstr;
    const wchar_t *_wcharstr;
    const char8_t *_char8str;
    const char16_t *_char16str;
  };
  size_t _length{0};
  unsigned _zero_terminated : 1;
  unsigned _passthrough : 1;
  unsigned _char : 1;
  unsigned _wchar : 1;
  unsigned _utf8 : 1;
  unsigned _utf16 : 1;

  constexpr path_view_component()
      : _zero_terminated(false)
      , _passthrough(false)
      , _char(false)
      , _wchar(false)
      , _utf8(false)
      , _utf16(false)
  {
  }  // NOLINT
  constexpr path_view_component(const byte *b, size_t l, bool zt)
      : _bytestr(b)
      , _length(l)
      , _zero_terminated(zt)
      , _passthrough(true)
      , _char(false)
      , _wchar(false)
      , _utf8(false)
      , _utf16(false)
  {
  }
  constexpr path_view_component(const char *b, size_t l, bool zt)
      : _charstr(b)
      , _length(l)
      , _zero_terminated(zt)
      , _passthrough(false)
      , _char(true)
      , _wchar(false)
      , _utf8(false)
      , _utf16(false)
  {
  }
  constexpr path_view_component(const wchar_t *b, size_t l, bool zt)
      : _wcharstr(b)
      , _length(l)
      , _zero_terminated(zt)
      , _passthrough(false)
      , _char(false)
      , _wchar(true)
      , _utf8(false)
      , _utf16(false)
  {
  }
  constexpr path_view_component(const char8_t *b, size_t l, bool zt)
      : _char8str(b)
      , _length(l)
      , _zero_terminated(zt)
      , _passthrough(false)
      , _char(false)
      , _wchar(false)
      , _utf8(true)
      , _utf16(false)
  {
  }
  constexpr path_view_component(const char16_t *b, size_t l, bool zt)
      : _char16str(b)
      , _length(l)
      , _zero_terminated(zt)
      , _passthrough(false)
      , _char(false)
      , _wchar(false)
      , _utf8(false)
      , _utf16(true)
  {
  }
  template <class U> constexpr auto _invoke(U &&f) const noexcept
  {
    return _utf8 ? f(basic_string_view<char8_t>(_char8str, _length))  //
                   :
                   (_utf16 ? f(basic_string_view<char16_t>(_char16str, _length))  //
                             :
                             (_wchar ? f(basic_string_view<wchar_t>(_wcharstr, _length))  //
                                       :
                                       f(basic_string_view<char>((const char *) _bytestr, _length))));
  }
  constexpr auto _find_first_sep(size_t startidx = 0) const noexcept
  {
#ifdef _WIN32
    return _utf8 ? basic_string_view<char8_t>(_char8str, _length).find_first_of((const char8_t *) "/\\", startidx)  //
                   :
                   (_utf16 ? basic_string_view<char16_t>(_char16str, _length).find_first_of((const char16_t *) L"/\\", startidx)  //
                             :
                             (_wchar ? basic_string_view<wchar_t>(_wcharstr, _length).find_first_of(L"/\\", startidx)  //
                                       :
                                       basic_string_view<char>((const char *) _bytestr, _length).find_first_of((const char *) "/\\", startidx)));
#else
    return _utf8 ? basic_string_view<char8_t>(_char8str, _length).find(preferred_separator, startidx)  //
                   :
                   (_utf16 ? basic_string_view<char16_t>(_char16str, _length).find(preferred_separator, startidx)  //
                             :
                             (_wchar ? basic_string_view<wchar_t>(_wcharstr, _length).find(preferred_separator, startidx)  //
                                       :
                                       basic_string_view<char>((const char *) _bytestr, _length).find(preferred_separator, startidx)));
#endif
  }
  constexpr auto _find_last_sep(size_t endidx = _npos) const noexcept
  {
#ifdef _WIN32
    return _utf8 ? basic_string_view<char8_t>(_char8str, _length).find_last_of((const char8_t *) "/\\", endidx)  //
                   :
                   (_utf16 ? basic_string_view<char16_t>(_char16str, _length).find_last_of((const char16_t *) L"/\\", endidx)  //
                             :
                             (_wchar ? basic_string_view<wchar_t>(_wcharstr, _length).find_last_of(L"/\\", endidx)  //
                                       :
                                       basic_string_view<char>((const char *) _bytestr, _length).find_last_of("/\\", endidx)));
#else
    return _utf8 ? basic_string_view<char8_t>(_char8str, _length).rfind(preferred_separator, endidx)  //
                   :
                   (_utf16 ? basic_string_view<char16_t>(_char16str, _length).rfind(preferred_separator, endidx)  //
                             :
                             (_wchar ? basic_string_view<wchar_t>(_wcharstr, _length).rfind(preferred_separator, endidx)  //
                                       :
                                       basic_string_view<char>((const char *) _bytestr, _length).rfind(preferred_separator, endidx)));
#endif
  }

public:
  path_view_component(const path_view_component &) = default;
  path_view_component(path_view_component &&) = default;
  path_view_component &operator=(const path_view_component &) = default;
  path_view_component &operator=(path_view_component &&) = default;
  ~path_view_component() = default;

  //! True if empty
  constexpr bool empty() const noexcept { return _length == 0; }

  //! Returns the size of the view in characters.
  constexpr size_t native_size() const noexcept
  {
    return _invoke([](const auto &v) { return v.size(); });
  }

  //! Swap the view with another
  constexpr void swap(path_view_component &o) noexcept
  {
    path_view_component x = *this;
    *this = o;
    o = x;
  }

  // True if the view contains any of the characters `*`, `?`, (POSIX only: `[` or `]`).
  constexpr bool contains_glob() const noexcept
  {
    return _invoke([](const auto &v) {
      using value_type = typename std::remove_reference<decltype(*v.data())>::type;
#ifdef _WIN32
      const value_type *tofind = sizeof(value_type) > 1 ? (const value_type *) L"*?" : (const value_type *) "*?";
#else
      const value_type *tofind = sizeof(value_type) > 1 ? (const value_type *) L"*?[]" : (const value_type *) "*?[]";
#endif
      return string_view::npos != v.find_first_of(tofind);
    });
  }

  //! Returns a view of the filename without any file extension
  constexpr path_view_component stem() const noexcept
  {
    auto sep_idx = _find_last_sep();
    return _invoke([sep_idx, this](const auto &v) {
      auto dot_idx = v.rfind('.');
      if(_npos == dot_idx || (_npos != sep_idx && dot_idx < sep_idx) || dot_idx == sep_idx + 1 || (dot_idx == sep_idx + 2 && v[dot_idx - 1] == '.'))
      {
        return path_view_component(v.data() + sep_idx + 1, v.size() - sep_idx - 1, false);
      }
      return path_view_component(v.data() + sep_idx + 1, dot_idx - sep_idx - 1, _zero_terminated);
    });
  }
  //! Returns a view of the file extension part of this view
  constexpr path_view_component extension() const noexcept
  {
    auto sep_idx = _find_last_sep();
    return _invoke([sep_idx, this](const auto &v) {
      auto dot_idx = v.rfind('.');
      if(_npos == dot_idx || (_npos != sep_idx && dot_idx < sep_idx) || dot_idx == sep_idx + 1 || (dot_idx == sep_idx + 2 && v[dot_idx - 1] == '.'))
      {
        return path_view_component();
      }
      return path_view_component(v.data() + dot_idx, v.size() - dot_idx, _zero_terminated);
    });
  }

private:
  template <class CharT> static filesystem::path _path_from_char_array(CharT *d, size_t l) { return {d, l}; }
  static filesystem::path _path_from_char_array(const char8_t *d, size_t l) { return filesystem::u8path((const char *) d, (const char *) d + l); }

  template <class InternT, class ExternT> struct _codecvt : std::codecvt<InternT, ExternT, std::mbstate_t>
  {
    template <class... Args>
    _codecvt(Args &&... args)
        : std::codecvt<InternT, ExternT, std::mbstate_t>(std::forward<Args>(args)...)
    {
    }
    ~_codecvt() {}
  };
  template <class CharT> static _codecvt<CharT, char> &_to_utf8(basic_string_view<CharT> /*unused*/) noexcept
  {
    static _codecvt<CharT, char> ret;
    return ret;
  }
  static _codecvt<char, char> &_to_utf8(basic_string_view<char8_t> /*unused*/) noexcept
  {
    static _codecvt<char, char> ret;
    return ret;
  }
  template <class CharT> static int _compare(basic_string_view<CharT> a, basic_string_view<CharT> b) noexcept { return a.compare(b); }
#ifdef _WIN32
  // On Windows only, char is the native narrow encoding, which is locale dependent
  LLFIO_HEADERS_ONLY_MEMFUNC_SPEC std::unique_ptr<char8_t[]> _ansi_path_to_utf8(basic_string_view<char8_t> &out) noexcept;
  static int _compare(basic_string_view<char> a, basic_string_view<char> b) noexcept { return a.compare(b); }
  template <class CharT> static int _compare(basic_string_view<CharT> a, basic_string_view<char> b) noexcept { return -_compare(b, a); }
  template <class CharT> static int _compare(basic_string_view<char> a, basic_string_view<CharT> b) noexcept
  {
    // Convert a from native narrow encoding to utf8
    basic_string_view<char8_t> a_utf8;
    auto h = _ansi_path_to_utf8(a_utf8);
    if(!h)
    {
      // Failure to allocate memory, or convert
      assert(h);
      return -99;
    }
    return _compare(a_utf8, b);
  }
#endif
  template <class Char1T, class Char2T> static int _compare(basic_string_view<Char1T> a, basic_string_view<Char2T> b) noexcept
  {
    static constexpr size_t codepoints_at_a_time = 8 * 4;
    // Convert both to utf8, then to utf32, and compare
    auto &convert_a = _to_utf8(a);
    auto &convert_b = _to_utf8(b);
    std::mbstate_t a_state{}, b_state{};
    auto *a_ptr = a.data();
    auto *b_ptr = b.data();
    while(a_ptr <= &a.back() && b_ptr <= &b.back())
    {
      // Try to convert 5 to 32 chars at a time
      char a_out[codepoints_at_a_time + 1], b_out[codepoints_at_a_time + 1], *a_out_end = a_out, *b_out_end = b_out;
      auto a_result = convert_a.out(a_state, a_ptr, &a.back() + 1, a_ptr, a_out, a_out + codepoints_at_a_time, a_out_end);
      auto b_result = convert_b.out(b_state, b_ptr, &b.back() + 1, b_ptr, b_out, b_out + codepoints_at_a_time, b_out_end);
      if(std::codecvt_base::partial == a_result && a_out_end == a_out + codepoints_at_a_time)
      {
        // Needs one more character from input
        a_result = convert_a.out(a_state, a_ptr, a_ptr + 1, a_ptr, a_out + codepoints_at_a_time, a_out + codepoints_at_a_time + 1, a_out_end);
        assert(std::codecvt_base::partial != a_result);
      }
      if(std::codecvt_base::error == a_result)
      {
        assert(false);
        return -99;
      }
      if(std::codecvt_base::partial == b_result && b_out_end == b_out + codepoints_at_a_time)
      {
        // Needs one more character from input
        b_result = convert_b.out(b_state, b_ptr, b_ptr + 1, b_ptr, b_out + codepoints_at_a_time, b_out + codepoints_at_a_time + 1, b_out_end);
        assert(std::codecvt_base::partial != b_result);
      }
      if(std::codecvt_base::error == b_result)
      {
        assert(false);
        return 99;
      }
      if((a_out_end - a_out) < (b_out_end - b_out))
      {
        return -2;
      }
      if((a_out_end - a_out) > (b_out_end - b_out))
      {
        return 2;
      }
#if !defined(__CHAR8_TYPE__) && __cplusplus < 20200000
      // Before C++ 20, no facility to char_traits::compare utf8, so convert to utf32
      const char *a_out_end_ = a_out_end, *b_out_end_ = b_out_end;
      char32_t a32[codepoints_at_a_time + 1], b32[codepoints_at_a_time + 1], *a32_end = a32, *b32_end = b32;
      std::mbstate_t a32_state{}, b32_state{};
      auto &convert32 = _to_utf8(basic_string_view<char32_t>());
      convert32.in(a32_state, a_out, a_out_end, a_out_end_, a32, a32 + codepoints_at_a_time + 1, a32_end);
      convert32.in(b32_state, b_out, b_out_end, b_out_end_, b32, b32 + codepoints_at_a_time + 1, b32_end);
      if((a32_end - a32) < (b32_end - b32))
      {
        return -2;
      }
      if((a32_end - a32) > (b32_end - b32))
      {
        return 2;
      }
      int ret = std::char_traits<char32_t>::compare(a32, b32, a32_end - a32);
#else
      int ret = std::char_traits<char8_t>::compare(a_out, b_out, a_out_end - a_out);
#endif
      if(ret != 0)
      {
        return ret;
      }
    }
    if(a_ptr >= &a.back())
    {
      return -2;
    }
    if(b_ptr >= &b.back())
    {
      return 2;
    }
    return 0;  // equal
  }

public:
  //! Return the path view as a path. Allocates and copies memory!
  filesystem::path path() const
  {
    return _invoke([](const auto &v) { return _path_from_char_array(v.data(), v.size()); });
  }

  /*! Compares the two path views for equivalence or ordering.
  Be aware that comparing path views of differing source encodings will be expensive
  as a conversion to utf8 is performed. Be further aware that on
  Windows, `char` source must undergo a narrow native encoding to utf8 conversion via
  the Windows conversion APIs, which is extremely expensive, if not comparing `char`-`char`
  views.
  */
  constexpr int compare(const path_view_component &p) const noexcept
  {
    return _invoke([&p](const auto &self) { return p._invoke([&self](const auto &other) { return _compare(self, other); }); });
  }
  //! \overload
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr int compare(const Char *s) const noexcept { return compare(path_view_component(s)); }
  //! \overload
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr int compare(const basic_string_view<Char> s) const noexcept { return compare(path_view_component(s)); }
};


/*! \class path_view
\brief A borrowed view of a path. A lightweight trivial-type alternative to
`std::filesystem::path`.

LLFIO is sufficiently fast that `std::filesystem::path` as a wrapper of an
underlying `std::basic_string<>` can be problematically expensive for some
filing system operations due to the potential memory allocation. LLFIO
therefore works exclusively with borrowed views of other path storage.

Some of the API for `std::filesystem::path` is replicated here, however any
APIs which modify the path other than taking subsets are obviously not
possible with borrowed views.

\todo Lots of member functions remain to be implemented. `char8_t` and `char16_t`
support is not implemented yet.

Each consumer of `path_view` defines what the "native platform transport" and
"native platform encoding" is. For LLFIO, the native platform transport is
defined to be `std::filesystem::path::value_type`, which is as follows:

- POSIX: The native platform transport is `char`.
- Microsoft Windows: The native platform transport is `wchar_t`.

**If** the input to `path_view` equals the native platform transport, the bits
supplied will be passed through to the operating system without translation (see below).
*If* the consuming API expects null termination, and the input to `path_view` is null
terminated, then you are *guaranteed* that the originally supplied buffer is passed
through. If the input is not null terminated, a bitwise identical copy is made into
temporary storage (which will be the stack for smaller strings), which is then null
terminated before passing to the consuming API.

If the input to `path_view` does NOT equal the native platform transport, then
a translation of the input bits will be performed into temporary storage just before
calling the consuming API. The rules are as follows:

- POSIX: The native platform encoding is assumed to be UTF-8. If the input is `char8_t`
or `char`, it is not translated. If the input is `char16_t`, a UTF-16 to UTF-8 translation
is performed.

- Microsoft Windows: The native platform encoding is assumed to be UTF-16. If the input
is `char16_t` or `wchar_t`, it is not translated. If the input is `char8_t`, a UTF-8 to UTF-16
translation is performed. If the input is `char`, the Microsoft Windows API for ANSI to
UTF-16 translation is invoked in order to match how Windows ANSI APIs are mapped onto the
Windows Unicode APIs (be aware this is very slow).

# Windows specific notes:

On Microsoft Windows, filesystem paths may require to be zero terminated,
or they may not. Which is the case depends on whether LLFIO calls the NT kernel
API directly rather than the Win32 API. As a general rule as to when which
is used, the NT kernel API is called instead of the Win32 API when:

- For any paths relative to a `path_handle` (the Win32 API does not provide a
race free file system API).
- For any paths beginning with `\!!\`, we pass the path + 3 characters
directly through. This prefix is a pure LLFIO extension, and will not be
recognised by other code.
- For any paths beginning with `\??\`, we pass the path + 0 characters
directly through. Note the NT kernel keeps a symlink at `\??\` which refers to
the DosDevices namespace for the current login, so as an incorrect relation
which you should **not** rely on, the Win32 path `C:\foo` probably will appear
at `\??\C:\foo`.

These prefixes are still passed to the Win32 API:

- `\\?\` which is used to tell a Win32 API that the remaining path is longer
than a DOS path.
- `\\.\` which since Windows 7 is treated exactly like `\\?\`.

If the NT kernel API is used directly then:

- Paths are matched case sensitively as raw bytes via `memcmp()`, not case
insensitively (requires slow locale conversion).
- The path limit is 32,767 characters.

If you really care about performance, you are very strongly recommended to use
the NT kernel API wherever possible. Where paths are involved, it is often
three to five times faster due to the multiple memory allocations and string
translations that the Win32 functions perform before calling the NT kernel
routine.

If however you are taking input from some external piece of code, then for
maximum compatibility you should still use the Win32 API.
*/
class LLFIO_DECL path_view
{
public:
  friend class detail::path_view_iterator;
  //! Const iterator type
  using const_iterator = detail::path_view_iterator;
  //! iterator type
  using iterator = const_iterator;
  //! Reverse iterator
  using reverse_iterator = std::reverse_iterator<iterator>;
  //! Const reverse iterator
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  //! Size type
  using size_type = std::size_t;
  //! Difference type
  using difference_type = std::ptrdiff_t;

  //! The preferred separator type
  static constexpr auto preferred_separator = filesystem::path::preferred_separator;

  //! Character type for passthrough input
  using byte = LLFIO_V2_NAMESPACE::byte;
#if !defined(__CHAR8_TYPE__) && __cplusplus < 20200000
  using char8_t = detail::char8_t;
#endif
#if !defined(__CHAR16_TYPE__) && !defined(_MSC_VER)  // VS2015 onwards has built in char16_t
  enum class char16_t : unsigned short
  {
  };
#endif

private:
  static constexpr auto _npos = string_view::npos;

  path_view_component _state;

public:
  //! Constructs an empty path view
  constexpr path_view() {}  // NOLINT
  ~path_view() = default;

  //! Implicitly constructs a path view from a path. The input path MUST continue to exist for this view to be valid.
  path_view(const filesystem::path &v) noexcept  // NOLINT
      : _state(v.native().c_str(), v.native().size(), true)
  {
  }
  //! Implicitly constructs a path view from a path view component. The input path MUST continue to exist for this view to be valid.
  path_view(path_view_component v) noexcept  // NOLINT
      : _state(v)
  {
  }

  //! Implicitly constructs a path view from a zero terminated `const char *`. Convenience wrapper for the `byte` constructor. The input string MUST continue to exist for this view to be valid.
  constexpr path_view(const char *v) noexcept  // NOLINT
      : _state(v, detail::constexpr_strlen(v), true)
  {
  }
  //! Implicitly constructs a path view from a zero terminated `const wchar_t *`. Convenience wrapper for the `byte` constructor. The input string MUST continue to exist for this view to be valid.
  constexpr path_view(const wchar_t *v) noexcept  // NOLINT
      : _state(v, detail::constexpr_strlen(v), true)
  {
  }
  //! Implicitly constructs a path view from a zero terminated `const char8_t *`. Performs a UTF-8 to native encoding if necessary. The input string MUST continue to exist for this view to be valid.
  constexpr path_view(const char8_t *v) noexcept  // NOLINT
      : _state(v, detail::constexpr_strlen(v), true)
  {
  }
  //! Implicitly constructs a path view from a zero terminated `const char16_t *`. Performs a UTF-16 to native encoding if necessary. The input string MUST continue to exist for this view to be valid.
  constexpr path_view(const char16_t *v) noexcept  // NOLINT
      : _state(v, detail::constexpr_strlen(v), true)
  {
  }

  /*! Constructs a path view from a lengthed array of one of
  `byte`, `char`, `wchar_t`, `char8_t` or `char16_t`. The input
  string MUST continue to exist for this view to be valid.
  */
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr path_view(const Char *v, size_t len, bool is_zero_terminated) noexcept
      : _state(v, len, is_zero_terminated)
  {
  }
  /*! Constructs from a basic string if the character type is one of
  `char`, `wchar_t`, `char8_t` or `char16_t`.
  */
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr path_view(const std::basic_string<Char> &v) noexcept  // NOLINT
      : path_view(v.data(), v.size(), true)
  {
  }
  /*! Constructs from a basic string view if the character type is one of
  `char`, `wchar_t`, `char8_t` or `char16_t`.
  */
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr path_view(basic_string_view<Char> v, bool is_zero_terminated) noexcept  // NOLINT
      : path_view(v.data(), v.size(), is_zero_terminated)
  {
  }

  //! Default copy constructor
  path_view(const path_view &) = default;
  //! Default move constructor
  path_view(path_view &&o) noexcept = default;
  //! Default copy assignment
  path_view &operator=(const path_view &p) = default;
  //! Default move assignment
  path_view &operator=(path_view &&p) noexcept = default;

  //! Swap the view with another
  constexpr void swap(path_view &o) noexcept { _state.swap(o._state); }

  //! True if empty
  constexpr bool empty() const noexcept { return _state.empty(); }
  constexpr bool has_root_path() const noexcept { return !root_path().empty(); }
  constexpr bool has_root_name() const noexcept { return !root_name().empty(); }
  constexpr bool has_root_directory() const noexcept { return !root_directory().empty(); }
  constexpr bool has_relative_path() const noexcept { return !relative_path().empty(); }
  constexpr bool has_parent_path() const noexcept { return !parent_path().empty(); }
  constexpr bool has_filename() const noexcept { return !filename().empty(); }
  constexpr bool has_stem() const noexcept { return !stem().empty(); }
  constexpr bool has_extension() const noexcept { return !extension().empty(); }
  constexpr bool is_absolute() const noexcept
  {
    auto sep_idx = _state._find_first_sep();
    if(_npos == sep_idx)
    {
      return false;
    }
#ifdef _WIN32
    if(is_ntpath())
      return true;
    return _state._invoke([sep_idx](const auto &v) {
      if(sep_idx == 0)
      {
        if(v[sep_idx + 1] == preferred_separator)  // double separator at front
          return true;
      }
      auto colon_idx = v.find(':');
      return colon_idx < sep_idx;  // colon before first separator
    });
#else
    return sep_idx == 0;
#endif
  }
  constexpr bool is_relative() const noexcept { return !is_absolute(); }
  // True if the path view contains any of the characters `*`, `?`, (POSIX only: `[` or `]`).
  constexpr bool contains_glob() const noexcept { return _state.contains_glob(); }
#ifdef _WIN32
  // True if the path view is a NT kernel path starting with `\!!\` or `\??\`
  constexpr bool is_ntpath() const noexcept
  {
    return _state._invoke([](const auto &v) {
      if(v.size() < 4)
      {
        return false;
      }
      const auto *d = v.data();
      if(d[0] == '\\' && d[1] == '!' && d[2] == '!' && d[3] == '\\')
      {
        return true;
      }
      if(d[0] == '\\' && d[1] == '?' && d[2] == '?' && d[3] == '\\')
      {
        return true;
      }
      return false;
    });
  }
  // True if the path view matches the format of an LLFIO deleted file
  constexpr bool is_llfio_deleted() const noexcept
  {
    return filename()._invoke([](const auto &v) {
      if(v.size() == 64 + 8)
      {
        // Could be one of our "deleted" files, is he all hex + ".deleted"?
        for(size_t n = 0; n < 64; n++)
        {
          auto c = v[n];
          if(!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
          {
            return false;
          }
        }
        return v[64] == '.' && v[65] == 'd' && v[66] == 'e' && v[67] == 'l' && v[68] == 'e' && v[69] == 't' && v[70] == 'e' && v[71] == 'd';
      }
      return false;
    });
  }
#endif

  //! Returns an iterator to the first path component
  constexpr inline const_iterator cbegin() const noexcept;
  //! Returns an iterator to the first path component
  constexpr inline const_iterator begin() const noexcept;
  //! Returns an iterator to the first path component
  constexpr inline iterator begin() noexcept;
  //! Returns an iterator to after the last path component
  constexpr inline const_iterator cend() const noexcept;
  //! Returns an iterator to after the last path component
  constexpr inline const_iterator end() const noexcept;
  //! Returns an iterator to after the last path component
  constexpr inline iterator end() noexcept;

  //! Returns a copy of this view with the end adjusted to match the final separator.
  constexpr path_view remove_filename() const noexcept
  {
    auto sep_idx = _state._find_last_sep();
    if(_npos == sep_idx)
    {
      return *this;
    }
    return _state._invoke([sep_idx](auto v) { return path_view(v.data(), sep_idx, false); });
  }
  //! Returns the size of the view in characters.
  constexpr size_t native_size() const noexcept { return _state.native_size(); }
  //! Returns a view of the root name part of this view e.g. C:
  constexpr path_view root_name() const noexcept
  {
    auto sep_idx = _state._find_first_sep();
    if(_npos == sep_idx)
    {
      return path_view();
    }
    return _state._invoke([sep_idx](const auto &v) { return path_view(v.data(), sep_idx, false); });
  }
  //! Returns a view of the root directory, if there is one e.g. /
  constexpr path_view root_directory() const noexcept
  {
    auto sep_idx = _state._find_first_sep();
    if(_npos == sep_idx)
    {
      return path_view();
    }
    return _state._invoke([sep_idx](const auto &v) {
#ifdef _WIN32
      auto colon_idx = v.find(':');
      if(colon_idx < sep_idx)
      {
        return path_view(v.data() + sep_idx, 1, false);
      }
#endif
      if(sep_idx == 0)
      {
        return path_view(v.data(), 1, false);
      }
      return path_view();
    });
  }
  //! Returns, if any, a view of the root path part of this view e.g. C:/
  constexpr path_view root_path() const noexcept
  {
    auto sep_idx = _state._find_first_sep();
    if(_npos == sep_idx)
    {
      return path_view();
    }
#ifdef _WIN32
    return _state._invoke([this, sep_idx](const auto &v) {
      if(is_ntpath())
      {
        return path_view(v.data() + 3, 1, false);
      }
      // Special case \\.\ and \\?\ to match filesystem::path
      if(v.size() >= 4 && sep_idx == 0 && v[1] == '\\' && (v[2] == '.' || v[2] == '?') && v[3] == '\\')
      {
        return path_view(v.data() + 0, 4, false);
      }
      auto colon_idx = v.find(':');
      if(colon_idx < sep_idx)
      {
        return path_view(v.data(), sep_idx + 1, false);
      }
#else
    return _state._invoke([sep_idx](const auto &v) {
#endif
      if(sep_idx == 0)
      {
        return path_view(v.data(), 1, false);
      }
      return path_view();
    });
  }
  //! Returns a view of everything after the root path
  constexpr path_view relative_path() const noexcept
  {
    auto sep_idx = _state._find_first_sep();
    if(_npos == sep_idx)
    {
      return *this;
    }
#ifdef _WIN32
    return _state._invoke([this, sep_idx](const auto &v) {
      // Special case \\.\ and \\?\ to match filesystem::path
      if(v.size() >= 4 && sep_idx == 0 && v[1] == '\\' && (v[2] == '.' || v[2] == '?') && v[3] == '\\')
      {
        return path_view(v.data() + 4, v.size() - 4, _state._zero_terminated);
      }
      auto colon_idx = v.find(':');
      if(colon_idx < sep_idx)
      {
        return path_view(v.data() + sep_idx + 1, v.size() - sep_idx - 1, _state._zero_terminated);
      }
#else
    return _state._invoke([this, sep_idx](const auto &v) {
#endif
      if(sep_idx == 0)
      {
        return path_view(v.data() + 1, v.size() - 1, _state._zero_terminated);
      }
      return *this;
    });
  }
  //! Returns a view of the everything apart from the filename part of this view
  constexpr path_view parent_path() const noexcept
  {
    auto sep_idx = _state._find_last_sep();
    if(_npos == sep_idx)
    {
      return path_view();
    }
    return _state._invoke([sep_idx](const auto &v) { return path_view(v.data(), sep_idx, false); });
  }
  //! Returns a view of the filename part of this view.
  constexpr path_view_component filename() const noexcept
  {
    auto sep_idx = _state._find_last_sep();
    if(_npos == sep_idx)
    {
      return _state;
    }
    return _state._invoke([sep_idx, this](const auto &v) { return path_view_component(v.data() + sep_idx + 1, v.size() - sep_idx - 1, _state._zero_terminated); });
  }
  //! Returns a view of the filename without any file extension
  constexpr path_view_component stem() const noexcept { return _state.stem(); }
  //! Returns a view of the file extension part of this view
  constexpr path_view_component extension() const noexcept { return _state.extension(); }

  //! Return the path view as a path. Allocates and copies memory!
  filesystem::path path() const { return _state.path(); }

  /*! Compares the two path views for equivalence or ordering.
  Be aware that comparing path views of differing source encodings will be expensive
  as a conversion to utf8 is performed for each path component. Be further aware that on
  Windows, `char` source must undergo a narrow native encoding to utf8 conversion via
  the Windows conversion APIs, which is extremely expensive, if not comparing `char`-`char`
  views.
  */
  constexpr inline int compare(const path_view &o) const noexcept;
  //! \overload
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr int compare(const Char *s) const noexcept { return _state.compare(s); }
  //! \overload
  LLFIO_TEMPLATE(class Char)
  LLFIO_TREQUIRES(LLFIO_TPRED(path_view_component::_is_constructible<Char>))
  constexpr int compare(const basic_string_view<Char> s) const noexcept { return _state.compare(s); }

  //! Instantiate from a `path_view` to get a zero terminated path suitable for feeding to the kernel
  struct LLFIO_DECL c_str
  {
    //! Number of characters, excluding zero terminating char, at buffer
    uint16_t length{0};
    const filesystem::path::value_type *buffer{nullptr};

#ifdef _WIN32
    c_str(const path_view &view, bool ntkernelapi) noexcept
    {
      if(!view._state._utf16.empty())
      {
        if(view._state._utf16.size() > 32768)
        {
          LLFIO_LOG_FATAL(&view, "Attempt to send a path exceeding 64Kb to kernel");
          abort();
        }
        length = static_cast<uint16_t>(view._state._utf16.size());
        // Is this going straight to a NT kernel API? If so, use directly
        if(ntkernelapi)
        {
          buffer = view._state._utf16.data();
          return;
        }
        // Is the byte just after the view a zero? If so, use directly
        if(0 == view._state._utf16.data()[length])
        {
          buffer = view._state._utf16.data();
          return;
        }
        // Otherwise use _buffer and zero terminate.
        if(length > sizeof(_buffer) - 1)
        {
          LLFIO_LOG_FATAL(&view, "Attempt to send a path exceeding 64Kb to kernel");
          abort();
        }
        memcpy(_buffer, view._state._utf16.data(), length);
        _buffer[length] = 0;
        buffer = _buffer;
        return;
      }
      if(!view._state._utf8.empty())
      {
        _from_utf8(view);
        return;
      }
#else
    c_str(const path_view &view) noexcept  // NOLINT
    {
      if(!view._state._utf8.empty())
      {
        if(view._state._utf8.size() > 32768)
        {
          LLFIO_LOG_FATAL(&view, "Attempt to send a path exceeding 64Kb to kernel");
          abort();
        }
        length = static_cast<uint16_t>(view._state._utf8.size());
        // Is the byte just after the view a zero? If so, use directly
        if(0 == view._state._utf8.data()[length])
        {
          buffer = view._state._utf8.data();
          return;
        }
        // Otherwise use _buffer and zero terminate.
        if(length > sizeof(_buffer) - 1)
        {
          LLFIO_LOG_FATAL(&view, "Attempt to send a path exceeding 32Kb to kernel");
          abort();
        }
        memcpy(_buffer, view._state._utf8.data(), length);
        _buffer[length] = 0;
        buffer = _buffer;
        return;
      }
#endif
      length = 0;
      _buffer[0] = 0;
      buffer = _buffer;
    }
    ~c_str() = default;
    c_str(const c_str &) = delete;
    c_str(c_str &&) = delete;
    c_str &operator=(const c_str &) = delete;
    c_str &operator=(c_str &&) = delete;

  private:
    filesystem::path::value_type _buffer[32768]{};
#ifdef _WIN32
    LLFIO_HEADERS_ONLY_MEMFUNC_SPEC void _from_utf8(const path_view &view) noexcept;
#endif
  };
  friend struct c_str;
};
inline constexpr bool operator==(path_view x, path_view y) noexcept
{
  if(x.native_size() != y.native_size())
  {
    return false;
  }
  return x.compare(y) == 0;
}
inline constexpr bool operator!=(path_view x, path_view y) noexcept
{
  if(x.native_size() != y.native_size())
  {
    return true;
  }
  return x.compare(y) != 0;
}
inline constexpr bool operator<(path_view x, path_view y) noexcept
{
  return x.compare(y) < 0;
}
inline constexpr bool operator>(path_view x, path_view y) noexcept
{
  return x.compare(y) > 0;
}
inline constexpr bool operator<=(path_view x, path_view y) noexcept
{
  return x.compare(y) <= 0;
}
inline constexpr bool operator>=(path_view x, path_view y) noexcept
{
  return x.compare(y) >= 0;
}
inline std::ostream &operator<<(std::ostream &s, const path_view &v)
{
  return s << v.path();
}

namespace detail
{
  template <class T> class fake_pointer
  {
    T _v;

  public:
    constexpr fake_pointer(T o)
        : _v(o)
    {
    }
    constexpr const T &operator*() const noexcept { return _v; }
    constexpr T &operator*() noexcept { return _v; }
    constexpr const T *operator->() const noexcept { return &_v; }
    constexpr T *operator->() noexcept { return &_v; }
  };
  class path_view_iterator
  {
    friend class path_view;

  public:
    //! Value type
    using value_type = path_view_component;
    //! Reference type
    using reference = value_type;
    //! Const reference type
    using const_reference = const value_type;
    //! Pointer type
    using pointer = fake_pointer<value_type>;
    //! Const pointer type
    using const_pointer = fake_pointer<const value_type>;
    //! Size type
    using size_type = size_t;

  private:
    const path_view *_parent{nullptr};
    size_type _begin{0}, _end{0};

    static constexpr auto _npos = string_view::npos;
    constexpr bool _is_end() const noexcept { return (nullptr == _parent) || _parent->native_size() == _begin; }
    constexpr value_type _get() const noexcept
    {
      assert(_parent != nullptr);
      return _parent->_state._invoke([this](const auto &v) {
        assert(_begin + _end <= v.size());
        return path_view_component(v.data() + _begin, _end, (_begin + _end == v.size()) ? _parent->_state._zero_terminated : false);
      });
    }
    constexpr void _inc() noexcept
    {
      _begin = _end;
      _end = _parent->_state._find_first_sep(_begin + 1);
      if(_npos == _end)
      {
        _parent->_state._invoke([this](const auto &v) { _end = v.size(); });
      }
    }
    constexpr void _dec() noexcept
    {
      _end = _begin;
      _begin = _parent->_state._find_last_sep(_end - 1);
      if(_npos == _begin)
      {
        _begin = 0;
      }
    }

    constexpr path_view_iterator(const path_view *p, bool end)
        : _parent(p)
        , _begin(end ? p->native_size() : 0)
        , _end(end ? p->native_size() : 0)
    {
      if(!end)
      {
        _inc();
      }
    }

  public:
    path_view_iterator() = default;
    path_view_iterator(const path_view_iterator &) = default;
    path_view_iterator(path_view_iterator &&) = default;
    path_view_iterator &operator=(const path_view_iterator &) = default;
    path_view_iterator &operator=(path_view_iterator &&) = default;
    ~path_view_iterator() = default;

    constexpr const_reference operator*() const noexcept { return _get(); }
    constexpr reference operator*() noexcept { return _get(); }
    constexpr const_pointer operator->() const noexcept { return _get(); }
    constexpr pointer operator->() noexcept { return _get(); }

    constexpr bool operator!=(path_view_iterator o) const noexcept
    {
      if(_is_end() && o._is_end())
      {
        return false;
      }
      return _parent != o._parent || _begin != o._begin || _end != o._end;
    }
    constexpr bool operator==(path_view_iterator o) const noexcept
    {
      if(_is_end() && o._is_end())
      {
        return true;
      }
      return _parent == o._parent && _begin == o._begin && _end == o._end;
    }

    constexpr path_view_iterator &operator--() noexcept
    {
      _dec();
      return *this;
    }
    constexpr path_view_iterator operator--(int) noexcept
    {
      auto self(*this);
      _dec();
      return self;
    }
    constexpr path_view_iterator &operator++() noexcept
    {
      _inc();
      return *this;
    }
    constexpr path_view_iterator operator++(int) noexcept
    {
      auto self(*this);
      _inc();
      return self;
    }
  };
}  // namespace detail

constexpr inline path_view::const_iterator path_view::cbegin() const noexcept
{
  return const_iterator(this, false);
}
constexpr inline path_view::const_iterator path_view::cend() const noexcept
{
  return const_iterator(this, true);
}
constexpr inline path_view::const_iterator path_view::begin() const noexcept
{
  return cbegin();
}
constexpr inline path_view::iterator path_view::begin() noexcept
{
  return cbegin();
}
constexpr inline path_view::const_iterator path_view::end() const noexcept
{
  return cend();
}
constexpr inline path_view::iterator path_view::end() noexcept
{
  return cend();
}
constexpr inline int path_view::compare(const path_view &o) const noexcept
{
  auto it1 = begin(), it2 = o.begin();
  for(; it1 != end() && it2 != o.end(); ++it1, ++it2)
  {
    int res = it1->compare(*it2);
    if(res != 0)
    {
      return res;
    }
  }
  if(it1 == end() && it2 != o.end())
  {
    return -1;
  }
  if(it1 != end() && it2 == o.end())
  {
    return 1;
  }
  return 0;  // identical
}


#ifndef NDEBUG
static_assert(std::is_trivially_copyable<path_view>::value, "path_view is not a trivially copyable!");
#endif

LLFIO_V2_NAMESPACE_END

#if LLFIO_HEADERS_ONLY == 1 && !defined(DOXYGEN_SHOULD_SKIP_THIS)
#define LLFIO_INCLUDED_BY_HEADER 1
#ifdef _WIN32
#include "detail/impl/windows/path_view.ipp"
#endif
#undef LLFIO_INCLUDED_BY_HEADER
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
