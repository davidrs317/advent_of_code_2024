// the language is neat but i'm definitely still confused
// it's honestly a lot more fun to write than Rust tbh
// I think I still need to get a hang of the optional syntax
// and learn more of the standard library.
// Sorting wasn't as simple to grok as C++'s std::sort
// but I understand that's part of the philosophy of the
// langugage and it's lean standard library
// I think I'm going to keep attempting problems
// in zig to see how much use I can get out of it.
// I really want to play around with the notion of types
// as first class citizens and get a handle on the way zig
// handles generics

const std = @import("std");
const Allocator = std.mem.Allocator;
const ArrayList = std.ArrayList;

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    var allocator = gpa.allocator();
    const alloc_ptr: *Allocator = &allocator;
    try partOne(alloc_ptr, "assets/day1input.txt");
    try partTwo(alloc_ptr, "assets/day1input.txt");
}

fn readFile(allocator: *Allocator, filename: []const u8) ![]u8 {
    const file = try std.fs.cwd().openFile(
        filename,
        .{ .mode = .read_only },
    );
    defer file.close();

    const stat = try file.stat();
    const buff = try file.readToEndAlloc(allocator.*, stat.size);
    return buff;
}

fn partOne(allocator: *Allocator, filename: []const u8) !void {
    var left_list = ArrayList(i32).init(allocator.*);
    var right_list = ArrayList(i32).init(allocator.*);
    var distance: u32 = 0;

    const buff = try readFile(allocator, filename);
    defer allocator.free(buff);

    var lines = std.mem.splitScalar(u8, buff, '\n');

    while (lines.next()) |line| {
        var words = std.mem.splitSequence(u8, line, "   ");
        var count: u32 = 0;
        while (words.next()) |word| : (count += 1) {
            // std.debug.print("{s}\n", .{word});
            if (count == 0) {
                const parsedInt = std.fmt.parseInt(i32, word, 10) catch |err| {
                    std.debug.print("{}\n", .{err});
                    continue;
                };
                try left_list.append(parsedInt);
            } else {
                const parsedInt = std.fmt.parseInt(i32, word, 10) catch |err| {
                    std.debug.print("{}\n", .{err});
                    continue;
                };
                try right_list.append(parsedInt);
            }
        }
    }

    std.mem.sort(i32, (&left_list.items).*, {}, comptime std.sort.asc(i32));
    std.mem.sort(i32, (&right_list.items).*, {}, comptime std.sort.asc(i32));
    for (left_list.items, 0..) |item, index| {
        distance += @abs(item - right_list.items[index]);
    }

    std.debug.print("distance is {}\n", .{distance});
}

fn partTwo(allocator: *Allocator, filename: []const u8) !void {
    var left_list = ArrayList(u32).init(allocator.*);
    var right_list = ArrayList(u32).init(allocator.*);
    var num_appearances = std.AutoHashMap(u32, u32).init(allocator.*);
    defer num_appearances.deinit();
    var distance: u32 = 0;

    const buff = try readFile(allocator, filename);
    defer allocator.free(buff);

    var lines = std.mem.splitScalar(u8, buff, '\n');
    while (lines.next()) |line| {
        var words = std.mem.splitSequence(u8, line, "   ");
        var count: u32 = 0;
        while (words.next()) |word| : (count += 1) {
            if (count == 0) {
                const parsedInt = std.fmt.parseInt(u32, word, 10) catch |err| {
                    std.debug.print("{}\n", .{err});
                    continue;
                };
                try left_list.append(parsedInt);
            } else {
                const parsedInt = std.fmt.parseInt(u32, word, 10) catch |err| {
                    std.debug.print("{}\n", .{err});
                    continue;
                };
                try right_list.append(parsedInt);
            }
        }
    }

    for (left_list.items) |left_number| {
        try num_appearances.put(left_number, 0);
        for (right_list.items) |right_number| {
            if (left_number == right_number) {
                const old_value: u32 = num_appearances.get(left_number).?;
                try num_appearances.put(left_number, old_value + 1);
            }
        }
    }

    var key_iterator = num_appearances.keyIterator();
    while (key_iterator.next()) |key_ptr| {
        distance += (key_ptr.* * num_appearances.get(key_ptr.*).?);
    }

    std.debug.print("part two distance is: {}\n", .{distance});
}
