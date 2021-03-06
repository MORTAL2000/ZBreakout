/*
ZBreakout Project

Copyright (c) 2018 Nikita Kogut (Telegram @MrOnlineCoder | github.com/MrOnlineCoder | mronlinecoder@gmail.com)
All rights reserved.

Only original author is allowed to edit and share this file.
Unauthorized copying or editing this file, via any medium is strictly prohibited
Proprietary and confidential
*/

#include "Engine.h"
#include "Game/Random.h"

Engine::Engine(int argc, char* argv[])
: server(assets) {
	_LOG_.init();
	ELOG("Created engine instance");

	ELOG("Loading assets...");
	assets.loadAll();

	ELOG("Creating game window...");
	window.create(sf::VideoMode(1280, 720), "ZBreakout", sf::Style::Close | sf::Style::Titlebar);
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);

	ELOG("Creating StateManager...");
	stateManager.init(&window, &assets, &server);

	ELOG("Initialzing random generator...");
	Random::init();
}

int Engine::run() {
	ELOG("Entering engine loop");

	sf::Event ev;
	while (window.isOpen()) {

		while(window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) window.close();

			if (window.hasFocus()) stateManager.handleInput(ev);
		}

		if (window.hasFocus()) stateManager.update();
		assets.update();

		window.clear();

		stateManager.render();

		window.display();
	}

	shutdown();
	return 0;
}

void Engine::shutdown() {
	ELOG("Shutting down the game...");
	ELOG("Destoying assets manager...");
	assets.destroy();

	ELOG("Trying to shutdown game server...");
	server.stop();

	ELOG("Done. Exiting.");
}
