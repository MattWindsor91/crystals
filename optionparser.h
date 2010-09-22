/** @file    optionparser.h
 *  @author  Michael Walker
 *  @brief   Prototypes and declarations for the option parser
 */

#ifndef _OPTIONPARSER_H
#define _OPTIONPARSER_H

/* -- CONSTANTS -- */

enum
  {
    OPTION_PARAM_NONE   = 0, /**< Constant to show an option takes no parameters */
    OPTION_PARAM_INT    = 1, /**< Constant to show an option takes an integer parameter */
    OPTION_PARAM_FLOAT  = 2, /**< Constant to show an option takes a float parameter */
    OPTION_PARAM_STRING = 3  /**< Constant to show an option takes a string parameter */
  };

/* -- STRUCTURES -- */

/** The option structure
 *  
 *  This structure is used to define an option the engine takes.
 */

typedef struct
{
  char* longname;  /**< The long name of the option */
  char  shortname; /**< The short name of the option */
  int   paramtype; /**< The type of parameter it takes */
  void* variable;  /**< Pointer to the variable to set */
  char* helptext;  /**< Help text to be displayed */
} option;

/* -- PROTOTYPES -- */

/** Return whether an option is null or not
 *  
 *  @param check The option to check
 *  
 *  @return Either SUCCESS or FAILURE, defined in main.h
 */
int
is_null_option (option check);

/** Returns an option that matches the given short or long name
 *  
 *  @param shortname The one-character short name, or NULL to use the long name.
 *  @param longname  The long name of the option.
 *  @param options   The list of options to search.
 *
 *  @return Either the matching option, or the null option if not found.
 */
option
get_option (char shortname, char* longname, option options[]);

/** Prints help text and exits.
 *  
 *  @param options The list of options to print
 */
void
print_help_text (option options[]);

/** Prints the version text and exits.
 */
void
print_version_text (void);

/** Parse a set of options
 *  
 *  This parses at most 'argc' options from 'argv', storing the results in
 *  the variables pointed to in 'options'. The last entry of options should
 *  all be set to NULL, to mark the end of the structure.
 *  
 *  @param argc    The maximum number of arguments to parse
 *  @param argv    A list of strings to parse
 *  @param options A list of options to use
 *  
 *  @return Either SUCCESS or FAILURE, defined in main.h
 */
int
parse_options (int argc, char* argv[], option options[]);

#endif /* _OPTIONPARSER_H */
