Scene switching:
/// Scene switching:
/// - internally by scenes collaboration (user keyboard/mouse input)
/// - by global callbacks (GlfwWindowUserPointer)

Why not inheritance? It looks like heavy unnecessary overhead:
for each loop interaction we add 2-7 asm instructions.
We aren't ready to sacrifice such cpu resources.

General:
enum class GameState {
kMainGame,
kMainMenu,
kTerminate
};
___
___
Rendering:

enum class SceneRenderMenuState {
LoadScreen,
StartScreen,
ConfigGeneral,
ConfigLocalization,
ConfigIO,
ConfigSound,
ConfigGraphic,
ConfigKeys
};

// OpenGL context
void Config();

// render loop iteration
void ProcessLoadScreen();
void ProcessStartScreen();
void ProcessConfigGeneral();
void ProcessConfigLocalization();
void ProcessConfigIO();
void ProcessConfigSound();
void ProcessConfigGraphic();
void ProcessConfigKeys();

enum class SceneRenderGameState {
LoadScreen,
Default,
Inventory1,
Inventory2,
Inventory3,
Inventory4,
Pause
};

___

// OpenGL context
void ConfigLoadScreen();
void ConfigDefault();
void ConfigInventory1();
void ConfigInventory2();
void ConfigInventory3();
void ConfigInventory4();
void ConfigPause();

// render loop iteration
void ProcessLoadScreen();
void ProcessDefault();
void ProcessInventory1();
void ProcessInventory2();
void ProcessInventory3();
void ProcessInventory4();
void ProcessPause();

___
___
 Input:

enum class SceneInputMenuState {
LoadScreen,
StartScreen
};
// no pooling
___
enum class SceneInputGameState {
LoadScreen,
Default,
Battle,
Inventory
};
// callbacks
void ConfigLoadScreen();
void ConfigDefault();
void ConfigBattle();
void ConfigInventory();

// pooling
void ProcessLoadScreen();
void ProcessDefault();
void ProcessBattle();
void ProcessInventory();
___
___
Audio:
enum class SceneAudioMenuState {
LoadScreen,
StartScreen
};
void ConfigLoadScreen();
void ConfigStartScreen();
___

enum class SceneAudioGameState {
Default,
Battle,
Biome1,
Biome2,
Pause
};
void ConfigDefault();
void ConfigBattle();
void ConfigBiome1();
void ConfigBiome2();
void ConfigPause();
___
___
Collision:
enum class SceneCollisionMenuState {
LoadScreen,
StartScreen,
ConfigGeneral,
ConfigLocalization,
ConfigIO,
ConfigSound,
ConfigGraphic,
ConfigKeys
};
void ConfigLoadScreen();
void ConfigStartScreen();
void ConfigConfigGeneral();
void ConfigConfigLocalization();
void ConfigConfigIO();
void ConfigConfigSound();
void ConfigConfigGraphic();
void ConfigConfigKeys();
___
enum class SceneCollisionGameState {
LoadScreen,
Default,
Inventory1,
Inventory2,
Inventory3,
Inventory4,
Pause
};
void ConfigLoadScreen();
void ConfigDefault();
void ConfigInventory1();
void ConfigInventory2();
void ConfigInventory3();
void ConfigInventory4();
void ConfigPause();
___
___
Update:
(depends solely on GameState)
void ConfigMenu();
void ConfigGame();
___
___
