#include "config4cpp/ConfigurationException.h"
#include "config4cpp/ConfigurationExt.h"
#include "src/ConfigScope.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// The project has no dependency on a testing framework, and I don't want to add
// one (yet), so we will just do something very basic here.

namespace {
namespace cfg = CONFIG4CPP_NAMESPACE;
using namespace std::literals;

std::string
random_string()
{
    static char c[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    unsigned const len = (std::rand() % 90) + 10;
    result.reserve(len);
    for (unsigned i = 0; i < len; ++i) {
        result += c[std::rand() % (sizeof(c) - 1)];
    }
    return result;
}

std::string
random_string(std::vector<std::string> const & names)
{
    std::string result;
    do {
        result = random_string();
    } while (std::find(names.begin(), names.end(), result) != names.end());
    return result;
}

struct Test
{
    Test();
    ~Test();
    void verifyListOrder(
        cfg::StringVector const & vec,
        char const * prefix = "") const;
    void verifyListOrder(
        std::vector<cfg::ext::Name> const & vec,
        char const * prefix = "") const;

    cfg::Configuration * cfg;
    cfg::ext::Configuration cfg_ext;

    // The ordered list of all names that have been added to the scope.
    std::vector<std::string> ordered;
};

Test::
~Test()
{
    cfg->destroy();
}

Test::
Test()
: cfg(cfg::Configuration::create())
{
    cfg->ensureScopeExists("", "foo");
    auto name = cfg::ext::Name("foo");
    cfg_ext.ensureScopeExists(name);
    while (ordered.size() < 5000u) {
        ordered.push_back(random_string(ordered));
        cfg->insertString("foo", ordered.back().c_str(), "");
        auto n = name / ordered.back().c_str();
        cfg_ext.insertString(name / ordered.back().c_str(), "");
    }
}

void
Test::
verifyListOrder(cfg::StringVector const & vec, char const * prefix) const
{
    if (ordered.size() != std::size_t(vec.length())) {
        throw std::runtime_error(
            "scope does not have the expected number of entries");
    }

    for (unsigned i = 0; i < ordered.size(); ++i) {
        if (prefix + ordered[i] != vec[i]) {
            char index[64];
            snprintf(index, sizeof(index), "%u", i);
            throw std::runtime_error(
                "Expected name \"" + ordered[i] + "\" at index " + index +
                ", got \"" + vec[i] + '"');
        }
    }
}

void
Test::
verifyListOrder(std::vector<cfg::ext::Name> const & vec, char const * prefix)
    const
{
    if (ordered.size() != vec.size()) {
        throw std::runtime_error(
            "scope does not have the expected number of entries");
    }

    for (unsigned i = 0; i < ordered.size(); ++i) {
        if (prefix + ordered[i] != vec[i]) {
            char index[64];
            snprintf(index, sizeof(index), "%u", i);
            throw std::runtime_error(
                "Expected name \"" + ordered[i] + "\" at index " + index +
                ", got \"" + vec[i].c_str() + '"');
        }
    }
}

void
listFullyScopedNames_in_order(Test const & test)
{
    cfg::StringVector filter_patterns, vec;
    test.cfg->listFullyScopedNames(
        "",
        "foo",
        cfg::Configuration::CFG_SCOPE_AND_VARS,
        false,
        filter_patterns,
        vec);
    test.verifyListOrder(vec, "foo.");

    {
        auto names = test.cfg_ext.listFullyScopedNames(
            "foo",
            cfg::Configuration::CFG_SCOPE_AND_VARS,
            false);
        test.verifyListOrder(names, "foo.");
    }
}

void
listLocallyScopedNames_in_order(Test const & test)
{
    cfg::StringVector filter_patterns, vec;
    test.cfg->listLocallyScopedNames(
        "",
        "foo",
        cfg::Configuration::CFG_SCOPE_AND_VARS,
        false,
        vec);
    test.verifyListOrder(vec);

    {
        auto names = test.cfg_ext.listLocallyScopedNames(
            "foo",
            cfg::Configuration::CFG_SCOPE_AND_VARS,
            false);
        test.verifyListOrder(names);
    }
}

#define EXPECT(X) \
    [&](auto && x) { \
        if (not x) { \
            std::stringstream strm; \
            strm << "EXPECT(" << #X << ") failed"; \
            throw std::runtime_error(strm.str()); \
        } \
        return true; \
    }(X)

#define EXPECT_EQ(X, Y) \
    [&](auto && x, auto && y) { \
        if (not (x == y)) { \
            std::stringstream strm; \
            strm << "EXPECT_EQ(" << #X << ", " << #Y << ") failed: " << '[' \
                << x << "] != [" << y << ']'; \
            throw std::runtime_error(strm.str()); \
        } \
        return true; \
    }(X, Y)

void
test_valid_callables()
{
    cfg::ext::Configuration config;
    config->addCallable(
        "foo",
        [&](cfg::StringBuffer & dst, cfg::StringVector const & args) {
            for (int i = 0; i < args.length(); ++i) {
                if (i > 0) {
                    dst << ' ';
                }
                dst << args[i];
            }
        });
    config->addCallable(
        "bar",
        2,
        [](cfg::StringBuffer & dst, cfg::StringVector const & args) {
            for (int i = 0; i < args.length(); ++i) {
                if (i > 1) {
                    dst << ", ";
                }
                for (auto c : std::string_view(args[i])) {
                    dst << static_cast<char>(std::toupper(c));
                }
                if (i == 0) {
                    dst << '(';
                }
            }
            dst << ')';
        });

    config.parse(
        cfg::ext::Configuration::INPUT_STRING,
        R"(foo="bar";
           f0=call("foo");
           f0_1=call("foo", []);
           f1=call("foo", ["bar"]);
           b2=call("bar", ["1", "2"]);)");

    EXPECT(config.lookupString("f0")) &&
        EXPECT_EQ("foo"s, *config.lookupString("f0"));
    EXPECT(config.lookupString("f0_1")) &&
        EXPECT_EQ("foo"s, *config.lookupString("f0_1"));
    EXPECT(config.lookupString("f1")) &&
        EXPECT_EQ("foo bar"s, *config.lookupString("f1"));
    EXPECT(config.lookupString("b2")) &&
        EXPECT_EQ("BAR(1, 2)"s, *config.lookupString("b2"));
}

void
test_unknown_callable()
{
    cfg::ext::Configuration config;
    try {
        config.parse(
            cfg::ext::Configuration::INPUT_STRING,
            R"(foo="bar";
           f0=call("foo");)");
    } catch (cfg::ConfigurationException const & ex) {
        EXPECT_EQ(
            "<string-based configuration>, line 2: call(\"foo\") failed: "
            "not registered"s,
            ex.c_str());
        return;
    }
    EXPECT(not "Expected exception");
}

void
test_conditional_callable()
{
    cfg::ext::Configuration config;
    config->addCallable(
        "foo",
        [&](cfg::StringBuffer & str, cfg::StringVector const &) {
            str = "blargy";
        });
    config.parse(
        cfg::ext::Configuration::INPUT_STRING,
        R"(foo="bar";
               @if (isCallable("bar")) {
                   b0=call("bar");
               }
               @if (isCallable("foo")) {
                   f0=call("foo");
               })");

    EXPECT(not config.lookupString("b0"));
    EXPECT(config.lookupString("f0")) &&
        EXPECT_EQ("blargy"s, *config.lookupString("f0"));
}

void
test_callable_with_expected_num_args()
{
    cfg::ext::Configuration config;
    config->addCallable(
        "bar",
        3,
        [](cfg::StringBuffer &, cfg::StringVector const &) {});

    try {
        config.parse(
            cfg::ext::Configuration::INPUT_STRING,
            R"(foo="bar";
           b2=call("bar", ["1", "2"]);)");
    } catch (cfg::ConfigurationException const & ex) {
        EXPECT_EQ(
            "<string-based configuration>, line 2: call(\"bar\", "
            "[\"1\", \"2\"]) failed: expected exactly 3 arguments"s,
            ex.c_str());
        return;
    }
    EXPECT(not "Expected exception");
}

void
test_transform()
{
    cfg::ext::Configuration config;
    config.parse(
        cfg::ext::Configuration::INPUT_STRING,
        R"(foo="bar";
           bar="BAR";
           f=transform(["foo", "bar"], @arg + "-" + @arg);)");

    if (EXPECT(config.lookupList("f"))) {
        auto f = *config.lookupList("f");
        EXPECT_EQ(2u, f.size()) &&
            EXPECT_EQ("foo-foo"s, f[0]) &&
            EXPECT_EQ("bar-bar"s, f[1]);
    }
}

int
Main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;
    Test test;
    listFullyScopedNames_in_order(test);
    listLocallyScopedNames_in_order(test);
    test_valid_callables();
    test_unknown_callable();
    test_callable_with_expected_num_args();
    test_conditional_callable();
    test_transform();
    return 0;
}

} // anonymous namespace

int
main(int argc, char * argv[])
{
    std::string error;

    // I don't want to depend on advanced things just for this test.
    // We don't need a rigorous RNG.
    unsigned seed = std::time(nullptr);
    std::srand(seed);
    try {
        return Main(argc, argv);
    } catch (cfg::ConfigurationException const & ex) {
        error = std::string("exception: ") + ex.c_str();
    } catch (std::exception const & ex) {
        error = std::string("exception: ") + ex.what();
    } catch (...) {
        error = "unknown exception";
    }
    std::cerr << error << '\n' << "random seed is " << seed << '\n';
    return 1;
}
