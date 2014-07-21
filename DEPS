vars = {
  "chromium_git": "https://chromium.googlesource.com",
  "buildtools_revision": "59b93247766e1cdac6e482637ad493df38f7aeb7",
}

deps = {
  "src/buildtools":
    Var("chromium_git") + "/chromium/buildtools.git@" +
    Var("buildtools_revision"),
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
                "-s", "src/buildtools/win/gn.exe.sha1",
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
                "-s", "src/buildtools/mac/gn.sha1",
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
                "-s", "src/buildtools/linux32/gn.sha1",
    ],
  },
]
