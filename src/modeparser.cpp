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

#include "modeparser.h"
#include "instance.h"
#include "ircdprotocol.h"
#include "channel.h"
#include "user.h"
#include "utils.h"

/**
 ModeParser::ModeParser

 constructor
*/
ModeParser::ModeParser()
{ }

/**
 ModeParser::~ModeParser

 destructor
*/
ModeParser::~ModeParser()
{ }

/**
 ModeParser::checkModes

 strip all invalid modes from string
*/
void ModeParser::checkModes(nstring::str &modes)
{
    std::vector<nstring::str> parts;

    utils::explode(" ", modes, parts);
    modes.clear();

    for (nstring::str::iterator is = parts.at(0).begin(); is != parts.at(0).end(); ++is)
    {
        if (*is == '+' || *is == '-')
            continue;

        // this isn't a valid mode..
        if (instance->ircdProtocol->modes.find(*is) == nstring::str::npos)
            parts.at(0).erase(is);
    }

    // if it is only one char long, and equal to -/+, empty it
    if (parts.at(0).length() == 1 && (parts.at(0) == "-" || parts.at(0) == "+"))
        parts.at(0).clear();

    // if it isn't empty and the first character isnt a -/+ add a + to it
    if (!parts.at(0).empty() && (parts.at(0).at(0) != '+' && parts.at(0).at(0) != '-'))
        parts.at(0).insert(0, "+");

    // re-create the string
    for (std::vector<nstring::str>::iterator i = parts.begin(); i != parts.end(); ++i)
    {
        modes += *i;
        modes += ' ';
    }
    modes.erase((modes.end() - 1));
}

/**
 ModeParser::sortModes

 sort the modes into containers
*/
void ModeParser::sortModes(nstring::str &modes, irc::modes &mc, irc::params &pc, bool forceCheck = true)
{
    std::vector<nstring::str> params;
    char* modeType = (char*)"";
    int x = 0;

    if (modes.at(0) != '+' || modes.at(0) != '-')
		modes.insert(modes.begin(), '+');
	// if the first char isnt +/-

    // if the string contains a space, theres params, split it
    if (modes.find(' ') != nstring::str::npos)
    {
        utils::explode(" ", modes, params);

        // new modes string
        modes = params.at(0);

        // remove the first param, as it's now in modes
        params.erase(params.begin());
    }

    // setup these
    mc["plus"] = "";
    mc["minus"] = "";

    // loop through our mode string
    for (nstring::str::iterator i = modes.begin(); i != modes.end(); ++i)
    {
        // what's the mode type?
        if (*i == '+')
        {
            modeType = (char*)"plus";
            continue;
        }
        else if (*i == '-')
        {
            modeType = (char*)"minus";
            continue;
        }

        if (modeType == NULL)
            continue;

		// is the mode valid?
        if (instance->ircdProtocol->modes.find(*i) == nstring::str::npos && forceCheck)
            continue;

        // the mode is valid, is it a mode without params?
        if (!forceCheck || instance->ircdProtocol->modesWithParams.find(*i) == nstring::str::npos)
        {
            mc[modeType] += utils::toStr(*i);
            continue;
        }
        
		if ((instance->ircdProtocol->statusModes.find(*i) == nstring::str::npos && instance->ircdProtocol->restrictModes.find(*i) == nstring::str::npos) && modeType == (char*)"minus")
		{
			mc[modeType] += utils::toStr(*i);
			continue;
		}
        
		nstring::str xParam = params.at(x).c_str();
		utils::filter(xParam);

		irc::params::iterator it = pc.find(xParam);
		if (it != pc.end())
		{
			// the mode is set, but it's being unset, so we remove it accordingly
			if (pc[xParam]["plus"].find(*i) != nstring::str::npos && modeType == (char*)"minus")
			{
				pc[xParam][modeType].erase(pc[xParam]["plus"].find(*i));
				++x;
				continue;
			}
			// the mode is already there, skip
			if (pc[xParam][modeType].find(*i) != nstring::str::npos)
			{
				++x;
				continue;
			}

			pc[xParam][modeType] += utils::toStr(*i);
			++x;
		}
		else
		{
			pc[xParam][modeType] += utils::toStr(*i);
			++x;
		}
    }
}

/**
 ModeParser::saveModes

 save the modes to channel records, etc
*/
void ModeParser::saveModes(Channel* chan, irc::modes &mc, irc::params &pc)
{
	nstring::str nstr;
    std::vector<nstring::str> parts;
    int pos;

    // do we have any plus modes?
    if (!mc["plus"].empty())
    {
        for (nstring::str::iterator is = mc["plus"].begin(); is != mc["plus"].end(); ++is)
        {
            if (chan->modes.find(*is) != nstring::str::npos)
                continue;
            else
                chan->modes += *is;
        }
    }
	
	// do we have any minus modes??
    if (!mc["minus"].empty())
    {
        for (nstring::str::iterator is = mc["minus"].begin(); is != mc["minus"].end(); ++is)
        {
            utils::explode(" ", chan->modes, parts);

            if (parts.empty())
                parts[0] = "";

            if (instance->ircdProtocol->modesWithParams.find(*is) != nstring::str::npos)
            {
                // a parameter mode, being unset, without a parameter, confusing. maybe?
                nstr.clear();

                for (nstring::str::iterator istr = parts.at(0).begin(); istr != parts.at(0).end(); ++istr)
                    nstr += utils::toStr(*istr);

                // remove the parameter
                pos = (int) nstr.find(*is);
                parts.erase(parts.begin() + (pos + 1));

                // remove the mode
                pos = (int) parts.at(0).find(*is);
                parts.at(0).erase(pos, 1);

                // save the changes
                chan->modes.clear();

                for (std::vector<nstring::str>::iterator i = parts.begin(); i != parts.end(); ++i)
                {
                    chan->modes += *i;
                    chan->modes += ' ';
                }
                chan->modes.erase((chan->modes.end() - 1));
            }
            else
            {
                // normal mode being unset
                if (chan->modes.find(*is) != nstring::str::npos)
                {
                    // remove the mode
                    pos = (int) chan->modes.find(*is);
                    chan->modes.erase(pos, 1);
                }
            }
        }
    }

    // handle modes with params
    for (irc::params::iterator pit = pc.begin(); pit != pc.end(); ++pit)
    {
        nstring::str pFirst = pit->first.c_str();
        utils::filter(pFirst);

        // plus modes?
        if (!pit->second["plus"].empty())
        {
            for (nstring::str::iterator is = pit->second["plus"].begin(); is != pit->second["plus"].end(); ++is)
            {
                // are we dealing with a status mode, eg (qaohv) (let the ircd determine this)
                if (instance->ircdProtocol->statusModes.find(*is) != nstring::str::npos)
                {
                    // we check if the user exists?
                    std::map<nstring::str, nstring::str>::iterator uit = chan->users.find(pit->first);

                    if (uit != chan->users.end())
                    {
                        // is the mode already set?
                        if (uit->second.find(*is) != nstring::str::npos)
                            continue;
                        else
                            uit->second += *is;
                    }

                    continue;
                }

                // or a restrict mode? eg (bIe) again we let the ircd determine this
                if (instance->ircdProtocol->restrictModes.find(*is) != nstring::str::npos)
                {
                    if (chan->oModes[pFirst].find(*is) != nstring::str::npos)
                        continue;
                    else
                        chan->oModes[pFirst] += *is;

                    continue;
                }

                // params
                parts.clear();
                utils::explode(" ", chan->modes, parts);

                if (parts.empty())
                    parts[0] = "";

                // add the mode and it's param
                if (parts.at(0).find(*is) != nstring::str::npos)
                    continue;
                else
                {
                    parts.at(0) += *is;
                    parts.push_back(pit->first);

                    // save the changes
                    chan->modes.clear();
                    for (std::vector<nstring::str>::iterator i = parts.begin(); i != parts.end(); ++i)
                    {
                        chan->modes += *i;
                        chan->modes += ' ';
                    }
                    chan->modes.erase((chan->modes.end() - 1));
                }
            }
        }

        // minus modes?
        if (!pit->second["minus"].empty())
        {
            for (nstring::str::iterator is = pit->second["minus"].begin(); is != pit->second["minus"].end(); ++is)
            {
                // are we dealing with a status mode, eg (qaohv) (let the ircd determine this)
                if (instance->ircdProtocol->statusModes.find(*is) != nstring::str::npos)
                {
                    // we check if the user exists?
                    std::map<nstring::str, nstring::str>::iterator uit = chan->users.find(pit->first);

                    if (uit != chan->users.end())
                    {
                        // is the mode set?
                        pos = uit->second.find(*is);
                        if ((unsigned int) pos != nstring::str::npos)
                        {
                            pos = (int) pos;
                            uit->second.erase(pos, 1);
                        }
                    }

                    continue;
                }

                // or a restrict mode? eg (bIe) again we let the ircd determine this
                if (instance->ircdProtocol->restrictModes.find(*is) != nstring::str::npos)
                {
                    // check if the record exists
                    std::map<nstring::str, nstring::str>::iterator uit = chan->oModes.find(pFirst);

                    pos = uit->second.find(*is);
                    if ((unsigned int) pos != nstring::str::npos)
                    {
                        pos = (int) pos;
                        uit->second.erase(pos, 1);
                    }

                    // we no longer want to keep it, if it's empty
                    if (uit->second.empty())
                        chan->oModes.erase(uit);

                    continue;
                }
                // we don't have to deal with other modes here, cause the block above does all that
            }
        }
    }
}

/**
 ModeParser::saveModes

 save the modes to user records
*/
void ModeParser::saveModes(User* user, irc::modes &mc)
{
	nstring::str nstr;
    std::vector<nstring::str> parts;
    int pos;

    // do we have any plus modes?
    if (!mc["plus"].empty())
    {
        for (nstring::str::iterator is = mc["plus"].begin(); is != mc["plus"].end(); ++is)
        {
            if (user->modes.find(*is) != nstring::str::npos)
                continue;
            else
                user->modes += *is;
        }
    }
	
	// do we have any minus modes??
    if (!mc["minus"].empty())
    {
        for (nstring::str::iterator is = mc["minus"].begin(); is != mc["minus"].end(); ++is)
        {
			// normal mode being unset
			if (user->modes.find(*is) != nstring::str::npos)
			{
				// remove the mode
				pos = (int) user->modes.find(*is);
				user->modes.erase(pos, 1);
			}
		}
	}
}
