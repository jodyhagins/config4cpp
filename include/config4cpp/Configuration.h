//-----------------------------------------------------------------------
// Copyright 2011 Ciaran McHale.
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

#ifndef CONFIG4CPP_CONFIGURATION_H_
#define CONFIG4CPP_CONFIGURATION_H_





//--------
// #include's
//--------
#include <config4cpp/namespace.h>
#include <config4cpp/ConfigurationException.h>
#include <config4cpp/StringBuffer.h>
#include <config4cpp/StringVector.h>
#include <stddef.h>

#include <functional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>



namespace CONFIG4CPP_NAMESPACE {





struct EnumNameAndValue {
	const char *	name;
	intmax_t				value;
};





//--------
// Class Configuration
//--------

class Configuration
{
public:
	enum Type {                 // bit masks
		CFG_NO_VALUE       = 0,
		CFG_STRING         = 1, // 0001
		CFG_LIST           = 2, // 0010
		CFG_SCOPE          = 4, // 0100
		CFG_VARIABLES      = 3, // 0011 = CFG_STRING | CFG_LIST
		CFG_SCOPE_AND_VARS = 7  // 0111 = CFG_STRING | CFG_LIST | CFG_SCOPE
	};

	enum SourceType {INPUT_FILE, INPUT_STRING, INPUT_EXEC};

	static Configuration * create();
	virtual void destroy();

	static void mergeNames(
					const char *		scope,
					const char *		localName,
					StringBuffer &		fullyScopedName);

	static int mbstrlen(const char * str);

	virtual void setFallbackConfiguration(Configuration * cfg) = 0;
	virtual void setFallbackConfiguration(
					Configuration::SourceType	sourceType,
					const char *				source,
					const char *				sourceDescription = "") = 0;
	virtual const Configuration * getFallbackConfiguration() = 0;

	virtual void setOverrideConfiguration(Configuration * cfg) = 0;
	virtual void setOverrideConfiguration(
					Configuration::SourceType	sourceType,
					const char *				source,
					const char *				sourceDescription = "") = 0;
	virtual const Configuration * getOverrideConfiguration() = 0;

	virtual void setSecurityConfiguration(
					Configuration *		cfg,
					bool				takeOwnership,
					const char *		scope = "") = 0; 

	virtual void setSecurityConfiguration(
					const char *		cfgInput,
					const char *		scope = "") = 0; 
	virtual void getSecurityConfiguration(
					const Configuration *&	cfg,
					const char *&			scope) = 0;

	virtual void parse(
					Configuration::SourceType	sourceType,
					const char *				source,
					const char *				sourceDescription = "") = 0;
	inline void	parse(const char * sourceTypeAndSource);

	virtual const char * fileName() const = 0;

	virtual void listFullyScopedNames(
					const char *		scope,
					const char *		localName,
					Type				typeMask,
					bool				recursive,
					StringVector &		names) const = 0;
	virtual void listFullyScopedNames(
					const char *		scope,
					const char *		localName,
					Type				typeMask,
					bool				recursive,
					const char *		filterPattern,
					StringVector &		names) const = 0;
	virtual void listFullyScopedNames(
					const char *				scope,
					const char *				localName,
					Type						typeMask,
					bool						recursive,
					const StringVector &		filterPatterns,
					StringVector &				names) const = 0;

	virtual void listLocallyScopedNames(
					const char *		scope,
					const char *		localName,
					Type				typeMask,
					bool				recursive,
					StringVector &		names) const = 0;
	virtual void listLocallyScopedNames(
					const char *		scope,
					const char *		localName,
					Type				typeMask,
					bool				recursive,
					const char *		filterPattern,
					StringVector &		names) const = 0;
	virtual void listLocallyScopedNames(
					const char *				scope,
					const char *				localName,
					Type						typeMask,
					bool						recursive,
					const StringVector &		filterPatterns,
					StringVector &				names) const = 0;

	virtual Type type(const char * scope, const char * localName) const = 0;

	static bool	patternMatch(const char * str, const char * pattern);

	virtual bool uidEquals(const char * s1, const char * s2) const = 0;
	virtual void expandUid(StringBuffer & spelling) = 0;
	virtual const char * unexpandUid(
					const char *		spelling,
					StringBuffer &		buf) const = 0;

	//--------
	// Dump part or all of the configuration
	//--------
	virtual void dump(StringBuffer & buf, bool wantExpandedUidNames) const = 0;

	virtual void dump(
					StringBuffer &		buf,
					bool				wantExpandedUidNames,
					const char *		scope,
					const char *		localName) const = 0;

	virtual bool isBoolean(const char * str) const = 0;
	virtual bool isInt(const char * str) const = 0;
	virtual bool isFloat(const char * str) const = 0;
	virtual bool isDurationMicroseconds(const char * str) const = 0;
	virtual bool isDurationMilliseconds(const char * str) const = 0;
	virtual bool isDurationSeconds(const char * str) const = 0;
	virtual bool isMemorySizeBytes(const char * str) const = 0;
	virtual bool isMemorySizeKB(const char * str) const = 0;
	virtual bool isMemorySizeMB(const char * str) const = 0;
	virtual bool isEnum(
					const char *				str,
					const EnumNameAndValue *	enumInfo,
					int							numEnums) const = 0;
	virtual bool isFloatWithUnits(
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize) const = 0;
	virtual bool isIntWithUnits(
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize) const = 0;

	virtual bool isUnitsWithFloat(
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize) const = 0;
	virtual bool isUnitsWithInt(
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize) const = 0;

	virtual bool stringToBoolean(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToInt(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual float stringToFloat(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToDurationSeconds(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToDurationMilliseconds(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToDurationMicroseconds(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToMemorySizeBytes(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToMemorySizeKB(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToMemorySizeMB(
					const char *		scope,
					const char *		localName,
					const char *		str) const = 0;
	virtual int stringToEnum(
					const char *				scope,
					const char *				localName,
					const char *				typeName,
					const char *				str,
					const EnumNameAndValue *	enumInfo,
					int 						numEnums) const = 0;
	virtual void stringToFloatWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult) const = 0;
	virtual void stringToUnitsWithFloat(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult) const = 0;
	virtual void stringToIntWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult) const = 0;
	virtual void stringToUnitsWithInt(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char *		str,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult) const = 0;

	//--------
	// lookup<Type>() operations, with and without default values.
	//--------
	virtual const char * lookupString(
					const char *		scope,
					const char *		localName,
					const char *		defaultVal) const = 0;
	virtual const char * lookupString(
					const char *		scope,
					const char *		localName) const = 0;

	virtual void lookupList(
					const char *		scope,
					const char *		localName,
					const char **&		array,
					int &				arraySize,
					const char **		defaultArray,
					int					defaultArraySize) const = 0;
	virtual void lookupList(
					const char *		scope,
					const char *		localName,
					const char **&		array,
					int &				arraySize) const = 0;

	virtual void lookupList(
					const char *				scope,
					const char *				localName,
					StringVector &				list,
					const StringVector &		defaultList) const = 0;
	virtual void lookupList(
					const char *		scope,
					const char *		localName,
					StringVector &		list) const = 0;

	virtual int lookupInt(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupInt(
					const char *		scope,
					const char *		localName) const = 0;

	virtual float lookupFloat(
					const char *		scope,
					const char *		localName,
					float				defaultVal) const = 0;
	virtual float lookupFloat(
					const char *		scope,
					const char *		localName) const = 0;

	virtual int lookupEnum(
					const char *				scope,
					const char *				localName,
					const char *				typeName,
					const EnumNameAndValue *	enumInfo,
					int 						numEnums,
					const char *				defaultVal) const = 0;
	virtual int lookupEnum(
					const char *				scope,
					const char *				localName,
					const char *				typeName,
					const EnumNameAndValue *	enumInfo,
					int 						numEnums,
					int							defaultVal) const = 0;
	virtual int lookupEnum(
					const char *				scope,
					const char *				localName,
					const char *				typeName,
					const EnumNameAndValue *	enumInfo,
					int 						numEnums) const = 0;

	virtual bool lookupBoolean(
					const char *		scope,
					const char *		localName,
					bool				defaultVal) const = 0;
	virtual bool lookupBoolean(
					const char *		scope,
					const char *		localName) const = 0;

	virtual void lookupFloatWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult) const = 0;
	virtual void lookupFloatWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult,
					float				defaultFloat,
					const char *		defaultUnits) const = 0;

	virtual void lookupUnitsWithFloat(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult) const = 0;
	virtual void lookupUnitsWithFloat(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					float &				floatResult,
					const char *&		unitsResult,
					float				defaultFloat,
					const char *		defaultUnits) const = 0;

	virtual void lookupIntWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult) const = 0;
	virtual void lookupIntWithUnits(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult,
					int					defaultInt,
					const char *		defaultUnits) const = 0;

	virtual void lookupUnitsWithInt(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult) const = 0;
	virtual void lookupUnitsWithInt(
					const char *		scope,
					const char *		localName,
					const char *		typeName,
					const char **		allowedUnits,
					int					allowedUnitsSize,
					int &				intResult,
					const char *&		unitsResult,
					int					defaultInt,
					const char *		defaultUnits) const = 0;

	virtual int lookupDurationMicroseconds(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupDurationMicroseconds(
					const char *		scope,
					const char *		localName) const = 0;

	virtual int lookupDurationMilliseconds(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupDurationMilliseconds(
					const char *		scope,
					const char *		localName) const = 0;

	virtual int lookupDurationSeconds(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupDurationSeconds(
					const char *		scope,
					const char *		localName) const = 0;

	virtual int lookupMemorySizeBytes(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupMemorySizeBytes(
					const char *		scope,
					const char *		localName) const = 0;
	virtual int lookupMemorySizeKB(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const = 0;
	virtual int lookupMemorySizeKB(
					const char *		scope,
					const char *		localName) const = 0;
	virtual int lookupMemorySizeMB(
					const char *		scope,
					const char *		localName,
					int					defaultVal) const
											 = 0;
	virtual int lookupMemorySizeMB(
					const char *		scope,
					const char *		localName) const = 0;

	virtual void lookupScope(
					const char *		scope,
					const char *		localName) const = 0;

	//--------
	// Update operations
	//--------
	virtual void insertString(
					const char *		scope,
					const char *		localName,
					const char *		strValue) = 0;

	virtual void insertList(
					const char *		scope,
					const char *		localName,
					const char **		array,
					int					arraySize) = 0;

	virtual void insertList(
					const char *		scope,
					const char *		localName,
					const char **		nullTerminatedArray) = 0;

	virtual void insertList(
					const char *				scope,
					const char *				localName,
					const StringVector &		vec) = 0;

	virtual void ensureScopeExists(
					const char *		scope,
					const char *		localName) = 0;

	virtual void remove(const char * scope, const char * localName) = 0;

	virtual void empty() = 0;

        void addCallable(
            char const * name,
            std::function<void(StringBuffer &, StringVector const &)> callable)
        {
            addCallable(name, -1, std::move(callable));
        }

        virtual void addCallable(
            char const * name,
            int num_args,
            std::function<void(StringBuffer &, StringVector const &)>) = 0;

        virtual std::vector<
            std::tuple<
                std::string,
                int,
                std::function<void(StringBuffer &, StringVector const &)>>>
        getCallables() const = 0;

protected:
	//--------
	// Available only to the implementation subclass
	//--------
	Configuration();
	virtual ~Configuration();

private:
	static bool patternMatchInternal(
					const wchar_t *		wStr,
					int					wStrIndex,
					int					wStrLen,
					const wchar_t *		wPattern,
					int					wPatternIndex,
					int					wPatternLen);
	//--------
	// Not implemented
	//--------
	Configuration(const Configuration & ex);
	Configuration & operator=(const Configuration & ex);
};





inline void
Configuration::parse(const char * str)
{
	if (strncmp(str, "exec#", 5) == 0) {
		parse(Configuration::INPUT_EXEC, &(str[5]));
	} else if (strncmp(str, "file#", 5) == 0) {
		parse(Configuration::INPUT_FILE, &(str[5]));
	} else if (strncmp(str, "str#", 4) == 0) {
		parse(Configuration::INPUT_STRING, &(str[4]));
	} else {
		parse(Configuration::INPUT_FILE, str);
	}
}





}; // namespace CONFIG4CPP_NAMESPACE
#endif
