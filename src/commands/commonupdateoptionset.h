/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/
#ifndef ZYPPER_COMMANDS_COMMONUPDATEOPTIONSET_INCLUDED
#define ZYPPER_COMMANDS_COMMONUPDATEOPTIONSET_INCLUDED

#include "basecommand.h"

class CommonUpdateOptionSet : public BaseCommandOptionSet
{
public:
 using BaseCommandOptionSet::BaseCommandOptionSet;

  // BaseCommandOptionSet interface
public:
  std::vector<zypp::ZyppFlags::CommandGroup> options() override;
  void reset() override;
};

#endif
