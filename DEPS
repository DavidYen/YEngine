vars = {
  "buildtools_git": "https://chromium.googlesource.com/chromium/buildtools.git",
  "buildtools_revision": "cbc33b9c0a9d1bb913895a4319a742c504a2d541",

  "gflags_git": "https://github.com/gflags/gflags.git",
  "gflags_revision": "v2.2.1",

  "gmock_git": "https://github.com/DavidYen/googlemock.git",
  "gmock_revision": "9b1a944ec48b7f84ad2f4466ed3babd232a20db5",

  "gtest_git": "https://github.com/DavidYen/googletest.git",
  "gtest_revision": "1197daf3571161590dce2bc4879512ef7bc1ba67",

  "rapidjson_git": "https://github.com/miloyip/rapidjson.git",
  "rapidjson_revision": "v1.1.0",

  "flatbuffers_git": "https://github.com/google/flatbuffers.git",
  "flatbuffers_revision": "v1.7.1",

  "crunch_git": "https://github.com/BinomialLLC/crunch.git",
  "crunch_revision": "ea9b8d8c00c8329791256adafa8cf11e4e7942a2",

  "stb_git": "https://github.com/nothings/stb.git",
  "stb_revision": "9d9f75eb682dd98b34de08bb5c489c6c561c9fa6",
}

deps = {
  "yengine/buildtools":
    Var("buildtools_git") + "@" + Var("buildtools_revision"),
  "yengine/third_party/crunch":
    Var("crunch_git") + "@" + Var("crunch_revision"),
  "yengine/third_party/google/flatbuffers":
    Var("flatbuffers_git") + "@" + Var("flatbuffers_revision"),
  "yengine/third_party/google/gflags":
    Var("gflags_git") + "@" + Var("gflags_revision"),
  "yengine/third_party/google/gmock":
    Var("gmock_git") + "@" + Var("gmock_revision"),
  "yengine/third_party/google/gtest":
    Var("gtest_git") + "@" + Var("gtest_revision"),
  "yengine/third_party/rapidjson":
    Var("rapidjson_git") + "@" + Var("rapidjson_revision"),
  "yengine/third_party/stb":
    Var("stb_git") + "@" + Var("stb_revision"),
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
