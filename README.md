# cacaMap
CacaMap  is a Qt based  map browser widget that works with a bunch of tile servers.
![cacamap gif](cacamap.gif)

## Build & Run
```bash
# assuming you already have qt4 installed, probably would work with other qt versions but haven't tested it.
# this will generate the Makefile
qmake -qt=
make
./cacamap
```
## Usage
Just add cacaMap to your widget as a child.
If you need to draw anything on top of the map then create
another class that inherits from cacaMap. Implement the paintEvent,
and inside call cacaMap's paintEventHandler and then draw whatever
you want.

```c++
void myDerivedMap::paintEvent(QPaintEvent *e)
{
  cacaMap::paintEvent(e);
  QPainter p(this);
  //you can draw here whatever you want
  //it will show up overlaid on the map
  p.drawSomething(...)
}
```

## License
copyright 2010 Jean Fairlie
jmfairlie@gmail.com

CacaMap is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
