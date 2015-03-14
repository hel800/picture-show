# Known Issues for version 2 #
  * None known

# Known Issues for version 0.9 #

The following issues are known and still present in the latest release. They could not be handled yet because of technical restrictions or other external limitations:

  * When showing or hiding the open/settings dialog in full screen mode the screen flickers under Windows (MSVC 2010). This is caused by interference of different Qt paint() instances for the full screen widget and the dialog widget. This could not be resolved so far, any hints on this topic will be greatly appreciated.

  * If picture-show is not running smoothly on your machine, either your machine is not capable of running OpenGL applications it is to old to perform the actions in a reasonable amount of time.

  * Windows 8: Some Intel Graphics Adapter do not support OpenGL properly through the standard graphics card driver provided by Windows 8!