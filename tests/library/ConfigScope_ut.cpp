#include "src/ConfigScope.h"

#include "config4cpp/ConfigurationException.h"

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

struct TestConfigScope
: cfg::ConfigScope
{
    TestConfigScope();
    void verifyListOrder(cfg::StringVector const & vec) const;

    using cfg::ConfigScope::listLocalNames;
    using cfg::ConfigScope::listScopedNamesHelper;

    // The ordered list of all names that have been added to the scope.
    std::vector<std::string> ordered;
};

TestConfigScope::
TestConfigScope()
: cfg::ConfigScope(NULL, "")
{
    // Insert a bunch of random things into the scope, which should hopefully
    // provide a good mix that we should be able to ensure that the order in
    // which we insert them is the order that they are presented.
    //
    // Yes, I know the random-number stuff is really poor, but it is good enough
    // for this task, and I don't want to rely on an outside library, nor C++11,
    // since the library itself does not do so.
    while (ordered.size() < 5000u) {
        switch (std::rand() % 5) {
        case 0: // addOrReplaceString
        {
            ordered.push_back(random_string(ordered));
            if (not addOrReplaceString(
                    ordered.back().c_str(),
                    random_string().c_str()))
            {
                throw std::runtime_error(
                    "ConfigScope::addOrReplaceString returned false");
            }
        } break;
        case 1: // addOrReplaceList with array
        {
            std::vector<std::string> values;
            int const limit = (std::rand() % 10) + 1;
            for (int i = 0; i < limit; ++i) {
                values.push_back(random_string());
            }
            std::vector<char const *> ptrs;
            for (int i = 0; i < limit; ++i) {
                ptrs.push_back(values[i].c_str());
            }

            ordered.push_back(random_string(ordered));
            if (not addOrReplaceList(ordered.back().c_str(), &ptrs[0], limit)) {
                throw std::runtime_error(
                    "ConfigScope::addOrReplaceList returned false");
            }
        } break;
        case 2: // addOrReplaceString with StringVector
        {
            cfg::StringVector values;
            int const limit = (std::rand() % 10) + 1;
            for (int i = 0; i < limit; ++i) {
                values.add(random_string().c_str());
            }

            ordered.push_back(random_string(ordered));
            if (not addOrReplaceList(ordered.back().c_str(), values)) {
                throw std::runtime_error(
                    "ConfigScope::addOrReplaceList returned false");
            }
        } break;
        case 3: // ensureScopeExists
        {
            cfg::ConfigScope * new_scope;
            ordered.push_back(random_string(ordered));
            if (not ensureScopeExists(ordered.back().c_str(), new_scope)) {
                throw std::runtime_error(
                    "ConfigScope::ensureScopeExists returned false");
            }
        } break;
        case 4: // removeItem
        {
            if (ordered.size() > 10) {
                unsigned index = std::rand() % ordered.size();
                std::string name = ordered[index];
                ordered.erase(ordered.begin() + index);
                if (not removeItem(name.c_str())) {
                    throw std::runtime_error(
                        "ConfigScope::removeItem returned false");
                }
            }
        } break;
        }
    }
}

void
TestConfigScope::
verifyListOrder(cfg::StringVector const & vec) const
{
    if (ordered.size() != std::size_t(vec.length())) {
        throw std::runtime_error(
            "scope does not have the expected number of entries");
    }

    for (unsigned i = 0; i < ordered.size(); ++i) {
        if (ordered[i] != vec[i]) {
            char index[64];
            snprintf(index, sizeof(index), "%u", i);
            throw std::runtime_error(
                "Expected name \"" + ordered[i] + "\" at index " + index +
                ", got \"" + vec[i] + '"');
        }
    }
}

void
listFullyScopedNames_in_order(TestConfigScope const & scope)
{
    // The official documentation states that listing scope names has no
    // guaranteed order, because the implementation uses a hash table.  This
    // test is to ensure that the order in which they are provided is the order
    // in which they were inserted into the scope.  Since this order is one of
    // the possible unguaranteed orderings, nothing should break.

    cfg::StringVector filter_patterns, vec;
    scope.listFullyScopedNames(
        cfg::Configuration::CFG_SCOPE_AND_VARS,
        false,
        filter_patterns,
        vec);
    scope.verifyListOrder(vec);
}

void
listLocallyScopedNames_in_order(TestConfigScope const & scope)
{
    cfg::StringVector filter_patterns, vec;
    scope.listLocallyScopedNames(
        cfg::Configuration::CFG_SCOPE_AND_VARS,
        false,
        filter_patterns,
        vec);
    scope.verifyListOrder(vec);
}

void
listLocalNames_in_order(TestConfigScope const & scope)
{
    cfg::StringVector vec;
    scope.listLocalNames(cfg::Configuration::CFG_SCOPE_AND_VARS, vec);
    scope.verifyListOrder(vec);
}

void
listScopedNamesHelper_in_order(TestConfigScope const & scope)
{
    cfg::StringVector filter_patterns, vec;
    scope.listScopedNamesHelper(
        "",
        cfg::Configuration::CFG_SCOPE_AND_VARS,
        false,
        filter_patterns,
        vec);
    scope.verifyListOrder(vec);
}

int
Main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;
    TestConfigScope scope;
    listFullyScopedNames_in_order(scope);
    listLocallyScopedNames_in_order(scope);
    listLocalNames_in_order(scope);
    listScopedNamesHelper_in_order(scope);
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
