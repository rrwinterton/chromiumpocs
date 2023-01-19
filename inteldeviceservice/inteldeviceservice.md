# Intel Device Service (name will change)
## Intel Device Service 
The device service is an mojo defined service that will take application capacity information and provide the information to the browser process as a performance hint. This hint can be used by the operating system and the SoC to help determine frequency and core selection. The device service is a proof of concept that takes from the render component a actual time and max time to before user experience degradation ratio (could be changed) and sends the information to the browser process as a first step in preventing oversubscription of frequency and core selection.
## copy script (src2dst.cmd)
