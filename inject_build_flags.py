# inkkit build hook: force two defines on for the WHOLE build, including
# SdFat, which compiles as its own library so a define in this library.json's
# "flags" would not reach it.
#
# - USE_UTF8_LONG_NAMES=1: without it SdFat returns mangled names for any file
#   with a non-ASCII character. Inherited from the freeink-sdk SDCardManager
#   build hook, which this file is adapted from.
# - DESTRUCTOR_CLOSES_FILE=1: FsFile's destructor closes the file, which the
#   vendored HalStorage/HalFile layer relies on (CrossPoint Reader convention).
Import("env")

_DEFINES = (("USE_UTF8_LONG_NAMES", "1"), ("DESTRUCTOR_CLOSES_FILE", "1"))


def _append(e):
    defines = {d[0] if isinstance(d, tuple) else d for d in e.get("CPPDEFINES", [])}
    for define in _DEFINES:
        if define[0] not in defines:
            e.Append(CPPDEFINES=[define])


_append(env)
_append(DefaultEnvironment())
for lb in env.GetLibBuilders():
    _append(lb.env)
