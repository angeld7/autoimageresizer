autoimageresizer
================
By Angel Delgado

Automatically resize images in a directory.  Currently working for OSX.

Monitors folders with Apple's System Event API.  When a file is added to the folder Opencv is used to resize the image.  This program also uses inih(ini not invented here), to parse the config file.

Resize Modes
- Normal resize: Resize the picture to a specific resolution.
- Scale: Scale the picture with a specific scaling factor.
- Border: Use a letter box to resize to a specific resolution without distortion.
- Crop: Resize the image to specific resolution cropping excess to avoid distortion.
