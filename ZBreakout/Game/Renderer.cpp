/*
ZBreakout Project

Copyright (c) 2018 Nikita Kogut (Telegram @MrOnlineCoder | github.com/MrOnlineCoder | mronlinecoder@gmail.com)
All rights reserved.

Only original author is allowed to edit and share this file.
Unauthorized copying or editing this file, via any medium is strictly prohibited
Proprietary and confidential
*/

#include "Renderer.h"
#include <math.h>
#include <sstream>
#include <iomanip>
#include "Random.h"

std::stringstream roundingStream;

Renderer::Renderer(){
	Random::init();

	player = NULL;
}

std::string roundNumber(float num) {
	roundingStream.str("");
	roundingStream << std::fixed << std::setprecision(2) << num;
	return roundingStream.str();
}

void Renderer::init(StateManager & mgr){
	manager = &mgr;
	window = &manager->getWindow();

	_LOG_.log("Renderer", "Initializing Renderer...");

	setup();

	_LOG_.log("Renderer", "Renderer ready.");
}

void Renderer::setLevel(tmx::Map & map) {
	levelTiles.loadFromMap(map, 0);
}

void Renderer::setDoors(std::vector<Door>& d) {
	doors = &d;
}

void Renderer::setPlayer(Player & pl) {
	player = &pl;
}

void Renderer::drawPlayer(Player & pl) {

	if (pl.isHoldingWeapon()) {
		if (pl.getWeapon().getType() == Weapon::AK47) {
			plSpr.setTexture(manager->getAssets().getTexture("player_ak47"));
		}
		else if (pl.getWeapon().getType() == Weapon::PISTOL || pl.getWeapon().getType() == Weapon::REVOLVER) {
			plSpr.setTexture(manager->getAssets().getTexture("player_pistol"));
			plSpr.setTextureRect(sf::IntRect(0, 0, plSpr.getTexture()->getSize().x, plSpr.getTexture()->getSize().y));
		}
	}
	else {
		plSpr.setTexture(manager->getAssets().getTexture("player"));
	}

	plSpr.setPosition(pl.pos);
	plSpr.setRotation(pl.direction * 90);
	plSpr.setOrigin(plSpr.getGlobalBounds().width / 2, plSpr.getGlobalBounds().height / 2);

	window->draw(plSpr);

	plName.setString(pl.getNickname());

	sf::Vector2f namePos = plSpr.getPosition();

	namePos.x = plSpr.getPosition().x - plName.getGlobalBounds().width / 2;
	namePos.y = plSpr.getPosition().y - (plName.getGlobalBounds().height * 6) - 5;

	plName.setPosition(namePos);

	window->draw(plName);
}

void Renderer::drawPlayerHPBar(Player & pl) {
	if (player->hp < (Constants::MAX_PLAYER_HP * 0.20)) {
		hpBar.setFillColor(sf::Color::Red);
	} else {
		hpBar.setFillColor(sf::Color(113, 219, 94));
	}

	hpBar.setSize(sf::Vector2f((player->hp * HPBAR_SIZE) / Constants::MAX_PLAYER_HP, HPBAR_HEIGHT));
	window->draw(hpBar);
	window->draw(hpBarOut);
}

void Renderer::drawInventory(Player & pl) {
	window->draw(wpnText);

	window->draw(goldShape);

	goldText.clear();
	goldText << sf::Text::Bold << sf::Color::White << "Gold: " << sf::Color(199, 133, 39) << std::to_string(player->gold);
	goldText.setPosition(
		goldShape.getPosition().x + goldShape.getGlobalBounds().width / 2 - goldText.getGlobalBounds().width /2,
		goldShape.getPosition().y + goldShape.getGlobalBounds().height / 2 - goldText.getGlobalBounds().height / 2);

	window->draw(goldText);
}

void Renderer::drawBullet(Bullet& b) {
	bulletShape.setPosition(b.pos);
	window->draw(bulletShape);
}

void Renderer::drawLevel() {
	window->draw(levelTiles);
}

void Renderer::drawDebug() {
	for (auto& rect : *debug.walls) {
		wallRect.setSize(sf::Vector2f(rect.width, rect.height));
		wallRect.setPosition(sf::Vector2f(rect.left, rect.top));

		window->draw(wallRect);
	}

	plRect.setPosition(sf::Vector2f(plSpr.getPosition().x - plSpr.getGlobalBounds().width/2, plSpr.getPosition().y - plSpr.getGlobalBounds().height/2));
	plRect.setSize(sf::Vector2f(plSpr.getGlobalBounds().width, plSpr.getGlobalBounds().height));
	window->draw(plRect);
}

void Renderer::drawZombie(Zombie & z) {
	zombieSpr.setPosition(z.pos);

	float angle = std::atan2(z.pos.y - player->pos.y, z.pos.x - player->pos.x);
	angle = 180 + angle * 180 / 3.1415;

	zombieSpr.setRotation(angle);

	if (z.hp < z.getMaxHp()) {
		zombieHp.setSize(sf::Vector2f((z.hp * ZOMBIEBAR_SIZE) / z.getMaxHp(), 10));
		zombieHp.setOrigin(zombieHp.getGlobalBounds().width / 2, zombieHp.getGlobalBounds().height / 2);
		zombieHp.setPosition(zombieSpr.getPosition() + sf::Vector2f(0, -(zombieSpr.getGlobalBounds().height/2)-3));

		window->draw(zombieHp);
	}
	
	window->draw(zombieSpr);
}

void Renderer::drawDoors() {
	for (auto i = 0; i < doors->size(); i++) {
		Door& door = doors->at(i);

		doorTxt.setFillColor(door.open ? sf::Color::Green : sf::Color::White);

		doorTxt.setString(door.name);
		doorTxt.setPosition(door.bounds.left + door.bounds.width/2 - doorTxt.getGlobalBounds().width/2, 
			door.bounds.top - doorTxt.getGlobalBounds().height - 15);
		window->draw(doorTxt);
	}
}

void Renderer::drawPossibleActions() {
	//doors open
	for (auto i = 0; i < doors->size(); i++) {
		Door& door = doors->at(i);

		if (door.open) continue;

		float dist = GameMath::distanceSquared(player->pos, sf::Vector2f(door.bounds.left + door.bounds.width / 2, door.bounds.top + door.bounds.height / 2));

		if (dist < Constants::ACTION_DISTANCE * Constants::ACTION_DISTANCE) {
			setActionText("G", "Open door " + door.name + " (" + std::to_string(door.price) + " gold)");
			drawActionText();
		}
	}
}

void Renderer::updateItemText(Player& pl) {
	wpnText.clear();

	if (pl.isSlotFree(pl.currentSlot)) {
		wpnText << sf::Color::White << sf::Text::Bold << "[ Empty ]";
	} else if (pl.getItem().type == ItemType::WEAPON) {
		Weapon& wpn = pl.getWeapon();

		if (wpn.getAmmo() == 0) {
			float reloadTimeLeft = wpn.getReloadTime() - wpn.reloadClock.getElapsedTime().asSeconds();
			wpnText << sf::Color::White << "[ " << sf::Text::Bold << Weapon::getWeaponName(wpn.getType()) << sf::Text::Regular << " ] Reloading... (" << sf::Color::Yellow << roundNumber(reloadTimeLeft) << " s." << sf::Color::White << ")";
		} else {
			wpnText << sf::Color::White << "[ " << sf::Text::Bold << Weapon::getWeaponName(wpn.getType()) << sf::Text::Regular << " ] " << sf::Color(153,153,153) << std::to_string(wpn.getAmmo()) << sf::Text::Bold << " | " << sf::Text::Regular <<  std::to_string(wpn.getMaxAmmo());
		}

		
	}

	wpnText.setPosition(sf::Vector2f(manager->getWindowSize().x / 2 - wpnText.getGlobalBounds().width / 2,
		manager->getWindowSize().y - ITEMTEXT_PADDING - wpnText.getGlobalBounds().height));
}

void Renderer::playDamageEffect(sf::Vector2f pos, int dmg) {
	DamageHit hit;

	sf::Vector2f tpos = pos;

	tpos.x += Random::randomInt(-50, 50);
	tpos.y += Random::randomInt(-50, 50);

	hit.pos = tpos;

	hit.dmg = dmg;
	hit.scale = 0.6f;

	hits.push_back(hit);
}

void Renderer::renderDamageHits() {
	for (std::vector<DamageHit>::size_type i = 0; i < hits.size(); i++) {
		DamageHit& hit = hits[i];

		hitTxt.setPosition(hit.pos);
		hitTxt.setString("- "+std::to_string(hit.dmg));
		hitTxt.setOrigin(hitTxt.getGlobalBounds().width / 2, hitTxt.getGlobalBounds().height / 2);
		hitTxt.setScale(hit.scale, hit.scale);
		
		window->draw(hitTxt);

		if (hit.scale < 1.0f) hit.scale += 0.33f;

		if (hit.alive.getElapsedTime().asMilliseconds() > 500) {
			hits.erase(hits.begin() + i);
			return;
		}
	}
}

sf::Vector2f Renderer::getPlayerCenter(sf::Vector2f arg) {
	sf::Vector2f res;

	res.x = arg.x;
	res.y = arg.y;

	return res;
}

void Renderer::setup() {
	plSpr.setTexture(manager->getAssets().getTexture("player"));

	plName.setFillColor(sf::Color::White);
	plName.setCharacterSize(18);
	plName.setFont(manager->getAssets().getFont("main"));

	//hp bar
	hpBar.setSize(sf::Vector2f(HPBAR_SIZE, HPBAR_HEIGHT));
	hpBar.setPosition(manager->getWindowSize().x - HPBAR_SIZE - HPBAR_OUTLINE - 15, 15);

	hpBarOut.setOutlineColor(sf::Color::White);
	hpBarOut.setFillColor(sf::Color(255,255,255,0));
	hpBarOut.setPosition(hpBar.getPosition().x - HPBAR_OUTLINE, hpBar.getPosition().y - HPBAR_OUTLINE);
	hpBarOut.setOutlineThickness(HPBAR_OUTLINE);
	hpBarOut.setSize(sf::Vector2f(hpBar.getGlobalBounds().width + HPBAR_OUTLINE, 
		hpBar.getGlobalBounds().height + HPBAR_OUTLINE));
	
	//items and inventory
	wpnText.setCharacterSize(28);
	wpnText.setFont(manager->getAssets().getFont("main"));

	//gold
	goldShape.setFillColor(sf::Color(29, 26, 19, 200));
	goldShape.setOutlineColor(sf::Color(128, 0, 0, 255));
	goldShape.setOutlineThickness(1.0f);
	goldShape.setCornersRadius(5.0f);
	goldShape.setCornerPointCount(10);
	goldShape.setSize(sf::Vector2f(HPBAR_SIZE, 50));
	goldShape.setPosition(window->getSize().x - goldShape.getGlobalBounds().width - 15,hpBarOut.getPosition().y + hpBarOut.getGlobalBounds().height + 15);

	goldText.setFont(manager->getAssets().getFont("main"));
	goldText.setPosition(goldShape.getPosition() + sf::Vector2f(5,5));
	goldText.setCharacterSize(24);

	//bullets
	bulletShape.setFillColor(sf::Color::Yellow);
	bulletShape.setRadius(BULLET_RADIUS);

	resetText(hitTxt);
	hitTxt.setFillColor(sf::Color::Yellow);
	hitTxt.setOutlineColor(sf::Color::White);

	//doors
	resetText(doorTxt);

	//center action
	resetText(centerTxt);
	resetText(centerKey);

	centerKey.setFillColor(sf::Color::Black);
	centerKey.setCharacterSize(32.0f);
	centerKey.setOutlineThickness(0.0f);

	centerShape.setFillColor(sf::Color::White);
	centerShape.setOutlineThickness(.75f);
	centerShape.setOutlineColor(sf::Color::Black);
	centerShape.setSize(sf::Vector2f(50.0f, 50.0f));
	centerShape.setCornersRadius(5.0f);
	centerShape.setCornerPointCount(30);

	//debug
	wallRect.setFillColor(sf::Color::White);
	wallRect.setOutlineThickness(1.5f);
	wallRect.setOutlineColor(sf::Color::Yellow);

	plRect.setFillColor(sf::Color(255,255,255,0));
	plRect.setOutlineColor(sf::Color::White);
	plRect.setOutlineThickness(2.5f);

	//Zombie
	zombieSpr.setTexture(manager->getAssets().getTexture("zombie_idle"));
	zombieSpr.setOrigin(zombieSpr.getGlobalBounds().width / 2, zombieSpr.getGlobalBounds().height / 2);
	zombieHp.setFillColor(sf::Color::Green);
	zombieHp.setSize(sf::Vector2f(ZOMBIEBAR_SIZE, ZOMBIEBAR_SIZE));
}

void Renderer::resetText(sf::Text & txt) {
	txt.setFont(manager->getAssets().getFont("main"));
	txt.setFillColor(sf::Color::White);
	txt.setOutlineThickness(0.55f);
	txt.setOutlineColor(sf::Color::Black);
	txt.setCharacterSize(20);
	txt.setStyle(sf::Text::Bold);
}

void Renderer::setActionText(std::string key, std::string text) {
	centerTxt.setString(text);
	centerTxt.setPosition(window->getSize().x / 2 - centerTxt.getGlobalBounds().width/2,
		window->getSize().y - ITEMTEXT_PADDING *2 - centerTxt.getGlobalBounds().height);

	centerShape.setPosition(
		centerTxt.getPosition().x - 30 - centerShape.getGlobalBounds().width,
		centerTxt.getPosition().y - centerShape.getGlobalBounds().height/2 + centerTxt.getGlobalBounds().height/2
	);
	centerKey.setString(key);
	/*centerKey.setPosition(
		centerShape.getPosition().x + centerShape.getGlobalBounds().width / 2 - centerKey.getGlobalBounds().width / 2,
		centerShape.getPosition().y + centerShape.getGlobalBounds().height / 2 - centerKey.getGlobalBounds().height / 2
	);*/

	centerKey.setPosition(
		centerShape.getPosition().x + centerShape.getGlobalBounds().width / 2 - (centerKey.getGlobalBounds().width / 4) * 3,
		centerShape.getPosition().y + centerShape.getGlobalBounds().height / 2 - (centerKey.getGlobalBounds().height / 4) * 3.5
	);
}

void Renderer::drawActionText() {
	window->draw(centerTxt);
	window->draw(centerShape);
	window->draw(centerKey);
}
