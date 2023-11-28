#include "config4cpp/ConfigurationException.h"
#include "src/ConfigScope.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>

// The project has no dependency on a testing framework, and I don't want to add
// one (yet), so we will just do something very basic here.

namespace {
namespace cfg = CONFIG4CPP_NAMESPACE;

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

    cfg::Configuration * cfg;

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
    while (ordered.size() < 5000u) {
        ordered.push_back(random_string(ordered));
        cfg->insertString("foo", ordered.back().c_str(), "");
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
}

int
Main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;
    Test test;
    listFullyScopedNames_in_order(test);
    listLocallyScopedNames_in_order(test);
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
