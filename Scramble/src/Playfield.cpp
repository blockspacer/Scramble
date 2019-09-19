#include "Playfield.h"
#include "Components/PlayerComponent.h"
#include "Components/Vector2D.h"
using namespace bloom::components;
void Playfield::handleInput(double deltaTime) {
	using bloom::input::MouseButton;
	using bloom::input::KeyboardKey;
	auto input = m_gameInstance->input;
	m_dt -= deltaTime;
	m_registry.group<>(entt::get<PlayerComponent, Vector2D, bloom::components::Size, Hitbox, Positionf, bloom::graphics::Sprite>).each(
		[&](PlayerComponent& playerComp, Vector2D& vector, bloom::components::Size& size, Hitbox& hitbox, Positionf& position, bloom::graphics::Sprite& sprite) {
			vector = Vector2D();
			if (input.keyboard.isPressed(KeyboardKey::KEY_UP))
				vector.y -= 1;
			if (input.keyboard.isPressed(KeyboardKey::KEY_DOWN))
				vector.y += 1;
			if (input.keyboard.isPressed(KeyboardKey::KEY_LEFT))
				vector.x -= 1;
			if (input.keyboard.isPressed(KeyboardKey::KEY_RIGHT))
				vector.x += 1;
			if (input.keyboard.isPressed(KeyboardKey::KEY_LEFT_SHIFT))
				playerComp.focused = true;
			else playerComp.focused = false;

			double length = std::sqrt((vector.x * vector.x) + (vector.y * vector.y));
			if (length != 0.0) {
				vector.x /= length;
				vector.y /= length;
			}

			vector.x *= playerComp.speed;
			vector.y *= playerComp.speed;
			if (playerComp.focused) {
				if (!playerComp.wasFocused) {
					sprite = bloom::graphics::Sprite(m_gameInstance->textures.load(ASSETSDIR / "Assets" / "Sprites" / "PlayerSpriteFocused.png"));
					playerComp.wasFocused = true;
				}
				vector.x /= 2, vector.y /= 2;
			}
			else {
				if (playerComp.wasFocused) {
					sprite = bloom::graphics::Sprite(m_gameInstance->textures.load(ASSETSDIR / "Assets" / "Sprites" / "PlayerSprite.png"));
					playerComp.wasFocused = false;
				}
			}

			if (input.mouse.isPressed(MouseButton::MOUSE_LEFT) && m_dt <= 0.0) {
				Positionf spawn{ position.x + size.w / 2, position.y + size.h / 2 };
				Positionf target{ input.mouse.getX() - spawn.x, input.mouse.getY() - spawn.y };
				bullets.emplace_back(m_registry, m_gameInstance).init(spawn, target, Hitbox::Type::friendlyBullet);
				m_dt = 200.0;
			}
		}
	);
}

void Playfield::update(double deltaTime) {
	enemyMovementSystem.update(deltaTime);
	movementSystem.update(deltaTime);
	bulletMovementSystem.update(deltaTime);
	collisionSystem.update(deltaTime);
	normalizerSystem.update();
}

void Playfield::draw() {
	renderSystem.update();
	auto input = m_gameInstance->input;
	m_gameInstance->textures.load(ASSETSDIR / "Assets" / "Sprites" / "Crosshair32.png")->render(std::nullopt, { input.mouse.getX() - 16, input.mouse.getY() - 16, 32, 32 });
}


