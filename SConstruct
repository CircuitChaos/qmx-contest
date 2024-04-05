env = Environment()
env['CCFLAGS']  = '-Wall -Wextra -std=c++17 -O2'
env['CPPPATH']  = 'src'
env['LIBS']	= ['asound']

env.VariantDir('build', 'src', duplicate = 0)
qmxcontest = env.Program('build/qmx-contest', Glob('build/*.cpp'))

env.Install('/usr/local/bin', qmxcontest)
env.Alias('install', '/usr/local/bin')
