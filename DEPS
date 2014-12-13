vars = {
  "buildtools_git": "https://chromium.googlesource.com/chromium/buildtools.git",
  "buildtools_revision": "05dd6a24723170d7c6ff35b537ee02947f619891",

  "gflags_git": "https://code.google.com/p/gflags.git",
  "gflags_revision": "05b155ff59114735ec8cd089f669c4c3d8f59029",

  "rapidjson_git": "https://github.com/miloyip/rapidjson.git",
  "rapidjson_revision": "879def80f2e466cdf4c86dc7e53ea2dd4cafaea0",

  "flatbuffers_git": "https://github.com/google/flatbuffers.git",
  "flatbuffers_revision": "f177663ce7b971886a22177b21912f351e6a5e29",
}

deps = {
  "yengine/buildtools":
    Var("buildtools_git") + "@" + Var("buildtools_revision"),
  "yengine/third_party/google/flatbuffers":
    Var("flatbuffers_git") + "@" + Var("flatbuffers_revision"),
  "yengine/third_party/google/gflags":
    Var("gflags_git") + "@" + Var("gflags_revision"),
  "yengine/third_party/rapidjson":
    Var("rapidjson_git") + "@" + Var("rapidjson_revision"),
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
