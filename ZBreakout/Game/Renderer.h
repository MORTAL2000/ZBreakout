/*
ZBreakout Project

Copyright (c) 2018 Nikita Kogut (Telegram @MrOnlineCoder | github.com/MrOnlineCoder | mronlinecoder@gmail.com)
All rights reserved.

Only original author is allowed to edit and share this file.
Unauthorized copying or editing this file, via any medium is strictly prohibited
Proprietary and confidential
*/

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>

#include "Player.h"

#include "../Logger.h"

#include "../GameState/StateManager.h"

const int HPBAR_SIZE = 250;
const int HPBAR_HEIGHT = 20;
const float HPBAR_OUTLINE = 3.0f;

const int ITEMTEXT_PADDING = 100;

const float BULLET_RADIUS = 2.5f;

class Renderer {
	public:
		void init(StateManager& mgr);
		void drawPlayer(Player& pl);
		void drawPlayerHPBar(Player& pl);
		void drawInventory(Player& pl);
		void drawBullet(Bullet& b);

		void updateItemText(Player& pl);

		sf::Vector2f getPlayerCenter(sf::Vector2f arg);
	private:
		void setup();

		sf::RenderWindow* window;
		StateManager* manager;

		//Player
		sf::Text plName;
		sf::Sprite plSpr;

		//Hp bar
		sf::RectangleShape hpBar;
		sf::RectangleShape hpBarOut;

		//Items and inventory
		sf::Text wpnText; //weapon title

		//Bullets
		sf::CircleShape bulletShape;
};

#endif