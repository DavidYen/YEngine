vars = {
  "chromium_git": "https://chromium.googlesource.com",
  "buildtools_revision": "f79f8ef8691cb0c91ecf36392f6defabd824af60",
  "gflags_git": "https://code.google.com/p/gflags",
  "gflags_revision": "05b155ff59114735ec8cd089f669c4c3d8f59029",
}

deps = {
  "yengine/buildtools":
    Var("chromium_git") + "/chromium/buildtools.git@" +
    Var("buildtools_revision"),
  "yengine/third_party/google/gflags":
    Var("gflags_git") + ".git@" + Var("gflags_revision"),
}

hooks = [
  # Pull GN binaries. This needs to be before running GYP below.
  {
    "name": "gn_win",
    "pattern": ".",
    "action": [ "download_from_google_storage",
                "--no_resume",
                "--platform=win32",
                "--no_auth",
                "--bucket", "chromium-gn",
                "-s", "yengine/buildtools/win/gn.exe.sha1",
    ],
  },
  {
    "name": "gn_mac",
    "pattern": ".",
    "action": [ "download_from_google_storage",
                "--no_resume",
                "--platform=darwin",
                "--no_auth",
                "--bucket", "chromium-gn",
                "-s", "yengine/buildtools/mac/gn.sha1",
    ],
  },
  {
    "name": "gn_linux32",
    "pattern": ".",
    "action": [ "download_from_google_storage",
                "--no_resume",
                "--platform=linux*",
                "--no_auth",
                "--bucket", "chromium-gn",
                "-s", "yengine/buildtools/linux32/gn.sha1",
    ],
  },

  # Generate Ninja files using GN
  {
    "name": "generate_ninja",
    "pattern": ".",
    "action": [ "python",
                "yengine/generate_ninja.py",
    ],
  },
]
