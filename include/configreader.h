//===================================================//
//                                                   //
//              ~ Acora IRC Services ~               //
//                                                   //
//       Acora (C) 2011 Acora Development Team       //
// Please see the file CREDITS for more information. //
//                                                   //
//   This program is free but copyrighted software   //
//      Please see the file COPYING for details.     //
//                                                   //
//===================================================//
// $Id: configreader.h 706 2009-01-31 20:17:11Z ankit $
//===================================================//

#ifndef	CONFIGREADER_H
#define	CONFIGREADER_H

#include "base.h"

#include <map>
#include <deque>

// errors for configreader
namespace err
{
	namespace configreader
	{
		enum
		{
			none,		// no error
			notFound,	// config file could not be found
			loop,		// include loop
			errors,		// there were errros in config file
			warnings	// there were warnings in config file
		};
	}
}

// reads config file and saves values
class ConfigReader
{
protected:
	// the actual read procedure called by public Read()
	virtual ErrorCode read(const nstring::str &file, std::deque<nstring::str> &errors, std::deque<nstring::str> &fileStack, std::map<std::string, std::map<std::string, std::deque<nstring::str> > > &vals);

	// data read from the config file
	std::map<std::string, std::map<std::string, std::deque<nstring::str> > > values;

public:
	// constructor
	ConfigReader();
	// destructor
	virtual ~ConfigReader();

	// read config file
	virtual ErrorCode read(const nstring::str file, std::deque<nstring::str> &errors);

	// true if the requested key has a blank value or doesn't exist. block and key must be lowercase.
	virtual bool isEmpty(const std::string block, const std::string key, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);

	// get a string value from config file. block and key must be lowercase.
	virtual void getValue(nstring::str &in, const std::string block, const std::string key, nstring::str def = nstring::str(), std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);
	// get an integer value from config file. block and key must be lowercase.
	virtual void getValue(int &in, const std::string block, const std::string key, int def = 0, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);
	// get a bool value from config file. "yes"/"true"/"1" is true, "no"/"false"/"0" is false. block and key must be lowercase.
	virtual void getValue(bool &in, const std::string block, const std::string key, bool def, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);

	// get multiple string value from config file. block and key must be lowercase.
	virtual void getValues(std::deque<nstring::str> &in, const std::string block, const std::string key, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);

	// return values instead of having to pass references. block and key must be lowercase.
	template<typename T>
	T getValue(const std::string block, const std::string key, T def, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals = NULL);
};

// implementation of the getValue template
template<typename T>
T ConfigReader::getValue(const std::string block, const std::string key, T def, std::map<std::string, std::map<std::string, std::deque<nstring::str> > >* vals)
{
	T ret;
	getValue(ret, block, key, def, vals);
	return ret;
}

#endif // CONFIGREADER_H

