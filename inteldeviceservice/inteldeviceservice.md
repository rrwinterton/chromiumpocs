# Intel Device Service (name will change)
## Intel Device Service 
This is a branch of the chromium source code from revision 110.0.5481.0.  This repo had to be based off of this version due to a bug that was introduced in a checkin to chromium that required a later version of the clang compiler than currently available when using MSVC 2022.  [chromium build issue](https://bugs.chromium.org/p/chromium/issues/detail?id=1402885). At some point we will be The device service is an mojo defined service that will take application capacity information and provide the information to the browser process as a performance hint. This hint can be used by the operating system and the SoC to help determine frequency and core selection. The device service is a proof of concept that takes from the render component a actual time and max time to before user experience degradation ratio (could be changed) and sends the information to the browser process as a first step in preventing oversubscription of frequency and core selection.

## Building Device Service Changes
- Set up your build environment as usual when building chromium
- After doing a fetch chromium do a gclient sync --revision 110.0.5481.0.  This will update the repo to the correct version that is currently being used.  
- Then do a git clone of this repo [chromiumpocs](https://github.com/rrwinterton/chromiumpocs.git) and go to the chromiumpocs/inteldeviceservice/dst/src/ and copy directories under the src directy to the src directory you synced with the 110.0.5481.0 src.  The currently is 58 files.  If we add other files we need to update the src2dst.cmd to make it easier to copy files to and from.  When ready we will update to the tip of tree of the repo and the compiler issue is fixed.

## copy script ([src2dst.cmd](https://github.com/rrwinterton/chromiumpocs/blob/0d82df0984977533f296ff97e1ee9b325898f9f1/inteldeviceservice/src2dst.cmd)) 
The copy script src2dst.cmd will copy the changed source from the src ... directory of the environment variable srcpath to src ... dstpath environment variable.  

When you make changes to a new file add the file to the src2dst.cmd path so that we can keep track of the changes and easily copy the code to the repo.  