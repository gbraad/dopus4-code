DirectoryOpus 4.18.44 OS4

This is a minor update of DirectoryOpus4 for OS4. Please retain
any previous installation of DirectoryOpus4 until you are sure
you have no serious problems with this release.

IMPORTANT: The language catalogs and helpfiles have not been well
tested. If they cause problems please use the builtin-english
catalog and the English help file. Translation files are included
if you would like to improve the current catalogs.

INSTALLATION: Just drag the DirectoryOpus drawer to the location
of your choice, open the drawer and doubleclick the program icon.
If you have run a previous version of DirectoryOpus, you may need
to reboot before trying this version. Be sure that you don't have
a previous version of dopus.library installed or running from a
different location on your hard-disk. This version will not work
properly with previous versions of the library or language catalogs.

CONFIGURATION: This release includes an example configuration
file which can be deleted or renamed if you want to use the builtin
configuration or your own custom configuration. However, if your
configuration does not open DirectoryOpus on it's own screen you
should read and change the included DopusWBStartup script.

WBSTARTUP: This release includes a WBStartup script and icon which
requires the included example configuration file or a user configuration
that opens DirectoryOpus on it's own screen. Please read the script
for other WBStartup options.

Refer to the changelogs in the History directory for more information
about changes to individual DirectoryOpus4 components.

Please report serious bugs or problems at the Opus4 YahooGroups
email list. Include the version number in any reports.


Changes in this release (4.18.44) include:

*Fixed directory sizes displayed in a lister when searching for a
 filename (or pattern) in multiple directories with the hunt command.

*Fixed a hunt command failure to highlight the found file when the file
 was located in a subdirectory of the directory being searched. When the
 user selected the OK button in the "Shall I go there?" requester,
 the hunt command would display a requester stating that the file was
 not found but then load the directory where the file was located.

*Fixed a crash in the Dopus4 text viewer. If Dopus4 was on the
 Workbench screen and the text viewer was open when the user quit
 the Dopus4 program, the viewer would crash (Grim readper).

*Fixed a bug in Dopus_Print that prevented updating of the output
 filename in the requester.

*Changed makefiles to create 'stabs' debug binaries and improve
 compiler output readability. All Dopus4 components will now display
 specific function information in a stack trace if there is a failure
 when running debug binaries.

*Replaced all internal string functions and unbounded 'C' library string
 functions with size limited 'C' library string functions to eliminate
 possible buffer overflows.

*Replaced all deprecated memory allocation functions with new OS4 memory
 allocation functions.

*Replaced all deprecated message related functions with new OS4 functions.

*Fixed all compiler warnings to make it easier to spot new warnings.

* Added a missing Tooltype to the DirectoryOpus icon.

*Changed the copyright data in about window.

*Updated copyright data in english DirectoryOpus.guide.

*Updated history files.

***********************************************************************

Changes in release 4.18.43 include:

*Fixed removal of wrong file from destination lister when doing
 a copyas from an archive (previous fix broken by recent changes).

*Fixed copying of directories from an archive when a directory with
 the same name exists in the destination lister.

*Fixed dopus.library version string.

*Replaced some deprecated functions with correct OS4 functions.

*Updated history files.

***********************************************************************

Changes in release 4.18.42 include:

DirectoryOpus:

*Consolidated all iff definitions & eliminated duplicated definitions.

*Added missing pragma pack directives to all header files.

*Updated the application library interface opening to version 2 for
 the current OS4 SDK includes.

*Fixed a problem with filetype failures when double-clicking a file.

*Improved the config file included in the distribution archive.

*Updated History files.

Dopus_Print:

*Replaced string functions with size bounded string functions to avoid
 buffer overflows.

*Replaced deprecated functions with new OS4 functions.

*Added file/path size definitions in the library header file for consistant
 declarations in future updates.

*Changed file/path buffer sizes to those defined in the library header file.

*Created some string length definitions for consistancy.

***********************************************************************

Changes in release 4.18.41 include:

*Added missing arguments check in DopusRT to prevent a crash
 if DopusRT is started with no arguments.

*Fixed access of freed memory when closing an asynchronous
 Read window. Fixed several potential memory leaks.

*Added signal notification to asynchronous Read windows when quitting
 Dopus4 or returning with changes from ConfigOpus. Previously the
 asynchronous Read windows could be forcibly closed at users request
 and leave Read processes running with no window.

*Fixed launching of asynchrouous Read processes and added checks
 for Ctrl-C to shut down those processes.

*Fixed reopening of Dopus4 on front screen when returning from
 ConfigOpus changes if Dopus4 was initially started with "BEHINDWB"
 TOOLTYPE or "-B" CLI argument.

*Eliminated a crash in the Dopus4 ANSIRead command.

*Changed the "close foreign windows" requester to open on the
 Dopus4 screen when quitting Dopus4 from it's own screen.

*Fixed copying of directories form archives opened in a lister.

*Fixed some failures to unlock public screens after failed memory
 allocations or file access.

*Updated "alien windows" requester text to match previously
 updated requester actions.

*Added Czech catalogs and French catalog. Updated all catalogs.
 Program catalog  definition file and translation files updated.

*Changed "Hidden" bit in Protect requester to "Hold" bit for OS4.
 Eliminated hidden bit setting in ConfigOpus which activated hiding
 of files with the H bit set. The H bit is actually a hold bit in OS4
 and indicates to AmigaDOS that the file should be added to the
 resident list if the P (pure) bit is also set.

*Added popupmenu & xadmaster header files to sources so others
 can compile Dopus4 without hunting for missing includes.

*Updated history files.

***********************************************************************

Changes in release 4.18.40 include:

*Fixed infinite recursion that occurred if the user tried to copy or
 move a directory into one of it's sub-directories.

*Added 'real' replace option (activated with new Tooltype REPLACEDIRS)
 when copying a directory and a destination directory with the same name
 already exists. As before, Dopus4 merges the 2 directories if the
 Tooltype isn't set. Shell argument -r is the equivalent of REPLACEDIRS
 Tooltype for WorkBench.

*Fixed the Rename command so it no longer hides the destination file if
 the source filename had to be renamed a second time because a filename
 already exists in the destination.

*Switched to a large global copy buffer to speed up large file copies
 and eliminate unnecessary memory fragmentation.

*Fixed crash that occurred when Dopus4 was deiconified to a closed public
 screen. Dopus4 will now open on Workbench when deiconified if the screen
 it  was iconified from has been closed.

*Changed quit procedure (shutdown) to retry guest window check
 instead of closing guest windows and leaving them in a permanent
 wait state.

*Fixed directory datestamp, protection & comment by adding a requester
 to choose between applying the change to the directory and all it's
 contents or just the directory itself. Previously, all files in the
 directory were changed which may not be what a user would expect.

*Eliminated extra DOS requester when attempting to copy a directory to
 a copy-protected device.

*Removed file deletion in copy operations that caused destination file
 loss if source file was read protected.

*Fixed failures in some ARexx commands (like "status 13 0 set")
 caused by case-sensitive string comparisons.

*Fixed a silent copy that could occur after a file move failed or
 was cancelled.

*Added a requester for copying a directory to an existing directory.

*Fixed encrypting a file from an archive.

*Added new functions to fix copying to multi-assign directories.
 Dopus was incorrectly detecting existing files in a primary
 assignment because they existed in a secondary assignment.

*Added new checks for file copy/move completion to avoid deleting
 files without replacing them as selected by the user.

*Eliminated problems when copying files from archives displayed in
 a Dopus lister.

*Changed most requesters to OS4 style requesters with purpose
 identifier images and standardized appearance.

*Fixed error reporting for Makelink command. It was reporting same error
 for all failures.

***********************************************************************

Changes in release 4.18.38 include:

*Since DirectoryOpus4 copies linked files instead of reproducing links
 in a 'copy' operation, the destination lister display has been fixed
 to display the copied files correctly.

*Moving a directory link no longer removes the files from the linked
 directory. Only the link is removed after the move.

*Hard links are now display as links (underlined) instead of normal files.

*The 'makelink' command was fixed and can now create relative or absolute
 soft links. The makelink requester defaults to soft links since hard links
 currently only work on FFS partitions.

*Hex search was fixed to work as documented in the DirectoryOpus guide.

*Fixed 'rename' crash that could occur as a result of increasing the
 internal default filename length.

*Added setting of protection bits when extracting an archive.

*Main window changed to NOT open active and take the focus if
 DirectoryOpus is opened behind Workbench.

*Fixed CLI argument parsing because DirectoryOpus was reading extra
 arguments which could conflict with the actual command line args.

*Added setting of protection bits when extracting an archive.

*Updated the DirectoryOpus.guide documentation.

*Updated old language catalogs.

*Added several options to the Dopus_Print module that might be useful
 for some printers. The new options are not contained in the docs but
 should be self explanatory.

