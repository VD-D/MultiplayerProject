## **Forward and Game Rules**

This is a simple re-imagining of the Garry's Mod game Prop Hunt ([Steam Link](https://steamcommunity.com/sharedfiles/filedetails/?id=135509255)). Players are divided into two teams: "Props" and "Hunters".

- Props have the ability to turn into arbitrary static meshes littered around the level (with a few exceptions). If hit by a Hunter's laser, they lose 1 health, briefly become invulnerable, then teleport to a random area.
-  Hunters can fire a laser, which goes through all items until it hits a wall. If the laser does not hit a Prop character, the Hunter loses 1 health.

The game ends when any of the following conditions are met:

- All Props reach 0 health (Hunters win).
- All Hunters reach 0 Health (Props win).
- The game time runs out (Props win).

## **Starting a New Game In-Editor**

 1. Ensure you are on the MainMenu map - this is both the game default and editor default map. (For reference, it is found under `Content/ProjectFiles/Maps/MainMenu`).
 2. Net Mode should be Play Standalone and the number of players - by default - is 3 (this can be changed through config, see section below).
 3. One player must act as the Host, the others must connect to their session (please be patient with this, from testing the session refresh sometimes needs to happen twice before the system finds the session).
 4. Once all players have joined, the host can start the game. (Again, this takes a few seconds).

## **General Game Architecture**

- Players start in the Main Menu. Here, they can create, find and join sessions (see `/Shared/Subsystems/SessionManager` and the classes in `/Shared/Sessions/`).
- On creating or joining a session, players are moved to a lobby level (as the server if hosting, or client if joining), done through local travel. See `MainMenu/Actors/LobbyPawn`.
- On starting the game, the host server-travels to the actual game level. They initially briefly possess a pre-game pawn, before roles are assigned. The Game Mode spawns a special Game Manager actor (see `/Game/Actors/GameManager`) which manages the game timer and initializes player roles, by dividing them up into team and spawning and possessing the relevant characters (see `/Game/Actors/MultiplayerGameCharacter` and its sub-classes `/Game/Actors/HunterCharacter` and `/Game/Actors/PropCharacter`).
- There is then a (by default) 10 second period, during which Props can go hide. Hunters can use their camera, but otherwise cannot act.
- The game (by default) lasts 5 minutes (300 seconds) and ends as per the conditions above.
- If any player dies, they turn into a "spectator" which observes a random living player of the same role (so a dead Prop observes a living Prop and a dead Hunter observes a living Hunter). See `/Game/Actors/MultiplayerSpectatorPawn`.
- On game end, a "scoreboard" is shown which side won and everyone returns to the Main Menu level.

## **Custom Configuration**

If you wish to change the initial health for Props or Hunters, go to `/Content/ProjectFiles/GameplayAbilities/GameplayEffects/GE_PropInitial` and `/Content/ProjectFiles/GameplayAbilities/GameplayEffects/GE_HunterInitial`. From there, you will need to change Health and MaxHealth Scalable Float Magnitude attributes under Gameplay Effect -> Modifiers.

For other settings, go to Project Settings -> Game ->Multiplayer Game Settings.

The two most important settings are:

- Hunter to Prop Ratio : The sum of this is how many players there are. E.g. `Num Props = 2` and `Num Hunters = 1` means there will be 2 props and 1 hunter, for a total of 3 players.
- Game Phase Duration : This drives game duration. `GameCountdown` is the number of seconds the props have to hide while Hunters are disabled, while `InGame` the the total game duration (in seconds).


## **Future Considerations**

- Integration into Steam and EoS.
- I used GAS to implement player abilities - it should be fairly straightforward to add new abilities, as the underlying architecture is there.
- Visual effects and spectator pawn behavior could be improved.

## **Assets used**

- [Robot Recon](https://www.fab.com/listings/6d3983f7-f988-4bff-a633-1b8609fe6ba8)
- [Office Scene](https://www.fab.com/listings/c8ef3191-e74f-48a0-bcb8-fe2572286446)
- [Backrooms Level](https://itch.io/queue/c/4935219/unreal-templates?game_id=2641321&password=)
- (The level itself was my own amateur level design).