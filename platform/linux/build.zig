const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
    });

    mod.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/hooks.c"),
        .flags = &.{"-std=c99"},
    });

    mod.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/vector.c"),
        .flags = &.{"-std=c99"},
    });

    const lib = b.addLibrary(.{
        .name = "PhotonCore",
        .root_module = mod,
        .linkage = .static,
    });

    b.installArtifact(lib);
}
