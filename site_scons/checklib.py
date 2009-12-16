"""
Provides a function similar to CheckLib, but allows you to specify the
directory where we expect to find the library.  Also defines a
command-line option that lets the user override this directory.
"""

from SCons.Script import *

def CheckLibInPath(context, libname, library, call, header=""):
    AddOption('--with_%s' % libname,
              dest='%s_path' % libname,
              type='string',
              nargs=1,
              action='store',
              metavar='DIR',
              default='/usr',
              help='Location of %s library' % libname)

    context.Message("Checking for %s..." % libname)

    try:
        lastLIBS = context.env['LIBS']
    except KeyError:
        lastLIBS = []

    try:
        lastLIBPATH = context.env['LIBPATH']
    except KeyError:
        lastLIBPATH = []

    try:
        lastCPPPATH = context.env['CPPPATH']
    except KeyError:
        lastCPPPATH = []

    libpath = GetOption('%s_path' % libname)

    context.env.Append(LIBS=library,
                       LIBPATH="%s/lib" % libpath,
                       CPPPATH="%s/include" % libpath)

    ret = context.TryLink("""
%s

int
main(int argc, char **argv) {
    %s;
    return 0;
}
""" % (header, call), ".c")

    if not ret:
        context.env.Replace(LIBS=lastLIBS,
                            LIBPATH=lastLIBPATH,
                            CPPPATH=lastCPPPATH)

    context.Result(ret)
    return ret
