from SCons.Util import WhereIs
import os
import string
import sys

# Courtesy of Ben Scott
def ParseConfig(env, command, options):
    "Parses xxx-config style output for compilation directives"

    # Run the command
    where = WhereIs(command)
    if not where:
        print '%s not found in PATH' % command
        sys.exit(-1)
    print "Found " + str(command) + ": " + where
    cmd = where + ' ' + string.join(options)
    params = string.split(os.popen(cmd).read())

    # Parse its output
    for arg in params:
        switch = arg[0:1]
        option = arg[1:2]
        if switch == '-':
            if option == 'I':
                env.Append(CPPPATH = [arg[2:]])
            elif option == 'L':
                env.Append(LIBPATH = [arg[2:]])
            elif option == 'l':
                env.Append(LIBS = [arg[2:]])
            elif arg[0:11] == '-Wl,-rpath,':
                env.Append(LINKFLAGS = [arg])
            else:
                env.Append(CXXFLAGS = [arg])
        else:
            # Must be a static library, add it to the libs
            env.Append(LIBS = [arg])
