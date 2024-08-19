# 4coder_nickel
## What is this
My [4coder](https://4coder.net) [custom layer](https://4coder.handmade.network/forums/articles/t/7319-customization_layer_-_getting_started__4coder_4.1_) based on [4coder_fleury](https://github.com/4coder-archive/4coder_fleury/)  

![screenshot.png](https://raw.githubusercontent.com/nickel-dev/4coder_nickel/a56242d28646a0ed32eb4a0d55b263a62801b41d/screenshot.png)  

## How to compile
Get the source code for [4coder_fleury](https://github.com/4coder-archive/4coder_fleury/) and put the files from this repo into the source code of [4coder_fleury](https://github.com/4coder-archive/4coder_fleury/).  
The put the resulting source folder in your ".../[4coder](https://4coder.net)/custom/" directory and execute the build.bat in the just moved source folder (Make sure to execute the build.bat in [Visual Studio](https://visualstudio.com) command prompt).  

## Feature set
### 4coder_fleury
- [4coder_fleury](https://github.com/4coder-archive/4coder_fleury/) feature set
- The \*compilation\* buffer is now on the right side like in default 4coder
- \*compilation\* buffer now has background color, aswell as line wrapping  

### Custom stuff
- custom file headers in [4coder_fleury_nickel.cpp](https://github.com/nickel-dev/4coder_nickel/blob/main/4coder_fleury_nickel.cpp#L38)
- Hex color preview in theme files (copied from [hex_colors.cpp](https://gist.github.com/thevaber/58bb6a1c03ebe56309545f413e898a95))
- grep (ripgrep) command inside of the editor
- manpages (using wsl) inside of the editor

Have fun!
