## Building

Clone this repo and inside you'll find gk1.sln file. Open it with Microsoft Visual Studio (preferably 2019). If you're using one of the older versions, you might want to change Windows SDK Version and Platform Toolset currently installed on your machine. Inside MVS you can build and/or run the app by pressing F5 key.

## Running
Command line syntax:
```
gk1.exe <PixelSize> <WindowWidth> <WindowHeight> <InitialWindowXPos> <InitialWindowYPos>
```
Any argument not supplied will be replaced with default value:
```
gk1.exe 2 800 400 0 0
```
Any excessive values shall be ignored.

## Quick guide

- Q-key: Free mode (move verticies and polygons (CTRL) around)
- W-key: Vertex creation mode
- E-key: Polygon creation mode
- A-key: Add equal length constraint mode
- S-key: Add perpendicular constraint mode
- D-key: Deletion mode
- Space: Create sample polygon

Double-click is widely used to perform some actions. Windows' title shows you the mode you're in.

## Long guide

When you run the app, you'll be greeted with black screen. By default you are in **Free mode [q key]**. In free mode you can move edges (green lines) and verticies (red dots) around. When **CTRL** key is pressed in this mode, you are able to move whole polygons around.

In order to create your first polygon, press E key in order to enter **Polygon creation mode [e key]**. While in polygon creation mode, **double-click** in three places on the screen in order to create a triangle with verticies in exactly those places.

In order to create more sophisticated polygon, enter **Vertex creation mode [w key]**. While in vertex creation mode, you can double-click on polygons' edge in order to split it in half (**this action removes constraint on the edge**).

You can also set constraints on pairs of polygons' edges. **Remember, you can set only one constraint per edge and between edges of one polygon**. In order to set **Equal length constraint [a key]**, press A key and double-click on two edges you want to set constraint on. When done, notice there's some text nearby edges you just chose. The first character is '=' sign meaning that the edge in question has a equal length constraint set on it. The rest is a **unique cosntraint ID** used to distinguish constraints. You may also want to set **Perpendicular constraint [s key]**. To apply this constraint, act just like as you would when setting equal length constraint. This time, the label nearby edges with perpendicular constraint starts with '⊥' and ends with unique constraint ID as before.

In order to delete verticies and/or constraints enter **Deletion mode [d key]**. In deletion mode, you can delete verticies by double-clicking on them (**this action removes adjacent edges' constraints**) and remove constraints set on edges by double-clicking on the edge you want to remove constraint from. When you attempt to remove a vertex from a triangle, the whole polygon will be deleted.

If you want to create sample polygon, just press **Space**.

Window title changes depending on the mode you're in.