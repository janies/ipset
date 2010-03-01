# -*- coding: utf-8 -*-

from pkgconfig import *
import sys

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
                ".scons.vars",
                ".scons.vars.buddy",
                ".scons.vars.check",
                "config.log",
               ])

# Only run the configuration steps if we're actually going to build
# something.

if not GetOption('clean') and not GetOption('help'):
    conf = root_env.Configure(custom_tests=
                              {
                               'CheckPKGConfig' : CheckPKGConfig,
                               'CheckPKG' : CheckPKG,
                              })

    if not conf.CheckPKGConfig('0.15.0'):
        print 'pkg-config >= 0.15.0 not found.'
        Exit(0)

    if not conf.CheckCC():
        print "!! Your compiler and/or environment is not properly configured."
        Exit(0)

    if not conf.CheckPKG("cudd"):
        print "!! Cannot find the buddy library."
        Exit(0)

    if not conf.CheckPKG("check"):
        print "!! Cannot find the check library."
        Exit(0)

    root_env = conf.Finish()
    root_env.ParseConfig("pkg-config --cflags --libs cudd")
    root_env.ParseConfig("pkg-config --cflags --libs check")

# Include the subdirectory SConscripts

Export('root_env')
SConscript([
            'include/SConscript',
            'src/SConscript',
            'tests/SConscript',
           ])
