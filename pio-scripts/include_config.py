# Small script to inject the user configs into the build process of some dependencies

Import('env')

def add_include(env, node, configs):
    return env.Object(
        node,
        CPPPATH=[env["PROJECT_INCLUDE_DIR"]] + env["CPPPATH"],
        CCFLAGS=env["CCFLAGS"] + [("-include", config) for config in configs]
    )

VUSB_CONFIGS = ["include/usbconfig.h"]
WS2812_CONFIGS = ["include/ws2812_config.h"]

env.AddBuildMiddleware(lambda env, node: add_include(env, node, VUSB_CONFIGS), "*/v-usb/*")
env.AddBuildMiddleware(lambda env, node: add_include(env, node, WS2812_CONFIGS), "*/light_ws2812/*")
