/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/

#ifndef LOCALES_H
#define LOCALES_H


#include "Zypper.h"

extern ZYpp::Ptr God;

void listLocales( Zypper & zypper, const std::vector<std::string> &localeArgs, bool showAll );

void localePackages( Zypper & zypper, const std::vector<std::string> &localeArgs, bool showAll );

void addLocales( Zypper & zypper_r, const std::vector<std::string> &localeArgs_r );

void removeLocales( Zypper & zypper, const std::vector<std::string> &localeArgs );


#endif
