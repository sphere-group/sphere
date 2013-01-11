import os

Import('base_env COMPILER BUILD_DIR')

env = base_env.Clone()
if env.Detect('unix2dos'):
    env['BUILDERS']['TextFile'] = Builder(
        action = 'unix2dos < $SOURCE > $TARGET')
else:
    env['BUILDERS']['TextFile'] = Builder(
        action = 'cat < $SOURCE > $TARGET')

# files that should be converted from unix format to DOS format as part
# of the build
files = ['changelog.txt',
         'gpl.txt',
         'index.txt',
         'legal.txt',
         'readme.txt']

files.extend(map(lambda n: 'build/' + n,
             ['aaulia_changelog.txt',
              'aaulia_readme.txt', 
              'build-cygwin.txt', 
              'build-linux.txt', 
              'build-linux-old.txt', 
              'build-msvc.txt', 
              'build-unix.txt', 
              'index.txt', 
              'release-howto.txt']))

files.extend(map(lambda n: 'development/' + n,
             ['api.txt',
              'ide_keyboard_shortcuts.txt',
              'index.txt',
              'keys.txt',
              'network.txt',
              'optimization.txt',
              'tutorial.txt']))

files.extend(map(lambda n: 'internal/' + n,
             ['coding-style.txt',
              'game.sgm.txt',
              'font.rfn.txt',
              'map.rmp.txt',
              'spriteset.rss.txt',
              'tileset.rts.txt',
              'windowstyle.rws.txt',
              'package.spk.txt',
              'videodriver-specs.txt']))

files.extend(map(lambda n: 'system_scripts/' + n,
             ['animation.txt',
              'audio.txt',
              'circles.txt',
              'clock.txt',
              'convert.txt',
              'fademessage.txt',
              'index.txt',
              'intro.txt',
              'layers.txt',
              'map_animation.txt',
              'menu.txt',
              'named_parameters.txt',
              'timer.txt',
              'wintro.txt']))

files.extend(map(lambda n: 'online/' + n,
             ['index.txt',
              'irc.txt',
              'links.txt']))

files.extend(map(lambda n: 'running/' + n,
             ['command_line.txt',
              'index.txt',
              'keyboard_shortcuts.txt',
              'linux_config.txt',
              'packages.txt',
              'wine.txt']))

# files that should be installed without unix->dos translation
binfiles = map(lambda n: 'contributed/' + n,
               ['WT_tutorial.doc',
                'sphere_doc.pdf',
                'sphere.chm'])

for f in files:
    env.TextFile(os.path.join(BUILD_DIR, 'docs', f), f)
for f in binfiles:
    env.InstallAs(os.path.join(BUILD_DIR, 'docs', f), f)
