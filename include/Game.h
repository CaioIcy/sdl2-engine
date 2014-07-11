#pragma once

#include "SDLWrapper.h"
#include "Window.h"
#include "StateGame.h"

#include <map>
#include <memory>

#include "AudioHandler.h"
#include "InputHandler.h"
#include "ResourceManager.h"

/**
* Main structure class for the game.
* Contains all the necessary functionalities to loop and update the game.
* 	Is the state machine (controls current state), contains all the different possible states.
*/
class Game {

	public:
		/**
		* All possible game states.
		*/
		enum GStates : uint8_t {
			PLACEHOLDER = 0
		};

		/**
		* Singleton imeplementation for Game.
		* @return The instance for a Game.
		*/
		static Game& instance();

		/**
		* The destructor.
		* Destroys the game's Window and states, and unloads current state.
		*/
		~Game();
		
		/**
		* The main game loop.
		* Orders the game structure, such as inputs, updates, and rendering.
		*/
		void runGame();

		/**
		* Sets the current game state.
		* @see StateGame::load()
		* @see StateGame::unload()
		* @param state_ : The state you want to be changed into.
		*/
		void changeState(const GStates state_);

		/**
		* @return The Game audioHandler.
		*/
		AudioHandler& getAudioHandler();

		/**
		* @return The boolean array recieved from the InputHandler.
		*/
		std::array<bool, GameKeys::MAX> getInput();

		/**
		* @return The resource manager.
		*/
		ResourceManager& getResources();

		/**
		* Stops execution and closes the game.
		*/
		void stop();

		void clearKeyFromInput(const GameKeys key_);
		void resizeWindow(const unsigned int width_, const unsigned int height_);

	private:
		/**
		* The constructor.
		* Sets the game window and tells the game that it is OK to begin looping. Also, it
		* 	begins the FPS manager.
		* @note If the Window cannot be created, the game will not begin.
		*/
		Game();

		/**
		* Loads all the states.
		* Every new state implemented should be initialized here.
		*/
		void initializeStates();

		/**
		* Deletes all the loaded states.
		* Every new state implemented should be deleted here.
		*/
		void destroyStates();

		Window* window; /**< The game Window. */
		bool isRunning; /**< Whether the game is currently running/looping or not. */

		AudioHandler* audioHandler; /**< The Game AudioHandler. */
		InputHandler* inputHandler; /**< The Game InputHandler. */
		ResourceManager* resourceManager; /**< The Game ResourceManager. */

		StateGame* currentState; /**< The current state, which the game is in. */

		std::map<GStates, StateGame*> statesMap; /**< Map containing all possible states. */

		FPSmanager fpsManager; /**< The FPSManager from SDL2_GFX. Handles the framerate
			capping. */

};
