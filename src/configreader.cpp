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
// $Id: configreader.cpp 692 2009-01-29 09:15:03Z ankit $
//===================================================//

#include "configreader.h"
#include "config.h"
#include "utils.h"
#include "instance.h"
#include "ircdprotocol.h"
#include "charmaps.h"

#include <fstream>


// stuff used by the reader
namespace reader
{
	// reader position
	namespace pos
	{
		enum
		{
			global,	// in global
			block,	// in a block
			value	// in a value
		};
	}
	
	// append error to errors deque
	void addError(const String &file, std::deque<String> &errors, int &lno, const String error)
	{
		errors.push_back(file + ":" + utils::toString<int>(lno) + ": " + error);
	}
}


ConfigReader::ConfigReader()
{ }


ConfigReader::~ConfigReader()
{ }


ErrorCode ConfigReader::read(const String &file, std::deque<String> &errors, std::deque<String> &filestack, std::map<std::string, std::map<std::string, std::deque<String> > > &vals)
{
	for (std::deque<String>::iterator i = filestack.begin(); i != filestack.end(); ++i)
	{
		if (*i == file)
			return err::configreader::loop;
	}
	
	filestack.push_back(file);
	instance->debug("Reading config file \"" + file + "\" ...");
	
	std::ifstream fin((String(config::CONFDIR) + file).c_str());
	if (!fin)
		return err::configreader::notFound;
	
	// make sure the file isn't blank
	fin.seekg(0, std::ios_base::end);
	
	if (fin.tellg())
	{
		// reset cursor
		fin.seekg(0, std::ios_base::beg);
		
		int lno = 0;
		std::string line, block, key;
		String value;
		
		short pos = reader::pos::global;
		bool commented = false;
		
		bool glock = false;
		bool klock = false;
		
		while (fin)
		{
			// read a line
			++lno;
			std::getline(fin, line);
			
			// convert tabs to spaces
			for (std::string::iterator i = line.begin(); i != line.end(); ++i)
			{
				if (*i == '\t')
					*i = ' ';
			}
			
			// remove spaces from front to check if the line is empty
			line.erase(0, line.find_first_not_of(' '));

			// if line is empty, skip to next line			
			if (line.empty())
				continue;
			
			for (std::string::iterator i = line.begin(); i != line.end(); ++i)
			{
				// escape character
				if (*i == '\\')
				{
					if (i + 1 != line.end())
					{
						++i;
						
						// if we need special escape sequences, we put conditions here
						
						if (pos == reader::pos::global)
							block += *i;
						else if (pos == reader::pos::block)
							key += *i;
						else if (pos == reader::pos::value)
							value += *i;
					}
					
					continue;
				}
					
				if (pos != reader::pos::value)
				{
					// single line comment // or #
					if (*i == '#' || (i + 1 != line.end() && (*i == '/' && *(i + 1) == '/')))
						break;
					
					// multiline comment /* */
					else if (i + 1 != line.end() && (*i == '/' && *(i + 1) == '*'))
						commented = true;
					else if (i + 1 != line.end() && (*i == '*' && *(i + 1) == '/'))
					{
						if (!commented)
						{
							reader::addError(file, errors, lno, "No comment to end. (Unexpected \"*/\")");
							return err::configreader::errors;
						}
						
						commented = false;

						if (i + 2 == line.end())
							break;
						else
							i += 2;
					}
				}
				
				if (commented)
					continue;

				// initializing blocks
				if (pos == reader::pos::global)
				{
					if (block.empty() && *i == ' ')
						continue;
						
					if (*i == ')')
					{
						reader::addError(file, errors, lno, "No block to end. (Unexpected ')')");
						return err::configreader::errors;
					}
					else if (*i == '"')
					{
						reader::addError(file, errors, lno, "'\"' is not allowed outside a block");
						return err::configreader::errors;
					}
					
					if (!glock && *i != ' ' && *i != '(')
						block += charmaps::lower[static_cast<short>(*i)];
					else if (glock && *i != ' ' && *i != '(')
					{
						reader::addError(file, errors, lno, String() + "Missing block after \"" + block.c_str() + "\"");
						return err::configreader::errors;
					}
					else
						glock = true;
						
					if (*i == '(')
					{
						if (block.empty())
						{
							reader::addError(file, errors, lno, "Block is missing a label");
							return err::configreader::errors;
						}
						else
						{
							glock = false;
							pos = reader::pos::block;
							continue;
						}
					}
				}
				
				// processing blocks
				else if (pos == reader::pos::block)
				{
					if (key.empty() && *i == ' ')
						continue;

					if (*i == ')')
					{
						if (key.empty())
						{
							block.clear();
							pos = reader::pos::global;
							continue;
						}
						else
						{
							reader::addError(file, errors, lno, String() + "Unexpected end of block (Missing value after key \"" + key.c_str() + "\")");
							return err::configreader::errors;
						}
					}
					else if (*i == '(')
					{
						reader::addError(file, errors, lno, "'(' is not allowed inside a block");
						return err::configreader::errors;
					}

					if (!klock && *i != ' ' && *i != '"')
						key += charmaps::lower[static_cast<short>(*i)];
					else if (klock && *i != ' ' && *i != '"')
					{
						reader::addError(file, errors, lno, String() + "Missing value after key \"" + key.c_str() + "\"");
						return err::configreader::errors;
					}
					else
						klock = true;
			
					if (*i == '"')
					{
						if (key.empty())
						{
							reader::addError(file, errors, lno, "Value is missing a key");
							return err::configreader::errors;
						}
						else
						{
							klock = false;
							pos = reader::pos::value;
							continue;
						}
					}
				}
				
				// processing and saving values
				else if (pos == reader::pos::value)
				{
					if (*i == '"')
					{
						if (block == "include")
						{
							if (key == "file")
							{
								ErrorCode ret = read(value, errors, filestack, vals);
								
								if (ret == err::configreader::loop)
								{
									reader::addError(file, errors, lno, "File \"" + value + "\" is already included");
									ret = err::configreader::errors;
									
									return ret;
								}
								else if (ret != err::configreader::none && ret != err::configreader::warnings)
								{
									if (ret == err::configreader::notFound)
									{
										reader::addError(file, errors, lno, "Could not open included file \"" + value + "\"");
										ret = err::configreader::errors;
									}

									return ret;
								}
							}
						}
						else if (!value.empty())
							vals[block][key].push_back(value);

						value.clear();
						key.clear();

						pos = reader::pos::block;
						continue;
					}
					else
						value += *i;
				}
			}
			
			if (pos == reader::pos::global && !glock && !block.empty())
				glock = true;
			else if (pos == reader::pos::block && !klock && !key.empty())
				klock = true;
		
			if (pos == reader::pos::value)
			{
				reader::addError(file, errors, lno, "Unexpected end of line (Values cannot span over multiple lines)");
				return err::configreader::errors;
			}
		}
		
		if (!block.empty())
		{
			reader::addError(file, errors, lno, String() + "Unexpected end of file (Block \"" + block.c_str() + "\" is left open)");
			return err::configreader::errors;
		}
	}
	
	filestack.pop_back();
	instance->debug("Finished reading config file \"" + file + "\".");
	
	if (errors.empty())
		return err::configreader::none;
	else
		return err::configreader::warnings;
}


ErrorCode ConfigReader::read(const String file, std::deque<String> &errors)
{
	std::deque<String> filestack;
	std::map<std::string, std::map<std::string, std::deque<String> > > vals;

	ErrorCode ret = read(file, errors, filestack, vals);
	
	if (ret == err::configreader::errors || ret == err::configreader::notFound)
		return ret;
	else
	{
		std::size_t size = errors.size();
		
		if (isEmpty("modules", "socketengine", &vals))
			errors.push_back("Missing required value: Modules::socketengine");
		if (isEmpty("modules", "ircdprotocol", &vals))
			errors.push_back("Missing required value: Modules::ircdprotocol");

		if (isEmpty("remoteserver", "address", &vals))
			errors.push_back("Missing required value: RemoteServer::address");
		if (isEmpty("remoteserver", "port", &vals))
			errors.push_back("Missing required value: RemoteServer::port");
		if (isEmpty("remoteserver", "password", &vals))
			errors.push_back("Missing required value: RemoteServer::password");

		if (isEmpty("servicesserver", "name", &vals))
			errors.push_back("Missing required value: ServicesServer::name");
		if (isEmpty("servicesserver", "desc", &vals))
			errors.push_back("Missing required value: ServicesServer::desc");
		if (isEmpty("servicesserver", "network", &vals))
			errors.push_back("Missing required value: ServicesServer::network");
		if (instance->ircdProtocol && instance->ircdProtocol->requireNumeric && isEmpty("servicesserver", "numeric", &vals))
			errors.push_back("Missing required value: ServicesServer::numeric");
		
		if (size != errors.size())
			return err::configreader::errors;
	}

	values = vals;
	return ret;
}


bool ConfigReader::isEmpty(const std::string block, const std::string key, std::map<std::string, std::map<std::string, std::deque<String> > >* vals)
{
	if (!vals)
		vals = &values;

	std::map<std::string, std::map<std::string, std::deque<String> > >::iterator i = vals->find(block);
	
	if (i != vals->end())
	{
		std::map<std::string, std::deque<String> >::iterator j = i->second.find(key);
		
		if (j != i->second.end())
			return false;
	}

	return true;
}


void ConfigReader::getValue(String &in, const std::string block, const std::string key, String def, std::map<std::string, std::map<std::string, std::deque<String> > >* vals)
{
	if (!vals)
		vals = &values;
	
	std::map<std::string, std::map<std::string, std::deque<String> > >::iterator i = vals->find(block);
	
	if (i != vals->end())
	{
		std::map<std::string, std::deque<String> >::iterator j = i->second.find(key);
		
		if (j != i->second.end())
		{
			in = *(j->second.begin());
			return;
		}
	}
	
	in = def;
}


void ConfigReader::getValue(int &in, const std::string block, const std::string key, int def, std::map<std::string, std::map<std::string, std::deque<String> > >* vals)
{
	if (!vals)
		vals = &values;

	std::map<std::string, std::map<std::string, std::deque<String> > >::iterator i = vals->find(block);
	
	if (i != vals->end())
	{
		std::map<std::string, std::deque<String> >::iterator j = i->second.find(key);
		
		if (j != i->second.end())
		{
			utils::toInt<int>(in, *(j->second.begin()));
			return;
		}
	}
	
	in = def;
}


void ConfigReader::getValue(bool &in, const std::string block, const std::string key, bool def, std::map<std::string, std::map<std::string, std::deque<String> > >* vals)
{
	if (!vals)
		vals = &values;
		
	std::map<std::string, std::map<std::string, std::deque<String> > >::iterator i = vals->find(block);
	
	if (i != vals->end())
	{
		std::map<std::string, std::deque<String> >::iterator j = i->second.find(key);
		
		if (j != i->second.end())
		{
			if (*(j->second.begin()) == "yes" || *(j->second.begin()) == "true" || *(j->second.begin()) == "1")
				in = true;
			else if (*(j->second.begin()) == "no" || *(j->second.begin()) == "false" || *(j->second.begin()) == "0")
				in = false;
			else
				in = def;

			return;
		}
	}
	
	in = def;
}


void ConfigReader::getValues(std::deque<String> &in, const std::string block, const std::string key, std::map<std::string, std::map<std::string, std::deque<String> > >* vals)
{
	if (!vals)
		vals = &values;
	
	std::map<std::string, std::map<std::string, std::deque<String> > >::iterator i = vals->find(block);
	
	if (i != vals->end())
	{
		std::map<std::string, std::deque<String> >::iterator j = i->second.find(key);
		
		if (j != i->second.end())
			in = j->second;
	}
}

