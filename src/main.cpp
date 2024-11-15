#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#define PI 3.14159265358979323846

void updateArc(sf::ConvexShape& shape, const sf::Vector2f& center, float radius, float startAngle, float endAngle) {
    int pointCount = shape.getPointCount();
    float angleStep = (startAngle <= endAngle)
        ? (endAngle - startAngle) / (pointCount - 2)
        : (360.0f - (startAngle - endAngle)) / (pointCount - 2);

    float angle = startAngle;
    for (int i = 0; i <= pointCount - 2; i++) {
        float radian = angle * (PI / 180.0f);
        shape.setPoint(i + 1, sf::Vector2f(radius * cos(radian), -radius * sin(radian)));
        angle += angleStep;
    }
    shape.setPoint(0, sf::Vector2f(0, 0));
    shape.setPosition(center);
}

float calculateDistance(const sf::Vector2f& point1, const sf::Vector2f& point2) {
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}

float calculateAngle(const sf::Vector2f& origin, const sf::Vector2f& target) {
    sf::Vector2f direction = target - origin;
    return atan2(-direction.y, direction.x) * 180.0f / PI;
}

bool isPointWithinFOV(const sf::Vector2f& point, const sf::Vector2f& origin, float fovAngle, float fovRadius, float angleToMouse) {
    float distance = calculateDistance(origin, point);
    if (distance > fovRadius) return false;

    float angleToPoint = calculateAngle(origin, point);
    float angleDifference = std::abs(angleToMouse - angleToPoint);

    return angleDifference <= fovAngle;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Moving Circle with FOV Cone");
    window.setFramerateLimit(60);

    sf::CircleShape circleA(20);
    circleA.setOrigin(20, 20);
    circleA.setFillColor(sf::Color::Blue);
    circleA.setPosition(300, 300);


    sf::CircleShape circleB(20);
    circleB.setOrigin(20, 20);
    circleB.setFillColor(sf::Color::Red);
    sf::Vector2f circleBPosition(100, 100);
    circleB.setPosition(circleBPosition);


    sf::ConvexShape fovCone(30);
    float fovRadius = 150.0f;
    float fovAngle = 45.0f;
    float fovStartAngle = 0.0f;
    float fovEndAngle = 0.0f;

    const int numCheckPoints = 12;

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) circleBPosition.y -= 2.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) circleBPosition.y += 2.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) circleBPosition.x -= 2.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) circleBPosition.x += 2.0f;
        circleB.setPosition(circleBPosition);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float angle = calculateAngle(circleBPosition, sf::Vector2f(mousePos));

        fovStartAngle = angle - fovAngle;
        fovEndAngle = angle + fovAngle;
        updateArc(fovCone, circleBPosition, fovRadius, fovStartAngle, fovEndAngle);

        bool isWithinFOV = false;
        float angleStep = 360.0f / numCheckPoints;

        for (int i = 0; i < numCheckPoints; ++i) {
            float currentAngle = i * angleStep * (PI / 180.0f);
            sf::Vector2f pointOnCircumference = circleA.getPosition() + sf::Vector2f(circleA.getRadius() * cos(currentAngle), circleA.getRadius() * sin(currentAngle));

            if (isPointWithinFOV(pointOnCircumference, circleBPosition, fovAngle, fovRadius, angle)) {
                isWithinFOV = true;
                break;
            }
        }

        if (isWithinFOV) {
            fovCone.setFillColor(sf::Color(255, 0, 0));
        }
        else {
            fovCone.setFillColor(sf::Color(0, 255, 0));
        }

        window.clear();
        window.draw(circleA);
        window.draw(fovCone);
        window.draw(circleB);
        window.display();
    }

    return 0;
}
