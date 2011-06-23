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

#include "module.h"

Module::Module(void* h, const nstring::str ver, const nstring::str t, unsigned long f) :
	handle(h),
	version(ver),
	type(t),
	flags(f)
{ }

Module::~Module()
{ }

void Module::onChanCreate(nstring::str&, std::map<nstring::str, nstring::str>&)
{ }

void Module::onJoin(nstring::str&, nstring::str&)
{ }

void Module::onPart(nstring::str&, nstring::str&)
{ }

void Module::onCMode(nstring::str&, irc::modes&, irc::params&)
{ }

void Module::onTopic(nstring::str&, nstring::str&, nstring::str&)
{ }

void Module::onConnect(nstring::str&, nstring::str&, nstring::str&, nstring::str&, nstring::str&)
{ }

void Module::onQuit(nstring::str&)
{ }

void Module::onNick(nstring::str&, nstring::str&)
{ }

void Module::onUMode(nstring::str&, irc::modes&)
{ }

void Module::onHostChange(nstring::str&, nstring::str&)
{ }

void Module::onOper(nstring::str&, bool&)
{ }
