#import <Foundation/Foundation.h>

// Tells macOS this is a latency-critical interactive app:
// - Disables App Nap
// - Prevents CPU/GPU throttling
// - Equivalent to what Game Mode does at process level
void enableHighPerformanceMode()
{
    [[NSProcessInfo processInfo]
        beginActivityWithOptions:
            NSActivityLatencyCritical |
            NSActivityUserInitiated |
            NSActivityIdleSystemSleepDisabled
        reason:@"High-performance real-time rendering"];

    // Also try gamepolicyctl in-process (Xcode developer tools)
    NSString *gamectl = @"/Applications/Xcode.app/Contents/Developer/usr/bin/gamepolicyctl";
    if ([[NSFileManager defaultManager] isExecutableFileAtPath:gamectl]) {
        NSTask *task = [[NSTask alloc] init];
        task.launchPath = gamectl;
        task.arguments  = @[@"game-mode", @"set", @"on"];
        [task launch];
        [task waitUntilExit];
    }
}
