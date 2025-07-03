# Polyrack
Polyrack is a 3D eurorack style plugin thats focused on making the creation of modules easy using an easy to use editor.
### Requirements
To compile polyrack you mainly just need CMake. The editor, Polyrack Studio, is able to run in any browser. You probably wont be able to run the main plugin for now though as it requres to have assets in %appdata%/polyrack/assets to run

# Creating a module
The Polyrack Studio editor is intented to make module creation simple and easy.
The editor consists of multiple tabs:
* [Layout](#the-layout-editor)
* [Textures](#the-texture-editor)
* [Models](#the-model-editor)
* [Animations](#the-animation-editor)
* [DSP](#the-direct-signal-processing-editor)
* [File](#the-file-editor)

### Editor Commands
Like most other software, Polyrack Studio features a list of commands. These might be changed at some point later on so be wary of that.
* CTRL + S = save
* CTRL + D = Import or load

## The Layout Editor
In the Layout editor, the positioning and settings of user inputs, cable inputs and outputs, models and more are decided. The module's layout consists of different types on components:
- **Static meshes**  
    Static meshes are self explanitory, good for being used as a base board or a decorative element.
- **Inputs**  
    Inputs are how the user can control your module. They take in an animation that will be displayed based on its values. [for more info on animations](#the-animation-editor)
- **Sockets**  
    Sockets connect different modules together and carry a voltage. They can be either an INPUT or an OUTPUT .They're used in the [DSP](#the-direct-signal-processing-editor) of your module.

## The Texture Editor
The Texture editor is where the textures that your module uses are Imported and listed out.

## The Model Editor
In the Model editor, you can import, view, and also edit models if you forgot to export them correctly from the software you created them in. Remember that positive Z points towards the camera. As of now, only **OBJ** type models are supported for importing.

## The Animation Editor
Animations are composed of multiple models that are transformed based on a set of mathematical functions. Models have a Duplication parameter that lets you create a series of the same model. Here is what the math functions have access to:
- **Functions**
    - min
    - max
    - sin
    - cos
    - tan
    - lerp
    - clamp
- **Values**
    - pi    : pi
    - e     : e
    - x     : the x value of the current input
    - y     : the y value of the current input
    - n     : the index of the current instance of the model
    - c     : the duplication count of the current model
    - nc    : n/c
    - nm    : n/(c-1)
    - t     : current time in seconds
## The Direct Signal Processing Editor

## The File Editor
In the File Editor you can save and load your module without using the commands as well as edit the name, description, and tags of your module.