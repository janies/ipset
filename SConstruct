# -*- coding: utf-8 -*-

from checklib import *

package = 'ipset'

vars = Variables('.scons.vars', ARGUMENTS)

vars.AddVariables(
    PathVariable("prefix", "Installation prefix", "/usr",
                 PathVariable.PathAccept),
    )


root_env = Environment(BINDIR = "$prefix/bin",
                       LIBDIR = "$prefix/lib",
                       INCLUDEDIR = "$prefix/include")

root_env.MergeFlags('-g -O2 -Wall -Werror')

vars.Update(root_env)
vars.Save(".scons.vars", root_env)

# An action that can clean up the scons temporary files.

root_env.Clean("clean-scons",
               [
                ".sconsign.dblite",
                ".sconf_temp",
                "config.log",
               ])

# Only run the configuration steps if we're actually going to build
# something.

if not GetOption('clean') and not GetOption('help'):
    conf = root_env.Configure(custom_tests=
                              {
                               'CheckLibInPath': CheckLibInPath,
                              })

    if not conf.CheckCC():
        print "!! Your compiler and/or environment is not properly configured."
        Exit(0)


    if not conf.CheckLibInPath("buddy",
                               library="libbdd",
                               call="bdd_init(1000, 1000)",
                               header="#include <bdd.h>"):
        print "!! Cannot find the buddy library."
        Exit(0)

    root_env = conf.Finish()

# Include the subdirectory SConscripts

Export('root_env')
SConscript([
            'include/SConscript',
            'src/SConscript',
           ])
