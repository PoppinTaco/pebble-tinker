
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')

    ctx.pbl_program(
        source=ctx.path.ant_glob(['src/**/*.c']),
        target='pebble-app.elf',
        includes=[ctx.path.find_dir('ext/cl_util/include').abspath()],
        stlibpath=[ctx.path.find_dir('ext/cl_util/lib').abspath()],
        stlib=['cl_util'])

    ctx.pbl_bundle(elf='pebble-app.elf',
                   js=ctx.path.ant_glob('src/js/**/*.js'))
