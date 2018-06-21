/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <iterator>
#include <list>

#include <zypp/ZYpp.h>
#include <zypp/base/Logger.h>
#include <zypp/base/IOStream.h>
#include <zypp/base/String.h>
#include <zypp/base/Flags.h>
#include <zypp/ui/Selectable.h>
#include <zypp/base/Regex.h>
#include <zypp/sat/LocaleSupport.h>

#include "output/Out.h"
#include "main.h"
#include "getopt.h"
#include "Table.h"
#include "utils/messages.h"
#include "utils/misc.h" // for xml_encode
#include "locales.h"
#include "solve-commit.h"

static std::string isRequestedLocale( zypp::Locale lang )
{
    if ( God->pool().isRequestedLocale( lang ) )
      return _("Yes");
    else
      return _("No");
}

static void printLocaleList( Zypper & zypper, const zypp::LocaleSet &locales )
{
  Table tbl;
  // header
  TableHeader th;

  // translators: header of table column - the language code, e.g. en_US
  th << _("Code");
  // translators: header of table column - the language, e.g. English (United States)
  th << _("Language");
  // translators: header of table column - is the language requested? (Yes/No)
  th << _("Requested");

  tbl << th;

  for_( it, locales.begin(), locales.end() )
  {
    TableRow tr(3);

    tr << (*it).code();
    tr << (*it).name();
    tr << isRequestedLocale(*it);
    tbl << tr;
  }

  tbl.sort(1);

  cout << tbl;
}

static void printXmlLocaleList( Zypper & zypper, const zypp::LocaleSet &locales )
{

}

static void printLocalePackages( Zypper & zypper, const zypp::sat::LocaleSupport & myLocale )
{
  Table tbl;
  TableHeader th;

  // translators: header of table column - S is for 'Status' (package installed or not)
  th << _("S");
  // translators: header of table column - the name of the package
  th << _("Name");
  // translators: header of table column - the package summary
  th << _("Description");

  tbl << th;

  for_( it, myLocale.selectableBegin(), myLocale.selectableEnd() )
  {
    TableRow tr(3);
    zypp::ui::Status status = (*it)->status();

    if ( status == zypp::ui::S_KeepInstalled || status == zypp::ui::S_Protected )
      tr << "i";
    else
      tr << " ";
    tr << (*it)->name();
    tr << (*it)->theObj()->summary();

    tbl << tr;
  }

  tbl.sort(1);

  cout << tbl;
}

static zypp::LocaleSet relevantLocales( const std::vector<std::string> &localeArgs, bool glob )
{
  const zypp::LocaleSet & availableLocales( God->pool().getAvailableLocales() );

  zypp::LocaleSet resultSet;

  for_( it, availableLocales.begin(), availableLocales.end() )
  {
    if ( localeArgs.empty() )
    {
      // without argument take only requested locales
      if ( God->pool().isRequestedLocale(*it) )
      {
        resultSet.insert(*it);
      }
    }
    else
    {
      // take given locales (if available)
      for_( loc, localeArgs.begin(), localeArgs.end() )
      {
        if ( glob )
        {
          std::string reg = str::form( "^%s.*", (*loc).c_str() );
          if ( str::regex_match( (*it).code().c_str(), reg) )
          {
            resultSet.insert(*it);
          }
        }
        else if ( *loc == (*it).code().c_str() )
        {
          resultSet.insert(*it);
        }
      }
    }
  }

  return resultSet;
}

void listLocales( Zypper & zypper, const std::vector<std::string> &localeArgs, bool showAll )
{
  zypp::LocaleSet locales;

  if ( showAll )
    locales = God->pool().getAvailableLocales();
  else
    locales = relevantLocales( localeArgs, true );

  // print xml output
  if ( zypper.out().type() == Out::TYPE_XML )
    printXmlLocaleList(zypper, locales);
  else
    printLocaleList( zypper, locales );
}

void localePackages( Zypper &zypper, const std::vector<std::string> &localeArgs, bool showAll )
{
   zypp::LocaleSet locales;

   if ( showAll )
    locales = God->pool().getAvailableLocales();
  else
    locales = relevantLocales( localeArgs, true );

  for_( it, locales.begin(), locales.end() )
  {
    const zypp::sat::LocaleSupport & myLocale(*it);
    cout << endl;
    zypper.out().info( str::form( _("Packages for %s (locale '%s'):"),
                                  (*it).name().c_str(), (*it).code().c_str() ) );
    cout << endl;
    printLocalePackages( zypper, myLocale );
  }
}

void addLocales( Zypper &zypper, const std::vector<std::string> &localeArgs_r )
{
  const zypp::LocaleSet locales = relevantLocales( localeArgs_r, false );

  for_( it, locales.begin(), locales.end() ) {
    bool success = false;

    if ( !God->pool().isRequestedLocale(*it) ) {
      success = God->pool().addRequestedLocale( *it );
      if ( success ) {
        zypper.out().info( str::form( _("Added locale: %s"), (*it).code().c_str() ) );
      } else {
        zypper.out().error( str::form( _("ERROR: cannot add %s"), (*it).code().c_str() ) );
      }
    } else {
      zypper.out().info( str::form( _(" %s is already requested."), (*it).code().c_str() ) );
    }
  }

  if ( copts.count("packages") ) {
    solve_and_commit( zypper );
  } else {
    God->commit( ZYppCommitPolicy() );
  }
}

void removeLocales( Zypper &zypper, const std::vector<std::string> &localeArgs )
{
  for_( it, localeArgs.begin(), localeArgs.end() ) {
    bool success = false;

    zypp::Locale loc( *it );
    if ( God->pool().isRequestedLocale( loc ) ) {
      success = God->pool().eraseRequestedLocale( loc );
      if ( success ) {
        zypper.out().info( str::form( _("Removed locale: %s"), (*it).c_str() ) );
      } else {
        zypper.out().error( str::form( _("ERROR: cannot remove %s"), (*it).c_str() ) ) ;
      }
    } else {
      zypper.out().info( str::form( _("%s was not requested."), (*it).c_str() ) );
    }
  }

  if ( copts.count("packages") ) {
    solve_and_commit( zypper );
  } else {
    God->commit( ZYppCommitPolicy() );
  }
}
