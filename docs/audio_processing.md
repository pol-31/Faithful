thread#1 (RenderThread) - OpenGL&GLFW
thread#2 (AudioThread) - OpenAL


case 1 - no music:
AudioThread help other's (e.g. loading)
case 2 - only background (player afk OR cut_scene):
AudioThread simply plays 1 AL_source
case 3 - moving from location#1 to location#2
AudioThread should fade location#1_music and
make louder location#2_music
case 4 - NPC speaking (let's only 1 can speak at the same time)
AudioThread should reduce all other gain and
make speaking louder (+ NPC_gain)

AudioThread_States:
- Normal
- SilentMusic (music turned off)
- SilentSounds (sounds turned off)
- SilentALL (music & sounds turned off)
- StaticLoadingStage (all turned off & help LoadQueue)

AudioThread_sound_queue:


AudioThread_OpenAL-sources:
- inside player (damage in/out)
- (0, 0, 0) - background music
- (0, 0, 0) - weather effects
- extra: NPC, enemy <-- let's take 3 extra, so 6 TOTAL

AudioThread_loop:
case AudioThread_State::Normal:
if (BattleMode)
-> reduced music play buffer
-> sound has first priority




...
if (enemy_hit_player) {
// other actions
AudioThread->PlaySound(player.sound.demaged.id);
}

void PlaySound(int sound_id) {
#if FAITHFUL_DEBUG
Logger->LogIf(sound_table_.)
#endif
sound_table.find())
} <-- should be blocking?























