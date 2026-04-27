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

    mod.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/search.c"),
        .flags = &.{"-std=c99"},
    });

    const lib = b.addLibrary(.{
        .name = "PhotonCore",
        .root_module = mod,
        .linkage = .static,
    });

    b.installFile("../../core/src/hooks.h", "include/hooks.h");
    b.installFile("../../core/src/vector.h", "include/vector.h");
    b.installFile("../../core/src/search.h", "include/search.h");

    b.installArtifact(lib);

    // Test runner
    const test_exe = b.addExecutable(.{
        .name = "photon_test",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
            .link_libcpp = true,
        }),
    });

    test_exe.root_module.addIncludePath(b.path("zig-out/include"));
    test_exe.root_module.addCSourceFile(.{
        .file = b.path("../../test/src/main.cpp"),
        .flags = &.{"-std=c++11"},
    });
    
    // Add core sources directly to test executable to bypass linking issues
    test_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/hooks.c"),
        .flags = &.{"-std=c99"},
    });
    test_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/vector.c"),
        .flags = &.{"-std=c99"},
    });
    test_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/search.c"),
        .flags = &.{"-std=c99"},
    });

    const run_test_cmd = b.addRunArtifact(test_exe);
    run_test_cmd.step.dependOn(b.getInstallStep());

    const test_step = b.step("run_test", "Run the project tests");
    test_step.dependOn(&run_test_cmd.step);

    // Example runner
    const example_exe = b.addExecutable(.{
        .name = "photon_example",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    example_exe.root_module.addIncludePath(b.path("zig-out/include"));
    example_exe.root_module.addCSourceFile(.{
        .file = b.path("../../examples/basic_usage.c"),
        .flags = &.{"-std=c99"},
    });

    // Add core sources directly to example executable
    example_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/hooks.c"),
        .flags = &.{"-std=c99"},
    });
    example_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/vector.c"),
        .flags = &.{"-std=c99"},
    });
    example_exe.root_module.addCSourceFile(.{
        .language = .c,
        .file = b.path("../../core/src/search.c"),
        .flags = &.{"-std=c99"},
    });

    const run_example_cmd = b.addRunArtifact(example_exe);
    run_example_cmd.step.dependOn(b.getInstallStep());

    const example_step = b.step("run_example", "Run the basic usage example");
    example_step.dependOn(&run_example_cmd.step);
}
