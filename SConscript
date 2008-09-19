import os
Import('env')
srcFiles,headers,pyconfigs = SConscript('config/libfiles.py')

if len(srcFiles) != 0:
    if env['static']:
        lib = env.StaticLibrary('glue', srcFiles)
    else:
        lib = env.SharedLibrary('glue', srcFiles)
    env.Install(os.path.join(env.installDir, 'lib'), lib )
