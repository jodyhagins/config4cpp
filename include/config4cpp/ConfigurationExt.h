//-----------------------------------------------------------------------
// Copyright 2024 Jody Hagins.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions.
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//----------------------------------------------------------------------
#ifndef CONFIG4CPP_CONFIGURATIONEXT_H_
#define CONFIG4CPP_CONFIGURATIONEXT_H_

#include "config4cpp/Configuration.h"

#include <charconv>
#include <cstdlib>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace CONFIG4CPP_NAMESPACE::ext {

class Name
{
    std::string value;

public:
    template <typename... ArgTs>
    Name(ArgTs &&... args)
    requires std::is_constructible_v<std::string, ArgTs...>
    : value(std::forward<ArgTs>(args)...)
    { }

    char const * c_str() const { return value.c_str(); }

    operator char const * () const { return c_str(); }

    operator std::string_view () const { return value; }

    Name & pop()
    {
        if (auto n = value.rfind('.'); n < value.size()) {
            value = value.substr(0, n);
        } else {
            value.clear();
        }
        return *this;
    }

    Name last() const
    {
        if (auto n = value.rfind('.'); n < value.size()) {
            return Name{value.substr(n + 1)};
        }
        return *this;
    }

    Name scope() const
    {
        auto result = *this;
        result.pop();
        return result;
    }

    Name local_name() const { return last(); }

    template <typename ArgT>
    auto operator /= (ArgT && arg)
    -> decltype(std::string_view(arg), std::declval<Name &>())
    {
        auto x = std::string_view(arg);
        if (value.empty()) {
            value = x;
        } else if (not x.empty()) {
            value += '.';
            value += x;
        }
        return *this;
    }

    template <typename ArgT>
    auto operator / (ArgT && arg) const
    -> std::remove_cvref_t<decltype(std::declval<Name &>() /= arg)>
    {
        auto result = *this;
        result /= arg;
        return result;
    }

    friend std::ostream & operator << (std::ostream & strm, Name const & name)
    {
        return strm << name.value;
    }

    template <typename T>
    friend auto operator == (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value == std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator == (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x == y.value;
    }

    template <typename T>
    friend auto operator != (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value != std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator != (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x != y.value;
    }

    template <typename T>
    friend auto operator <= (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value <= std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator <= (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x <= y.value;
    }

    template <typename T>
    friend auto operator >= (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value >= std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator >= (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x >= y.value;
    }

    template <typename T>
    friend auto operator < (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value < std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator < (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x < y.value;
    }

    template <typename T>
    friend auto operator > (Name const & x, T const & y)
    -> decltype(std::string_view(y), bool())
    {
        return x.value > std::string_view(y);
    }

    template <
        typename T,
        std::enable_if_t<not std::is_same_v<T, Name>, bool> = true>
    friend auto operator > (T const & x, Name const & y)
    -> decltype(std::string_view(x), bool())
    {
        return x > y.value;
    }
};

/**
 * This class adds a more convenient to use API over the top of Configuration.
 * This may add substantial cost for some operations.  However, configuration
 * information should not be parsed and processed in the fast path.  One day,
 * maybe I'll just rewrite everything to be more appropriate modern C++, and
 * address the performance considerations as well.  But that would be a time
 * consuming undertaking, and I doubt I'll ever have time for that.
 *
 * Most of these problems are due to the mantra of the original author to
 * explicitly not use the C++ standard library.
 */
class Configuration
{
    using cfg = CONFIG4CPP_NAMESPACE::Configuration;
    std::unique_ptr<cfg, void (*)(cfg *)> impl{cfg::create(), [](cfg * p) {
                                                   if (p) {
                                                       p->destroy();
                                                   }
                                               }};

    template <typename T>
    struct AsVector
    {
        static std::vector<T> _(StringVector const & svec)
        {
            std::vector<T> result;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
            for (auto s = svec.c_array(); *s; ++s) {
#pragma clang diagnostic pop
                result.emplace_back(*s);
            }
            return result;
        }

        static std::vector<T> _(char const ** arr, int len)
        {
            std::vector<T> result;
            for (int i = 0; i < len; ++i) {
                result.emplace_back(arr[i]);
            }
            return result;
        }
    };

    template <typename T, typename... ArgTs>
    static std::vector<T> as_vector(ArgTs &&... args)
    {
        return AsVector<T>::_(std::forward<ArgTs>(args)...);
    }

public:
    static constexpr auto CFG_NO_VALUE = cfg::CFG_NO_VALUE;
    static constexpr auto CFG_STRING = cfg::CFG_STRING;
    static constexpr auto CFG_LIST = cfg::CFG_LIST;
    static constexpr auto CFG_SCOPE = cfg::CFG_SCOPE;
    static constexpr auto CFG_VARIABLES = cfg::CFG_VARIABLES;
    static constexpr auto CFG_SCOPE_AND_VARS = cfg::CFG_SCOPE_AND_VARS;
    static constexpr auto INPUT_FILE = cfg::INPUT_FILE;
    static constexpr auto INPUT_STRING = cfg::INPUT_STRING;
    static constexpr auto INPUT_EXEC = cfg::INPUT_EXEC;

    using Type = cfg::Type;

    // Access to the underlying basic Configuration object, if the original API
    // is desired.
    CONFIG4CPP_NAMESPACE::Configuration * operator -> () { return impl.get(); }

    // Access to the underlying basic Configuration object, if the original API
    // is desired.
    CONFIG4CPP_NAMESPACE::Configuration const * operator -> () const
    {
        return impl.get();
    }

    void parse(
        cfg::SourceType sourceType,
        char const * source,
        char const * sourceDescription = "")
    {
        return impl->parse(sourceType, source, sourceDescription);
    }

    void parse(char const * sourceTypeAndSource)
    {
        return impl->parse(sourceTypeAndSource);
    }

    void parse(
        cfg::SourceType sourceType,
        std::string const & source,
        std::string const & sourceDescription = "")
    {
        return parse(sourceType, source.c_str(), sourceDescription.c_str());
    }

    void parse(std::string const & sourceTypeAndSource)
    {
        return parse(sourceTypeAndSource.c_str());
    }

    std::string_view fileName() const { return impl->fileName(); }

    std::vector<Name> listFullyScopedNames(
        Name const & name,
        Type typeMask,
        bool recursive = true,
        std::vector<std::string> filter_patterns = {}) const
    {
        StringVector filter;
        for (auto const & s : filter_patterns) {
            filter.add(s.c_str());
        }
        StringVector names;
        impl->listFullyScopedNames(
            name.scope(),
            name.local_name(),
            typeMask,
            recursive,
            filter,
            names);
        return as_vector<Name>(names);
    }

    std::vector<Name> listLocallyScopedNames(
        Name const & name,
        Type typeMask,
        bool recursive = true,
        std::vector<std::string> filter_patterns = {}) const
    {
        StringVector filter;
        for (auto const & s : filter_patterns) {
            filter.add(s.c_str());
        }
        StringVector names;
        impl->listLocallyScopedNames(
            name.scope(),
            name.local_name(),
            typeMask,
            recursive,
            filter,
            names);
        return as_vector<Name>(names);
    }

    Type type(Name const & name) const
    {
        return impl->type(name.scope(), name.local_name());
    }

    void dump(
        std::string & destination,
        bool wantExpandedUidNames = true,
        Name const & name = {})
    {
        StringBuffer buf;
        impl->dump(buf, wantExpandedUidNames, name.scope(), name.local_name());
        destination.append(buf.c_str(), static_cast<std::size_t>(buf.length()));
    }

    template <std::size_t N>
    std::size_t lookupName(
        Name const & name,
        std::array<std::string_view, N> const & names) const
    {
        if (auto s = lookupString(name)) {
            return static_cast<std::size_t>(std::distance(
                names.begin(),
                std::find(names.begin(), names.end(), *s)));
        }
        return N;
    }

    std::optional<std::string_view> lookupString(Name const & name) const
    {
        char c = '\0';
        if (auto result =
                impl->lookupString(name.scope(), name.local_name(), &c);
            result != &c)
        {
            return result;
        }
        return std::nullopt;
    }

    std::optional<std::vector<std::string_view>> lookupList(
        Name const & name) const
    {
        char const ** arr;
        int len;
        char const * defarr[1] = {};
        impl->lookupList(name.scope(), name.local_name(), arr, len, defarr, 0);
        if (arr != defarr) {
            return as_vector<std::string_view>(arr, len);
        } else {
            return std::nullopt;
        }
    }

    template <typename FnT>
    auto with_string(Name const & name, FnT && fn) const
    -> decltype(std::forward<FnT>(fn)(std::declval<std::string_view>()))
    {
        char c = '\0';
        if (auto c_str =
                impl->lookupString(name.scope(), name.local_name(), &c);
            c_str != &c)
        {
            return std::forward<FnT>(fn)(std::string_view(c_str));
        }
        return std::nullopt;
    }

    std::optional<std::intmax_t> lookupInt(Name const & name) const
    {
        return with_string(name, [](auto s) -> std::optional<std::intmax_t> {
            std::intmax_t result;
            if (std::from_chars(s.begin(), s.end(), result).ec == std::errc()) {
                return result;
            } else {
                return std::nullopt;
            }
        });
    }

    std::optional<float> lookupFloat(Name const & name) const
    {
        return with_string(name, [](auto s) -> std::optional<float> {
            // We got the value from the config library, so it will be
            // nul-terminated.
            char * end;
            auto result = std::strtof(s.data(), &end);
            if (*end == '\0') {
                return result;
            }
            return std::nullopt;
        });
    }

    std::optional<double> lookupDouble(Name const & name) const
    {
        return with_string(name, [](auto s) -> std::optional<double> {
            // We got the value from the config library, so it will be
            // nul-terminated.
            char * end;
            auto result = std::strtod(s.data(), &end);
            if (*end == '\0') {
                return result;
            }
            return std::nullopt;
        });
    }

    std::optional<std::intmax_t> lookupEnum(
        Name const & name,
        EnumNameAndValue const * enumInfo,
        std::size_t numEnums) const
    {
        return with_string(name, [&](auto s) -> std::optional<std::intmax_t> {
            for (std::size_t i = 0; i < numEnums; i++) {
                if (s == enumInfo[i].name) {
                    return enumInfo[i].value;
                }
            }
            return std::nullopt;
        });
    }

    template <std::size_t N>
    std::optional<std::intmax_t> lookupEnum(
        Name const & name,
        std::array<EnumNameAndValue, N> const & enum_info) const
    {
        return lookupEnum(name, enum_info.data(), N);
    }

    std::optional<bool> lookupBoolean(Name const & name)
    {
        return lookupEnum(
            name,
            std::array<EnumNameAndValue, 2>{{{"false", 0}, {"true", 1}}});
    }

    void insertString(Name const & name, char const * str)
    {
        impl->insertString(name.scope(), name.local_name(), str);
    }

    void insertString(Name const & name, std::string const & str)
    {
        impl->insertString(name.scope(), name.local_name(), str.c_str());
    }

    void insertList(Name const & name, std::vector<char const *> const & vec)
    {
        impl->insertList(
            name.scope(),
            name.local_name(),
            const_cast<char const **>(vec.data()),
            static_cast<int>(vec.size()));
    }

    void insertList(Name const & name, std::vector<std::string> const & vec)
    {
        std::vector<char const *> v;
        for (auto const & s : vec) {
            v.push_back(s.c_str());
        }
        insertList(name, v);
    }

    void ensureScopeExists(Name const & name)
    {
        impl->ensureScopeExists(name.scope(), name.local_name());
    }

    void remove(Name const & name)
    {
        impl->remove(name.scope(), name.local_name());
    }

    void clear() { impl->empty(); }
};

} // namespace CONFIG4CPP_NAMESPACE::ext

#endif // CONFIG4CPP_CONFIGURATIONEXT_H_
