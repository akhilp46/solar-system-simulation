#include <iostream>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

const double G = 6.67430e-11; // Gravitational constant (m^3 kg^-1 s^-2)
const double TIME_STEP = 10000; // Time step in seconds (for simulation purposes)
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const double SCALE = 1e-9; // Scale factor for visualization

struct Vector3 {
    double x, y, z;

    Vector3 operator+(const Vector3& v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    Vector3 operator-(const Vector3& v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    Vector3 operator*(double scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vector3 operator/(double scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }
};

struct CelestialBody {
    std::string name;
    double mass;       // Mass in kilograms
    Vector3 position;  // Position in meters
    Vector3 velocity;  // Velocity in meters per second
    sf::CircleShape shape; // Visual representation
};

Vector3 calculateGravitationalForce(const CelestialBody& a, const CelestialBody& b) {
    Vector3 direction = b.position - a.position;
    double distance = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    double forceMagnitude = (G * a.mass * b.mass) / (distance * distance);
    Vector3 unitDirection = direction / distance;
    return unitDirection * forceMagnitude;
}

int main() {
    // Define celestial bodies
    CelestialBody sun = {"Sun", 1.989e30, {0, 0, 0}, {0, 0, 0}, sf::CircleShape(20)};
    sun.shape.setFillColor(sf::Color::Yellow);
    sun.shape.setOrigin(20, 20);
    sun.shape.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    CelestialBody earth = {"Earth", 5.972e24, {1.496e11, 0, 0}, {0, 29780, 0}, sf::CircleShape(5)};
    earth.shape.setFillColor(sf::Color::Blue);
    earth.shape.setOrigin(5, 5);

    std::vector<CelestialBody> bodies = {sun, earth};

    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Solar System Simulation");

    // Run the simulation
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Calculate forces on each body
        std::vector<Vector3> forces(bodies.size(), {0, 0, 0});

        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = 0; j < bodies.size(); ++j) {
                if (i != j) {
                    forces[i] = forces[i] + calculateGravitationalForce(bodies[i], bodies[j]);
                }
            }
        }

        // Update velocities and positions of each body
        for (size_t i = 0; i < bodies.size(); ++i) {
            Vector3 acceleration = forces[i] / bodies[i].mass;
            bodies[i].velocity = bodies[i].velocity + acceleration * TIME_STEP;
            bodies[i].position = bodies[i].position + bodies[i].velocity * TIME_STEP;

            // Update visual representation
            if (bodies[i].name != "Sun") { // Keep Sun at the center
                bodies[i].shape.setPosition(
                    WINDOW_WIDTH / 2 + bodies[i].position.x * SCALE,
                    WINDOW_HEIGHT / 2 + bodies[i].position.y * SCALE
                );
            }
        }

        // Render
        window.clear();
        for (const auto& body : bodies) {
            window.draw(body.shape);
        }
        window.display();
    }

    return 0;
}
