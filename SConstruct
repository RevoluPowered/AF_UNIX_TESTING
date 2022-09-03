import os, platform

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = Environment()

# Define our options.
target_aliases = {
    "d": "debug",
    "r": "release",
}
opts.Add(EnumVariable("target", "Compilation target", "debug", ["debug", "release"], target_aliases))
opts.Add(BoolVariable("use_llvm", "Use the LLVM / Clang compiler", "no"))

# Platform options.
platform_array = ["", "windows", "linux", "macos"]
platform_aliases = {
    "osx": "macos",
    "darwin": "macos",
    "x11": "linux",
    "linuxbsd": "linux",
}
opts.Add(EnumVariable("platform", "Platform (operating system)", "", platform_array, platform_aliases))
opts.Add(EnumVariable("p", "Alias for 'platform'", "", platform_array, platform_aliases))

# CPU architecture options.
architecture_array = ["", "universal", "x86_64", "arm64", "rv64"]
architecture_aliases = {
    "x64": "x86_64",
    "amd64": "x86_64",
    "arm": "arm64",
    "armv8": "arm64",
    "arm64v8": "arm64",
    "aarch64": "arm64",
    "rv": "rv64",
    "riscv": "rv64",
    "riscv64": "rv64",
}
opts.Add(EnumVariable("arch", "CPU architecture", "", architecture_array, architecture_aliases))

# Updates the environment with the option variables.
opts.Update(env)

# Process platform arguments.
if env["p"] != "":
    env["platform"] = env["p"]

if env["platform"] == "":
    host_platform = platform.system().lower()
    if host_platform in platform_array:
        env["platform"] = host_platform
    elif host_platform in platform_aliases.keys():
        env["platform"] = platform_aliases[host_platform]
    else:
        print("Unsupported platform: " + host_platform)
        Exit()

env_platform = env["platform"]

# Process CPU architecture argument.
if env["arch"] == "":
    # No architecture specified. Default to universal if building for macOS,
    # otherwise default to the host architecture.
    if env_platform == "macos":
        env["arch"] = "universal"
    else:
        host_machine = platform.machine().lower()
        if host_machine in architecture_array:
            env["arch"] = host_machine
        elif host_machine in architecture_aliases.keys():
            env["arch"] = architecture_aliases[host_machine]
        else:
            print("Unsupported CPU architecture: " + host_machine)
            Exit()

env_arch = env["arch"]

# Check our platform specifics.
if env_platform == "macos":
    if not env["use_llvm"]:
        env["use_llvm"] = "yes"
    if env_arch not in ("universal", "x86_64", "arm64"):
        print("Only universal, x86_64, and arm64 are supported on macOS. Exiting.")
        Exit()

    env.Append(CXXFLAGS=["-std=c++11"])
    if env_arch == "universal":
        env.Append(CCFLAGS=["-arch", "x86_64", "-arch", "arm64"])
        env.Append(LINKFLAGS=["-arch", "x86_64", "-arch", "arm64"])
    else:
        env.Append(CCFLAGS=["-arch", env_arch])
        env.Append(LINKFLAGS=["-arch", env_arch])

    if env["target"] == "debug":
        env.Append(CCFLAGS=["-g", "-O2"])
    else:
        env.Append(CCFLAGS=["-g", "-O3"])
elif env_arch == "universal":
    print("The universal architecture is only supported on macOS. Exiting.")
    Exit()

elif env_platform == "linux":
    env.Append(LINKFLAGS=["-pthread"])
    env.Append(CXXFLAGS=["-std=c++11"])
    if env_arch == "x86_64":
        env.Append(CCFLAGS=["-m64", "-march=x86-64"])
        env.Append(LINKFLAGS=["-m64", "-march=x86-64"])
    elif env_arch == "arm64":
        env.Append(CCFLAGS=["-march=armv8-a"])
        env.Append(LINKFLAGS=["-march=armv8-a"])
    elif env_arch == "rv64":
        env.Append(CCFLAGS=["-march=rv64gc"])
        env.Append(LINKFLAGS=["-march=rv64gc"])

    if env["target"] == "debug":
        env.Append(CCFLAGS=["-fPIC", "-g3", "-Og"])
    else:
        env.Append(CCFLAGS=["-fPIC", "-g", "-O3"])

elif env_platform == "windows":
    if env_arch != "x86_64":
        print("Only x86_64 is supported on Windows. Exiting.")
        Exit()

    # This makes sure to keep the session environment variables
    # on Windows, so that you can run scons in a Visual Studio prompt
    # and it will find all the required tools.
    env = Environment(ENV=os.environ)
    opts.Update(env)

    env.Append(LIBS=["ws2_32"])
    env.Append(CCFLAGS=["-DWIN32", "-D_WIN32", "-D_WINDOWS", "-W3", "-GR", "-D_CRT_SECURE_NO_WARNINGS"])
    if env["target"] == "debug":
        env.Append(CCFLAGS=["-EHsc", "-D_DEBUG", "-MDd"])
    else:
        env.Append(CCFLAGS=["-O2", "-EHsc", "-DNDEBUG", "-MD"])

# Process other arguments.
if env["use_llvm"]:
    env["CC"] = "clang"
    env["CXX"] = "clang++"

# We need to re-set arch and platform if we call opts.Update()
env["arch"] = env_arch
env["platform"] = env_platform
env["p"] = env_platform

suffix = "." + env_platform + "." + env_arch
env["OBJSUFFIX"] = suffix + env["OBJSUFFIX"]

print("Building for architecture " + env_arch + " on platform " + env_platform)

# Tweak this if you want to use different folders,
# or more folders, to store your source code in.
env.Append(CPPPATH=["./"])
sources = Glob("*/*.cpp")

Help(opts.GenerateHelpText(env))
env.Program("build/ipc_client", Glob("src/*.cpp") + ["tests/ipc_client.cpp"])
env.Program("build/ipc_server", Glob("src/*.cpp") + ["tests/ipc_server.cpp"])
env.Program("build/ipc_tests", Glob("src/*.cpp") + ["tests/ipc_tests.cpp"])
