﻿/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/
#ifndef ZYPPFLAGS_H
#define ZYPPFLAGS_H

#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <exception>
#include <initializer_list>
#include <set>

#include <boost/optional.hpp>

#include "main.h" // for gettext macros

// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_ALIAS              _( "ALIAS" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_REPOSITORY         _( "ALIAS|#|URI" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_CATEGORY           _( "CATEGORY" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_DIR                _( "DIR" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_FILE               _( "FILE" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_FILE_repo          _( "FILE.repo" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_FORMAT             _( "FORMAT" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_INTEGER            _( "INTEGER" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_PATH               _( "PATH" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_SEVERITY           _( "SEVERITY" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_STRING             _( "STRING" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_TAG                _( "TAG" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_TYPE               _( "TYPE" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_URI                _( "URI" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_YYYY_MM_DD         _( "YYYY-MM-DD" )
// translator: Option argument like '--export <FILE.repo>'. Do do not translate lowercase wordparts
#define ARG_MODE               _( "MODE" )

namespace zypp {
namespace ZyppFlags {

  struct CommandOption;

  using DefValueFun  = std::function< boost::optional<std::string>( ) >;
  using PreWriteHook = std::function< bool ( const CommandOption &, const boost::optional<std::string> & ) >;
  using SetterFun    = std::function< void ( const CommandOption &, const boost::optional<std::string> & ) >;
  using PreWriteHook = std::function< bool ( const CommandOption &, const boost::optional<std::string> & ) >;
  using PostWriteHook = SetterFun;

  enum ArgFlags : int {
    NoArgument        = 0x00,
    RequiredArgument  = 0x01,
    OptionalArgument  = 0x02,
    ArgumentTypeMask  = 0x0F,

    Repeatable         = 0x10, // < the argument can be repeated
    Hidden             = 0x20, // < command is hidden in help
    Deprecated         = 0x40  // < the option is about to be removed in the future
  };

  /**
   * @class Value
   * Composite type to provide a generic way to write variables and get the default value for them.
   * This type should be only used directly when implementing a new argument type.
   */
  class Value {
  public:
    /**
     * \param defValue takes a functor that returns the default value for the option as string
     * \param setter takes a functor that writes a target variable based on the argument input
     * \param argHint Gives a indicaton what type of data is accepted by the argument
     */
    Value ( DefValueFun &&defValue, SetterFun &&setter, std::string argHint = std::string() );

    /**
     * Calls the setter functor, with either the given argument or the optional argument
     * if the \a in parameter is null. Additionally it checks if the argument was already seen
     * before and fails if that \a Repeatable flag is not set
     */
    void set( const CommandOption &opt, const boost::optional<std::string> in );

    /**
     * Calls the \sa never hook, the option was never given on CLI
     */
    void neverUsed ();

    /**
     * Returns the default value represented as string, or a empty
     * boost::optional if no default value is given
     */
    boost::optional<std::string> defaultValue ( ) const;

    /**
     * returns the hint for the input a command accepts,
     * used in the help
     */
    std::string argHint () const;


    bool wasSet () const;

    /**
     * Callback to call before writing the value
     */
    Value &before( PreWriteHook &&preWriteHook );

    /**
     * Callback to call after writing the value
     */
    Value &after ( PostWriteHook &&postWriteHook );

    /**
     * Callback to call after writing the value
     */
    Value &after ( std::function<void ()> &&postWriteHook );

    /**
     * Callback to call in case the option was never seen
     */
    Value &notSeen ( std::function<void ()> &&notFoundHook );

  private:
    bool _wasSet = false;
    DefValueFun _defaultVal;
    SetterFun _setter;
    std::string _argHint;
    std::vector<PreWriteHook>  _preWriteHook;
    std::vector<PostWriteHook> _postWriteHook;
    std::function<void ()> _notFoundHook;
  };

  struct CommandOption
  {
    CommandOption ( std::string &&name_r, char shortName_r, int flags_r, Value &&value_r, std::string &&help_r = std::string() );

    inline CommandOption &setPriority ( int prio ) {
      priority = prio;
      return *this;
    }

    inline CommandOption &setDependencies ( std::set<std::string> deps ) {
      dependencies = deps;
      return *this;
    }

    std::string optionHelp () const;
    std::string flagDesc   ( bool shortOptFirst = true ) const;

    static std::string nameStr( const std::string & name_r );	///< "--name" or empty string if empty
    std::string nameStr() const { return nameStr( name ); }

    static std::string shortNameStr( char shortName_r );	///< "-n" or empty string if 0
    std::string shortNameStr() const { return shortNameStr( shortName ); }

    std::string name;
    char  shortName;
    int flags;
    Value value;
    std::string help;
    int priority = 0;
    std::set<std::string> dependencies;
  };

  class ConflictingFlagsEntry : public std::vector< std::string >
  {
    public:
      ConflictingFlagsEntry ( std::initializer_list<std::string> in ) : std::vector<std::string>( std::move(in) ) {}
      ConflictingFlagsEntry ( std::vector< std::string > in ) : std::vector<std::string>( std::move(in) ) {}
      ConflictingFlagsEntry ( std::vector< std::string > &&in ) : std::vector<std::string>( std::move(in) ) {}
  };

  using ConflictingFlagsList = std::vector< ConflictingFlagsEntry >;

  struct  CommandGroup
  {
    /**
     * Creates a empty default group
     */
    CommandGroup ();

    /**
     * Creates the default command group with given options and conflicting flags
     */
    CommandGroup ( std::vector<CommandOption> &&options_r, ConflictingFlagsList &&conflictingOptions_r = ConflictingFlagsList() );

    /**
     * Creates the command group with given options, conflicting flags and the custom \a name_r
     */
    CommandGroup ( std::string &&name_r, std::vector<CommandOption> &&options_r, ConflictingFlagsList &&conflictingOptions_r = ConflictingFlagsList() );

    inline CommandGroup &operator<< ( CommandOption && opt ) {
      options.push_back( std::move(opt) );
      return *this;
    }

    inline CommandGroup &operator<< ( std::initializer_list<CommandOption> li ) {
      options.insert( options.end(), li );
      return *this;
    }

    std::string name; //< The name of the command group
    std::vector<CommandOption> options; //< The flags the command group supports
    ConflictingFlagsList conflictingOptions; //< A list of conflicting flag pairs, each element specifies two flags that can't be used together
  };

  /**
   * parseCLI normally fails instantly if a switch given is not a exact match
   * to one of the registered switches. Setting this to true it will only write a warning instead.
   */
  bool &onlyWarnOnAbbrevSwitches();

  /**
   * Parses the command line arguments based on \a options.
   * \returns The first index in argv that was not parsed
   * \throws ZyppFlagsException or any subtypes of it
   */
  int parseCLI (const int argc, char * const *argv, const std::vector<CommandGroup> &options);

  /**
   * Renders the \a options help string
   */
  void renderHelp ( const std::vector<CommandGroup> &options );

}}

#endif // ZYPPFLAGS_H
