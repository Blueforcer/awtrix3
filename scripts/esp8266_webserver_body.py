# ESP8266 only: keep one copy of an HTTP request body in RAM, not two.
#
# The web server in Arduino core 3.1.2 (ESP8266WebServer/src/Parsing-impl.h) reads a POST
# body into a local String that grows as the data arrives, then copies it into the "plain"
# argument. A JSON notification therefore needs its size twice, in pieces a heap with
# about 3 KB in one block (MQTT and Home Assistant on) often doesn't have.
#
# This script patches a copy of the library in the build folder (the installed framework
# isn't touched) and puts that copy first on the include path. The library is header-only
# (templates), so every file that includes it picks up the patch:
#   - reserve the body's full size once, before reading
#   - move the body into the "plain" argument instead of copying it
# Taken from PixelIt (pixelit-project/PixelIt, extra_script.py).
from pathlib import Path
import shutil

Import("env")

if env.PioPlatform().name == "espressif8266":
    framework = Path(env.PioPlatform().get_package_dir("framework-arduinoespressif8266"))
    original = framework / "libraries" / "ESP8266WebServer" / "src"
    patched = Path(env.subst("$BUILD_DIR")) / "patched_webserver"
    shutil.copytree(original, patched, dirs_exist_ok=True)

    parser = patched / "Parsing-impl.h"
    text = (original / "Parsing-impl.h").read_text()

    def replace_once(text, old, new):
        if text.count(old) != 1:
            raise RuntimeError(
                "esp8266_webserver_body.py: '%s' not found once in Parsing-impl.h; "
                "check the patch against this framework version" % old)
        return text.replace(old, new)

    text = replace_once(text, "  S2Stream dataStream(data);",
                        "  if (!data.reserve(maxLength + 1)) return false;\n  S2Stream dataStream(data);")
    text = replace_once(text, "arg.value = plainBuf;", "arg.value = std::move(plainBuf);")
    # checked in ServerManager.cpp, so a build that picks up the original header fails
    text = "#define AWTRIX_WEBSERVER_BODY_PATCH 1\n" + text
    if not parser.exists() or parser.read_text() != text:
        parser.write_text(text)

    # CPPPATH for the project sources, CCFLAGS so libraries (lib/webserver) see it first too
    env.Prepend(CPPPATH=[str(patched)])
    env.Prepend(CCFLAGS=["-I" + str(patched)])
