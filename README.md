# Pathfind Server Module for MTA:SA
## Developing
1. Download and install the EditorConfig plugin for Visual Studio: [https://github.com/editorconfig/editorconfig-visualstudio#readme](https://github.com/editorconfig/editorconfig-visualstudio#readme)
2. Launch `premake.bat`
3. Open `Build/Pathfind.sln`

## Installing
1. Put the resulting `Bin/ml_gps.dll/.so` into your MTA:SA server `modules` folder.
2. Add `<module src="ml_gps.dll"/>` (or `.so` for Linux) to your `mtaserver.conf`.
3. Start the server

## API
### Function: `loadPathGraph`
```cpp
int loadPathGraph(string path)
```
* __path:__ The path to the graph definition file (relative to the resource directory)

Returns _graphId_ if everything went fine, _false_ otherwise.

### Function: `unloadPathGraph`
```cpp
bool unloadPathGraph(int graphId)
```
Returns _true_ if the graph has been unloaded, _false_ otherwise.

### Function: `findShortestPathBetween`
```cpp
bool findShortestPathBetween(int graphId, float startX, float startY, float startZ, float endX, float endY, float endZ, function callback)
```
* __startX, startY, startZ:__ The start position
* __endX, endY, endZ:__ The end position
* __callback:__ The callback function (parameters: `table nodes`)

Returns _true_ if the route calculation has been scheduled successfully, _false_ otherwise.

### Function: `isGraphLoaded`
```cpp
bool isGraphLoaded(int graphId)
```
Returns _true_ if the graph is already loaded, _false_ otherwise.

### Function: `findNodeAt`
```cpp
int, int, int, int findNodeAt(int graphId, float positionX, float positionY, float positionZ)
```
Returns _4 integers representing NodeId und his position_ if a node was found, _false_ otherwise.

## Contributors
* Justus H.
* Stefan K.
