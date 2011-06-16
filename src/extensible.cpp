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
// $Id: extensible.cpp 685 2009-01-28 22:47:17Z ankit $
//===================================================//

#include "extensible.h"

Extensible::Extensible() : metadata(new extensible::Metadata)
{ }

Extensible::~Extensible()
{
	delete metadata;
}

