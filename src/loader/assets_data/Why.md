Situation: take audio & textures:
1) assume AudioThreadPool need music data - then we
separate music from textures
2) Music.h and MusicPool.h don't know about each other,
they know only about MusicData.h --> what is also looks appealing