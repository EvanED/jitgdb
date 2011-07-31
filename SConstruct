env = Environment()

env.AppendUnique(CCFLAGS=['-g', '-W', '-Wall'])

Export('env')

lib = env.SConscript('src/SConscript', variant_dir = '.build/lib')
Install('lib', lib)

example = env.SConscript('example/SConscript', variant_dir = '.build/example')
Install('.', example)
