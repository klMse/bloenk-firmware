# Small script to inject the user configs into the build process of some dependencies

Import('env')

def add_include(env, node, configs):
    return env.Object(
        node,
        CPPPATH=[env["PROJECT_INCLUDE_DIR"]] + env["CPPPATH"],
        # May I familiarize you with -include :/
        # This is currently needed because light_ws2812 has a ws2812_config.h in the same
        # directory as the source files that include it. So we have to force the include and
        # use the same header guards.
        CCFLAGS=env["CCFLAGS"] + [("-include", config) for config in configs]
    )

VUSB_CONFIGS = ["include/usbconfig.h"]
WS2812_CONFIGS = ["include/ws2812_config.h"]

env.AddBuildMiddleware(lambda env, node: add_include(env, node, VUSB_CONFIGS), "*/v-usb/*")
env.AddBuildMiddleware(lambda env, node: add_include(env, node, WS2812_CONFIGS), "*/light_ws2812/*")
