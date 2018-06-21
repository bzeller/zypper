/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/

#include <map>

#include <zypp/base/NamedValue.h>
#include <zypp/base/Exception.h>
#include <zypp/base/String.h>

#include "main.h"
#include "Command.h"

using namespace zypp;

// redefine _ gettext macro defined by ZYpp
#ifdef _
#undef _
#endif
#define _(MSG) ::gettext(MSG)

///////////////////////////////////////////////////////////////////
namespace
{
  static NamedValue<ZypperCommand::Command> & table()
  {
    static NamedValue<ZypperCommand::Command> _table;
    if ( _table.empty() )
    {
#define _t(C) _table( ZypperCommand::C )
      _t( NONE_e )		| "NONE"		| "none" | "";
      _t( SUBCOMMAND_e)		| "subcommand";

      _t( ADD_SERVICE_e )	| "addservice"		| "as" | "service-add" | "sa";
      _t( REMOVE_SERVICE_e )	| "removeservice"	| "rs" | "service-delete" | "sd";
      _t( MODIFY_SERVICE_e )	| "modifyservice"	| "ms";
      _t( LIST_SERVICES_e )	| "services"		| "ls" | "service-list" | "sl";
      _t( REFRESH_SERVICES_e )	| "refresh-services"	| "refs";

      _t( ADD_REPO_e )		| "addrepo" 		| "ar";
      _t( REMOVE_REPO_e )	| "removerepo"		| "rr";
      _t( RENAME_REPO_e )	| "renamerepo"		| "nr";
      _t( MODIFY_REPO_e )	| "modifyrepo"		| "mr";
      _t( LIST_REPOS_e )	| "repos"		| "lr" | "catalogs" | "ca";
      _t( REFRESH_e )		| "refresh"		| "ref";
      _t( CLEAN_e )		| "clean"		| "cc" | "clean-cache" | "you-clean-cache" | "yc";

      _t( INSTALL_e )		| "install"		| "in";
      _t( REMOVE_e )		| "remove"		| "rm";
      _t( SRC_INSTALL_e )	| "source-install"	| "si";
      _t( VERIFY_e )		| "verify"		| "ve";
      _t( INSTALL_NEW_RECOMMENDS_e )| "install-new-recommends" | "inr";

      _t( UPDATE_e )		| "update"		| "up";
      _t( LIST_UPDATES_e )	| "list-updates"	| "lu";
      _t( PATCH_e )		| "patch";
      _t( LIST_PATCHES_e )	| "list-patches"	| "lp";
      _t( PATCH_CHECK_e )	| "patch-check"		| "pchk";
      _t( DIST_UPGRADE_e )	| "dist-upgrade"	| "dup";

      _t( SEARCH_e )		| "search"		| "se";
      _t( INFO_e )		| "info"		| "if";
      _t( PACKAGES_e )		| "packages"		| "pa" | "pkg";
      _t( PATCHES_e )		| "patches"		| "pch";
      _t( PATTERNS_e )		| "patterns"		| "pt";
      _t( PRODUCTS_e )		| "products"		| "pd";

      _t( WHAT_PROVIDES_e )	| "what-provides"	| "wp";
      //_t( WHAT_REQUIRES_e )	| "what-requires"	| "wr";
      //_t( WHAT_CONFLICTS_e )	| "what-conflicts"	| "wc";

      _t( ADD_LOCK_e )		| "addlock"		| "al" | "lock-add" | "la";
      _t( REMOVE_LOCK_e )	| "removelock"		| "rl" | "lock-delete" | "ld";
      _t( LIST_LOCKS_e )	| "locks"		| "ll" | "lock-list";
      _t( CLEAN_LOCKS_e )	| "cleanlocks"		| "cl" | "lock-clean";

      _t( TARGET_OS_e )		| "targetos"		| "tos";
      _t( VERSION_CMP_e )	| "versioncmp"		| "vcmp";
      _t( LICENSES_e )		| "licenses";
      _t( PS_e )		| "ps";
      _t( DOWNLOAD_e )		| "download";
      _t( SOURCE_DOWNLOAD_e )	| "source-download";

      _t( HELP_e )		| "help"		| "?";
      _t( SHELL_e )		| "shell"		| "sh";
      _t( SHELL_QUIT_e )	| "quit"		| "exit" | "\004";
      _t( MOO_e )		| "moo";

      _t( CONFIGTEST_e)		|  "configtest";

      _t( RUG_PATCH_INFO_e )	| "patch-info";
      _t( RUG_PATTERN_INFO_e )	| "pattern-info";
      _t( RUG_PRODUCT_INFO_e )	| "product-info";
      _t( RUG_SERVICE_TYPES_e )	| "service-types"	| "st";
      _t( RUG_LIST_RESOLVABLES_e )| "list-resolvables";	// "lr" CONFLICT with repos
      _t( RUG_MOUNT_e )		| "mount";
      //_t( RUG_INFO_PROVIDES_e )| "info-provides"	| "ip";
      //_t( RUG_INFO_CONFLICTS_e )| "info-requirements"	| "ir";
      //_t( RUG_INFO_OBSOLETES_e )| "info-conflicts"	| "ic";
      //_t( RUG_INFO_REQUIREMENTS_e )| "info-obsoletes"	| "io";
      _t( RUG_PATCH_SEARCH_e )	| "patch-search" | "pse";
      _t( RUG_PING_e )		| "ping";

      _t( LOCALES_e )              | "locales" | "lloc";
      _t( ADD_LOCALE_e ) | "addlocale" | "aloc";
      _t( REMOVE_LOCALE_e ) | "remove-locale" | "rloc";
#undef _t
    }
    return _table;
  }
} // namespace
///////////////////////////////////////////////////////////////////

#define DEF_ZYPPER_COMMAND(C) const ZypperCommand ZypperCommand::C( ZypperCommand::C##_e )
DEF_ZYPPER_COMMAND( NONE );
DEF_ZYPPER_COMMAND( SUBCOMMAND );

DEF_ZYPPER_COMMAND( ADD_SERVICE );
DEF_ZYPPER_COMMAND( REMOVE_SERVICE );
DEF_ZYPPER_COMMAND( MODIFY_SERVICE );
DEF_ZYPPER_COMMAND( LIST_SERVICES );
DEF_ZYPPER_COMMAND( REFRESH_SERVICES );

DEF_ZYPPER_COMMAND( ADD_REPO );
DEF_ZYPPER_COMMAND( REMOVE_REPO );
DEF_ZYPPER_COMMAND( RENAME_REPO );
DEF_ZYPPER_COMMAND( MODIFY_REPO );
DEF_ZYPPER_COMMAND( LIST_REPOS );
DEF_ZYPPER_COMMAND( REFRESH );
DEF_ZYPPER_COMMAND( CLEAN );

DEF_ZYPPER_COMMAND( INSTALL );
DEF_ZYPPER_COMMAND( REMOVE );
DEF_ZYPPER_COMMAND( SRC_INSTALL );
DEF_ZYPPER_COMMAND( VERIFY );
DEF_ZYPPER_COMMAND( INSTALL_NEW_RECOMMENDS );

DEF_ZYPPER_COMMAND( UPDATE );
DEF_ZYPPER_COMMAND( LIST_UPDATES );
DEF_ZYPPER_COMMAND( PATCH );
DEF_ZYPPER_COMMAND( LIST_PATCHES );
DEF_ZYPPER_COMMAND( PATCH_CHECK );
DEF_ZYPPER_COMMAND( DIST_UPGRADE );

DEF_ZYPPER_COMMAND( SEARCH );
DEF_ZYPPER_COMMAND( INFO );
DEF_ZYPPER_COMMAND( PACKAGES );
DEF_ZYPPER_COMMAND( PATCHES );
DEF_ZYPPER_COMMAND( PATTERNS );
DEF_ZYPPER_COMMAND( PRODUCTS );

DEF_ZYPPER_COMMAND( WHAT_PROVIDES );
//DEF_ZYPPER_COMMAND( WHAT_REQUIRES );
//DEF_ZYPPER_COMMAND( WHAT_CONFLICTS );

DEF_ZYPPER_COMMAND( ADD_LOCK );
DEF_ZYPPER_COMMAND( REMOVE_LOCK );
DEF_ZYPPER_COMMAND( LIST_LOCKS );
DEF_ZYPPER_COMMAND( CLEAN_LOCKS );

DEF_ZYPPER_COMMAND( TARGET_OS );
DEF_ZYPPER_COMMAND( VERSION_CMP );
DEF_ZYPPER_COMMAND( LICENSES );
DEF_ZYPPER_COMMAND( PS );
DEF_ZYPPER_COMMAND( DOWNLOAD );
DEF_ZYPPER_COMMAND( SOURCE_DOWNLOAD );

DEF_ZYPPER_COMMAND( HELP );
DEF_ZYPPER_COMMAND( SHELL );
DEF_ZYPPER_COMMAND( SHELL_QUIT );
DEF_ZYPPER_COMMAND( MOO );

DEF_ZYPPER_COMMAND( RUG_PATCH_INFO );
DEF_ZYPPER_COMMAND( RUG_PATTERN_INFO );
DEF_ZYPPER_COMMAND( RUG_PRODUCT_INFO );
DEF_ZYPPER_COMMAND( RUG_SERVICE_TYPES );
DEF_ZYPPER_COMMAND( RUG_LIST_RESOLVABLES );
DEF_ZYPPER_COMMAND( RUG_MOUNT );
//DEF_ZYPPER_COMMAND( RUG_INFO_PROVIDES );
//DEF_ZYPPER_COMMAND( RUG_INFO_CONFLICTS );
//DEF_ZYPPER_COMMAND( RUG_INFO_OBSOLETES );
//DEF_ZYPPER_COMMAND( RUG_INFO_REQUIREMENTS );
DEF_ZYPPER_COMMAND( RUG_PATCH_SEARCH );
DEF_ZYPPER_COMMAND( RUG_PING );
DEF_ZYPPER_COMMAND( LOCALES );
DEF_ZYPPER_COMMAND( ADD_LOCALE );
DEF_ZYPPER_COMMAND( REMOVE_LOCALE );

#undef DEF_ZYPPER_COMMAND
///////////////////////////////////////////////////////////////////

ZypperCommand::ZypperCommand( const std::string & strval_r )
  : _command( parse(strval_r ) )
{}

ZypperCommand::Command ZypperCommand::parse( const std::string & strval_r ) const
{
  ZypperCommand::Command cmd = SUBCOMMAND_e;	// Exception if not true
  if ( ! table().getValue( strval_r, cmd ) )
  {
    bool isSubcommand( const std::string & strval_r );	// in subcommand.cc

    if ( ! isSubcommand( strval_r ) )
    {
      ZYPP_THROW( Exception( str::form(_("Unknown command '%s'"), strval_r.c_str() ) ) );
    }
  }
  return cmd;
}

const std::string & ZypperCommand::asString() const
{ return table().getName( _command ); }
